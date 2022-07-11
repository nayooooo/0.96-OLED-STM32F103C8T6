/**
 * @file oled.h
 * @author YEWANhub
 * @brief 实现OLED的驱动和一些基础图形的绘制
 * @version V1.0.0
 * @date 2022-06-10
 * @encoding GB2312
 * @details 1. 在绘制旋转图形时，角度参数是角度制
 * @details 2. 若要修改OLED的接线方式，请修改spi.h中的SPI端口定义
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
#ifndef __OLED_H
#define __OLED_H			  	 
#include "sys.h"
#include "stdlib.h"

#define SLAVE_ADDRESS 0x78  /* 从机地址 */

#define OLED_COL_MAX (128)				/* OLED列数 */
#define OLED_ROW_MAX (64)				/* OLED行数 */
#define OLED_PAGE_MAX (OLED_ROW_MAX/8)	/* OLED页数 */
#define OLED_ROW_COL_MAX ((OLED_COL_MAX>OLED_ROW_MAX)?(OLED_COL_MAX):(OLED_ROW_MAX))  /* OLED行、列数中的最大值 */
#define OLED_ROW_COL_MIN ((OLED_COL_MAX<OLED_ROW_MAX)?(OLED_COL_MAX):(OLED_ROW_MAX))  /* OLED行、列数中的最小值 */
		     
#define OLED_CMD  0  /* OLED写指令命令 */
#define OLED_DATA 1  /* OLED写数据命令 */

#define FILL 1		/* 填充1 */
#define CLEAR 0		/* 填充0 */

/* ---------------- OLED端口定义 ---------------- */

#define OLED_SCLK_Clr()		SPI_SCLK_Clr()	// CLK清除
#define OLED_SCLK_Set()		SPI_SCLK_Set()	// CLK置位
#define OLED_SCLK_Pin		SPI_SCLK_Pin	// CLK引脚

#define OLED_SDIN_Clr()		SPI_SDIN_Clr()	// DIN清除
#define OLED_SDIN_Set()		SPI_SDIN_Set()	// DIN置位
#define OLED_SDIN_Pin		SPI_SDIN_Pin	// DIN引脚

#define OLED_RST_Clr()		SPI_RST_Clr()	// RSE清除
#define OLED_RST_Set()		SPI_RST_Set()	// RSE置位
#define OLED_RST_Pin		SPI_RST_Pin		// RSE引脚

#define OLED_DC_Clr()		SPI_DC_Clr()	// DC清除
#define OLED_DC_Set()		SPI_DC_Set()	// DC置位
#define OLED_DC_Pin			SPI_DC_Pin		// DC引脚
 		                
#define OLED_CS_Clr()		SPI_CS_Clr()	// CS清除
#define OLED_CS_Set()		SPI_CS_Set()	// CS置位
#define OLED_CS_Pin			SPI_CS_Pin		// CS引脚

/* ---------------- OLED端口定义 ---------------- */

/* ---------------- 点结构体定义 ---------------- */

typedef struct
{
	uint8_t x;
	uint8_t y;
}Point;

typedef struct
{
	int8_t x;
	int8_t y;
}Point_Signed;

typedef struct
{
	uint16_t x;
	uint16_t y;
}Point_uint16_t;

typedef struct
{
	int16_t x;
	int16_t y;
}Point_int16_t;

typedef struct
{
	float x;
	float y;
}Point_Float;

typedef struct
{
	uint8_t x0;
	uint8_t y0;
	uint8_t r;
}Circle;

/* ---------------- 点结构体定义 ---------------- */

/* ---------------- 函数实现方法定义 ---------------- */
#define REFRESH_START_POS_COL 0  // 刷新时的起始坐标的列坐标
#define REFRESH_START_POS_PAGE 0  // 刷新时的起始坐标的页坐标

// 选择刷新函数的实现方式（修改模式前请先修改OLED初始化配置）
// 0->水平地址模式下的刷新函数
// 1->垂直地址模式下的刷新函数
// 2->垂直地址模式下的DMA刷新函数
#define REFRESH_MODE 1
// 如果您使用了OLED_SetPos，为预防显示错位请将下方的宏定义为1
#define REFRESH_DMA_PREVENT_DISLOCATION_MODE 0

#define SHOW_EVERY_STEEP 0  // 每变化一个点都将调用一次OLED_Refresh_Gram函数
#define OLED_DRAW_LINE_BRESENHAM 1  // 使用brasenham方法画线，1，使用；0，禁用
#define OLED_DRAW_ELLIPSE_BRESENHAM 1  // 使用brasenham方法画椭圆，1，使用；0，禁用
/* ---------------- 函数实现方法定义 ---------------- */

