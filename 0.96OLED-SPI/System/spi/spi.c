/**
 * @file spi.c
 * @author YEWANhub
 * @brief 实现硬件SPI
 * @version V1.0.0
 * @date 2022-06-10
 * @encoding GB2312
 */
/******************************************************************************
	说明: 
	----------------------------------------------------------------
	GND    电源地
	VCC  接5V或3.3v电源
	D0   接PA5（SCL）
	D1   接PA7（SDA）
	RES  接PB0
	DC   接PB1
	CS   接PA4
	----------------------------------------------------------------
******************************************************************************/
#include "sys.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_spi.h"
#include "spi.h"

/**
 * @fn SPI_GPIO_Init
 * @brief 初始化SPI1
 * @return None
 */
void SPI_GPIO_Init(void)
{
#if USE_SPI_MODE == 0
	GPIO_InitTypeDef  GPIO_InitStructure;
 	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = SPI_CS_Pin|SPI_SCLK_Pin|SPI_SDIN_Pin;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOA,SPI_SCLK_Pin|SPI_SDIN_Pin|SPI_CS_Pin);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = SPI_RST_Pin|SPI_DC_Pin;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOB,SPI_RST_Pin|SPI_DC_Pin);
#elif USE_SPI_MODE == 1
	GPIO_InitTypeDef  GPIO_InitStructure;
	SPI_InitTypeDef SPI_InitStructure;
 	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_SPI1|\
							RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);

	// SCL、SDA
	GPIO_InitStructure.GPIO_Pin = SPI_SCLK_Pin|SPI_SDIN_Pin;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// CS
	GPIO_InitStructure.GPIO_Pin = SPI_CS_Pin;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOA,SPI_CS_Pin);

	//RES、DC
	GPIO_InitStructure.GPIO_Pin = SPI_RST_Pin|SPI_DC_Pin;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOB,SPI_RST_Pin|SPI_DC_Pin);

	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;  // 2分频
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;  // 在第一个跳变沿采集数据
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;  // 串行同步时钟空闲状态为低电平
	SPI_InitStructure.SPI_CRCPolynomial = 7;  // CRC计算的多项式
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;  // SPI发送接收8位帧结构
	SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;  // SPI单线只发送
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;  // MSB先发送
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;  // SPI主机模式
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;  // 软件控制片选（CS）
	SPI_Init(SPI1, &SPI_InitStructure);
#if USE_DMA
	SPI1->CR2 = 1 << 1;  // 允许DMA往缓冲区内发送数据
#endif
	SPI_Cmd(SPI1, ENABLE);
#endif
}

/**
 * @brief 主机向从机（SSD1306）写入一个字节
 * @param dat 要写入的数据/命令
 * @param cmd 数据/命令标志 0,表示命令;1,表示数据
 * @return None
 */
void SPI_WR_Byte(u8 dat, u8 cmd)
{
	if(cmd) SPI_DC_Set();
	else SPI_DC_Clr();
	SPI_CS_Clr();
#if USE_SPI_MODE == 0
	u8 i;
	for(i=0;i<8;i++)
	{
		SPI_SCLK_Clr();
		if(dat&0x80) SPI_SDIN_Set();
		else SPI_SDIN_Clr();
		SPI_SCLK_Set();
		dat<<=1;   
	}
#elif USE_SPI_MODE == 1
	u8 retry = 0;
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET)
	{
		retry++;
		if(retry>200) return;
	}
	SPI_I2S_SendData(SPI1, dat);
	retry = 0;
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE)==RESET)
	{
		retry++;
		if(retry>200) return;
	}
#endif
	SPI_CS_Set();
	SPI_DC_Set();
}
