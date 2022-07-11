/**
 * @file oled.c
 * @author YEWANhub
 * @brief ʵ��OLED��������һЩ����ͼ�εĻ���
 * @version V1.0.0
 * @date 2022-06-10
 * @encoding GB2312
 * @details �ڻ�����תͼ��ʱ���ǶȲ����ǽǶ���
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
/**
 * ����Ϊ��xoyֱ������ϵ�е�y=x��y=-x�������Ὣ�仮��Ϊ8�������
 * �õ��������ţ��ں���������д�����л����õ�
 */
/******************************************************************************

			��		һ
		��				��
		��				��
			��		��

******************************************************************************/

#include "sys.h"
#include "stdlib.h"
#include "math.h"
#include "spi.h"
#include "dma.h"
#include "oled.h"
#include "oledfont.h"
//OLED���Դ�
//��Ÿ�ʽ���£���ͷ���Ϊ����������±꣬�ұ�ΪOLEDҳ�꣩��
// 0 -> [7]0 1 2 3 ... 127
// 1 -> [6]0 1 2 3 ... 127
// 2 -> [5]0 1 2 3 ... 127
// 3 -> [4]0 1 2 3 ... 127
// 4 -> [3]0 1 2 3 ... 127
// 5 -> [2]0 1 2 3 ... 127
// 6 -> [1]0 1 2 3 ... 127
// 7 -> [0]0 1 2 3 ... 127
//ÿ���ֽڴ洢�����ݶ�Ӧ��OLED����λ��y�᷽������ԭ���ڵ�7ҳ��0�У�
u8 OLED_GRAM[OLED_COL_MAX][OLED_PAGE_MAX];

//m^n
u32 mypow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;
	return result;
}

/**
 * @brief ������ӻ���SSD1306��д��һ���ֽ�
 * @param[dat] Ҫд�������/����
 * @param[cmd] ����/�����־ 0,��ʾ����;1,��ʾ����
 * @return None
 */
static void OLED_WR_Byte(u8 dat,u8 cmd)
{
	SPI_WR_Byte(dat, cmd);   	  
}

/**
 * @brief ����OLED�����ʼ��
 * @details ҳֻ�����ó�ҳ��ʼλ��
 * @param col ��Ҫ���õĹ��λ�õ�������
 * @param page ��Ҫ���õĹ��λ�õ�ҳ����
 * @return None
 */
static void OLED_SetPos(u8 col, u8 page)
{
	OLED_WR_Byte (0xb0+page,OLED_CMD);			// ����ҳ����
	OLED_WR_Byte (0x00+col&0X0F,OLED_CMD);		// �����е�ַ����4λ��
	OLED_WR_Byte (0x10+((col&0XF0)>>4),OLED_CMD);	// �����е�ַ����4λ��
}

/**
 * @brief ���»��浽SSD1306
 * @return None
 */
void OLED_Refresh_Gram(void)
{
#if REFRESH_MODE == 0
	// ˮƽ��ַģʽ�µ�ˢ�º���
	u8 i,n;
	for(i=0;i<OLED_PAGE_MAX;i++)
	{
		OLED_SetPos(REFRESH_START_POS_COL, (REFRESH_START_POS_PAGE+i)%8);
		for(n=0;n<OLED_COL_MAX;n++)OLED_WR_Byte(OLED_GRAM[n][i],OLED_DATA);
	}
#elif REFRESH_MODE == 1
	// ��ֱ��ַģʽ�µ�ˢ�º���
	u8 i,n;
	OLED_SetPos(REFRESH_START_POS_COL, REFRESH_START_POS_PAGE);
	for(i=0;i<OLED_COL_MAX;i++)
		for(n=0;n<OLED_PAGE_MAX;n++)
			OLED_WR_Byte(OLED_GRAM[i][n],OLED_DATA);
#elif USE_DMA && REFRESH_MODE == 2
	// ��ֱ��ַģʽ�µ�DMAˢ�º���
#if REFRESH_DMA_PREVENT_DISLOCATION_MODE
	// Ԥ����ʾ��λ
	OLED_SetPos(REFRESH_START_POS_COL, REFRESF_START_POS_PAGE);
#endif
	MyDMA_Enable();
#endif
}

/**
 * @brief OLED����ʾ
 * @return None
 */
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);
	OLED_WR_Byte(0X14,OLED_CMD);
	OLED_WR_Byte(0XAF,OLED_CMD);
}

/**
 * @brief OLED����ʾ
 * @return None
 */
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);
	OLED_WR_Byte(0X10,OLED_CMD);
	OLED_WR_Byte(0XAE,OLED_CMD);
}

/**
 * @brief OLED����
 * @return None
 */
void OLED_Clear(void)  
{  
	u8 i,n;  
	for(i=0;i<OLED_PAGE_MAX;i++)for(n=0;n<OLED_COL_MAX;n++)OLED_GRAM[n][i]=0X00;  
	OLED_Refresh_Gram();
}

/**
 * @brief OLED����
 * @details ��SHOW_EVERY_STEEP==1�������ÿһ�ε��øú���ʱ����ˢ��
 * @param x �����Ƶĵ�ĺ�����
 * @param y �����Ƶĵ��������
 * @param mode ����ģʽ��FILL�����1��CLEAR�����0
 * @return None
 */
void OLED_DrawPoint(u8 x,u8 y,u8 mode)
{
	u8 pos,bx,temp=0;
	if(x>OLED_COL_MAX-1||y>OLED_ROW_MAX-1)return;
	pos=7-y/8;
	bx=y%8;
	temp=1<<(7-bx);
	if(mode)OLED_GRAM[x][pos]|=temp;
	else OLED_GRAM[x][pos]&=~temp;
#if SHOW_EVERY_STEEP
	OLED_Refresh_Gram();
#endif
}

/**
 * @brief OLED����
 * @details ��ȡ���Ǵ�ʱ��OLED_GRAM�е���Ϣ����˲��ܷ�ӳ��ʱ����Ļ״̬
 * @param x �����ĵ�ĺ�����
 * @param y �����ĵ��������
 * @return int8_t �������״̬
 */
int8_t OLED_ReadPoint(u8 x,u8 y)
{
	u8 pos,bx,temp;
	if(x>OLED_COL_MAX-1||y>OLED_ROW_MAX-1)return -1;
	pos=7-y/8;
	bx=y%8;
	temp=1<<(7-bx);
	if(OLED_GRAM[x][pos]&temp) return 1;
	else return 0;
}

//bool OLED_ReadPoint_by_CMD(u8 x, u8 y)
//{
//	// only 6800 and 8080
//}

/* ---------------- base graphics ---------------- */

/**
 * @brief OLED����
 * @details ���Ըı�OLED_DRAW_LINE_BRESENHAM�ĺ궨����ѡ��ʵ�ַ���
 * @param x1 �˵�һ�ĺ�����
 * @param y1 �˵�һ��������
 * @param x2 �˵���ĺ�����
 * @param y2 �˵����������
 * @param mode ����ģʽ��FILL�����1��CLEAR�����0
 * @return None
 */
