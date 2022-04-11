
#include "saxpy_stream.h"
#define DATA_SIZE 32

// TRIPCOUNT identifier
const int c_size = DATA_SIZE;

//https://stackoverflow.com/questions/49458917/casting-from-unsigned-int-to-float

uint32_t  float_to_uint(const float f)
{
#pragma HLS INLINE
    union {
        uint32_t  u;
        float     f;
    } temp;

    temp.f = f;
    return temp.u;
}

float uint_to_float(const uint32_t u)
{
#pragma HLS INLINE
    union {
        uint32_t  u;
        float     f;
    } temp;

    temp.u = u;
    return temp.f;
}

float pop_stream(AXI_T const &e) // type casting AXI_T -> int
{
#pragma HLS INLINE
	AXI_T tmp;
	tmp = e;
	ap_uint<32> data_buf = tmp.data;
	uint32_t data_buf2 = data_buf;
	//float ret = *(float*)(&data_buf);
	float ret = uint_to_float(data_buf2);
	volatile ap_uint<4> keep = tmp.keep; // -1 : 1111
	volatile ap_uint<1> last = tmp.last; // only asserted at the last element
	return ret;
}

AXI_T push_stream(float const ret, int last) // type casting AXI_T -> int
{
#pragma HLS INLINE
	AXI_T e;
	uint32_t retbuf = float_to_uint(ret);
	ap_uint<32> retbuf2 = retbuf;
	e.data = retbuf2;
	e.keep = -1; // 1111
	e.last = last ? 1 : 0; // 1 when last
	return e;
}


//https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/Non-Blocking-Read
//https://support.xilinx.com/s/question/0D52E00006hprWiSAI/how-to-properly-dataflow-functions-in-hls?language=en_US
static void load_input(float* in, STREAM_T& inStream, int size) {
#pragma HLS STREAM variable=inStream depth=12
#pragma HLS DATA_PACK variable=in
mem_rd1234:
    for (int i = 0; i < size; i++) {
#pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
#pragma HLS PIPELINE II=1
        inStream.write(push_stream(in[i], i == size - 1 ? 1 : 0));
    }
}

static void compute_add(STREAM_T& in1_stream,
                        STREAM_T& in2_stream,
                        STREAM_T& out_stream,
						float a,
                        int size) {
#pragma HLS STREAM variable=in1_stream depth=12
#pragma HLS STREAM variable=in2_stream depth=12
#pragma HLS STREAM variable=out_stream depth=12
// The kernel is operating with vector of NUM_WORDS integers. The + operator performs
// an element-wise add, resulting in NUM_WORDS parallel additions.
	float abuf;
	abuf = a;

	AXI_T in1buf, in2buf, obuf;
execute:
    for (int i = 0; i < size; i++) {
#pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
#pragma HLS PIPELINE II=1
    	in1_stream.read(in1buf);
    	in2_stream.read(in2buf);

    	float x = uint_to_float(in1buf.data);
    	float y = uint_to_float(in2buf.data);
    	float axpy = a*x + y;
    	//uint32_t axpy_uint = float_to_uint(axpy);
        out_stream.write(push_stream(axpy,(i==size-1 ? 1 : 0)));
    }
}

static void store_result(float* out, STREAM_T& out_stream, int size) {
#pragma HLS STREAM variable=out_stream depth=12
#pragma HLS DATA_PACK variable=out
	AXI_T ostream;
mem_wr:
    for (int i = 0; i < size; i++) {
#pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
#pragma HLS PIPELINE II=1
    	out_stream.read(ostream);
        //out[i] = push_stream(ostream, (i == size - 1 ? 1 : 0));
    	out[i] = uint_to_float(ostream.data);
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
void krnl_saxpy(float* X, float* Y, float* Z, float a, int size) {
#pragma HLS INTERFACE axis port = X depth=4096
#pragma HLS INTERFACE axis port = Y depth=4096
#pragma HLS INTERFACE axis port = Z depth=4096
#pragma HLS INTERFACE s_axilite port = size
#pragma HLS INTERFACE s_axilite port = a
//#pragma HLS DATA_PACK variable=X
//#pragma HLS DATA_PACK variable=Y
//#pragma HLS DATA_PACK variable=Z

//#pragma HLS INTERFACE ap_ctrl_none port=return


    static STREAM_T X_stream("input_stream_X");
    static STREAM_T Y_stream("input_stream_Y");
    static STREAM_T Z_stream("Z_stream");

#pragma HLS dataflow

    int sz1 = size;
    int sz2 = size;

    // dataflow pragma instruct compiler to run following three APIs in parallel
    load_input(X, X_stream, sz1);
    load_input(Y, Y_stream, sz2);
    float abuf = a;
    compute_add(X_stream, Y_stream, Z_stream, abuf, size);
    store_result(Z, Z_stream, size);
}

