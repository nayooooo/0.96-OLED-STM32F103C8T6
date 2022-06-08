#include "sys.h"
#include <stdlib.h>
#include "bmp.h"

#define X_TEST 43
#define Y_TEST 61

int main()
{
	delay_init();
	LED_Init();
	OLED_SPI_Init();
	
	OLED_Clear();
	
//	OLED_Draw_Two_Vertical_Ellipses(63, 31, 30, 20, FILL); OLED_Refresh_Gram();
//	OLED_Draw_Two_Vertical_Ellipses_Rotate(63, 31, 30, 20, 0, FILL); OLED_Refresh_Gram();
//	OLED_Draw_Rounded_Cube(63, 31, 60, 30, 30, FILL);
//	OLED_Refresh_Gram();
	OLED_Show_DNA(63, 31, 60, 30, FILL);
	LED = LED_ON;
	
	while(1)
	{
//		OLED_Show_Rotating_Ellipse(63, 31, 30, 20, FILL);
//		OLED_Show_Rotating_Two_Vertical_Ellipses(63, 31, 30, 20, FILL);
//		OLED_Show_Rotating_Telescoping_Two_Vertical_Ellipses(63, 31, 30, 20, FILL);
//		OLED_Show_Cube_to_LikeCircle(63, 31, 60, 30, FILL);
		OLED_Show_DNA(63, 31, 60, 30, FILL);
	}
}