void OLED_DrawLine(u8 x1,u8 y1,u8 x2,u8 y2,u8 mode)
{
#if OLED_DRAW_LINE_BRESENHAM
	int16_t dx = x2 - x1, dy = y2 - y1;
	Point_int16_t p = { .x = 0, .y = 0 };  //��ֹ�� p.x=127��p.y=63 ʱ����
	uint8_t Point_Position_Inverse_Flag = 0X00;  // 0,1,2,3,4bit�ֱ��ʾ��һ���������������޼����� y=x �ԳƱ�ʶ
	
	if(dy==0)			// k==0
	{
		if(dx<0)
		{
			dx = -dx;
			x1 ^= x2; x2 ^= x1; x1 ^= x2;
		}
		for(; p.x<=dx; p.x++)
			OLED_DrawPoint(p.x+x1, p.y+y1, mode);
	}
	else if(dx==0)			// k������
	{
		if(dy<0)
		{
			dy = -dy;
			y1 ^= y2; y2 ^= y1; y1 ^= y2;
		}
		for(; p.y<=dy; p.y++)
			OLED_DrawPoint(p.x+x1, p.y+y1, mode);
	}
	else
	{
		//�� (x1, y1) �� (x2, y2) ����ɵ�һ�����е����
		//��������ת��֮����Կ��ɱ��������ڵ�һ������
		if(dx>0 && dy>0) Point_Position_Inverse_Flag |= 0X01;
		else if(dx>0 && dy <0)				//����ڶ�����
		{
			Point_Position_Inverse_Flag |= 0X02;
			dy = -dy;
			y2 = 2 * y1 - y2;
		}
		else if(dx<0 && dy < 0)			//�����������
		{
			Point_Position_Inverse_Flag |= 0X04;
			dx = -dx; dy = -dy;
			x1 ^= x2; x2 ^= x1; x1 ^= x2;
			y1 ^= y2; y2 ^= y1; y1 ^= y2;
		}
		else if(dx<0 && dy >0)		//�����������
		{
			Point_Position_Inverse_Flag |= 0X08;
			dx = -dx;
			x2 = 2 * x1 - x2;
		}
		//�� (x1, y1) �� (x2, y2) ����� 0<k<=1 �����Σ����任 { x0'=x1, y0'=y1 }��
		if(dx<dy)
		{
			Point_Position_Inverse_Flag |= 0X10;
			Point const temp = { .x = x2, .y = y2 };
			x2 = x1 - y1 + temp.y;
			y2 = -x1 + y1 + temp.x;
			dx = x2 - x1; dy = y2 - y1;  // ���¼��� dx, dy Ȼ���ٴ���ɵ�һ�����е����
		}
		
		int16_t incrE = 2 * dy, incrNE = 2 * (dy - dx);
		int16_t d = 2 * dy - dx;
		
		OLED_DrawPoint(p.x+x1, p.y+y1, mode);
		//��ʼ���������ƫ����
		for(p.x++; p.x<=dx; p.x++)
		{
			if(d<0){			//�Ӷ����������µ�����
				d += incrE;
			}else{				//�Ӷ������������µ�����
				p.y++;
				d += incrNE;
			}
			
			//���Ƶ�
			/* -- ����ڵ� (x0, y0) Ϊԭ��ķ�����֮���� --
			   -- x'= x0-y0+y --
			   -- y'=-x0+y0+x -- */
			switch(Point_Position_Inverse_Flag&0X0F)
			{
				case 0X01:		//��һ��������
				case 0X04:
					if(Point_Position_Inverse_Flag&0X10)
						OLED_DrawPoint(x1+p.y, y1+p.x, mode);
					else
						OLED_DrawPoint(x1+p.x, y1+p.y, mode);
					break;
				case 0X02:		//�ڶ�����
					if(Point_Position_Inverse_Flag&0X10)
						OLED_DrawPoint(x1+p.y, y1-p.x, mode);
					else
						OLED_DrawPoint(x1+p.x, y1-p.y, mode);
					break;
				case 0X08:		//��������
					if(Point_Position_Inverse_Flag&0X10)
						OLED_DrawPoint(x1-p.y, y1+p.x, mode);
					else
						OLED_DrawPoint(x1-p.x, y1+p.y, mode);
					break;
				default: break;
			}
		}
	}
#else
	u32 t; 
	int xerr=0, yerr=0, delta_x, delta_y, distance; 
	int incx, incy, uRow, uCol; 
	delta_x = x2-x1;
	delta_y = y2-y1; 
	uRow = x1; 
	uCol = y1;
	if(delta_x>0) incx=1;
	else if(delta_x==0) incx=0;
	else {incx = -1; delta_x = -delta_x;} 
	if(delta_y>0) incy = 1; 
	else if(delta_y==0) incy = 0;
	else{incy = -1; delta_y = -delta_y;} 
	if(delta_x>delta_y) distance = delta_x;
	else distance = delta_y; 
	for(t=0; t<=distance+1; t++ )
	{  
		OLED_DrawPoint(uRow, uCol, mode);
		
		xerr += delta_x; 
		yerr += delta_y;
		if(xerr>distance)
		{ 
			xerr -= distance; 
			uRow += incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr -= distance; 
			uCol += incy; 
		}
	}
#endif
}

/**
 * @brief OLED�ǶȻ��ߣ��������ü������ʾ����
 * @param x0 ����ĺ�����
 * @param y0 �����������
 * @param lenght ֱ�ߵļ���
 * @param angle ֱ�ߵļ���
 * @param mode ����ģʽ��FILL�����1��CLEAR�����0
 * @return None
 */
void OLED_DrawLine_Angle(u8 x0, u8 y0, u8 lenght, u16 angle, u8 mode)
{
	Point_Signed p;
	float rad = angle * (PI / 180);
	float sin_rad = sin(rad), cos_rad = cos(rad);
	
	p.x = lenght * cos_rad;
	p.y = lenght * sin_rad;
	
	OLED_DrawLine(x0, y0, p.x+x0, p.y+y0, mode);
}

/**
 * @brief OLED����
 * @param x1 ����ĳһ�Խ����ϵĶ˵�һ�ĺ�����
 * @param y1 ����ĳһ�Խ����ϵĶ˵�һ��������
 * @param x2 ����ĳһ�Խ����ϵĶ˵���ĺ�����
 * @param y2 ����ĳһ�Խ����ϵĶ˵����������
 * @param mode ����ģʽ��FILL�����1��CLEAR�����0
 * @return None
 */
void OLED_DrawRectangle(u8 x1,u8 y1,u8 x2,u8 y2, u8 mode)
{
	OLED_DrawLine(x1,y1,x2,y1,mode);
	OLED_DrawLine(x1,y2,x2,y2,mode);
	OLED_DrawLine(x1,y1,x1,y2,mode);
	OLED_DrawLine(x2,y1,x2,y2,mode);
}

/**
 * @brief OLED����䷽
 * @details ���õ���䷽������y�᷽��Ϊ��䷽�򣬱���(y1:y2+1)���ֱ��
 * @details ��˾��α߲�ƽ����������ʱ�������û������ϵ����
 * @param x1 ����ĳһ�Խ����ϵĶ˵�һ�ĺ�����
 * @param y1 ����ĳһ�Խ����ϵĶ˵�һ��������
 * @param x2 ����ĳһ�Խ����ϵĶ˵���ĺ�����
 * @param y2 ����ĳһ�Խ����ϵĶ˵����������
 * @param mode ����ģʽ��FILL�����1��CLEAR�����0
 * @return None
 */
void OLED_Draw_Filled_Rectangle(u8 x1,u8 y1,u8 x2,u8 y2, u8 mode)
{
	uint8_t i;
	
	// ���������
	if(y1>y2) y1 ^= y2, y2 ^= y1, y1 ^= y2;
	
	for(i=y1; i<y2; i++)
		OLED_DrawLine(x1, i, x2, i, mode);
}

/**
 * @brief OLED���������ܴ��������Ļ������һ�����ݴ��ʣ������ĵ�Ϊ����ԭ��
 * @details ������α߻������Ļ�������ܴ���������
 * @param x0 �������ĵĺ�����
 * @param y0 �������ĵ�������
 * @param a ���ε����±ߵĳ�
 * @param b ���ε����ұߵĳ�
 * @param mode ����ģʽ��FILL�����1��CLEAR�����0
 * @return None
 */
void OLED_DrawRectangle_Intelligent_Overflow(u8 x0, u8 y0, u8 a, u8 b, u8 mode)
{
	uint8_t Overflow_Flag = 0X00;  // 0, 1, 2, 3bit �ֱ��ʾ�е͸�λ������е͸�λ���
	Point p1, p2;
	
	// ��������������
	if(x0-a/2<0)
	{
		Overflow_Flag |= 0X01;
		b = (1.0 * b) * x0 / a;
		a = 2 * x0;
	}
	if(y0-b/2<0)
	{
		Overflow_Flag |= 0X04;
		a = (1.0 * a) * (2 * y0) / b;
		b = 2 * y0;
	}
	// ���մ���
	if(x0+a/2>=OLED_COL_MAX)
	{
		Overflow_Flag |= 0X02;
		b = 1.0 * b * (2 * (OLED_COL_MAX - x0)) / b;
		a = 2 * (OLED_COL_MAX - x0);
	}
	if(y0+b/2>=OLED_ROW_MAX)
	{
		Overflow_Flag |= 0X08;
		a = 1.0 * a * (2 * (OLED_ROW_MAX - y0)) / a;
		b = 2 * (OLED_ROW_MAX - y0);
	}
	
	p1.x = x0 - a / 2; p1.y = y0 - b / 2;
	p2.x = x0 + a / 2; p2.y = y0 + b / 2;
	
	OLED_DrawLine(p1.x, p2.y, p2.x, p2.y, mode);
	OLED_DrawLine(p1.x, p1.y, p2.x, p1.y, mode);
	OLED_DrawLine(p1.x, p2.y, p1.x, p1.y, mode);
	OLED_DrawLine(p2.x, p2.y, p2.x, p1.y, mode);
}

