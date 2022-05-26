#include <assert.h>
#include <ap_axi_sdata.h>
#include <stdio.h>

#define SIZE 64


//void matmul(float a[SIZE][SIZE*2], float b[SIZE*2][SIZE], float c[SIZE][SIZE]);
//void matmul(AXI_F a[SIZE*SIZE*2], AXI_F b[SIZE*2*SIZE], AXI_F c[SIZE*SIZE]);
//void matmul_simple2(AXI_F* a, AXI_F* b);


typedef ap_axiu<64,4,5,5> AXI_F;
typedef ap_axis<2,4,5,5> AXI_INT2;
typedef ap_axis<8,4,5,5> AXI_INT8;
void matmul_accel(AXI_F* a, AXI_F* b, AXI_INT8* c);

// --------------------------------------------------------
// functions to insert and extract elements from an axi stream
// includes conversion to correct data type

/*
ap_axi_sdata.h::

#include "ap_int.h"

template<int D,int U,int TI,int TD>
struct ap_axis{
 ap_int<D>    data;
 ap_uint<D/8> keep;
 ap_uint<D/8> strb;
 ap_uint<U>   user;
 ap_uint<1>   last;
 ap_uint<TI>  id;
 ap_uint<TD>  dest;
};
 */

//pop_stream<64>()
template <int T>
ap_int<2> pop_stream(AXI_F const &e)
{
#pragma HLS INLINE

	/*
	assert(sizeof(T) == sizeof(int)); // It will only support a dtype of 32bits (4bytes)
	union
	{
		int ival;
		T oval;
	} converter;
	converter.ival = e.data;
	T ret = converter.oval;
	*/

	ap_int<2> ret = (e.data & 0x8000000000000000 ? -1 : 1);

	volatile ap_uint<T/8> strb = e.strb;
	volatile ap_uint<T/8> keep = e.keep;
	volatile ap_uint<4> user = e.user;
	volatile ap_uint<1> last = e.last;
	volatile ap_uint<5> id = e.id;
	volatile ap_uint<5> dest = e.dest;

	return ret;
}

template <int T>
AXI_INT8 push_stream(ap_int<8> const &v, bool last = false){
#pragma HLS INLINE
	AXI_INT8 e;

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
