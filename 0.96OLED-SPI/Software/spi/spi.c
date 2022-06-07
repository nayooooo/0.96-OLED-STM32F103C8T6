/******************************************************************************
	˵��: 
	----------------------------------------------------------------
	GND    ��Դ��
	VCC  ��5V��3.3v��Դ
	D0   ��PA5��SCL��
	D1   ��PA7��SDA��
	RES  ��PB0
	DC   ��PB1
	CS   ��PA4               
	----------------------------------------------------------------
******************************************************************************/
#include "sys.h"

void My_SPI_Init(void)
{
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
}