/**
 * @brief OLED����׼Բ�Ǿ��Σ�Ԥ��Բ�����������ĵ�Ϊ����ԭ��
 * @method �Ȱ��վ��εĻ�������һ��ȱ�Ǿ��Σ�Ȼ���ջ�Բ�ķ��������ĸ�Բ��
 * @param x0 Բ�Ǿ������ĵĺ�����
 * @param y0 Բ�Ǿ������ĵ�������
 * @param a Բ�Ǿ��ε����±ߵĳ�
 * @param b Բ�Ǿ��ε����ұߵĳ�
 * @param r Բ�Ǿ��ε�Բ�ǵİ뾶
 * @param mode ����ģʽ��FILL�����1��CLEAR�����0
 * @return None
 */
void OLED_Draw_Rounded_Rectangle(u8 x0, u8 y0, u8 a, u8 b, u8 r, u8 mode)
{
	Point p1, p2;
	
	p1.x = x0-a/2; p1.y = y0-b/2;
	p2.x = x0+a/2; p2.y = y0+b/2;
	
	//��ֹԲ�����
	if(r>a/2 || r>b/2) r = (a<b)?(a/2):(b/2);
	
	//��ȱ�Ǿ���
	OLED_DrawLine( p1.x+r,	p2.y,	p2.x-r,	p2.y,	mode );  // ��
	OLED_DrawLine( p1.x+r,	p1.y,	p2.x-r,	p1.y,	mode );  // ��
	OLED_DrawLine( p1.x,	p2.y-r,	p1.x,	p1.y+r,	mode );  // ��
	OLED_DrawLine( p2.x,	p2.y-r,	p2.x,	p1.y+r,	mode );  // ��
	
	//��Բ��
	Point p = {
		.x = 0,
		.y = r
	};
	// ��Ϊȡ����һ����
	OLED_Draw_8_Pixels_Spread_Out_From_Center(x0, y0, p.x+x0, p.y+y0, a/2-r, b/2-r, mode);
	int8_t d = 1 - r;
	for(p.x=1; p.x<=p.y; p.x++)
	{
		if(d<0){				/* goEast */
			d += 2 * p.x + 1;
		}
		else{					/* goSouthEast */
			p.y--;
			d += 2 * (p.x - p.y) + 1;
		}
		OLED_Draw_8_Pixels_Spread_Out_From_Center(x0, y0, p.x+x0, p.y+y0, a/2-r, b/2-r, mode);
	}
}

/**
 * @brief OLED����׼Բ�Ǿ��Σ�ʹ�ò������������ĵ�Ϊ����ԭ��
 * @method �Ȱ��վ��εĻ�������һ��ȱ�Ǿ��Σ�Ȼ���ջ�Բ�ķ��������ĸ�Բ��
 * @param x0 Բ�Ǿ������ĵĺ�����
 * @param y0 Բ�Ǿ������ĵ�������
 * @param a Բ�Ǿ��ε����±ߵĳ�
 * @param b Բ�Ǿ��ε����ұߵĳ�
 * @param r Բ�Ǿ��ε�Բ�ǵİ뾶
 * @param mode ����ģʽ��FILL�����1��CLEAR�����0
 * @return None
 */
void OLED_Draw_Rounded_Rectangle_Erasure_Method(u8 x0, u8 y0, u8 a, u8 b, u8 r, u8 mode)
{
	Point p1, p2;
	
	p1.x = x0-a/2; p1.y = y0-b/2;
	p2.x = x0+a/2; p2.y = y0+b/2;
	
	//��ֹԲ�����
	if(r>a/2 || r>b/2) r = (a<b)?(a/2):(b/2);
	
	//��ȱ�Ǿ���
	OLED_DrawRectangle( p1.x, p1.y, p2.x, p2.y, mode );
	OLED_DrawLine( p1.x, p1.y, p1.x+r,	p1.y,	!mode);  // ��
	OLED_DrawLine( p2.x, p1.y, p2.x-r,	p1.y,	!mode);
	OLED_DrawLine( p1.x, p2.y, p1.x+r,	p2.y,	!mode);  // ��
	OLED_DrawLine( p2.x, p2.y, p2.x-r,	p2.y,	!mode);
	OLED_DrawLine( p1.x, p1.y, p1.x,	p1.y+r,	!mode);  // ��
	OLED_DrawLine( p1.x, p2.y, p1.x,	p2.y-r,	!mode);
	OLED_DrawLine( p2.x, p1.y, p2.x,	p1.y+r,	!mode);  // ��
	OLED_DrawLine( p2.x, p2.y, p2.x,	p2.y-r,	!mode);
	
	//��Բ��
	Point p = {
		.x = 0,
		.y = r
	};
	// ��Ϊȡ����һ����
	OLED_Draw_8_Pixels_Spread_Out_From_Center(x0, y0, p.x+x0, p.y+y0, a/2-r, b/2-r, mode);
	int8_t d = 1 - r;
	for(p.x=1; p.x<=p.y; p.x++)
	{
		if(d<0){				/* goEast */
			d += 2 * p.x + 1;
		}
		else{					/* goSouthEast */
			p.y--;
			d += 2 * (p.x - p.y) + 1;
		}
		OLED_Draw_8_Pixels_Spread_Out_From_Center(x0, y0, p.x+x0, p.y+y0, a/2-r, b/2-r, mode);
	}
}

/**
 * @brief OLED����׼Բ�Ǿ��Σ�����Բ�ǻ�����������ĵ�Ϊ����ԭ��
 * @method �Ȱ��վ��εĻ�������һ��ȱ�Ǿ��Σ�Ȼ���ջ�Բ�ķ��������ĸ�Բ��
 * @param x0 Բ�Ǿ������ĵĺ�����
 * @param y0 Բ�Ǿ������ĵ�������
 * @param a Բ�Ǿ��ε����±ߵĳ�
 * @param b Բ�Ǿ��ε����ұߵĳ�
 * @param r Բ�Ǿ��ε�Բ�ǵİ뾶
 * @param mode ����ģʽ��FILL�����1��CLEAR�����0
 * @return None
 */
void OLED_Draw_Rounded_Rectangle_Fillet_Overflow(u8 x0, u8 y0, u8 a, u8 b, u8 r, u8 mode)
{
	Point p1, p2;
	
	p1.x = x0-a/2; p1.y = y0-b/2;
	p2.x = x0+a/2; p2.y = y0+b/2;
	
	//��ȱ�Ǿ���
	OLED_DrawRectangle( p1.x, p1.y, p2.x, p2.y, mode );
	OLED_DrawLine( p1.x, p1.y, p1.x+r,	p1.y,	!mode);  // ��
	OLED_DrawLine( p2.x, p1.y, p2.x-r,	p1.y,	!mode);
	OLED_DrawLine( p1.x, p2.y, p1.x+r,	p2.y,	!mode);  // ��
	OLED_DrawLine( p2.x, p2.y, p2.x-r,	p2.y,	!mode);
	OLED_DrawLine( p1.x, p1.y, p1.x,	p1.y+r,	!mode);  // ��
	OLED_DrawLine( p1.x, p2.y, p1.x,	p2.y-r,	!mode);
	OLED_DrawLine( p2.x, p1.y, p2.x,	p1.y+r,	!mode);  // ��
	OLED_DrawLine( p2.x, p2.y, p2.x,	p2.y-r,	!mode);
	
	//��Բ��
	Point p = {
		.x = 0,
		.y = r
	};
	// ��Ϊȡ����һ����
	OLED_Draw_8_Pixels_Spread_Out_From_Center(x0, y0, p.x+x0, p.y+y0, a/2-r, b/2-r, mode);
	int8_t d = 1 - r;
	for(p.x=1; p.x<=p.y; p.x++)
	{
		if(d<0){				/* goEast */
			d += 2 * p.x + 1;
		}
		else{					/* goSouthEast */
			p.y--;
			d += 2 * (p.x - p.y) + 1;
		}
		OLED_Draw_8_Pixels_Spread_Out_From_Center(x0, y0, p.x+x0, p.y+y0, a/2-r, b/2-r, mode);
	}
}

