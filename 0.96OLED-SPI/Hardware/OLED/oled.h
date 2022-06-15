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

#define SLAVE_ADDRESS 0x78

#define OLED_COL_MAX (128)
#define OLED_ROW_MAX (64)
#define OLED_PAGE_MAX (OLED_ROW_MAX/8)
#define OLED_ROW_COL_MAX ((OLED_COL_MAX>OLED_ROW_MAX)?(OLED_COL_MAX):(OLED_ROW_MAX))
#define OLED_ROW_COL_MIN ((OLED_COL_MAX<OLED_ROW_MAX)?(OLED_COL_MAX):(OLED_ROW_MAX))
		     
#define OLED_CMD  0
#define OLED_DATA 1
#define OLED_CMD_CHR 0x00
#define OLED_DATA_CHR 0x40

#define FILL 1
#define CLEAR 0

/* ---------------- OLED端口定义 ---------------- */

#define OLED_SCLK_Clr()		SPI_SCLK_Clr()
#define OLED_SCLK_Set()		SPI_SCLK_Set()
#define OLED_SCLK_Pin		SPI_SCLK_Pin

#define OLED_SDIN_Clr()		SPI_SDIN_Clr()
#define OLED_SDIN_Set()		SPI_SDIN_Set()
#define OLED_SDIN_Pin		SPI_SDIN_Pin

#define OLED_RST_Clr()		SPI_RST_Clr()
#define OLED_RST_Set()		SPI_RST_Set()
#define OLED_RST_Pin		SPI_RST_Pin

#define OLED_DC_Clr()		SPI_DC_Clr()
#define OLED_DC_Set()		SPI_DC_Set()
#define OLED_DC_Pin			SPI_DC_Pin
 		                
#define OLED_CS_Clr()		SPI_CS_Clr()
#define OLED_CS_Set()		SPI_CS_Set()
#define OLED_CS_Pin			SPI_CS_Pin

/* ---------------- OLED端口定义 ---------------- */

/* ---------------- 结构体定义 ---------------- */

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

/* ---------------- 结构体定义 ---------------- */

/* ---------------- 函数实现方法定义 ---------------- */
#define SHOW_EVERY_STEEP 0
#define Stationary_Circle
//#define Dynamic_Circle
#define OLED_DRAW_LINE_BRESENHAM 1
/*
0 -> brasenham
*/
#define OLED_DRAW_ELLIPSE_METHOD 0
/* ---------------- 函数实现方法定义 ---------------- */

//OLED控制用函数
u32 mypow(u8 m,u8 n);
static void OLED_WR_Byte(u8 dat,u8 cmd);
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
void OLED_DrawCube(u8 x1,u8 y1,u8 x2,u8 y2, u8 mode);
void OLED_Draw_Filled_Cube(u8 x1,u8 y1,u8 x2,u8 y2, u8 mode);
void OLED_DrawCube_Intelligent_Overflow(u8 x0, u8 y0, u8 a, u8 b, u8 mode);
void OLED_Draw_Rounded_Cube(u8 x0, u8 y0, u8 a, u8 b, u8 r, u8 mode);
void OLED_Draw_Rounded_Cube_Erasure_Method(u8 x0, u8 y0, u8 a, u8 b, u8 r, u8 mode);
void OLED_Draw_Rounded_Cube_Fillet_Overflow(u8 x0, u8 y0, u8 a, u8 b, u8 r, u8 mode);
void OLED_Draw_Rounded_Cube_Fillet_Overflow_Erasure_Method(u8 x0, u8 y0, u8 a, u8 b, u8 r, u8 mode);

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
	 