//OLED控制用函数
u32 mypow(u8 m,u8 n);
static void OLED_WR_Byte(u8 dat,u8 cmd);
static void OLED_SetPos(u8 col, u8 page);
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Refresh_Gram(void);
static void OLED_GPIO_Init(void);
void OLED_Init(void);
void OLED_Clear(void);

void OLED_DrawPoint(u8 x,u8 y,u8 mode);
int8_t OLED_ReadPoint(u8 x,u8 y);
//bool OLED_ReadPoint_by_CMD(u8 x, u8 y);

void OLED_DrawLine(u8 x1,u8 y1,u8 x2,u8 y2, u8 mode);
void OLED_DrawLine_Angle(u8 x0, u8 y0, u8 lenght, u16 angle, u8 mode);
void OLED_DrawRectangle(u8 x1,u8 y1,u8 x2,u8 y2, u8 mode);
void OLED_Draw_Filled_Rectangle(u8 x1,u8 y1,u8 x2,u8 y2, u8 mode);
void OLED_DrawRectangle_Intelligent_Overflow(u8 x0, u8 y0, u8 a, u8 b, u8 mode);
void OLED_Draw_Rounded_Rectangle(u8 x0, u8 y0, u8 a, u8 b, u8 r, u8 mode);
void OLED_Draw_Rounded_Rectangle_Erasure_Method(u8 x0, u8 y0, u8 a, u8 b, u8 r, u8 mode);
void OLED_Draw_Rounded_Rectangle_Fillet_Overflow(u8 x0, u8 y0, u8 a, u8 b, u8 r, u8 mode);
void OLED_Draw_Rounded_Rectangle_Fillet_Overflow_Erasure_Method(u8 x0, u8 y0, u8 a, u8 b, u8 r, u8 mode);

void OLED_Draw_4_Pixels(u8 x0, u8 y0, u8 x, u8 y, u8 mode);
void OLED_Draw_4_Pixels_Lines(u8 x0, u8 y0, u8 x, u8 y, u8 mode);
void OLED_Draw_4_Pixels_Rotate(u8 x0, u8 y0, int16_t x, int16_t y, u16 angle, u8 mode);
void OLED_Draw_4_Pixels_Rotate_Lines(u8 x0, u8 y0, int16_t x, int16_t y, u16 angle, u8 mode);
void OLED_Draw_4_Pixels_Spread_Out_From_Center(u8 x0, u8 y0, u8 x, u8 y, u8 dx, u8 dy, u8 mode);
void OLED_Draw_8_Pixels(u8 x0, u8 y0, u8 x, u8 y, u8 mode);
void OLED_Draw_8_Pixels_Rotate(u8 x0, u8 y0, int16_t x, int16_t y, u16 angle, u8 mode);
void OLED_Draw_8_Pixels_Spread_Out_From_Center(u8 x0, u8 y0, u8 x, u8 y, u8 dx, u8 dy, u8 mode);
void OLED_Draw_8_Pixels_Lines(u8 x0, u8 y0, u8 x, u8 y, u8 mode);

void OLED_DrawCircle(u8 x0, u8 y0, u8 r, u8 mode);
void OLED_Draw_Filled_Circle(u8 x0, u8 y0, u8 r, u8 mode);
void OLED_DrawEllipse(u8 x0, u8 y0, u8 a, u8 b, u8 mode);
void OLED_Draw_Filled_Ellipse(u8 x0, u8 y0, u8 a, u8 b, u8 mode);
void OLED_Draw_Two_Vertical_Ellipses(u8 x0, u8 y0, u8 a, u8 b, u8 mode);
void OLED_DrawEllipse_Rotate(u8 x0, u8 y0, u8 a, u8 b, u16 angle, u8 mode);
void OLED_Draw_Filled_Ellipse_Rotate(u8 x0, u8 y0, u8 a, u8 b, u16 angle, u8 mode);
void OLED_Draw_Two_Vertical_Ellipses_Rotate(u8 x0, u8 y0, u8 a, u8 b, u16 angle, u8 mode);
void OLED_Draw_Nuno_Triangle(u8 x0, u8 y0, u8 r, u8 mode);

void OLED_Draw_Fish(u8 x0, u8 y0, u8 size, bool dir, u8 mode);
	
void OLED_ShowBMP(u8 x,u8 y,const u8 *p,u8 width,u8 height,u8 mode);
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 mode);

void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size,u8 mode);
void OLED_ShowString(u8 x,u8 y,const u8 *p,u8 size,u8 mode);

#endif
