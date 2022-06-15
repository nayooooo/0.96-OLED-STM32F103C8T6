#include "sys.h"
#include <stdlib.h>
#include "bmp.h"

#define X_TEST 43
#define Y_TEST 61

int main()
{
	delay_init();
	LED_Init();
	OLED_Init();
	
	OLED_Clear();
	
	LED = LED_ON;
	
	while(1)
	{
		Demo_01();
	}
}
