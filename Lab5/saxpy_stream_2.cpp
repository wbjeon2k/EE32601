
#include "vadd_dma.h"
#define DATA_SIZE 4096

// TRIPCOUNT identifier
const int c_size = DATA_SIZE;

float pop_stream(AXI_T const &e) // type casting AXI_T -> int
{
#pragma HLS INLINE

    union {
        float f;
        unsigned int u;
    } conv;

    conv tmp;
    tmp.u = e.data;

    float ret = tmp.f;
    volatile ap_uint<4> keep = e.keep; // -1 : 1111
	volatile ap_uint<1> last = e.last; // only asserted at the last element
	return ret;
}

AXI_T push_stream(float const ret, int last) // type casting AXI_T -> int
{
#pragma HLS INLINE
    union {
        float f;
        unsigned int u;
    } conv;

    conv tmp;
    tmp.f = ret;

    AXI_T e;
    e.data = tmp.u;
    e.keep = -1;           // 1111
    e.last = last ? 1 : 0; // 1 when last
	return e;
}



static void load_input(AXI_T* in, STREAM_INT& inStream, int size) {
mem_rd:
    for (int i = 0; i < size; i++) {
#pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
#pragma HLS PIPELINE II=1
        inStream.write(pop_stream(in[i]);
    }
}

static void compute_mul(STREAM_INT& x_stream,
                        STREAM_INT& ax_stream,
                        float a,
                        int size) {
// The kernel is operating with vector of NUM_WORDS integers. The + operator performs
// an element-wise add, resulting in NUM_WORDS parallel additions.
execute:
    for (int i = 0; i < size; i++) {
#pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
#pragma HLS PIPELINE II=1
        ax_stream.write(x_stream.read()*a);
    }
}

static void compute_add(STREAM_INT & ax_stream,
                        STREAM_INT & y_stream,
                        STREAM_INT & z_stream,
                        int size) {
// The kernel is operating with vector of NUM_WORDS integers. The + operator performs
// an element-wise add, resulting in NUM_WORDS parallel additions.
execute:
    for (int i = 0; i < size; i++) {
#pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
#pragma HLS PIPELINE II = 1
        out_stream.write(ax_stream.read() + y_stream.read());
    }
}

static void store_result(AXI_T* out, STREAM_INT& out_stream, int size) {
mem_wr:
    for (int i = 0; i < size; i++) {
#pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
#pragma HLS PIPELINE II=1
        out[i] = push_stream(out_stream.read(), i == size - 1 ? 1 : 0);
    }
}

void krnl_vadd(AXI_T* in1, AXI_T* in2, AXI_T* out, float a, int size) {
#pragma HLS INTERFACE axis port = in1 depth=4096
#pragma HLS INTERFACE axis port = in2 depth = 4096
#pragma HLS INTERFACE axis port = out depth = 4096
#pragma HLS INTERFACE s_axilite port = size
#pragma HLS INTERFACE s_axilite port = a

#pragma HLS INTERFACE ap_ctrl_none port=return


    static STREAM_INT x_stream("input_stream_x");
    static STREAM_INT y_stream("input_stream_y");
    static STREAM_INT z_stream("z_stream");
    static STREAM_INT ax_stream("input_stream_ax")

#pragma HLS dataflow
    // dataflow pragma instruct compiler to run following three APIs in parallel
    load_input(in1, in1_stream, size);
    load_input(in2, in2_stream, size);
    compute_mul(x_stream, ax_stream, a, size);
    compute_add(ax_stream, y_stream, z_stream, size);
    store_result(out, out_stream, size);
}

