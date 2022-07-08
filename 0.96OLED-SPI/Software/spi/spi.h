/**
 * @file spi.h
 * @author YEWANhub
 * @brief ʵ�����SPI
 * @version V1.0.0
 * @date 2022-06-10
 * @encoding GB2312
 * @details ��Ҫ�޸�OLED�Ľ��߷�ʽ�����޸Ĵ��ļ��е�SPI�˿ڶ���
 */
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

#define SPI_SCLK_Clr()	GPIO_ResetBits(GPIOA,GPIO_Pin_5)	// CLK���
#define SPI_SCLK_Set()	GPIO_SetBits(GPIOA,GPIO_Pin_5)		// CLK��λ
#define SPI_SCLK_Pin	GPIO_Pin_5							// CLK����

#define SPI_SDIN_Clr()	GPIO_ResetBits(GPIOA,GPIO_Pin_7)	// DIN���
#define SPI_SDIN_Set()	GPIO_SetBits(GPIOA,GPIO_Pin_7)		// DIN��λ
#define SPI_SDIN_Pin	GPIO_Pin_7							// DIN����

#define SPI_RST_Clr()	GPIO_ResetBits(GPIOB,GPIO_Pin_0)	// RES���
#define SPI_RST_Set()	GPIO_SetBits(GPIOB,GPIO_Pin_0)		// RES��λ
#define SPI_RST_Pin		GPIO_Pin_0							// RES����

#define SPI_DC_Clr()	GPIO_ResetBits(GPIOB,GPIO_Pin_1)	// DC���
#define SPI_DC_Set()	GPIO_SetBits(GPIOB,GPIO_Pin_1)		// DC��λ
#define SPI_DC_Pin		GPIO_Pin_1							// DC����

#define SPI_CS_Clr()	GPIO_ResetBits(GPIOA,GPIO_Pin_4)	// CS���
#define SPI_CS_Set()	GPIO_SetBits(GPIOA,GPIO_Pin_4)		// CS��λ
#define SPI_CS_Pin		GPIO_Pin_4							// CS����

/* ---------------- SPI�˿ڶ��� ---------------- */

void SPI_GPIO_Init(void);
void SPI_WR_Byte(u8 dat, u8 cmd);

#endif
