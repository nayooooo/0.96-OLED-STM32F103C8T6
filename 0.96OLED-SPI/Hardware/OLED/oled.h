/**
 * @file oled.h
 * @author YEWANhub
 * @brief ʵ��OLED��������һЩ����ͼ�εĻ���
 * @version V1.0.0
 * @date 2022-06-10
 * @encoding GB2312
 * @details 1. �ڻ�����תͼ��ʱ���ǶȲ����ǽǶ���
 * @details 2. ��Ҫ�޸�OLED�Ľ��߷�ʽ�����޸�spi.h�е�SPI�˿ڶ���
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
#ifndef __OLED_H
#define __OLED_H			  	 
#include "sys.h"
#include "stdlib.h"

#define SLAVE_ADDRESS 0x78  /* �ӻ���ַ */

#define OLED_COL_MAX (128)				/* OLED���� */
#define OLED_ROW_MAX (64)				/* OLED���� */
#define OLED_PAGE_MAX (OLED_ROW_MAX/8)	/* OLEDҳ�� */
#define OLED_ROW_COL_MAX ((OLED_COL_MAX>OLED_ROW_MAX)?(OLED_COL_MAX):(OLED_ROW_MAX))  /* OLED�С������е����ֵ */
#define OLED_ROW_COL_MIN ((OLED_COL_MAX<OLED_ROW_MAX)?(OLED_COL_MAX):(OLED_ROW_MAX))  /* OLED�С������е���Сֵ */
		     
#define OLED_CMD  0  /* OLEDдָ������ */
#define OLED_DATA 1  /* OLEDд�������� */

#define FILL 1		/* ���1 */
#define CLEAR 0		/* ���0 */

/* ---------------- OLED�˿ڶ��� ---------------- */

#define OLED_SCLK_Clr()		SPI_SCLK_Clr()	// CLK���
#define OLED_SCLK_Set()		SPI_SCLK_Set()	// CLK��λ
#define OLED_SCLK_Pin		SPI_SCLK_Pin	// CLK����

#define OLED_SDIN_Clr()		SPI_SDIN_Clr()	// DIN���
#define OLED_SDIN_Set()		SPI_SDIN_Set()	// DIN��λ
#define OLED_SDIN_Pin		SPI_SDIN_Pin	// DIN����

#define OLED_RST_Clr()		SPI_RST_Clr()	// RSE���
#define OLED_RST_Set()		SPI_RST_Set()	// RSE��λ
#define OLED_RST_Pin		SPI_RST_Pin		// RSE����

#define OLED_DC_Clr()		SPI_DC_Clr()	// DC���
#define OLED_DC_Set()		SPI_DC_Set()	// DC��λ
#define OLED_DC_Pin			SPI_DC_Pin		// DC����
 		                
#define OLED_CS_Clr()		SPI_CS_Clr()	// CS���
#define OLED_CS_Set()		SPI_CS_Set()	// CS��λ
#define OLED_CS_Pin			SPI_CS_Pin		// CS����

/* ---------------- OLED�˿ڶ��� ---------------- */

/* ---------------- ��ṹ�嶨�� ---------------- */

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

/* ---------------- ��ṹ�嶨�� ---------------- */

/* ---------------- ����ʵ�ַ������� ---------------- */
#define REFRESH_START_POS_COL 0  // ˢ��ʱ����ʼ�����������
#define REFRESH_START_POS_PAGE 0  // ˢ��ʱ����ʼ�����ҳ����

// ѡ��ˢ�º�����ʵ�ַ�ʽ���޸�ģʽǰ�����޸�OLED��ʼ�����ã�
// 0->ˮƽ��ַģʽ�µ�ˢ�º���
// 1->��ֱ��ַģʽ�µ�ˢ�º���
// 2->��ֱ��ַģʽ�µ�DMAˢ�º���
#define REFRESH_MODE 1
// �����ʹ����OLED_SetPos��ΪԤ����ʾ��λ�뽫�·��ĺ궨��Ϊ1
#define REFRESH_DMA_PREVENT_DISLOCATION_MODE 0

#define SHOW_EVERY_STEEP 0  // ÿ�仯һ���㶼������һ��OLED_Refresh_Gram����
#define OLED_DRAW_LINE_BRESENHAM 1  // ʹ��brasenham�������ߣ�1��ʹ�ã�0������
#define OLED_DRAW_ELLIPSE_BRESENHAM 1  // ʹ��brasenham��������Բ��1��ʹ�ã�0������
/* ---------------- ����ʵ�ַ������� ---------------- */

//OLED�����ú���
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