/**
 * @brief OLED����׼Բ�Ǿ��Σ�����Բ�ǻ������ʹ�õ��ǲ������������ĵ�Ϊ����ԭ��
 * @method �Ȱ��վ��εĻ�������һ��ȱ�Ǿ��Σ�Ȼ���ջ�Բ�ķ��������ĸ�Բ��
 * @param x0 Բ�Ǿ������ĵĺ�����
 * @param y0 Բ�Ǿ������ĵ�������
 * @param a Բ�Ǿ��ε����±ߵĳ�
 * @param b Բ�Ǿ��ε����ұߵĳ�
 * @param r Բ�Ǿ��ε�Բ�ǵİ뾶
 * @param mode ����ģʽ��FILL�����1��CLEAR�����0
 * @return None
 */
void OLED_Draw_Rounded_Rectangle_Fillet_Overflow_Erasure_Method(u8 x0, u8 y0, u8 a, u8 b, u8 r, u8 mode)
{
	Point p1, p2;
	
	p1.x = x0-a/2; p1.y = y0-b/2;
	p2.x = x0+a/2; p2.y = y0+b/2;
	
	//��ȱ�Ǿ���
	OLED_DrawRectangle( p1.x, p1.y, p2.x, p2.y, mode );
	OLED_DrawLine( p1.x, p1.y, p1.x+r,	p1.y,	!mode);  // ��
	OLED_DrawLine( p2.x, p1.y, p2.x-r,	p1.y,	!mode);
	OLED_DrawLine( p1.x, p2.y, p1.x+r,	p2.y,	!mode);  // ��
	OLED_DrawLine( p2.x, p2.y, p2.x-r,	p2.y,	!mode);
	OLED_DrawLine( p1.x, p1.y, p1.x,	p1.y+r,	!mode);  // ��
	OLED_DrawLine( p1.x, p2.y, p1.x,	p2.y-r,	!mode);
	OLED_DrawLine( p2.x, p1.y, p2.x,	p1.y+r,	!mode);  // ��
	OLED_DrawLine( p2.x, p2.y, p2.x,	p2.y-r,	!mode);
	
	//��Բ��
	Point p = {
		.x = 0,
		.y = r
	};
	// ��Ϊȡ����һ����
	OLED_Draw_8_Pixels_Spread_Out_From_Center(x0, y0, p.x+x0, p.y+y0, a/2-r, b/2-r, mode);
	int8_t d = 1 - r;
	for(p.x=1; p.x<=p.y; p.x++)
	{
		if(d<0){				/* goEast */
			d += 2 * p.x + 1;
		}
		else{					/* goSouthEast */
			p.y--;
			d += 2 * (p.x - p.y) + 1;
		}
		OLED_Draw_8_Pixels_Spread_Out_From_Center(x0, y0, p.x+x0, p.y+y0, a/2-r, b/2-r, mode);
	}
}

/**
 * @brief OLED���� 4 �����أ������ĵ�Ϊ����ԭ��
 * @param x0 4��������ĵĺ�����
 * @param y0 4��������ĵ�������
 * @param x 4�����е�һ��λ�ڵ�һ�����ϵĵ�ĺ�����
 * @param y 4�����е�һ��λ�ڵ�һ�����ϵĵ��������
 * @param mode ����ģʽ��FILL�����1��CLEAR�����0
 * @return None
 */
void OLED_Draw_4_Pixels(u8 x0, u8 y0, u8 x, u8 y, u8 mode)
{
	OLED_DrawPoint(x,		y,			mode);				/* ��һ���� */
	OLED_DrawPoint(x,		2*y0-y,		mode);				/* �ڶ����� */
	OLED_DrawPoint(2*x0-x,	2*y0-y,		mode);				/* �������� */
	OLED_DrawPoint(2*x0-x,	y,			mode);				/* �������� */
}

/**
 * @brief OLED���� 2 �������ߣ������������߹������ĵ㴦��x��Գ�
 * @param x0 4��������ĵĺ�����
 * @param y0 4��������ĵ�������
 * @param x 4���˵��е�һ��λ�ڵ�һ�����ϵĵ�ĺ�����
 * @param y 4���˵��е�һ��λ�ڵ�һ�����ϵĵ��������
 * @param mode ����ģʽ��FILL�����1��CLEAR�����0
 * @return None
 */
void OLED_Draw_4_Pixels_Lines(u8 x0, u8 y0, u8 x, u8 y, u8 mode)
{
	OLED_DrawLine(2*x0-x,	y,			x,			y,			mode);	/* x���Ϸ����� */
	OLED_DrawLine(2*x0-x,	2*y0-y,		x,			2*y0-y,		mode);	/* x���·����� */
}

/**
 * @brief OLED���� 4 ����������ת�����أ������ĵ�Ϊ����ԭ��
 * @param x0 4��������ĵĺ�����
 * @param y0 4��������ĵ�������
 * @param x 4���˵��е�һ��λ�ڵ�һ�����ϵĵ�ĺ�����
 * @param y 4���˵��е�һ��λ�ڵ�һ�����ϵĵ��������
 * @param angle ���x�����ת�Ƕȣ���ʱ�룩
 * @param mode ����ģʽ��FILL�����1��CLEAR�����0
 * @return None
 */
// x' = xcosb - ysinb
// y' = xsinb + ycosb
// ����ԳƵ�ǰ��Ҫ�任һ�����꣬���ʹ�� int16_t �������������㸺���ͷ�Χ����
void OLED_Draw_4_Pixels_Rotate(u8 x0, u8 y0, int16_t x, int16_t y, u16 angle, u8 mode)
{
	float rad = angle * (PI / 180);
	float sin_rad = sin(rad), cos_rad = cos(rad);
	x -= x0; y -= y0;  // �õ�����ƫ����
	float x_s = x*sin_rad,		x_c = x*cos_rad,		y_s = y*sin_rad,		y_c = y*cos_rad;
	float _x_s = (-x)*sin_rad,	_x_c = (-x)*cos_rad,	_y_s = (-y)*sin_rad,	_y_c = (-y)*cos_rad;
	
	//ע��������ĵ�����꣬��Ϊ��������ƫ����
	OLED_DrawPoint(x_c-y_s		+x0,	x_s+y_c		+y0,	mode);					/* ��һ���� */
	OLED_DrawPoint(x_c-_y_s		+x0,	x_s+_y_c	+y0,	mode);					/* �ڶ����� */
	OLED_DrawPoint(_x_c-_y_s	+x0,	_x_s+_y_c	+y0,	mode);					/* �������� */
	OLED_DrawPoint(_x_c-y_s		+x0,	_x_s+y_c	+y0,	mode);					/* �������� */
}

/**
 * @brief OLED�� 2 ����������ת�����ߣ������ĵ�Ϊ����ԭ��
 * @param x0 4��������ĵĺ�����
 * @param y0 4��������ĵ�������
 * @param x 4���˵��е�һ��λ�ڵ�һ�����ϵĵ�ĺ�����
 * @param y 4���˵��е�һ��λ�ڵ�һ�����ϵĵ��������
 * @param angle ���x�����ת�Ƕȣ���ʱ�룩
 * @param mode ����ģʽ��FILL�����1��CLEAR�����0
 * @return None
 */
// x' = xcosb - ysinb
// y' = xsinb + ycosb
// ����ԳƵ�ǰ��Ҫ�任һ�����꣬���ʹ�� int16_t �������������㸺���ͷ�Χ����
void OLED_Draw_4_Pixels_Rotate_Lines(u8 x0, u8 y0, int16_t x, int16_t y, u16 angle, u8 mode)
{
	float rad = angle * (PI / 180);
	float sin_rad = sin(rad), cos_rad = cos(rad);
	x -= x0; y -= y0;  // �õ�����ƫ����
	float x_s = x*sin_rad,		x_c = x*cos_rad,		y_s = y*sin_rad,		y_c = y*cos_rad;
	float _x_s = (-x)*sin_rad,	_x_c = (-x)*cos_rad,	_y_s = (-y)*sin_rad,	_y_c = (-y)*cos_rad;
	
	//ע��������ĵ�����꣬��Ϊ��������ƫ����
	OLED_DrawLine(_x_c-y_s +x0,  _x_s+y_c  +y0, x_c-y_s  +x0, x_s+y_c  +y0, mode);	/* �Գ����Ϸ� */
	OLED_DrawLine(_x_c-_y_s	+x0, _x_s+_y_c +y0, x_c-_y_s +x0, x_s+_y_c +y0, mode);	/* �Գ����·� */
}

