#include "sys.h"
#include "led.h"
#include "delay.h"
#include "usart.h"
#include "oled.h"
#include "bmp.h"
#include "screen.h"
#include "malloc.h"
#include "MMC_SD.h"
#include "MMC_SD_User.h"

int main()
{
	uint8_t i;
	
	delay_init();
	LED_Init();
	uart_init(115200);
	OLED_Init();
	mem_init();
	
	MMC_SD_Initializing_Screen();
	
	while(1)
	{
		for(i=0; i<VIDEO_FRAMES; i++)
		{
			SD_ReadDisk(USART_RX_BUF,0+i*2,2);
			OLED_ShowBMP(0, 0, USART_RX_BUF, 128, 64, FILL); OLED_Refresh_Gram();
			delay_ms(120);
		}
	}
}
