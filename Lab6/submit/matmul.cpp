#include "matmul.h"

/*
float pop_stream(AXI_F const &e) // type casting AXI_F -> int
{
#pragma HLS INLINE

    union conv {
        float f;
        unsigned int u;
    };

    conv tmp;
    tmp.u = e.data;

    float ret = tmp.f;
    volatile ap_uint<4> keep = e.keep; // -1 : 1111
	volatile ap_uint<1> last = e.last; // only asserted at the last element
	return ret;
}

AXI_F push_stream(float const ret, int last) // type casting AXI_F -> int
{
#pragma HLS INLINE
    union conv {
        float f;
        unsigned int u;
    };

    conv tmp;
    tmp.f = ret;

    AXI_F e;
    e.data = tmp.u;
    e.keep = -1;           // 1111
    e.last = last ? 1 : 0; // 1 when last
	return e;
}
*/

//
void matmul_proc(float a[SIZE][SIZE*2], float b[SIZE*2][SIZE], float c[SIZE][SIZE]){
//#pragma HLS ARRAY_RESHAPE variable=a complete dim=2
//#pragma HLS ARRAY_RESHAPE variable=b complete dim=2
//#pragma HLS resource variable=a core=RAM_1P_LUTRAM
//#pragma HLS resource variable=b core=RAM_1P_LUTRAM
//#pragma HLS resource variable=c core=RAM_1P_LUTRAM

	//latency : 42ms
	for(int k=0;k<SIZE*2;++k){
		for(int i=0;i<SIZE;++i){
			float aik = a[i][k];
			for(int j=0;j<SIZE;++j){
#pragma HLS UNROLL
				c[i][j] += aik*b[k][j];
			}
		}
	}

	/*
	// latency : 0.4 s
	for(int i=0;i<SIZE;++i){
		for(int j=0;j<SIZE;++j){
			float ctemp = 0;
			for(int k=0;k<SIZE*2;++k){
#pragma HLS UNROLL
				ctemp += a[i][k]*b[k][j];
			}
			c[i][j] = ctemp;
		}
	}
	*/

}

void export_output(AXI_F *c, float cbuf[SIZE][SIZE]){
//#pragma HLS INLINE
	for(int i=0;i<SIZE;++i){
		for(int j=0;j<SIZE;++j){
			int idx = i*SIZE + j;
			c[idx] = push_stream<float>(cbuf[i][j], idx == (SIZE*SIZE - 1));
		}
	}
}

template< int row, int col>
void import_input(AXI_F *x, float xbuf[row][col]){
//#pragma HLS INLINE
	for(int i=0;i<row;++i){
		for(int j=0;j<col;++j){
			int idx = i*col + j;
			xbuf[i][j] = pop_stream<float>(x[idx]);
		}
	}
}

void matmul_simple2(AXI_F *a, AXI_F *b, AXI_F *c){
#pragma HLS INTERFACE axis port=a depth = 32768
#pragma HLS INTERFACE axis port=b depth = 32768
#pragma HLS INTERFACE axis port=c depth = 32768
//#pragma HLS INTERFACE s_axilite port=return bundle=CTRL_BUS
#pragma HLS INTERFACE ap_ctrl_none port=return

#pragma HLS DATAFLOW
	float a_buf[SIZE][SIZE*2], b_buf[SIZE*2][SIZE], c_buf[SIZE][SIZE];

	import_input<SIZE,SIZE*2>(a,a_buf);
	import_input<SIZE*2,SIZE>(b,b_buf);

	matmul_proc(a_buf,b_buf,c_buf);
	export_output(c,c_buf);
}



/*
#include "simple_matmul.h"

float pop_stream(AXI_F const &e) // type casting AXI_F -> int
{
#pragma HLS INLINE

    union conv {
        float f;
        unsigned int u;
    };

    conv tmp;
    tmp.u = e.data;

    float ret = tmp.f;
    volatile ap_uint<4> keep = e.keep; // -1 : 1111
	volatile ap_uint<1> last = e.last; // only asserted at the last element
	return ret;
}

AXI_F push_stream(float const ret, int last) // type casting AXI_F -> int
{
#pragma HLS INLINE
    union conv {
        float f;
        unsigned int u;
    };

    conv tmp;
    tmp.f = ret;

    AXI_F e;
    e.data = tmp.u;
    e.keep = -1;           // 1111
    e.last = last ? 1 : 0; // 1 when last
	return e;
}

void matmul_proc(float a[SIZE][SIZE*2], float b[SIZE*2][SIZE], float c[SIZE][SIZE]){
#pragma HLS ARRAY_RESHAPE variable=a complete dim=2
#pragma HLS ARRAY_RESHAPE variable=b complete dim=2

#pragma HLS DATAFLOW

	float a_buf[SIZE][SIZE*2];
#pragma HLS ARRAY_RESHAPE variable=a_buf complete dim=2
	float b_buf[SIZE*2][SIZE];
#pragma HLS ARRAY_RESHAPE variable=b_buf complete dim=2
	float c_buf[SIZE][SIZE];

	for(int i=0;i<SIZE;++i){
		for(int j=0;j<SIZE*2;++j){
#pragma HLS UNROLL factor = 4
			a_buf[i][j] = a[i][j];
		}
	}

	for(int i=0;i<SIZE*2;++i){
		for(int j=0;j<SIZE;++j){
#pragma HLS UNROLL factor = 4
			b_buf[i][j] = b[i][j];
		}
	}

	for(int i=0;i<SIZE;++i){
		for(int j=0;j<SIZE;++j){
#pragma HLS PIPELINE II=256
			float c_tmp = 0;
			for(int k=0;k<SIZE*2;++k){
#pragma HLS UNROLL factor = 4
				c_tmp += a_buf[i][k]*b_buf[k][j];
			}
			c_buf[i][j] = c_tmp;
		}
	}

	for(int i=0;i<SIZE;++i){
		for(int j=0;j<SIZE;++j){
#pragma HLS UNROLL factor = 4
			c[i][j] = c_buf[i][j];
		}
	}

}

void export_output(AXI_F *c, float cbuf[SIZE][SIZE]){
#pragma HLS INLINE
	for(int i=0;i<SIZE;++i){
		for(int j=0;j<SIZE;++j){
#pragma HLS PIPELINE II=4
			int idx = i*SIZE + j;
			c[idx] = push_stream(cbuf[i][j], idx == (SIZE*SIZE - 1));
		}
	}
}

template< int row, int col>
void import_input(AXI_F *x, float xbuf[row][col]){
#pragma HLS INLINE
	for(int i=0;i<row;++i){
		for(int j=0;j<col;++j){
#pragma HLS PIPELINE II=4
			int idx = i*col + j;
			xbuf[i][j] = pop_stream(x[idx]);
		}
	}
}

void matmul_simple(AXI_F *a, AXI_F *b, AXI_F *c){
#pragma HLS INTERFACE axis port=a
#pragma HLS INTERFACE axis port=b
#pragma HLS INTERFACE axis port=c
//#pragma HLS INTERFACE s_axilite port=return bundle=CTRL_BUS
#pragma ap_ctrl_none port=return

#pragma HLS DATAFLOW
	float a_buf[SIZE][SIZE*2], b_buf[SIZE*2][SIZE], c_buf[SIZE][SIZE];

	import_input<SIZE,SIZE*2>(a,a_buf);
	import_input<SIZE*2,SIZE>(b,b_buf);

	matmul_proc(a_buf,b_buf,c_buf);
	export_output(c,c_buf);
}
*/