/**
 * @brief ��ĳһ��Ϊ���Ļ���4���㣬��4��������ĵ�֮��ľ���Ϊ (dx, dy)
 * @details ƫ�ƾ����������һ�����ϵĵ���Ե�
 * @param x0 �Գ����ĵĺ�����
 * @param y0 �Գ����ĵ�������
 * @param x ��Ҫ���Ƶ�4�����У�λ��һ����ĵ�ĺ�����
 * @param y ��Ҫ���Ƶ�4�����У�λ��һ����ĵ��������
 * @param dx ƫ�ƾ���ĺ������
 * @param dy ƫ�ƾ�����������
 * @param mode ����ģʽ��FILL�����1��CLEAR�����0
 * @return None
 */
void OLED_Draw_4_Pixels_Spread_Out_From_Center(u8 x0, u8 y0, u8 x, u8 y, u8 dx, u8 dy, u8 mode)
{
	OLED_DrawPoint(x		+dx,	y		+dy,	mode);		/* ��һ���� */
	OLED_DrawPoint(x		+dx,	2*y0-y	-dy,	mode);		/* �ڶ����� */
	OLED_DrawPoint(2*x0-x	-dx,	2*y0-y	-dy,	mode);		/* �������� */
	OLED_DrawPoint(2*x0-x	-dx,	y		+dy,	mode);		/* �������� */
}

/**
 * @brief OLED���� 8 ������
 * @param x0 8��������ĵĺ�����
 * @param y0 8��������ĵ�������
 * @param x 8���˵��е�һ��λ��һ�����ϵĵ�ĺ�����
 * @param y 8���˵��е�һ��λ��һ�����ϵĵ��������
 * @param mode ����ģʽ��FILL�����1��CLEAR�����0
 * @return None
 */
/* --------------------------------------------------- *
 * һ	x = x,					y = y
 * ��	x = y -y0+x0,			y = x-x0+y0
 * ��	x = y-y0+x0,			y = 2*y0-(x-x0+y0)
 * ��	x = x,					y = 2*y0-y
 * ��	x = 2*x0-x,				y = 2*y0-y
 * ��	x = 2*x0-(y-y0+x0),		y = 2*y0-(x-x0+y0)
 * ��	x = 2*x0-(y-y0+x0),		y = x-x0+y0
 * ��	x = 2*x0-x,				y = y
 * --------------------------------------------------- *
 */
void OLED_Draw_8_Pixels(u8 x0, u8 y0, u8 x, u8 y, u8 mode)
{
	OLED_DrawPoint(x,			y,				mode);				/* һ */
	OLED_DrawPoint(y+x0-y0,		x-x0+y0,		mode);				/* �� */
	OLED_DrawPoint(y+x0-y0,		-x+x0+y0,		mode);				/* �� */
	OLED_DrawPoint(x,			-y+y0+y0,		mode);				/* �� */
	OLED_DrawPoint(-x+x0+x0,	-y+y0+y0,		mode);				/* �� */
	OLED_DrawPoint(-y+x0+y0,	-x+x0+y0,		mode);				/* �� */
	OLED_DrawPoint(-y+x0+y0,	x-x0+y0,		mode);				/* �� */
	OLED_DrawPoint(-x+x0+x0,	y,				mode);				/* �� */
}

/**
 * @brief OLED���� 4 �������ߣ��� 4 �������߹������ĵ㴦��x��Գ�
 * @details һ�����еĵ���ص��ǣ�dy>dx>0
 * @param x0 8��������ĵĺ�����
 * @param y0 8��������ĵ�������
 * @param x 8���˵��е�һ��λ��һ�����ϵĵ�ĺ�����
 * @param y 8���˵��е�һ��λ��һ�����ϵĵ��������
 * @param mode ����ģʽ��FILL�����1��CLEAR�����0
 * @return None
 */
void OLED_Draw_8_Pixels_Lines(u8 x0, u8 y0, u8 x, u8 y, u8 mode)
{
	OLED_DrawLine(2*x0-x,	y,			x,			y,			mode);	/* x���Ϸ����� */
	OLED_DrawLine(-y+x0+y0,	x-x0+y0,	y+x0-y0,	x-x0+y0,	mode);
	OLED_DrawLine(-y+x0+y0,	-x+x0+y0,	y+x0-y0,	-x+x0+y0,	mode);	/* x���·����� */
	OLED_DrawLine(2*x0-x,	2*y0-y,		x,			2*y0-y,		mode);
}

/**
 * @brief ��ĳһ��Ϊ���Ļ���8���㣬��˸�������ĵ�֮��ľ���Ϊ (dx, dy)
 * @details ƫ�ƾ����������һ�����ϵĵ���Ե�
 * @param x0 �Գ����ĵĺ�����
 * @param y0 �Գ����ĵ�������
 * @param x 8���˵��е�һ��λ��һ�����ϵĵ�ĺ�����
 * @param y 8���˵��е�һ��λ��һ�����ϵĵ��������
 * @param dx ƫ�ƾ���ĺ������
 * @param dy ƫ�ƾ�����������
 * @param mode ����ģʽ��FILL�����1��CLEAR�����0
 * @return None
 */
void OLED_Draw_8_Pixels_Spread_Out_From_Center(u8 x0, u8 y0, u8 x, u8 y, u8 dx, u8 dy, u8 mode)
{
	OLED_DrawPoint(x		+dx,	y			+dy,	mode);		/* һ */
	OLED_DrawPoint(y+x0-y0	+dx,	x-x0+y0		+dy,	mode);		/* �� */
	OLED_DrawPoint(y+x0-y0	+dx,	-x+x0+y0	-dy,	mode);		/* �� */
	OLED_DrawPoint(x		+dx,	-y+y0+y0	-dy,	mode);		/* �� */
	OLED_DrawPoint(-x+x0+x0	-dx,	-y+y0+y0	-dy,	mode);		/* �� */
	OLED_DrawPoint(-y+x0+y0	-dx,	-x+x0+y0	-dy,	mode);		/* �� */
	OLED_DrawPoint(-y+x0+y0	-dx,	x-x0+y0		+dy,	mode);		/* �� */
	OLED_DrawPoint(-x+x0+x0	-dx,	y			+dy,	mode);		/* �� */
}

/**
 * @brief OLED���� 8 ����������ת�����أ������ĵ�Ϊ����ԭ��
 * @param x0 8��������ĵĺ�����
 * @param y0 8��������ĵ�������
 * @param x 8���˵��е�һ��λ��һ�����ϵĵ�ĺ�����
 * @param y 8���˵��е�һ��λ��һ�����ϵĵ��������
 * @param angle ���x�����ת�Ƕȣ���ʱ�룩
 * @param mode ����ģʽ��FILL�����1��CLEAR�����0
 * @return None
 */
// x' = xcosb - ysinb
// y' = xsinb + ycosb
// ����ԳƵ�ǰ��Ҫ�任һ�����꣬���ʹ�� int16_t �������������㸺���ͷ�Χ����
void OLED_Draw_8_Pixels_Rotate(u8 x0, u8 y0, int16_t x, int16_t y, u16 angle, u8 mode)
{
	float rad = angle * (PI / 180);
	float sin_rad = sin(rad), cos_rad = cos(rad);
	//������ת���ƫ�������·���Щ��û���������ƫ������ֻ�Ǽ��ټ���ʱ�䣩��1��ʾ���ţ���д�� X,Y �������ֺ�������
	float	x_X_s = (x-x0)*sin_rad,				x_X_c = (x-x0)*cos_rad,\
			y_Y_s = (y-y0)*sin_rad,				y_Y_c = (y-y0)*cos_rad,\
			y_x_1y_X_s = (y+x0-y0-x0)*sin_rad,	y_x_1y_X_c = (y+x0-y0-x0)*cos_rad,\
			x_1x_y_Y_s = (x-x0+y0-y0)*sin_rad,	x_1x_y_Y_c = (x-x0+y0-y0)*cos_rad,\
			_x_x_y_Y_s = (-x+x0+y0-y0)*sin_rad,	_x_x_y_Y_c = (-x+x0+y0-y0)*cos_rad,\
			_y_x_y_X_s = (-y+x0+y0-x0)*sin_rad,	_y_x_y_X_c = (-y+x0+y0-x0)*cos_rad,\
			_x_x_x_X_s = (-x+x0+x0-x0)*sin_rad,	_x_x_x_X_c = (-x+x0+x0-x0)*cos_rad,\
			_y_y_y_Y_s = (-y+y0+y0-y0)*sin_rad,	_y_y_y_Y_c = (-y+y0+y0-y0)*cos_rad;
	
	//ע��������ĵ�����꣬��Ϊ��������ƫ����
	OLED_DrawPoint( x_X_c-y_Y_s				+x0,	x_X_s+y_Y_c				+y0,	mode );				/* һ */
	OLED_DrawPoint( y_x_1y_X_c-x_1x_y_Y_s	+x0,	y_x_1y_X_s+x_1x_y_Y_c	+y0,	mode );				/* �� */
	OLED_DrawPoint( y_x_1y_X_c-_x_x_y_Y_s	+x0,	y_x_1y_X_s+_x_x_y_Y_c	+y0,	mode );				/* �� */
	OLED_DrawPoint( x_X_c-_y_y_y_Y_s		+x0,	x_X_s+_y_y_y_Y_c		+y0,	mode );				/* �� */
	OLED_DrawPoint( _x_x_x_X_c-_y_y_y_Y_s	+x0,	_x_x_x_X_s+_y_y_y_Y_c	+y0,	mode );				/* �� */
	OLED_DrawPoint( _y_x_y_X_c-_x_x_y_Y_s	+x0,	_y_x_y_X_s+_x_x_y_Y_c	+y0,	mode );				/* �� */
	OLED_DrawPoint( _y_x_y_X_c-x_1x_y_Y_s	+x0,	_y_x_y_X_s+x_1x_y_Y_c	+y0,	mode );				/* �� */
	OLED_DrawPoint( _x_x_x_X_c-y_Y_s		+x0,	_x_x_x_X_s+y_Y_c		+y0,	mode );				/* �� */
}

