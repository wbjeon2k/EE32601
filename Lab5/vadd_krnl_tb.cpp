/*
#include <stdio.h>
#define SIZE 4096

#include "vadd_krnl.h"

int main(){
	int a[SIZE];
	int b[SIZE];
	int c[SIZE];

	for(int i=0;i<SIZE;++i){
		a[i] = i;
		b[i] = i*2;
	}

	int sz = SIZE;
	krnl_vadd(a,b,c,sz);

	int ret=0;
	for(int i=0;i<SIZE;++i){
		if(c[i]!=a[i]+b[i]){
			ret=1;
			break;
		}
	}

	return ret;
}
*/

#define SZ 4096

#include <stdio.h>
#include "vadd_krnl.h"

int main(){
	/*
	float a[SZ];
	float b[SZ];
	float c[SZ];
	float d = 1.9;

	for(int i=0;i<SZ;++i){
		a[i] = 1.0*(i*1.0);
		b[i] = 2.0*(i*2.0);
	}

	krnl_saxpy(a,b,c,int(SZ));

	int ret=0;
	for(int i=0;i<SZ;++i){
		if(c[i]!=(a[i]+b[i])){
			ret=1;
			break;
		}
	}

	return ret;
	*/

	AXI_T a[SZ];
	AXI_T b[SZ];
	AXI_T c[SZ];
	//AXI_T d[SZ];

	for(int i=0;i<SZ;++i){
		a[i].data = 1;
		a[i].keep = -1;
		a[i].last = (i == SZ - 1 ? 1 : 0);

		b[i].data = 1;
		b[i].keep = -1;
		b[i].last = (i == SZ - 1 ? 1 : 0);

		//d[i].data = 1.0;
		//d[i].keep = -1;
		//d[i].last = (i == SZ - 1 ? 1 : 0);
	}

	krnl_vadd(a,b,c,int(SZ));

	int ret=0;
	for(int i=0;i<SZ;++i){
		if(c[i].data!=(a[i].data +b[i].data)){
			ret=1;
			break;
		}
	}

}

