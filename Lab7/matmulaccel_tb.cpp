#define SZ 64

#include <stdio.h>
#include <iostream>
#include <string.h>
#include "matmulaccel.h"

union conv {
	double f;
	unsigned long long u;
};

int main(){

	int a[SZ][SZ*2];
	int b[SZ*2][SZ];
	//float c[SZ][SZ];
	int c_sw[SZ][SZ];

	AXI_F axi_a [SZ*SZ*2];
	AXI_F axi_b [SZ*SZ*2];
	AXI_INT8 axi_c [SZ*SZ];

	for(int i=0;i<SZ;++i){
		for(int j=0;j<2*SZ;++j){
			int idx = i*SZ*2 + j;
			conv tmp;
			tmp.f = 1.0;
			axi_a[idx].data = tmp.u;
			a[i][j] = 1;
		}
	}

	for(int i=0;i<2*SZ;++i){
		for(int j=0;j<SZ;++j){
			int idx = i*SZ + j;
			conv tmp;
			tmp.f = -1.0;
			axi_b[idx].data = tmp.u;
			b[i][j] = -1;
		}
	}

	for(int k=0;k<SZ*2;++k){
		for(int i=0;i<SZ;++i){
			float tmp = a[i][k];
			for(int j=0;j<SZ;++j){
				c_sw[i][j] += tmp*b[k][j];
			}
		}
	}

	matmul_accel(axi_a,axi_b,axi_c);

	int ret=0;
	for(int i=0;i<SZ;++i){
		for(int j=0;j<SZ;++j){
			int idx = i*SZ + j;

			if(double(axi_c[idx].data) != c_sw[i][j]){
				ret=1;
				break;
			}
		}
	}

	return ret;

}