/**
 * @brief OLED��Բ
 * @method brasenham����
 * @param x0 Բ�ĵĺ�����
 * @param y0 Բ�ĵ�������
 * @param r Բ�İ뾶
 * @param mode ����ģʽ��FILL�����1��CLEAR�����0
 * @return None
 */
void OLED_DrawCircle(u8 x0, u8 y0, u8 r, u8 mode)
{
	Point p = {
		.x = 0,
		.y = r
	};
	OLED_Draw_8_Pixels(x0, y0, p.x+x0, p.y+y0, mode);
	int8_t d = 1 - r;
	for(p.x=1; p.x<=p.y; p.x++)
	{
		if(d<0){				/* goEast */
			d += 2 * p.x + 1;
		}
		else{					/* goSouthEast */
			p.y--;
			d += 2 * (p.x - p.y) + 1;
		}
		OLED_Draw_8_Pixels(x0, y0, p.x+x0, p.y+y0, mode);
	}
}

/**
 * @brief OLED�����Բ
 * @method brasenham����
 * @param x0 Բ�ĵĺ�����
 * @param y0 Բ�ĵ�������
 * @param r Բ�İ뾶
 * @param mode ����ģʽ��FILL�����1��CLEAR�����0
 * @return None
 */
void OLED_Draw_Filled_Circle(u8 x0, u8 y0, u8 r, u8 mode)
{
	Point p = {
		.x = 0,
		.y = r
	};
	OLED_Draw_8_Pixels_Lines(x0, y0, p.x+x0, p.y+y0, mode);
	int8_t d = 1 - r;
	for(p.x=1; p.x<=p.y; p.x++)
	{
		if(d<0){				/* goEast */
			d += 2 * p.x + 1;
		}
		else{					/* goSouthEast */
			p.y--;
			d += 2 * (p.x - p.y) + 1;
		}
		OLED_Draw_8_Pixels_Lines(x0, y0, p.x+x0, p.y+y0, mode);
	}
}

/**
 * @brief OLED�����⻡�Σ���Բ��Ϊ����ԭ��
 * @param x0 ����Բ�ĵĺ�����
 * @param y0 ����Բ�ĵ�������
 * @param r ���ΰ뾶
 * @param thet ����Բ�Ľ�
 * @param angle ���������x�����ת�Ƕȣ���ʱ�룩���Ƕ��ƣ�
 * @param mode ����ģʽ��FILL�����1��CLEAR�����0
 * @return None
 */
void OLED_DrawArc(u8 x0, u8 y0, u8 r, u8 thet, u8 angle, u8 mode)
{}

/**
 * @brief OLED����׼��Բ
 * @method brasenham����
 * @param x0 ��Բ���ĵĺ�����
 * @param y0 ��Բ���ĵ�������
 * @param a ��Բ�ĳ����᳤
 * @param b ��Բ�Ķ̰��᳤
 * @param mode ����ģʽ��FILL�����1��CLEAR�����0
 * @return None
 */
void OLED_DrawEllipse(u8 x0, u8 y0, u8 a, u8 b, u8 mode)
{
#if OLED_DRAW_ELLIPSE_BRESENHAM
	/* --------------- Bresenham --------------- */
	Point_Signed p = { .x = 0, .y = b };
	float d1 = b * b + a * a * (-b + 0.25);
	
	OLED_Draw_4_Pixels(x0, y0, x0+p.x, y0+p.y, mode);
	while (b * b * (p.x + 1) < a * a * (p.y - 0.5))
	{
		if (d1 <= 0)
		{
			d1 += b * b * (2 * p.x + 3);
			p.x++;
		}
		else
		{
			d1 += (b * b * (2 * p.x + 3) + a * a * (-2 * p.y + 2));
			p.x++;
			p.y--;
		}
		OLED_Draw_4_Pixels(x0, y0, x0+p.x, y0+p.y, mode);
	}
	float d2 = b * b * (p.x + 0.5) * (p.x + 0.5) + a * a * (p.y - 1) * (p.y - 1) - a * a * b * b;
	while (p.y > 0)
	{
		if (d2 <= 0)
		{
			d2 += b * b * (2 * p.x + 2) + a * a * (-2 * p.y + 3);
			p.x++;
			p.y--;
		}
		else
		{
			d2 += a * a * (-2 * p.y + 3);
			p.y--;
		}
		OLED_Draw_4_Pixels(x0, y0, x0+p.x, y0+p.y, mode);
	}
	/* --------------- Bresenham --------------- */
#endif
}

/**
 * @brief OLED����׼�����Բ
 * @param x0 ��Բ���ĵĺ�����
 * @param y0 ��Բ���ĵ�������
 * @param a ��Բ�ĳ����᳤
 * @param b ��Բ�Ķ̰��᳤
 * @param mode ����ģʽ��FILL�����1��CLEAR�����0
 * @return None
 */
void OLED_Draw_Filled_Ellipse(u8 x0, u8 y0, u8 a, u8 b, u8 mode)
{
#if OLED_DRAW_ELLIPSE_BRESENHAM
	/* --------------- Bresenham --------------- */
	Point_Signed p = { .x = 0, .y = b };
	float d1 = b * b + a * a * (-b + 0.25);
	
	OLED_Draw_4_Pixels_Lines(x0, y0, x0+p.x, y0+p.y, mode);
	while (b * b * (p.x + 1) < a * a * (p.y - 0.5))
	{
		if (d1 <= 0)
		{
			d1 += b * b * (2 * p.x + 3);
			p.x++;
		}
		else
		{
			d1 += (b * b * (2 * p.x + 3) + a * a * (-2 * p.y + 2));
			p.x++;
			p.y--;
		}
		OLED_Draw_4_Pixels_Lines(x0, y0, x0+p.x, y0+p.y, mode);
	}
	float d2 = b * b * (p.x + 0.5) * (p.x + 0.5) + a * a * (p.y - 1) * (p.y - 1) - a * a * b * b;
	while (p.y > 0)
	{
		if (d2 <= 0)
		{
			d2 += b * b * (2 * p.x + 2) + a * a * (-2 * p.y + 3);
			p.x++;
			p.y--;
		}
		else
		{
			d2 += a * a * (-2 * p.y + 3);
			p.y--;
		}
		OLED_Draw_4_Pixels_Lines(x0, y0, x0+p.x, y0+p.y, mode);
	}
	/* --------------- Bresenham --------------- */
#endif
}

/**
 * @brief OLED�������໥��ֱ�ı�׼��Բ
 * @param x0 ������Բ���ĵĺ�����
 * @param y0 ������Բ���ĵ�������
 * @param a ������Բ�ĳ����᳤
 * @param b ������Բ�Ķ̰��᳤
 * @param mode ����ģʽ��FILL�����1��CLEAR�����0
 * @return None
 */
