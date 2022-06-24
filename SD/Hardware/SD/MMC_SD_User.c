#include "sys.h"
#include "MMC_SD_User.h"
#include "MMC_SD.h"
#include "delay.h"
#include "led.h"
#include "oled.h"

#include "usart.h"

uint16_t Written_Size = 0;

/* -------------------- MMC_SD and OLED -------------------- */

void MMC_SD_Initializing_Screen(void)
{
	u8 r1;      // 存放SD卡的返回值
	MMC_SD_State_Typedef MMC_SD_State = {
		.MMC_SDCard_Exists = 0X02  // 允许循环提示
	};
	
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
}

/* -------------------- SD and OLED -------------------- */
