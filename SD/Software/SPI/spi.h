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
#ifndef __SPI_H
#define __SPI_H

#include "sys.h"

/* ---------------- SPI�˿ڶ��� ---------------- */

#define SPI_SCLK_Clr()	GPIO_ResetBits(GPIOA,GPIO_Pin_5)//CLK
#define SPI_SCLK_Set()	GPIO_SetBits(GPIOA,GPIO_Pin_5)
#define SPI_SCLK_Pin	GPIO_Pin_5

#define SPI_SDIN_Clr()	GPIO_ResetBits(GPIOA,GPIO_Pin_7)//DIN
#define SPI_SDIN_Set()	GPIO_SetBits(GPIOA,GPIO_Pin_7)
#define SPI_SDIN_Pin	GPIO_Pin_7

#define SPI_RST_Clr()	GPIO_ResetBits(GPIOB,GPIO_Pin_0)//RES
#define SPI_RST_Set()	GPIO_SetBits(GPIOB,GPIO_Pin_0)
#define SPI_RST_Pin		GPIO_Pin_0

#define SPI_DC_Clr()	GPIO_ResetBits(GPIOB,GPIO_Pin_1)//DC
#define SPI_DC_Set()	GPIO_SetBits(GPIOB,GPIO_Pin_1)
#define SPI_DC_Pin		GPIO_Pin_1

#define SPI_CS_Clr()	GPIO_ResetBits(GPIOA,GPIO_Pin_4)//CS
#define SPI_CS_Set()	GPIO_SetBits(GPIOA,GPIO_Pin_4)
#define SPI_CS_Pin		GPIO_Pin_4

#define SPI_METHOD			SPI2
#define SPI_GPIO_PORT		GPIOB
#define SPI_GPIO_CLK		RCC_APB2Periph_GPIOB
#define SPI_METHOD_CLK		RCC_APB1Periph_SPI2
#define SPI_SCK_GPIO_PIN	GPIO_Pin_13
#define SPI_MISO_GPIO_PIN	GPIO_Pin_14
#define SPI_MOSI_GPIO_PIN	GPIO_Pin_15

/* ---------------- SPI�˿ڶ��� ---------------- */

void SPI_GPIO_Init(void);
void SPI_WR_Byte(u8 dat, u8 cmd);

void SPI2_Init(void);			 //��ʼ��SPI��
void SPI2_SetSpeed(u8 SpeedSet); //����SPI�ٶ�
u8 SPI2_ReadWriteByte(u8 TxData);//SPI���߶�дһ���ֽ�

#endif
