#include <stdio.h>
#include <math.h>
#include "gcd_custom.h"

int main(){
	int a,b,gcd;

	a = 2310;
	b = 99;

	gcd_custom(a,b,gcd);

	printf("%d\n", gcd);

	return (gcd != 33);
}
