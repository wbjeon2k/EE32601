
#include "saxpy_stream.h"
#define DATA_SIZE 4096

// TRIPCOUNT identifier
const int c_size = DATA_SIZE;

//https://stackoverflow.com/questions/49458917/casting-from-unsigned-int-to-float

int float_to_uint(const float f)
{
#pragma HLS INLINE
    union {
        int  u;
        float     f;
    } temp;

    temp.f = f;
    return temp.u;
}

ap_uint<32> uint_to_float(const int u)
{
#pragma HLS INLINE
    union {
    	int  u;
        float     f;
    } temp;

    temp.u = u;
    return temp.f;
}

float pop_stream(AXI_T const &e) // type casting AXI_T -> int
{
#pragma HLS INLINE

	//float ret = *(float*)(&data_buf);
	float ret = uint_to_float(e.data);
	volatile ap_uint<4> keep = e.keep; // -1 : 1111
	volatile ap_uint<1> last = e.last; // only asserted at the last element
	return ret;
}

AXI_T push_stream(float const ret, int last) // type casting AXI_T -> int
{
#pragma HLS INLINE
	AXI_T e;
	e.data = float_to_uint(ret);
	e.keep = -1; // 1111
	e.last = ((last>0) ? 1 : 0); // 1 when last
	return e;
}

//https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/Non-Blocking-Read
//https://support.xilinx.com/s/question/0D52E00006hprWiSAI/how-to-properly-dataflow-functions-in-hls?language=en_US
static void load_input(AXI_T* in, STREAM_FLOAT& inStream, int size) {
mem_rd1234:
    for (int i = 0; i < size; i++) {
#pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
#pragma HLS PIPELINE II=1
        inStream.write(pop_stream(in[i]));
    }
}

static void compute_add(STREAM_FLOAT& in1_stream,
                        STREAM_FLOAT& in2_stream,
                        STREAM_FLOAT& out_stream,
                        int size) {
// The kernel is operating with vector of NUM_WORDS integers. The + operator performs
// an element-wise add, resulting in NUM_WORDS parallel additions.
	//float in1buf, in2buf;
execute:
    for (int i = 0; i < size; i++) {
#pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
#pragma HLS PIPELINE II=1
    	//in1_stream.read(in1buf);
    	//in2_stream.read(in2buf);
        //out_stream.write(in1buf + in2buf);
        out_stream.write(in1_stream.read() + in2_stream.read());
    }
}

static void compute_mul(STREAM_FLOAT& in1_stream,
                        STREAM_FLOAT& out_stream,
						float a,
                        int size) {
// The kernel is operating with vector of NUM_WORDS integers. The + operator performs
// an element-wise add, resulting in NUM_WORDS parallel additions.
	//float in1buf, abuf;
	//abuf = a;
execute:
    for (int i = 0; i < size; i++) {
#pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
#pragma HLS PIPELINE II=1
    	//in1_stream.read(in1buf);
        //out_stream.write(in1buf*abuf);
    	out_stream.write(in1_stream.read()*a);
    }
}


static void store_result(AXI_T* out, STREAM_FLOAT& out_stream, int size) {
	//float ostream;
mem_wr:
    for (int i = 0; i < size; i++) {
#pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
#pragma HLS PIPELINE II=1
    	//out_stream.read(ostream);
        //out[i] = push_stream(ostream, (i == size - 1 ? 1 : 0));
    	out[i] = push_stream(out_stream.read(),(i == size - 1 ? 1 : 0));
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
void krnl_saxpy(AXI_T* X, AXI_T* Y, AXI_T* Z, float a, int size) {
#pragma HLS INTERFACE axis port = X depth=4096
#pragma HLS INTERFACE axis port = Y depth=4096
#pragma HLS INTERFACE axis port = Z depth=4096
#pragma HLS INTERFACE s_axilite port = size
#pragma HLS INTERFACE s_axilite port = a

//#pragma HLS INTERFACE ap_ctrl_none port=return


    static STREAM_FLOAT X_stream("input_stream_X");
    static STREAM_FLOAT Y_stream("input_stream_Y");
    static STREAM_FLOAT AX_stream("input_stream_AX");
    static STREAM_FLOAT Z_stream("Z_stream");


#pragma HLS dataflow

    // dataflow pragma instruct compiler to run following three APIs in parallel
    load_input(X, X_stream, size);
    load_input(Y, Y_stream, size);
    //float abuf = a;
    compute_mul(X_stream, AX_stream, a, size);
    compute_add(AX_stream,Y_stream,Z_stream,size);
    store_result(Z, Z_stream, size);
}

