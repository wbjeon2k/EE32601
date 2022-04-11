#pragma once

#include <stdint.h>
#include <ap_axi_sdata.h>
#include <hls_stream.h>


typedef ap_axiu<32, 1, 1,1> AXI_T;
typedef hls::stream<int> STREAM_INT;

void krnl_vadd(AXI_T* in1, AXI_T* in2, AXI_T* out, int size);