void OLED_Draw_Two_Vertical_Ellipses(u8 x0, u8 y0, u8 a, u8 b, u8 mode)
{
#if OLED_DRAW_ELLIPSE_BRESENHAM
	/* --------------- Bresenham --------------- */
	Point_Signed p = { .x = 0, .y = b };
	float d1 = b * b + a * a * (-b + 0.25);
	
	OLED_Draw_8_Pixels(x0, y0, x0+p.x, y0+p.y, mode);
	while (b * b * (p.x + 1) < a * a * (p.y - 0.5))
	{
		if (d1 <= 0)
		{
			d1 += b * b * (2 * p.x + 3);
			p.x++;
		}
		else
		{
			d1 += (b * b * (2 * p.x + 3) + a * a * (-2 * p.y + 2));
			p.x++;
			p.y--;
		}
		OLED_Draw_8_Pixels(x0, y0, x0+p.x, y0+p.y, mode);
	}
	float d2 = b * b * (p.x + 0.5) * (p.x + 0.5) + a * a * (p.y - 1) * (p.y - 1) - a * a * b * b;
	while (p.y > 0)
	{
		if (d2 <= 0)
		{
			d2 += b * b * (2 * p.x + 2) + a * a * (-2 * p.y + 3);
			p.x++;
			p.y--;
		}
		else
		{
			d2 += a * a * (-2 * p.y + 3);
			p.y--;
		}
		OLED_Draw_8_Pixels(x0, y0, x0+p.x, y0+p.y, mode);
	}
	/* --------------- Bresenham --------------- */
#endif
}

/**
 * @brief OLED���Ǳ�׼��Բ
 * @method brasenham����
 * @param x0 ��Բ���ĵĺ�����
 * @param y0 ��Բ���ĵ�������
 * @param a ��Բ�ĳ����᳤
 * @param b ��Բ�Ķ̰��᳤
 * @param angle �����x�����ת�Ƕȣ���ʱ�룩���Ƕ��ƣ�
 * @param mode ����ģʽ��FILL�����1��CLEAR�����0
 * @return None
 */
// x' = xcosb - ysinb
// y' = xsinb + ycosb
void OLED_DrawEllipse_Rotate(u8 x0, u8 y0, u8 a, u8 b, u16 angle, u8 mode)
{
#if OLED_DRAW_ELLIPSE_BRESENHAM
	/* --------------- Bresenham --------------- */
	Point_Signed p = { .x = 0, .y = b };
	float d1 = b * b + a * a * (-b + 0.25);
	
	OLED_Draw_4_Pixels_Rotate(x0, y0, x0+p.x, y0+p.y, angle, mode);
	while (b * b * (p.x + 1) < a * a * (p.y - 0.5))
	{
		if (d1 <= 0)
		{
			d1 += b * b * (2 * p.x + 3);
			p.x++;
		}
		else
		{
			d1 += (b * b * (2 * p.x + 3) + a * a * (-2 * p.y + 2));
			p.x++;
			p.y--;
		}
		OLED_Draw_4_Pixels_Rotate(x0, y0, x0+p.x, y0+p.y, angle, mode);
	}
	float d2 = b * b * (p.x + 0.5) * (p.x + 0.5) + a * a * (p.y - 1) * (p.y - 1) - a * a * b * b;
	while (p.y > 0)
	{
		if (d2 <= 0)
		{
			d2 += b * b * (2 * p.x + 2) + a * a * (-2 * p.y + 3);
			p.x++;
			p.y--;
		}
		else
		{
			d2 += a * a * (-2 * p.y + 3);
			p.y--;
		}
		OLED_Draw_4_Pixels_Rotate(x0, y0, x0+p.x, y0+p.y, angle, mode);
	}
	/* --------------- Bresenham --------------- */
#endif
}

/**
 * @brief OLED���Ǳ�׼��Բ
 * @param x0 ��Բ���ĵĺ�����
 * @param y0 ��Բ���ĵ�������
 * @param a ��Բ�ĳ����᳤
 * @param b ��Բ�Ķ̰��᳤
 * @param angle �����x�����ת�Ƕȣ���ʱ�룩���Ƕ��ƣ�
 * @param mode ����ģʽ��FILL�����1��CLEAR�����0
 * @return None
 */
// x' = xcosb - ysinb
// y' = xsinb + ycosb
void OLED_Draw_Filled_Ellipse_Rotate(u8 x0, u8 y0, u8 a, u8 b, u16 angle, u8 mode)
{
#if OLED_DRAW_ELLIPSE_BRESENHAM
	/* --------------- Bresenham --------------- */
	Point_Signed p = { .x = 0, .y = b };
	float d1 = b * b + a * a * (-b + 0.25);
	
	OLED_Draw_4_Pixels_Rotate_Lines(x0, y0, x0+p.x, y0+p.y, angle, mode);
	while (b * b * (p.x + 1) < a * a * (p.y - 0.5))
	{
		if (d1 <= 0)
		{
			d1 += b * b * (2 * p.x + 3);
			p.x++;
		}
		else
		{
			d1 += (b * b * (2 * p.x + 3) + a * a * (-2 * p.y + 2));
			p.x++;
			p.y--;
		}
		OLED_Draw_4_Pixels_Rotate_Lines(x0, y0, x0+p.x, y0+p.y, angle, mode);
	}
	float d2 = b * b * (p.x + 0.5) * (p.x + 0.5) + a * a * (p.y - 1) * (p.y - 1) - a * a * b * b;
	while (p.y > 0)
	{
		if (d2 <= 0)
		{
			d2 += b * b * (2 * p.x + 2) + a * a * (-2 * p.y + 3);
			p.x++;
			p.y--;
		}
		else
		{
			d2 += a * a * (-2 * p.y + 3);
			p.y--;
		}
		OLED_Draw_4_Pixels_Rotate_Lines(x0, y0, x0+p.x, y0+p.y, angle, mode);
	}
	/* --------------- Bresenham --------------- */
#endif
}

/**
 * @brief OLED�������໥��ֱ�ķǱ�׼��Բ
 * @param x0 ������Բ���ĵĺ�����
 * @param y0 ������Բ���ĵ�������
 * @param a ������Բ�ĳ����᳤
 * @param b ������Բ�Ķ̰��᳤
 * @param angle �����x�����ת�Ƕȣ���ʱ�룩���Ƕ��ƣ�
 * @param mode ����ģʽ��FILL�����1��CLEAR�����0
 * @return None
 */
// x' = xcosb - ysinb
// y' = xsinb + ycosb
void OLED_Draw_Two_Vertical_Ellipses_Rotate(u8 x0, u8 y0, u8 a, u8 b, u16 angle, u8 mode)
{
#if OLED_DRAW_ELLIPSE_BRESENHAM
	/* --------------- Bresenham --------------- */
	Point_Signed p = { .x = 0, .y = b };
	float d1 = b * b + a * a * (-b + 0.25);
	
	OLED_Draw_8_Pixels_Rotate(x0, y0, x0+p.x, y0+p.y, angle, mode);
	while (b * b * (p.x + 1) < a * a * (p.y - 0.5))
	{
		if (d1 <= 0)
		{
			d1 += b * b * (2 * p.x + 3);
			p.x++;
		}
		else
		{
			d1 += (b * b * (2 * p.x + 3) + a * a * (-2 * p.y + 2));
			p.x++;
			p.y--;
		}
		OLED_Draw_8_Pixels_Rotate(x0, y0, x0+p.x, y0+p.y, angle, mode);
	}
	float d2 = b * b * (p.x + 0.5) * (p.x + 0.5) + a * a * (p.y - 1) * (p.y - 1) - a * a * b * b;
	while (p.y > 0)
	{
		if (d2 <= 0)
		{
			d2 += b * b * (2 * p.x + 2) + a * a * (-2 * p.y + 3);
			p.x++;
			p.y--;
		}
		else
		{
			d2 += a * a * (-2 * p.y + 3);
			p.y--;
		}
		OLED_Draw_8_Pixels_Rotate(x0, y0, x0+p.x, y0+p.y, angle, mode);
	}
	/* --------------- Bresenham --------------- */
#endif
}

/**
 * @brief OLED��һ��Ŭŵ������
 * @param x0 �������ĵĺ�����
 * @param y0 �������ĵ�������
 * @param r Ŭŵ�����α߳���Ӧ���εİ뾶
 * @param mode ����ģʽ��FILL�����1��CLEAR�����0
 * @return None
 */
void OLED_Draw_Nuno_Triangle(u8 x0, u8 y0, u8 r, u8 mode)
{}

/* ---------------- base graphics ---------------- */

/* ---------------- derived graphics ---------------- */

/* ---------------- derived graphics ---------------- */

/* ---------------- animals ---------------- */

