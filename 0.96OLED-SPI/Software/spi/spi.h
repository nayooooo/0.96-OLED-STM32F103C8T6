/**
 * @file spi.h
 * @author YEWANhub
 * @brief 实现软件SPI
 * @version V1.0.0
 * @date 2022-06-10
 * @encoding GB2312
 * @details 若要修改OLED的接线方式，请修改此文件中的SPI端口定义
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
#ifndef __SPI_H
#define __SPI_H

#include "sys.h"

/* ---------------- SPI端口定义 ---------------- */

#define SPI_SCLK_Clr()	GPIO_ResetBits(GPIOA,GPIO_Pin_5)	// CLK清除
#define SPI_SCLK_Set()	GPIO_SetBits(GPIOA,GPIO_Pin_5)		// CLK置位
#define SPI_SCLK_Pin	GPIO_Pin_5							// CLK引脚

#define SPI_SDIN_Clr()	GPIO_ResetBits(GPIOA,GPIO_Pin_7)	// DIN清除
#define SPI_SDIN_Set()	GPIO_SetBits(GPIOA,GPIO_Pin_7)		// DIN置位
#define SPI_SDIN_Pin	GPIO_Pin_7							// DIN引脚

#define SPI_RST_Clr()	GPIO_ResetBits(GPIOB,GPIO_Pin_0)	// RES清除
#define SPI_RST_Set()	GPIO_SetBits(GPIOB,GPIO_Pin_0)		// RES置位
#define SPI_RST_Pin		GPIO_Pin_0							// RES引脚

#define SPI_DC_Clr()	GPIO_ResetBits(GPIOB,GPIO_Pin_1)	// DC清除
#define SPI_DC_Set()	GPIO_SetBits(GPIOB,GPIO_Pin_1)		// DC置位
#define SPI_DC_Pin		GPIO_Pin_1							// DC引脚

#define SPI_CS_Clr()	GPIO_ResetBits(GPIOA,GPIO_Pin_4)	// CS清除
#define SPI_CS_Set()	GPIO_SetBits(GPIOA,GPIO_Pin_4)		// CS置位
#define SPI_CS_Pin		GPIO_Pin_4							// CS引脚

/* ---------------- SPI端口定义 ---------------- */

void SPI_GPIO_Init(void);
void SPI_WR_Byte(u8 dat, u8 cmd);

#endif
