void gcd_custom(int a,int b,int& gcd){

#pragma HLS INTERFACE ap_ctrl_none port=return

#pragma HLS INTERFACE s_axilite port=a
#pragma HLS INTERFACE s_axilite port=b
#pragma HLS INTERFACE s_axilite port=gcd

	int tmp;
	if(b>a){
		//swap a, b
		tmp = a;
		a = b;
		b = tmp;
	}

	//nonrecursive gcd
	while(b){
		tmp = b;
		b = a%b;
		a = tmp;
	}

	gcd = a;
}
