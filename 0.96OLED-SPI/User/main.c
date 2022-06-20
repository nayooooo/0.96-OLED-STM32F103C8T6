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
	
	OLED_ShowBMP(0, 0, BMP0, 128, 64, FILL);
	OLED_Refresh_Gram();
	
	LED = LED_ON;
	
	while(1)
	{
//		Demo_01();
	}
}
