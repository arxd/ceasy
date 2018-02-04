#include <stdio.h>

int pow(int base, int power)
{ // base = 10,   power=0
	//  +-*/%
	int answer = 1;//base*base*base*base...;
	
	while (power > 0 )
	{
		answer = answer*base;
		power = power-1;
	}
	
	return answer;
}


void put_number(int x, int num_digits, int base)
{
	//~ putchar(x%100000/10000 + 48); // 1000's
	//~ putchar(x%10000/1000 + 48); // 100's
	//~ putchar(x%1000/100 + 48); // 100's
	//~ putchar(x%100/10 + 48); // 10's
	//~ putchar(x%10/1 + 48); // 1's
	//~ stop!!!! mom == 1
		
	int mom =  pow(base, num_digits) ;
	while (mom >=base)
	{
		putchar(x%mom/(mom/base) + '0'); 
		mom = mom/base;
	}
	putchar('\n'); 
}


int main(int argc, char *argv[])
{	
	put_number(23, 8, 10);
	put_number(50, 8, 7);
	put_number(255, 8, 2);
	put_number(1999, 8, 9);
	put_number(0, 1, 100);
	return 0;
}




		//~ char bob[] = "ABCD";

	//~ char bob[4] = "9BC";
	//~ char bob[4] = {'9', 'B', 'C', 0};
	//~ char bob[4] = {58, 0x41, 66, 0};
	//~ bob[0] = 58;
	//~ bob[1] = 65;
	//~ bob[2] = 66;
	//~ bob[3] = 0;
	
	//~ char aaron = 100;
	//~ char mei[4] = {1,2,3,4};
	
	//~ mei[1] = 4;
