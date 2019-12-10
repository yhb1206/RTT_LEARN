#include "ARMCM3.h"                     // Device header

 uint32_t flag1;
 uint32_t flag2;

 
 void delay( uint32_t count )
 {
 for (; count!=0; count--);
 }

int main(void)
 {
	for (;;) 
	{
		flag1 = 1;
		delay( 100 );
		flag1 = 0;
		delay( 100 );

		flag2 = 1;
		delay( 100 );
		flag2 = 0;
		delay( 100 );
	}
}