/**
 * @brief OLED����
 * @method ��β�������α�ʾ�������ò�����Բ��ʾ����ͷ��Ŭŵ�����α�ʾ
 * @param x0 �����ھ��εļ������ĵĺ�����
 * @param y0 �����ھ��εļ������ĵ�������
 * @param size ��Ĵ�С��ָ��ĳ���
 * @param dir ��ͷ�ķ���false ��ʾ����true ��ʾ����
 * @param mode ����ģʽ��FILL�����1��CLEAR�����0
 * @return None
 */
void OLED_Draw_Fish(u8 x0, u8 y0, u8 size, bool dir, u8 mode)
{}

/* ---------------- animals ---------------- */

/* ---------------- message ---------------- */

/**
 * @brief OLED��BMP
 * @param x BMP�����ϽǵĶ���ĺ�����
 * @param y BMP�����ϽǵĶ����������
 * @param p BMP������׵�ַ
 * @param width BMP�Ŀ��
 * @param height BMP�ĸ߶�
 * @param mode ����ģʽ��FILL�����1��CLEAR�����0
 * @return None
 */
void OLED_ShowBMP(u8 x,u8 y,const u8 *p,u8 width,u8 height,u8 mode)
{
	u8 u_x, u_y;
	u8 temp;
	u16 csize;
	u16 i, j;
	
	if(x>OLED_COL_MAX-1 || y>OLED_ROW_MAX-1) return;
	u_x = x; u_y = y;
	csize = (height/8 + ((height%8)?1:0))*width;
	for(i=0; i<csize; i++)
	{
		temp = *p;
		for(j=0; j<8; j++)
		{
			temp&0X80 ? OLED_DrawPoint(u_x,u_y,mode) : OLED_DrawPoint(u_x,u_y,!mode);
			temp <<= 1;
			u_y++;
			if((u_y-y)==height)
			{
				u_y = y;
				u_x++;
				break;
			}
		}
		p++;
	}
}

/**
 * @brief OLED�������
 * @details ִ����˺�����ֱ�Ӹ���OLED
 * @param x1 �����һ���Խ��ߵĶ˵�һ�ĺ�����
 * @param y1 �����һ���Խ��ߵĶ˵�һ��������
 * @param x2 �����һ���Խ��ߵĶ˵���ĺ�����
 * @param y2 �����һ���Խ��ߵĶ˵����������
 * @param mode ����ģʽ��FILL�����1��CLEAR�����0
 * @return None
 */
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 mode)
{  
	u8 x,y;  
	for(x=x1;x<=x2;x++)
	{
		for(y=y1;y<=y2;y++)OLED_DrawPoint(x,y,mode);
	}
	OLED_Refresh_Gram();
}

/**
 * @brief OLED��ʾ�ַ�
 * @param x �ַ������ϽǵĶ���ĺ�����
 * @param y �ַ������ϽǵĶ����������
 * @param chr ��Ҫ��ʾ���ַ�
 * @param size �ַ��Ĵ�С����12��15��24��ѡ
 * @param mode ����ģʽ��FILL�����1��CLEAR�����0
 * @return None
 */
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode)
{      			    
	u8 temp,t,t1;
	u8 y0=y;
	u8 csize=(size/8+((size%8)?1:0))*(size/2);
	chr=chr-' ';
    for(t=0;t<csize;t++)
    {   
		if(size==12)temp=asc2_1206[chr][t];
		else if(size==16)temp=asc2_1608[chr][t];
		else if(size==24)temp=asc2_2412[chr][t];
		else return;
        for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)OLED_DrawPoint(x,y,mode);
			else OLED_DrawPoint(x,y,!mode);
			temp<<=1;
			y++;
			if((y-y0)==size)
			{
				y=y0;
				x++;
				break;
			}
		}  	 
    }          
}

/**
 * @brief OLED��ʾ����
 * @param x ���ֵ����ϽǵĶ���ĺ�����
 * @param y ���ֵ����ϽǵĶ����������
 * @param num ��Ҫ��ʾ������
 * @param len ���ֵ�λ��
 * @param size ���ֵĴ�С����12��15��24��ѡ
 * @param mode ����ģʽ��FILL�����1��CLEAR�����0
 * @return None
 */
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size,u8 mode)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size/2)*t,y,' ',size,mode);
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size/2)*t,y,temp+'0',size,1); 
	}
}

/**
 * @brief OLED��ʾ�ַ���
 * @param x �ַ��������ϽǵĶ���ĺ�����
 * @param y �ַ��������ϽǵĶ����������
 * @param p ��Ҫ��ʾ���ַ������׵�ַ
 * @param size �ַ����Ĵ�С����12��15��24��ѡ
 * @param mode ����ģʽ��FILL�����1��CLEAR�����0
 * @return None
 */
void OLED_ShowString(u8 x,u8 y,const u8 *p,u8 size,u8 mode)
{	
    while((*p<='~')&&(*p>=' '))
    {       
        if(x>(OLED_COL_MAX-(size/2))){x=0;y+=size;}
        if(y>(OLED_ROW_MAX-size)){y=x=0;OLED_Clear();}
        OLED_ShowChar(x,y,*p,size,mode);	 
        x+=size/2;
        p++;
    }
}

/* ---------------- message ---------------- */

/* ---------------- inition ---------------- */

/**
 * @brief ��ʼ��OLED���IO
 * @return None
 */
static void OLED_GPIO_Init(void)
{
	SPI_GPIO_Init();
}

/**
 * @brief ��ʼ��OLED
 * @return None
 */
void OLED_Init(void)
{
	OLED_GPIO_Init();
#if USE_DMA && REFRESH_MODE == 2
	MyDMA_Init();
#endif
					  
	OLED_WR_Byte(0xAE,OLED_CMD);  // �ر�OLED��ʾ
	OLED_WR_Byte(0xD5,OLED_CMD);  // ����ʱ�ӷ�Ƶ���ӡ���Ƶ��
	OLED_WR_Byte(80,OLED_CMD);    // [3,0]��Ƶ���ӣ�[7,4]��Ƶ��
	OLED_WR_Byte(0xA8,OLED_CMD);  // ��������·��
	OLED_WR_Byte(0X3F,OLED_CMD);  // Ĭ��0X3F(1/64)
	OLED_WR_Byte(0xD3,OLED_CMD);  // ������ʾƫ��
	OLED_WR_Byte(0X00,OLED_CMD);  // Ĭ��Ϊ0

	OLED_WR_Byte(0x40,OLED_CMD);  // ������ʾ��ʼ�У�[5,0]����
								
	OLED_WR_Byte(0x8D,OLED_CMD);  // ��ɱ�����
	OLED_WR_Byte(0x14,OLED_CMD);  // bit2����ʼ/�ر�
	OLED_WR_Byte(0x20,OLED_CMD);  // �����ڴ��ַģʽ
	OLED_WR_Byte(0x01,OLED_CMD);  // [1,0]��01->�е�ַģʽ��10->�У�ҳ����ַģʽ��Ĭ��10
	OLED_WR_Byte(0xA1,OLED_CMD);  // ���ض������ã�bit0��0->0��1->127
	OLED_WR_Byte(0xC0,OLED_CMD);  // ����COMɨ�跽��[3,0]������ѡ����ͨģʽ
	OLED_WR_Byte(0xDA,OLED_CMD);  // ����COMӲ����������
	OLED_WR_Byte(0x12,OLED_CMD);  // [5,4]����
		 
	OLED_WR_Byte(0x81,OLED_CMD);  // �Աȶ�����
	OLED_WR_Byte(0x7F,OLED_CMD);  // �������ã�Խ��Խ����[7,0]��Ĭ��0X7F
	OLED_WR_Byte(0xD9,OLED_CMD);  // ����Ԥ�������
	OLED_WR_Byte(0xF1,OLED_CMD);  // [3,0]PHASE 1��[7,4]PHASE 2
	OLED_WR_Byte(0xDB,OLED_CMD);  // ����VCOMH��ѹ����
	OLED_WR_Byte(0x30,OLED_CMD);  // [6,4]��000->0.65*VCC��001->0.77*VCC��011->0.83*VCC

	OLED_WR_Byte(0xA4,OLED_CMD);  // ȫ����ʾ������bit0��1->������0->�رգ�����/������
	OLED_WR_Byte(0xA6,OLED_CMD);  // ������ʾ��ʽ��bit0��1->������ʾ��0->������ʾ
	OLED_WR_Byte(0xAF,OLED_CMD);  // ������ʾ
	
	OLED_Clear();
}

/* ---------------- inition ---------------- */
