#include "matmulaccel.h"


//
void matmul_proc(ap_int<2> a[SIZE][SIZE*2], ap_int<2> b[SIZE*2][SIZE], ap_int<8> c[SIZE][SIZE]){
#pragma HLS ARRAY_RESHAPE variable=a complete dim=2
#pragma HLS ARRAY_RESHAPE variable=b complete dim=2
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

void export_output(AXI_INT8 *c, ap_int<8> cbuf[SIZE][SIZE]){
//#pragma HLS INLINE
	for(int i=0;i<SIZE;++i){
		for(int j=0;j<SIZE;++j){
#pragma HLS UNROLL
			int idx = i*SIZE + j;
			c[idx] = push_stream<8>(cbuf[i][j], idx == (SIZE*SIZE - 1));
		}
	}
}

template< int row, int col>
void import_input(AXI_F *x, ap_int<2> xbuf[row][col]){
//#pragma HLS INLINE
	for(int i=0;i<row;++i){
		for(int j=0;j<col;++j){
#pragma HLS UNROLL
			int idx = i*col + j;
			xbuf[i][j] = pop_stream<64>(x[idx]);
		}
	}
}

void matmul_accel(AXI_F* a, AXI_F* b, AXI_INT8* c){
#pragma HLS INTERFACE axis port=a depth = 32768
#pragma HLS INTERFACE axis port=b depth = 32768
#pragma HLS INTERFACE axis port=c depth = 32768
//#pragma HLS INTERFACE s_axilite port=return bundle=CTRL_BUS
#pragma HLS INTERFACE ap_ctrl_none port=return

#pragma HLS DATAFLOW
	ap_int<2> a_buf[SIZE][SIZE*2], b_buf[SIZE*2][SIZE];
	ap_int<8> c_buf[SIZE][SIZE];

	import_input<SIZE,SIZE*2>(a,a_buf);
	import_input<SIZE*2,SIZE>(b,b_buf);

	matmul_proc(a_buf,b_buf,c_buf);
	export_output(c,c_buf);
}

