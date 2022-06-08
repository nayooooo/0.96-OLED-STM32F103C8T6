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
/******************************************************************************

			八		一
		七				二
		六				三
			五		四

******************************************************************************/

#include "sys.h"
#include "oledfont.h"
#include "stdlib.h"
#include "math.h"
//OLED的显存
//存放格式如下.
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
//[4]0 1 2 3 ... 127	
//[5]0 1 2 3 ... 127	
//[6]0 1 2 3 ... 127	
//[7]0 1 2 3 ... 127
u8 OLED_GRAM[OLED_COL_MAX][OLED_PAGE_MAX];

//m^n
u32 mypow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;
	return result;
}

//向SSD1106写入一个字节。
//dat:要写入的数据/命令
//cmd:数据/命令标志 0,表示命令;1,表示数据;
static void OLED_WR_Byte(u8 dat,u8 cmd)
{
	u8 i;
	if(cmd) OLED_DC_Set();
	else OLED_DC_Clr();
	OLED_CS_Clr();
	for(i=0;i<8;i++)
	{
		OLED_SCLK_Clr();
		if(dat&0x80) OLED_SDIN_Set();
		else OLED_SDIN_Clr();
		OLED_SCLK_Set();
		dat<<=1;   
	}				 		  
	OLED_CS_Set();
	OLED_DC_Set();   	  
}

//更新缓存到OLED
void OLED_Refresh_Gram(void)
{
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);
		OLED_WR_Byte (0x00,OLED_CMD);
		OLED_WR_Byte (0x10,OLED_CMD);
		for(n=0;n<OLED_COL_MAX;n++)OLED_WR_Byte(OLED_GRAM[n][i],OLED_DATA); 
	}   
}

//OLED开显示
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);
	OLED_WR_Byte(0X14,OLED_CMD);
	OLED_WR_Byte(0XAF,OLED_CMD);
}

//OLED关显示
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);
	OLED_WR_Byte(0X10,OLED_CMD);
	OLED_WR_Byte(0XAE,OLED_CMD);
}

//OLED清屏
void OLED_Clear(void)  
{  
	u8 i,n;  
	for(i=0;i<OLED_PAGE_MAX;i++)for(n=0;n<OLED_COL_MAX;n++)OLED_GRAM[n][i]=0X00;  
	OLED_Refresh_Gram();
}

//OLED画点		   
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

/* ---------------- base graphics ---------------- */

