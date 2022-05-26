#include <assert.h>
#include <ap_axi_sdata.h>
#include <stdio.h>
#include <string.h>


#define SIZE 64
#define TILE 32

typedef ap_axiu<32,4,5,5> AXI_F;
typedef ap_axis<2,4,5,5> AXI_INT2;
typedef ap_axis<8,4,5,5> AXI_INT8;
typedef ap_axis<16,4,5,5> AXI_INT16;

//void fclayer(AXI_F* image, AXI_F* fc1w, AXI_F* fc2w, AXI_F* fc3w, AXI_INT16* out);
void fclayer(AXI_F* image, unsigned int* fc1w, unsigned int* fc2w, unsigned int* fc3w, AXI_INT16* out);
typedef ap_axis<1,4,5,5> AXI_INT1;

template <int T>
ap_uint<32> pop_stream(AXI_F const &e)
{
#pragma HLS INLINE

	//ap_uint<1> ret = ((e.data & 0x80000000) ? 0 : 1);

	ap_uint<32> ret = e.data;

	volatile ap_uint<T/8> strb = e.strb;
	volatile ap_uint<T/8> keep = e.keep;
	volatile ap_uint<4> user = e.user;
	volatile ap_uint<1> last = e.last;
	volatile ap_uint<5> id = e.id;
	volatile ap_uint<5> dest = e.dest;

	return ret;
}

template <int T>
AXI_INT16 push_stream(ap_int<T> const &v, bool last = false){
#pragma HLS INLINE
	AXI_INT16 e;

	e.data = v;

	// set it to sizeof(T) ones
	e.strb = -1;
	e.keep = 15; //e.strb;
	e.user = 0;
	e.last = last ? 1 : 0;
	e.id = 0;
	e.dest = 0;
	return e;
}

// you can use the template as follows.
// in_matrix[i][j] = pop_stream<float>(in_matrix[k]);
