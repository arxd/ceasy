#include <stdio.h>


void put_number(int x)
{ // x = 567
	int bob = 1;
	int mom = 1; 1000
	
	while (bob != 0)
	{
		mom = mom * 10;
		putchar ( bob%
		bob = x/10;
	}
	
	
	while (mom >= 10)
	{
		putchar(x%mom/(mom/10) + '0'); 
		mom = mom/10;
	}
	putchar('\n'); 
}

int main(int argc, char *argv[])
{	
	say_hello();
	put_number(43, 10000);
	//~ put_number(50);
	//~ put_number(255);
	//~ put_number(1999);
	//~ put_number(0);
	return 0;
}

	//~ putchar(x%100000/10000 + 48); // 10000's
	//~ putchar(x%10000/1000 + 48); // 1000's
	//~ putchar(x%1000/100 + 48); // 100's
	//~ putchar(x%100/10 + 48); // 10's
	//~ putchar(x%10/1 + 48); // 1's
	//~ stop!!!! mom == 1
		



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
