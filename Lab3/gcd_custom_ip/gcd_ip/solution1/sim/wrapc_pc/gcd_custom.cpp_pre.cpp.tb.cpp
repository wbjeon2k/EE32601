// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2020.1.1 (64-bit)
// Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
// ==============================================================
# 1 "C:/Xilinx/Vitis/2020.1/gcd_custom_ip/gcd_custom.cpp"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "C:/Xilinx/Vitis/2020.1/gcd_custom_ip/gcd_custom.cpp"
void gcd_custom(int a,int b,int& gcd){



#pragma HLS INTERFACE s_axilite port=a
#pragma HLS INTERFACE s_axilite port=b
#pragma HLS INTERFACE s_axilite port=gcd

 int tmp;
 if(b>a){

  tmp = a;
  a = b;
  b = tmp;
 }


 while(b){
  tmp = b;
  b = a%b;
  a = tmp;
 }

 gcd = a;
}
#ifndef HLS_FASTSIM
#ifdef __cplusplus
extern "C"
#endif
void apatb_gcd_custom_hw(int, int, int &);
#ifdef __cplusplus
extern "C"
#endif
void gcd_custom_hw_stub(int a, int b, int &gcd){
gcd_custom(a, b, gcd);
return ;
}
#ifdef __cplusplus
extern "C"
#endif
void apatb_gcd_custom_sw(int a, int b, int &gcd){
apatb_gcd_custom_hw(a, b, gcd);
return ;
}
#endif
# 25 "C:/Xilinx/Vitis/2020.1/gcd_custom_ip/gcd_custom.cpp"

