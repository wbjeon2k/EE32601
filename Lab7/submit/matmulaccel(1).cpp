#include "matmulaccel.h"


//
void matmul_proc(ap_int<2> a[SIZE][SIZE*2], ap_int<2> b[SIZE*2][SIZE],
				 ap_int<8> c[SIZE][SIZE]){
//#pragma HLS ARRAY_RESHAPE variable=c complete dim=2

	//latency : 42ms
	for(int k=0;k<SIZE*2;++k){
		for(int i=0;i<SIZE;++i){
#pragma HLS PIPELINE II=1
			ap_int<8> aik = a[i][k];
			for(int j=0;j<SIZE;++j){
#pragma HLS UNROLL
				c[i][j] += aik*b[k][j];
			}
		}
	}

}


void clear_cbuf(ap_int<8> c[SIZE][SIZE]){

	for(int i=0;i<SIZE;++i){
#pragma HLS PIPELINE II=1
		for(int j=0;j<SIZE;++j){
	//#pragma HLS PIPELINE II=1
	#pragma HLS UNROLL
			c[i][j]=0;
		}
	}
}



void export_output(AXI_INT8 *c, ap_int<8> cbuf[SIZE][SIZE]){
//#pragma HLS PIPELINE II=1
	for(int i=0;i<SIZE;++i){
		#pragma HLS PIPELINE II=1
		for(int j=0;j<SIZE;++j){
//#pragma HLS UNROLL
			int idx = i*SIZE + j;
			c[idx] = push_stream<8>(cbuf[i][j], idx == (SIZE*SIZE - 1));
		}
	}
}

template< int row, int col>
void import_input(AXI_F *x, ap_int<2> xbuf[row][col]){

	for(int i=0;i<row;++i){
		#pragma HLS PIPELINE II=1
		for(int j=0;j<col;++j){
//#pragma HLS UNROLL
			int idx = i*col + j;
			xbuf[i][j] = pop_stream<64>(x[idx]);
		}
	}
}


void matmul_init(AXI_F*a,ap_int<2> a_buf[SIZE][SIZE*2],
		AXI_F* b,ap_int<2>b_buf[SIZE*2][SIZE],
		ap_int<8> c_buf[SIZE][SIZE]){
#pragma HLS DATAFLOW
	import_input<SIZE,SIZE*2>(a,a_buf);
	import_input<SIZE*2,SIZE>(b,b_buf);
	clear_cbuf(c_buf);
}

void matmul_accel(AXI_F* a, AXI_F* b, AXI_INT8* c){
#pragma HLS INTERFACE axis port=a depth = 8192
#pragma HLS INTERFACE axis port=b depth = 8192
#pragma HLS INTERFACE axis port=c depth = 4096
//#pragma HLS INTERFACE s_axilite port=return bundle=CTRL_BUS
#pragma HLS INTERFACE ap_ctrl_none port=return

//#pragma HLS DATAFLOW
	ap_int<2> a_buf[SIZE][SIZE*2], b_buf[SIZE*2][SIZE];
//#pragma HLS ARRAY_RESHAPE variable=a_buf complete dim=2
#pragma HLS ARRAY_RESHAPE variable=b_buf complete dim=2
	ap_int<8> c_buf[SIZE][SIZE];
#pragma HLS ARRAY_RESHAPE variable=c_buf complete dim=2

	matmul_init(a,a_buf,b,b_buf,c_buf);
	//memset(c_buf, 0 ,SIZE*SIZE*sizeof(ap_int<8>));

	matmul_proc(a_buf,b_buf,c_buf);
	export_output(c,c_buf);
}

