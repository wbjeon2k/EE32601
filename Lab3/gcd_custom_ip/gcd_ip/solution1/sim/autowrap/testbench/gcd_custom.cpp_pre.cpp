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
