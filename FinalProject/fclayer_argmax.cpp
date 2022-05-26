#include "fclayer.h"

/*
template<int csize>
void export_output(AXI_INT16 *c, ap_int<16> cbuf[csize]){
//#pragma HLS PIPELINE II=1
	for(int i=0;i< csize - 1; ++i){
#pragma HLS UNROLL
		c[i] = push_stream<16>(cbuf[i], false);
	}
	c[csize-1] = push_stream<16>(cbuf[csize-1], true);
}
*/

template<int csize>
void export_output(AXI_INT16 *c, ap_int<16> cbuf[csize], bool last){
//#pragma HLS PIPELINE II=1
	for(int i=0;i< csize - 1; ++i){
#pragma HLS UNROLL
		c[i] = push_stream<16>(cbuf[i], false);
	}
	c[csize-1] = push_stream<16>(cbuf[csize-1], last);
}

template<int tsize>
void import_input(AXI_F *x, ap_uint<32> xbuf[tsize]){

	for(int i=0;i<tsize;++i){
#pragma HLS UNROLL factor=256
		xbuf[i] = pop_stream<32>(x[i]);
	}
}

template<int tsize>
void lut_xnor(ap_uint<tsize> a, ap_uint<tsize> b, ap_uint<tsize>& c){
#pragma HLS INLINE
	//return ~(a^b);
	//printf("%u %u %u\n",(unsigned int)a,(unsigned int)b,(unsigned int)(~(a^b)));
	//return ((a==b) ? 1 : 0);
	unsigned int tmp = (unsigned int)(~(a^b));
	c = tmp;
}

template<int tsize>
void pop_count(ap_uint<tsize> t, ap_int<16>& ret, int N){
#pragma HLS INLINE

	ap_int<16> tmp = 0;
	for(int i=0;i<32;++i){
#pragma HLS UNROLL
//#pragma HLS RESOURCE variable=tmp core=AddSub_DSP
		tmp += t[i];
	}
#pragma HLS RESOURCE variable=tmp core=DSP48
	tmp = (tmp*2);
#pragma HLS RESOURCE variable=tmp core=AddSub_DSP
	tmp = tmp - N;

	ret = tmp;
}

template<int asize, int bsize, int csize, int N>
void packed_matmul(ap_uint<32> a, ap_uint<32> b[bsize], ap_int<16> c[csize]){

	for(int j=0;j<csize;++j){
#pragma HLS UNROLL factor=12
		ap_uint<32> tmp = 0;
		ap_int<16> pcount = 0;
		lut_xnor<32> (a,b[j], tmp);
		pop_count<32>(tmp, pcount,N);
		c[j] += pcount;
	}
}

template<int asize, int bsize, int csize>
void packed_matmul_layer1(ap_uint<32> a, ap_uint<32> b[bsize], ap_int<16> c[csize]){

	ap_uint<32> all_bits_on = 0xffffffff;

	for(int j=0;j<csize;++j){
#pragma HLS UNROLL factor=12
		ap_uint<32> tmp = 0;
		ap_int<16> pcount = 0;
		lut_xnor<32> (a,b[j], tmp);

		ap_uint<32> turned_on = (all_bits_on & a);
		ap_int<8> turned_on_bits = 0;
		for(int i=0;i<32;++i){
#pragma HLS UNROLL
			turned_on_bits += turned_on[i];
		}

		ap_uint<32> tmp2 = turned_on & tmp;
		ap_int<8> tmp3 = 0;
		for(int i=0;i<32;++i){
#pragma HLS UNROLL
//#pragma HLS RESOURCE variable=tmp3 core=AddSub_DSP
			tmp3 += tmp2[i];
		}
#pragma HLS RESOURCE variable=tmp3 core=DSP48
		tmp3 = 2*tmp3;
#pragma HLS RESOURCE variable=tmp3 core=AddSub_DSP
		tmp3 -= turned_on_bits;
		c[j] += tmp3;
	}
}

template<int bufsize, int packsize>
void pack_abuf(ap_uint<32> a_buf[bufsize], ap_uint<32> a_pack[packsize]){

#pragma HLS DATAFLOW

	for(int i=0;i<packsize;++i){
#pragma HLS UNROLL
		a_pack[i] = 0;
	}

	for(unsigned int i=0;i<bufsize;++i){
#pragma HLS UNROLL factor=8

		unsigned int pack_idx;
#pragma HLS RESOURCE variable=pack_idx core=DSP48
		pack_idx  = i/32;
		unsigned int bit_idx = i%32;

		// a_buf : only 1 or 0
		//ap_uint<1> tmp = a_buf[i];
		a_pack[pack_idx][bit_idx] = a_buf[i];
	}

}

