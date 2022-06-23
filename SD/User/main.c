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
	u8 r1;      // 存放SD卡的返回值
	MMC_SD_State_Typedef MMC_SD_State = {
		.MMC_SDCard_Exists = 0X02  // 允许循环提示
	};
	uint8_t buf[1024];
	
	delay_init();
	LED_Init();
	uart_init(115200);
	OLED_Init();
	mem_init();
	
	LED = LED_ON;
	OLED_Clear();
	OLED_ShowString(0, 0, "SD Card initializing...", FontSize_Medium, FILL); OLED_Refresh_Gram();
	delay_ms(1000);
	while(r1 = SD_Initialize())  // 检测不到 SD 卡
	{
		if(r1 == No_MMC_SDCard && (MMC_SD_State.MMC_SDCard_Exists & 0X02))
		{
			MMC_SD_State.MMC_SDCard_Exists &= ~0X02;  // 禁止循环提示
			OLED_Clear();
			OLED_ShowString(0, 0, "No SD Card.", FontSize_Medium, FILL);
			OLED_ShowString(0, 0 + FontSize_Medium, "Please insert a MMC_SD Card...", FontSize_Medium, FILL);
			OLED_Refresh_Gram();
		}
		delay_ms(500);
	}
	MMC_SD_State.MMC_SDCard_Exists |= 0X01;
	
	OLED_Clear();
	OLED_ShowString(0, 0, "SD Card is ready!", FontSize_Medium, FILL); OLED_Refresh_Gram();
	delay_ms(1000);
	OLED_Clear();
	LED = LED_OFF;
	
//	OLED_Intelligent_ShowNum(0,0,sd_size>>11,24,FILL);
//	OLED_ShowString(0,31,"Test ",24,FILL); OLED_Refresh_Gram();
	
	while(SD_WriteDisk(BMP0,0X0000,2));
	while(SD_ReadDisk(buf, 0X0000, 2));
	OLED_ShowBMP(0, 0, buf, 128, 64, FILL); OLED_Refresh_Gram();
	
	while(1)
	{
		LED = LED_ON;
		delay_ms(500);
		LED = LED_OFF;
		delay_ms(500);
	}
}
