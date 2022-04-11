
#include "vadd_dma.h"
#define DATA_SIZE 4096

// TRIPCOUNT identifier
const int c_size = DATA_SIZE;

int pop_stream(AXI_T const &e) // type casting AXI_T -> int
{
#pragma HLS INLINE
	int ret = e.data;
	volatile ap_uint<4> keep = e.keep; // -1 : 1111
	volatile ap_uint<1> last = e.last; // only asserted at the last element
	return ret;
}

AXI_T push_stream(int const ret, int last) // type casting AXI_T -> int
{
#pragma HLS INLINE
	AXI_T e;
	e.data = ret;
	e.keep = -1; // 1111
	e.last = last ? 1 : 0; // 1 when last
	/*
	if(last)
	{
		e.last = 1;
	}
	else
	{
		e.last = 0;
	}*/
	return e;
}



static void load_input(AXI_T* in, STREAM_INT& inStream, int size) {
mem_rd:
    for (int i = 0; i < size; i++) {
#pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
#pragma HLS PIPELINE II=1
        inStream << pop_stream(in[i]);
    }
}

static void compute_add(STREAM_INT& in1_stream,
                        STREAM_INT& in2_stream,
                        STREAM_INT& out_stream,
                        int size) {
// The kernel is operating with vector of NUM_WORDS integers. The + operator performs
// an element-wise add, resulting in NUM_WORDS parallel additions.
execute:
    for (int i = 0; i < size; i++) {
#pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
#pragma HLS PIPELINE II=1
        out_stream << (in1_stream.read() + in2_stream.read());
    }
}

static void store_result(AXI_T* out, STREAM_INT& out_stream, int size) {
mem_wr:
    for (int i = 0; i < size; i++) {
#pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
#pragma HLS PIPELINE II=1
        out[i] = push_stream(out_stream.read(), i == size - 1);
    }
}

/*
    Vector Addition Kernel
    Arguments:
        in1  (input)  --> Input vector 1
        in2  (input)  --> Input vector 2
        out  (output) --> Output vector
        size (input)  --> Number of elements in vector
*/


/*
 *
 * ap_axiu<32, 1, 1, 1>
 *AXI_T.data : 32bit
 *AXI_T.
 *AXI_T.last
 *
 *
 */
void krnl_vadd(AXI_T* in1, AXI_T* in2, AXI_T* out, int size) {
#pragma HLS INTERFACE axis port = in1 depth=10
#pragma HLS INTERFACE axis port = in2 depth=10
#pragma HLS INTERFACE axis port = out depth=10
#pragma HLS INTERFACE s_axilite port = size
#pragma HLS INTERFACE ap_ctrl_none port=return


    static STREAM_INT in1_stream("input_stream_1");
    static STREAM_INT in2_stream("input_stream_2");
    static STREAM_INT out_stream("output_stream");

#pragma HLS dataflow
    // dataflow pragma instruct compiler to run following three APIs in parallel
    load_input(in1, in1_stream, size);
    load_input(in2, in2_stream, size);
    compute_add(in1_stream, in2_stream, out_stream, size);
    store_result(out, out_stream, size);
}