template<int asize, int bsize>
void pack_output_32(ap_int<16> output[asize], ap_uint<32> outpack[bsize]){

	for(int i=0;i<bsize;++i){
#pragma HLS UNROLL
		outpack[i] = 0;
	}

	for(int i=0;i<asize;++i){
#pragma HLS UNROLL factor=16
		int idx;
#pragma HLS RESOURCE variable=idx core=DSP48
		idx = i / 32;
		int bit_idx = i % 32;
		outpack[idx][bit_idx] = (output[i] > 0 ? 1 : 0);
	}
}

void argmax(ap_int<16> out_buf[100], ap_int<16> result_buf[10]){
	for(int i=0;i<10;++i){
#pragma HLS UNROLL
		int maxi = -128;
		int idx = 0;
		for(int j=0;j<10;++j){
			if(maxi < out_buf[i*10 + j]){
				maxi = out_buf[i*10 + j];
				idx = j;
			}
		}
		result_buf[i] = idx;
	}
}

void fclayer(AXI_F* image, AXI_F* fc1w, AXI_F* fc2w, AXI_F* fc3w, AXI_INT16* out){
#pragma HLS INTERFACE axis port=image depth = 784
#pragma HLS INTERFACE axis port=fc1w depth = 3200
#pragma HLS INTERFACE axis port=fc2w depth = 256
#pragma HLS INTERFACE axis port=fc3w depth = 20
#pragma HLS INTERFACE axis port=out depth=10
#pragma HLS INTERFACE ap_ctrl_none port=return

	ap_uint<32> image_buf[7840], fc1w_buf[3200], image_pack[25],
		fc2w_buf[256], fc3w_buf[20],X1[4],X2[2];

#pragma HLS RESOURCE variable=image_buf core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fc1w_buf core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fc2w_buf core=RAM_1P_BRAM

	ap_int<16> X1_buf[128], X2_buf[64],X3_buf[10];
//#pragma HLS RESOURCE variable=X1_buf core=RAM_2P_BRAM
//#pragma HLS RESOURCE variable=X2_buf core=RAM_2P_BRAM

#pragma HLS ARRAY_PARTITION variable=image_buf cyclic factor=64
#pragma HLS ARRAY_PARTITION variable=fc1w_buf cyclic factor=16
#pragma HLS ARRAY_PARTITION variable=X1 complete
#pragma HLS ARRAY_PARTITION variable=X1_buf complete
#pragma HLS ARRAY_PARTITION variable=X2 complete
#pragma HLS ARRAY_PARTITION variable=X2_buf complete
#pragma HLS ARRAY_PARTITION variable=X3_buf complete
#pragma HLS ARRAY_PARTITION variable=image_pack cyclic factor=4
#pragma HLS ARRAY_PARTITION variable=fc2w_buf cyclic factor=4
#pragma HLS ARRAY_PARTITION variable=fc3w_buf complete
	ap_int<16> out_buf[100];
	ap_int<16> result_buf[10];
#pragma HLS RESOURCE variable=out_buf core=RAM_2P_BRAM
#pragma HLS ARRAY_PARTITION variable=out_buf cyclic factor = 10
#pragma HLS ARRAY_PARTITION variable=result_buf cyclic factor = 10

	import_input<7840>(image, image_buf);
	import_input<3200>(fc1w, fc1w_buf);
	import_input<256>(fc2w, fc2w_buf);
	import_input<20>(fc3w, fc3w_buf);

	for(int batch=0;batch<10;++batch){
		x1buf_clear:
		for(int i=0;i<128;++i){
		#pragma HLS UNROLL
			X1_buf[i] = 0;
		}

		x2buf_clear:
		for(int i=0;i<64;++i){
		#pragma HLS UNROLL
			X2_buf[i] = 0;
		}

		x3buf_clear:
		for(int i=0;i<10;++i){
		#pragma HLS UNROLL
			X3_buf[i] = 0;
		}

		pack_abuf<784, 25>(image_buf + batch*784, image_pack);

		for(int i=0;i<25;++i){
			packed_matmul_layer1<1,128,128>(image_pack[i],fc1w_buf + i*128,X1_buf);
		}
		//import_input<128>(fc1w + 3072, fc1w_buf);
		//packed_matmul_layer1<1,128,128>(image_pack[24],fc1w_buf,X1_buf);

		/////////////////////////////
		pack_output_32<128,4>(X1_buf, X1);

		for(int i=0;i<4;++i){
			packed_matmul<1,64,64,32>(X1[i],fc2w_buf + i*64,X2_buf);
		}
		////////////////////////////////
		pack_output_32<64,2>(X2_buf, X2);

		for(int i=0;i<2;++i){
			packed_matmul<1,10,10,32>(X2[i],fc3w_buf + i*10,X3_buf);
		}

		//export_output<10>(out + batch*10, X3_buf, (batch == 9) ? true : false);
		for(int i=0;i<10;++i){
		#pragma HLS UNROLL
			out_buf[batch*10 + i] = X3_buf[i];
		}
	}
	argmax(out_buf, result_buf);
	export_output<10>(out, result_buf, true);
}