//OLED画线
void OLED_DrawLine(u8 x1,u8 y1,u8 x2,u8 y2,u8 mode)
{
#if OLED_DRAW_LINE_BRESENHAM
	int16_t dx = x2 - x1, dy = y2 - y1;
	Point_Signed p = { .x = 0, .y = 0 };
	uint8_t Point_Position_Inverse_Flag = 0X00;  // 0,1,2,3,4bit分别表示第一、二、三、四象限及关于 y=x 对称标识
	
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
	else if(dx==0)			// k不存在
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
		//将 (x1, y1) 和 (x2, y2) 处理成第一象限中的情况
		//第三象限转换之后可以看成本来就是在第一象限中
		if(dx>0 && dy>0) Point_Position_Inverse_Flag |= 0X01;
		else if(dx>0 && dy <0)				//处理第二象限
		{
			Point_Position_Inverse_Flag |= 0X02;
			dy = -dy;
			y2 = 2 * y1 - y2;
		}
		else if(dx<0 && dy < 0)			//处理第三象限
		{
			Point_Position_Inverse_Flag |= 0X04;
			dx = -dx; dy = -dy;
			x1 ^= x2; x2 ^= x1; x1 ^= x2;
			y1 ^= y2; y2 ^= y1; y1 ^= y2;
		}
		else if(dx<0 && dy >0)		//处理第四象限
		{
			Point_Position_Inverse_Flag |= 0X08;
			dx = -dx;
			x2 = 2 * x1 - x2;
		}
		//将 (x1, y1) 和 (x2, y2) 处理成 0<k<=1 的情形（做变换 { x0'=x1, y0'=y1 }）
		if(dx<dy)
		{
			Point_Position_Inverse_Flag |= 0X10;
			Point const temp = { .x = x2, .y = y2 };
			x2 = x1 - y1 + temp.y;
			y2 = -x1 + y1 + temp.x;
			dx = x2 - x1; dy = y2 - y1;  // 重新计算 dx, dy 然后再处理成第一象限中的情况
		}
		
		int16_t incrE = 2 * dy, incrNE = 2 * (dy - dx);
		int16_t d = 2 * dy - dx;
		
		OLED_DrawPoint(p.x+x1, p.y+y1, mode);
		//开始计算点坐标偏移量
		for(p.x++; p.x<=dx; p.x++)
		{
			if(d<0){			//从东方衍生出新的像素
				d += incrE;
			}else{				//从东北方衍生出新的像素
				p.y++;
				d += incrNE;
			}
			
			//绘制点
			/* -- 相对于点 (x0, y0) 为原点的反函数之坐标 --
			   -- x'= x0-y0+y --
			   -- y'=-x0+y0+x -- */
			switch(Point_Position_Inverse_Flag&0X0F)
			{
				case 0X01:		//第一、三象限
				case 0X04:
					if(Point_Position_Inverse_Flag&0X10)
						OLED_DrawPoint(x1+p.y, y1+p.x, mode);
					else
						OLED_DrawPoint(x1+p.x, y1+p.y, mode);
					break;
				case 0X02:		//第二象限
					if(Point_Position_Inverse_Flag&0X10)
						OLED_DrawPoint(x1+p.y, y1-p.x, mode);
					else
						OLED_DrawPoint(x1+p.x, y1-p.y, mode);
					break;
				case 0X08:		//第四象限
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

//OLED角度画线
void OLED_DrawLine_Angle(u8 x0, u8 y0, u8 lenght, u16 angle, u8 mode)
{
	Point_Signed p;
	float rad = angle * (PI / 180);
	float sin_rad = sin(rad), cos_rad = cos(rad);
	
	p.x = lenght * cos_rad;
	p.y = lenght * sin_rad;
	
	OLED_DrawLine(x0, y0, p.x+x0, p.y+y0, mode);
}

//OLED画方
void OLED_DrawCube(u8 x1,u8 y1,u8 x2,u8 y2, u8 mode)
{
	OLED_DrawLine(x1,y1,x2,y1,mode);
	OLED_DrawLine(x1,y2,x2,y2,mode);
	OLED_DrawLine(x1,y1,x1,y2,mode);
	OLED_DrawLine(x2,y1,x2,y2,mode);
}

/**
 * @brief OLED画标准圆角矩形
 * @method 先按照矩形的画法画出一个缺角矩形，然后按照画圆的方法画出四个圆角
 * @param (x0, y0) 矩形中心
 * @param a,b 矩形的长和宽，默认长指的是上下边
 * @param r 圆角矩形的圆角的半径
 */
void OLED_Draw_Rounded_Cube(u8 x0, u8 y0, u8 a, u8 b, u8 r, u8 mode)
{
	Point p1, p2;
	
	p1.x = x0-a/2; p1.y = y0-b/2;
	p2.x = x0+a/2; p2.y = y0+b/2;
	
	//防止圆角溢出
	if(r>a/2 || r>b/2) r = (a<b)?(a/2):(b/2);
	
	//画缺角矩形
	OLED_DrawCube( p1.x, p1.y, p2.x, p2.y, mode );
	OLED_DrawLine( p1.x, p1.y, p1.x+r,	p1.y,	!mode);  // 上
	OLED_DrawLine( p2.x, p1.y, p2.x-r,	p1.y,	!mode);
	OLED_DrawLine( p1.x, p2.y, p1.x+r,	p2.y,	!mode);  // 下
	OLED_DrawLine( p2.x, p2.y, p2.x-r,	p2.y,	!mode);
	OLED_DrawLine( p1.x, p1.y, p1.x,	p1.y+r,	!mode);  // 左
	OLED_DrawLine( p1.x, p2.y, p1.x,	p2.y-r,	!mode);
	OLED_DrawLine( p2.x, p1.y, p2.x,	p1.y+r,	!mode);  // 右
	OLED_DrawLine( p2.x, p2.y, p2.x,	p2.y-r,	!mode);
	
	//画圆角
	Point p = {
		.x = 0,
		.y = r
	};
	// 因为取点是一区域
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
 * @brief OLED画标准圆角矩形，但是圆角会溢出
 * @method 先按照矩形的画法画出一个缺角矩形，然后按照画圆的方法画出四个圆角
 * @param (x0, y0) 矩形中心
 * @param a,b 矩形的长和宽，默认长指的是上下边
 * @param r 圆角矩形的圆角的半径
 */
void OLED_Draw_Rounded_Cube_Fillet_Overflow(u8 x0, u8 y0, u8 a, u8 b, u8 r, u8 mode)
{
	Point p1, p2;
	
	p1.x = x0-a/2; p1.y = y0-b/2;
	p2.x = x0+a/2; p2.y = y0+b/2;
	
	//画缺角矩形
	OLED_DrawCube( p1.x, p1.y, p2.x, p2.y, mode );
	OLED_DrawLine( p1.x, p1.y, p1.x+r,	p1.y,	!mode);  // 上
	OLED_DrawLine( p2.x, p1.y, p2.x-r,	p1.y,	!mode);
	OLED_DrawLine( p1.x, p2.y, p1.x+r,	p2.y,	!mode);  // 下
	OLED_DrawLine( p2.x, p2.y, p2.x-r,	p2.y,	!mode);
	OLED_DrawLine( p1.x, p1.y, p1.x,	p1.y+r,	!mode);  // 左
	OLED_DrawLine( p1.x, p2.y, p1.x,	p2.y-r,	!mode);
	OLED_DrawLine( p2.x, p1.y, p2.x,	p1.y+r,	!mode);  // 右
	OLED_DrawLine( p2.x, p2.y, p2.x,	p2.y-r,	!mode);
	
	//画圆角
	Point p = {
		.x = 0,
		.y = r
	};
	// 因为取点是一区域
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
 * @breief OLED放置 4 个像素
 * @param (x0, y0) 4个点的中心
 * @param (x, y) 4个点中的一个位于第一象限上的点
 */
void OLED_Draw_4_Pixels(u8 x0, u8 y0, u8 x, u8 y, u8 mode)
{
	OLED_DrawPoint(x,		y,			mode);				/* 第一象限 */
	OLED_DrawPoint(x,		2*y0-y,		mode);				/* 第二象限 */
	OLED_DrawPoint(2*x0-x,	2*y0-y,		mode);				/* 第三象限 */
	OLED_DrawPoint(2*x0-x,	y,			mode);				/* 第四象限 */
}

/**
 * @breief OLED放置 4 个绕中心旋转的像素
 * @param (x0, y0) 4个点的中心
 * @param (x, y) 4个点中的一个位于第一象限上的点
 */
// x' = xcosb - ysinb
// y' = xsinb + ycosb
// 计算对称点前需要变换一下坐标，因此使用 int16_t 数据类型以满足负数和范围需求
void OLED_Draw_4_Pixels_Rotate(u8 x0, u8 y0, int16_t x, int16_t y, u16 angle, u8 mode)
{
	float rad = angle * (PI / 180);
	float sin_rad = sin(rad), cos_rad = cos(rad);
	x -= x0; y -= y0;  // 得到基础偏移量
	float x_s = x*sin_rad,		x_c = x*cos_rad,		y_s = y*sin_rad,		y_c = y*cos_rad;
	float _x_s = (-x)*sin_rad,	_x_c = (-x)*cos_rad,	_y_s = (-y)*sin_rad,	_y_c = (-y)*cos_rad;
	
	//注意加上中心点的坐标，因为计算结果是偏移量
	OLED_DrawPoint(x_c-y_s		+x0,	x_s+y_c		+y0,	mode);					/* 第一象限 */
	OLED_DrawPoint(x_c-_y_s		+x0,	x_s+_y_c	+y0,	mode);					/* 第二象限 */
	OLED_DrawPoint(_x_c-_y_s	+x0,	_x_s+_y_c	+y0,	mode);					/* 第三象限 */
	OLED_DrawPoint(_x_c-y_s		+x0,	_x_s+y_c	+y0,	mode);					/* 第四象限 */
}

/**
 * @breief OLED放置 8 个像素
 * @param (x0, y0) 8个点的中心
 * @param (x, y) 8个点中的一个位于一区域上的点
 * --------------------------------------------------- *
 * 一	x = x,					y = y
 * 二	x = y -y0+x0,			y = x-x0+y0
 * 三	x = y-y0+x0,			y = 2*y0-(x-x0+y0)
 * 四	x = x,					y = 2*y0-y
 * 五	x = 2*x0-x,				y = 2*y0-y
 * 六	x = 2*x0-(y-y0+x0),		y = 2*y0-(x-x0+y0)
 * 七	x = 2*x0-(y-y0+x0),		y = x-x0+y0
 * 八	x = 2*x0-x,				y = y
 * --------------------------------------------------- *
 */
void OLED_Draw_8_Pixels(u8 x0, u8 y0, u8 x, u8 y, u8 mode)
{
	OLED_DrawPoint(x,			y,				mode);				/* 一 */
	OLED_DrawPoint(y+x0-y0,		x-x0+y0,		mode);				/* 二 */
	OLED_DrawPoint(y+x0-y0,		-x+x0+y0,		mode);				/* 三 */
	OLED_DrawPoint(x,			-y+y0+y0,		mode);				/* 四 */
	OLED_DrawPoint(-x+x0+x0,	-y+y0+y0,		mode);				/* 五 */
	OLED_DrawPoint(-y+x0+y0,	-x+x0+y0,		mode);				/* 六 */
	OLED_DrawPoint(-y+x0+y0,	x-x0+y0,		mode);				/* 七 */
	OLED_DrawPoint(-x+x0+x0,	y,				mode);				/* 八 */
}

/**
 * @brief 以某一点为中心绘制8个点，这八个点和中心点之间的距离为 (dx, dy)
 * @param (x0, y0) 对称中心
 * @param (x, y) 需要绘制的8个点中，位于一区域的点
 * @param (dx, dy) 偏移距离
 */
void OLED_Draw_8_Pixels_Spread_Out_From_Center(u8 x0, u8 y0, u8 x, u8 y, u8 dx, u8 dy, u8 mode)
{
	OLED_DrawPoint(x		+dx,	y			+dy,	mode);		/* 一 */
	OLED_DrawPoint(y+x0-y0	+dx,	x-x0+y0		+dy,	mode);		/* 二 */
	OLED_DrawPoint(y+x0-y0	+dx,	-x+x0+y0	-dy,	mode);		/* 三 */
	OLED_DrawPoint(x		+dx,	-y+y0+y0	-dy,	mode);		/* 四 */
	OLED_DrawPoint(-x+x0+x0	-dx,	-y+y0+y0	-dy,	mode);		/* 五 */
	OLED_DrawPoint(-y+x0+y0	-dx,	-x+x0+y0	-dy,	mode);		/* 六 */
	OLED_DrawPoint(-y+x0+y0	-dx,	x-x0+y0		+dy,	mode);		/* 七 */
	OLED_DrawPoint(-x+x0+x0	-dx,	y			+dy,	mode);		/* 八 */
}

/**
 * @breief OLED放置 8 个绕中心旋转的像素
 * @param (x0, y0) 8个点的中心
 * @param (x, y) 8个点中的一个位于第一象限上的点
 */
// x' = xcosb - ysinb
// y' = xsinb + ycosb
// 计算对称点前需要变换一下坐标，因此使用 int16_t 数据类型以满足负数和范围需求
void OLED_Draw_8_Pixels_Rotate(u8 x0, u8 y0, int16_t x, int16_t y, u16 angle, u8 mode)
{
	float rad = angle * (PI / 180);
	float sin_rad = sin(rad), cos_rad = cos(rad);
	//计算旋转后的偏移量（下方这些并没有真正算出偏移量，只是减少计算时间），1表示负号，大写的 X,Y 用来区分横纵坐标
	float	x_X_s = (x-x0)*sin_rad,				x_X_c = (x-x0)*cos_rad,\
			y_Y_s = (y-y0)*sin_rad,				y_Y_c = (y-y0)*cos_rad,\
			y_x_1y_X_s = (y+x0-y0-x0)*sin_rad,	y_x_1y_X_c = (y+x0-y0-x0)*cos_rad,\
			x_1x_y_Y_s = (x-x0+y0-y0)*sin_rad,	x_1x_y_Y_c = (x-x0+y0-y0)*cos_rad,\
			_x_x_y_Y_s = (-x+x0+y0-y0)*sin_rad,	_x_x_y_Y_c = (-x+x0+y0-y0)*cos_rad,\
			_y_x_y_X_s = (-y+x0+y0-x0)*sin_rad,	_y_x_y_X_c = (-y+x0+y0-x0)*cos_rad,\
			_x_x_x_X_s = (-x+x0+x0-x0)*sin_rad,	_x_x_x_X_c = (-x+x0+x0-x0)*cos_rad,\
			_y_y_y_Y_s = (-y+y0+y0-y0)*sin_rad,	_y_y_y_Y_c = (-y+y0+y0-y0)*cos_rad;
	
	//注意加上中心点的坐标，因为计算结果是偏移量
	OLED_DrawPoint( x_X_c-y_Y_s				+x0,	x_X_s+y_Y_c				+y0,	mode );				/* 一 */
	OLED_DrawPoint( y_x_1y_X_c-x_1x_y_Y_s	+x0,	y_x_1y_X_s+x_1x_y_Y_c	+y0,	mode );				/* 二 */
	OLED_DrawPoint( y_x_1y_X_c-_x_x_y_Y_s	+x0,	y_x_1y_X_s+_x_x_y_Y_c	+y0,	mode );				/* 三 */
	OLED_DrawPoint( x_X_c-_y_y_y_Y_s		+x0,	x_X_s+_y_y_y_Y_c		+y0,	mode );				/* 四 */
	OLED_DrawPoint( _x_x_x_X_c-_y_y_y_Y_s	+x0,	_x_x_x_X_s+_y_y_y_Y_c	+y0,	mode );				/* 五 */
	OLED_DrawPoint( _y_x_y_X_c-_x_x_y_Y_s	+x0,	_y_x_y_X_s+_x_x_y_Y_c	+y0,	mode );				/* 六 */
	OLED_DrawPoint( _y_x_y_X_c-x_1x_y_Y_s	+x0,	_y_x_y_X_s+x_1x_y_Y_c	+y0,	mode );				/* 七 */
	OLED_DrawPoint( _x_x_x_X_c-y_Y_s		+x0,	_x_x_x_X_s+y_Y_c		+y0,	mode );				/* 八 */
}

//OLED画圆
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

//OLED画标准椭圆
void OLED_DrawEllipse(u8 x0, u8 y0, u8 a, u8 b, u8 mode)
{
#if OLED_DRAW_ELLIPSE_METHOD==0
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

//OLED画两个相互垂直的标准椭圆
void OLED_Draw_Two_Vertical_Ellipses(u8 x0, u8 y0, u8 a, u8 b, u8 mode)
{
#if OLED_DRAW_ELLIPSE_METHOD==0
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

//OLED画非标准椭圆
// x' = xcosb - ysinb
// y' = xsinb + ycosb
void OLED_DrawEllipse_Rotate(u8 x0, u8 y0, u8 a, u8 b, u16 angle, u8 mode)
{
#if OLED_DRAW_ELLIPSE_METHOD==0
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

//OLED画两个相互垂直的非标准椭圆
// x' = xcosb - ysinb
// y' = xsinb + ycosb
void OLED_Draw_Two_Vertical_Ellipses_Rotate(u8 x0, u8 y0, u8 a, u8 b, u16 angle, u8 mode)
{
#if OLED_DRAW_ELLIPSE_METHOD==0
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

/* ---------------- base graphics ---------------- */

/* ---------------- derived graphics ---------------- */

/* ---------------- derived graphics ---------------- */

/* ---------------- message ---------------- */

//OLED画BMP
void OLED_ShowBMP(u8 x,u8 y,const u8 *p,u8 width,u8 height,u8 mode)
{
	u8 u_x, u_y;
	u8 temp;
	u8 csize;
	u8 i, j;
	
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

//OLED填充
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 mode)
{  
	u8 x,y;  
	for(x=x1;x<=x2;x++)
	{
		for(y=y1;y<=y2;y++)OLED_DrawPoint(x,y,mode);
	}
	OLED_Refresh_Gram();
}

//OLED显示字符
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

//OLED显示数字
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

//OLED显示字符串
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

static void OLED_SPI_GPIO_Init(void)
{
	My_SPI_Init();
}

//初始化OLED
void OLED_SPI_Init(void)
{
	OLED_SPI_GPIO_Init();
					  
	OLED_WR_Byte(0xAE,OLED_CMD);
	OLED_WR_Byte(0xD5,OLED_CMD);
	OLED_WR_Byte(80,OLED_CMD);  
	OLED_WR_Byte(0xA8,OLED_CMD);
	OLED_WR_Byte(0X3F,OLED_CMD);
	OLED_WR_Byte(0xD3,OLED_CMD);
	OLED_WR_Byte(0X00,OLED_CMD);

	OLED_WR_Byte(0x40,OLED_CMD);
								
	OLED_WR_Byte(0x8D,OLED_CMD);
	OLED_WR_Byte(0x14,OLED_CMD);
	OLED_WR_Byte(0x20,OLED_CMD);
	OLED_WR_Byte(0x02,OLED_CMD);
	OLED_WR_Byte(0xA1,OLED_CMD);
	OLED_WR_Byte(0xC0,OLED_CMD);
	OLED_WR_Byte(0xDA,OLED_CMD);
	OLED_WR_Byte(0x12,OLED_CMD);
		 
	OLED_WR_Byte(0x81,OLED_CMD);
	OLED_WR_Byte(0xEF,OLED_CMD);
	OLED_WR_Byte(0xD9,OLED_CMD);
	OLED_WR_Byte(0xf1,OLED_CMD);
	OLED_WR_Byte(0xDB,OLED_CMD);
	OLED_WR_Byte(0x30,OLED_CMD);

	OLED_WR_Byte(0xA4,OLED_CMD);
	OLED_WR_Byte(0xA6,OLED_CMD);
	OLED_WR_Byte(0xAF,OLED_CMD);
	
	OLED_Clear();
}

/* ---------------- inition ---------------- */
