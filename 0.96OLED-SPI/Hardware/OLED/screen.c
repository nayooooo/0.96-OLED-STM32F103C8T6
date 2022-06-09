#include "sys.h"
#include <stdlib.h>
#include <math.h>

/* --------------- 基础元素 --------------- */

void OLED_Show_Picture(void)
{
	OLED_DrawCube(0, 0, OLED_COL_MAX-1, OLED_ROW_MAX-1, FILL);
	OLED_DrawLine(0, 0, OLED_COL_MAX-1, OLED_ROW_MAX-1, FILL);
	OLED_DrawLine(0, OLED_ROW_MAX-1, OLED_COL_MAX-1, 0, FILL);
	OLED_Refresh_Gram();
}

/**
 * @brief 画一个从最大缩小到指定位置的方形
 * @param (x0, y0) 缩小完成后的几何中心
 * @param (a, b) 缩小完成后的长宽，默认上下边为长
 */
void OLED_Show_Shrinking_Cube(u8 const x0, u8 const y0, u8 const a, u8 const b, u8 mode)
{
	uint8_t Shr_a = OLED_COL_MAX-1, Shr_b = OLED_ROW_MAX-1;
	
	for(;Shr_a>a && Shr_b>b;)
	{
		if(Shr_a>a) Shr_a--;
		if(Shr_b>b) Shr_b--;
		OLED_DrawCube_Intelligent_Overflow(x0, y0, Shr_a, Shr_b, mode);
		OLED_Refresh_Gram();
		OLED_DrawCube_Intelligent_Overflow(x0, y0, Shr_a, Shr_b, !mode);
	}
}

/**
 * @brief 画一个旋转直线
 * @param (x0, y0) 旋转中心
 * @param ledght 直线长度
 */
void OLED_Draw_Rotating_Line_Angle(u8 const x0, u8 const y0, u8 const lenght, u8 mode)
{
	uint16_t i;
	
	for(i=0; i<360; i++)
	{
		OLED_DrawLine_Angle(x0, y0, lenght, i, mode);
		OLED_Refresh_Gram();
		OLED_DrawLine_Angle(x0, y0, lenght, i, !mode);
	}
}

/**
 * @brief 画一个正在填充的园
 * @param (x0, y0) 圆心
 * @param r 圆半径
 * @param dir 填充方向，0表示向里填充，1表示向外填充
 */
void OLED_Draw_Filling_Circle(u8 const x0, u8 const y0, u8 const r, u8 dir, u8 mode)
{
	uint8_t i;
	
	switch(dir)
	{
		case Fill_Out:
			for(i=0; i<=r; i++)
			{
				OLED_DrawCircle(x0, y0, i, mode);
				OLED_Refresh_Gram();
			}
			break;
		case Fill_Into:
			for(i=r; i>0; i--)
			{
				OLED_DrawCircle(x0, y0, i, mode);
				OLED_Refresh_Gram();
			}
			//半径为0的圆
			OLED_DrawCircle(x0, y0, i, mode);
			OLED_Refresh_Gram();
			break;
		default: break;
	}
}

/**
 * @brief 画一个矩形变化到圆
 * @param (x0, y0) 几何中心
 * @param (a, b) 长和宽，默认上下边是长
 */
void OLED_Show_Cube_to_Circle(u8 const x0, u8 const y0, u8 const a, u8 const b, u8 mode)
{
	u8 i, j;
	u8 flag = (a>b)?(0):(1);  // 0表示a>b
	u8 a_b_min = (a<b)?(a):(b);
	u8 a_b_max = (a>b)?(a):(b);
	
	for(i=0; i<a_b_min/2; i++)
	{
		OLED_Draw_Rounded_Cube(x0, y0, a, b, i, mode);
		OLED_Refresh_Gram();
		OLED_Draw_Rounded_Cube(x0, y0, a, b, i, !mode);
	}
	for(j=a_b_max-1; j>a_b_min/2; j--)
	{
		OLED_Draw_Rounded_Cube(x0, y0, (flag)?(a):(j), (flag)?(j):(b), i, mode);
		OLED_Refresh_Gram();
		OLED_Draw_Rounded_Cube(x0, y0, (flag)?(a):(j), (flag)?(j):(b), i, !mode);
	}
	for(; j<a_b_max; j++)
	{
		OLED_Draw_Rounded_Cube(x0, y0, (flag)?(a):(j), (flag)?(j):(b), i, mode);
		OLED_Refresh_Gram();
		OLED_Draw_Rounded_Cube(x0, y0, (flag)?(a):(j), (flag)?(j):(b), i, !mode);
	}
	for(; i>0; i--)
	{
		OLED_Draw_Rounded_Cube(x0, y0, a, b, i, mode);
		OLED_Refresh_Gram();
		OLED_Draw_Rounded_Cube(x0, y0, a, b, i, !mode);
	}
}

/**
 * @brief 画一个圆角矩形，它的圆角的半径不断变化
 * @param (x0, y0) 圆心
 * @param r 圆半径
 * @param dir 填充方向，0表示向里填充，1表示向外填充
 */
void OLED_Show_Cube_to_LikeCircle(u8 const x0, u8 const y0, u8 const a, u8 const b, u8 mode)
{
	u8 i;
	u8 a_b_min = (a<b)?(a):(b);
	
	for(i=0; i<a_b_min/2; i++)
	{
		OLED_Draw_Rounded_Cube(x0, y0, a, b, i, mode);
		OLED_Refresh_Gram();
		OLED_Draw_Rounded_Cube(x0, y0, a, b, i, !mode);
	}
	for(i--; i>0; i--)
	{
		OLED_Draw_Rounded_Cube(x0, y0, a, b, i, mode);
		OLED_Refresh_Gram();
		OLED_Draw_Rounded_Cube(x0, y0, a, b, i, !mode);
	}
}

/**
 * @brief 画一个圆角矩形，它的圆角的半径不断变化，并且圆角会溢出
 * @param (x0, y0) 几何中心
 * @param (a, b) 长和宽，默认上下边是长
 */
void OLED_Show_Cube_to_LikeCircle_Fillet_Overflow(u8 const x0, u8 const y0, u8 const a, u8 const b, u8 mode)
{
	u8 i;
	u8 a_b_min = (a<b)?(a):(b);
	
	for(i=0; i<a_b_min/2; i++)
	{
		OLED_Draw_Rounded_Cube_Fillet_Overflow(x0, y0, a, b, i, mode);
		OLED_Refresh_Gram();
		OLED_Draw_Rounded_Cube_Fillet_Overflow(x0, y0, a, b, i, !mode);
	}
	for(i--; i>0; i--)
	{
		OLED_Draw_Rounded_Cube_Fillet_Overflow(x0, y0, a, b, i, mode);
		OLED_Refresh_Gram();
		OLED_Draw_Rounded_Cube_Fillet_Overflow(x0, y0, a, b, i, !mode);
	}
}

/**
 * @brief 画一个矩形变化到椭圆
 * @param (x0, y0) 几何中心
 * @param (a, b) 同时为矩形的长和宽的一半及椭圆的长半轴长和短半轴长
 */
void OLED_Show_Cube_Catched_by_Elliipse(u8 const x0, u8 const y0, u8 const a, u8 const b, u8 mode)
{
	uint8_t a_temp = 0, b_temp = 0;
	Point_Signed p;
	float d1;
	
	while(a_temp<a || b_temp<b)
	{
		{
			p.x = 0; p.y = b_temp;
			d1 = b * b + a * a * (-b + 0.25);
			OLED_Draw_4_Pixels_Spread_Out_From_Center(x0, y0, x0+p.x, y0+p.y, a-a_temp, b-b_temp, mode);
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
				OLED_Draw_4_Pixels_Spread_Out_From_Center(x0, y0, x0+p.x, y0+p.y, a-a_temp, b-b_temp, mode);
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
				OLED_Draw_4_Pixels_Spread_Out_From_Center(x0, y0, x0+p.x, y0+p.y, a-a_temp, b-b_temp, mode);
			}
		}
		OLED_DrawLine(x0-a+a_temp, y0+b-b_temp, x0+a-a_temp, y0+b-b_temp, mode);
		OLED_DrawLine(x0-a+a_temp, y0-b+b_temp, x0+a-a_temp, y0-b+b_temp, mode);
		OLED_DrawLine(x0-a+a_temp, y0+b-b_temp, x0-a+a_temp, y0-b+b_temp, mode);
		OLED_DrawLine(x0+a-a_temp, y0+b-b_temp, x0+a-a_temp, y0-b+b_temp, mode);
		OLED_Refresh_Gram();
		OLED_DrawLine(x0-a+a_temp, y0+b-b_temp, x0+a-a_temp, y0+b-b_temp, !mode);
		OLED_DrawLine(x0-a+a_temp, y0-b+b_temp, x0+a-a_temp, y0-b+b_temp, !mode);
		OLED_DrawLine(x0-a+a_temp, y0+b-b_temp, x0-a+a_temp, y0-b+b_temp, !mode);
		OLED_DrawLine(x0+a-a_temp, y0+b-b_temp, x0+a-a_temp, y0-b+b_temp, !mode);
		{
			p.x = 0; p.y = b_temp;
			d1 = b * b + a * a * (-b + 0.25);
			OLED_Draw_4_Pixels_Spread_Out_From_Center(x0, y0, x0+p.x, y0+p.y, a-a_temp, b-b_temp, !mode);
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
				OLED_Draw_4_Pixels_Spread_Out_From_Center(x0, y0, x0+p.x, y0+p.y, a-a_temp, b-b_temp, !mode);
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
				OLED_Draw_4_Pixels_Spread_Out_From_Center(x0, y0, x0+p.x, y0+p.y, a-a_temp, b-b_temp, !mode);
			}
		}
		if(a_temp<a) a_temp++;
		if(b_temp<a) b_temp++;
	}
}

/**
 * @brief 画一个DNA，由可溢出圆角矩形制作
 * @tips 还没有写出来
 * @param (x0, y0) 几何中心
 * @param (a, b) 长和宽，默认上下边是长
 */
void OLED_Show_DNA(u8 const x0, u8 const y0, u8 const a, u8 const b, u8 mode)
{
	u8 i;
	u8 a_b_min = (a<b)?(a):(b);
	
	for(i=0; i<a_b_min; i++)
	{
		OLED_Draw_Rounded_Cube_Fillet_Overflow(x0, y0, a, b, i, mode);
		OLED_Refresh_Gram();
		OLED_Draw_Rounded_Cube_Fillet_Overflow(x0, y0, a, b, i, !mode);
	}
	for(i--; i>0; i--)
	{
		OLED_Draw_Rounded_Cube_Fillet_Overflow(x0, y0, a, b, i, mode);
		OLED_Refresh_Gram();
		OLED_Draw_Rounded_Cube_Fillet_Overflow(x0, y0, a, b, i, !mode);
	}
	//清理残余
	OLED_Draw_Rounded_Cube_Fillet_Overflow(x0, y0, a, b, i, !mode);
}

/* --------------- 基础元素 --------------- */

/* --------------- 判断函数 --------------- */

static bool Judge_Ball_is_Out_of_Bounds(Circle_Dir *c)
{
	if(c->c.x0<c->c.r-1 || c->c.x0>OLED_COL_MAX-c->c.r-1 ||\
		c->c.y0<c->c.r-1 || c->c.y0>OLED_ROW_MAX-c->c.r-1)
		return true;
	return false;
}

//判断两个圆是否嵌入
static bool Judge_Two_Balls_is_Embed(Circle_Dir *c1, Circle_Dir *c2)
{
	if(fabs(c1->c.x0-c2->c.x0)<c1->c.r+c2->c.r &&\
		fabs(c1->c.y0-c2->c.y0)<c1->c.r+c2->c.r)
		return true;
	return false;
}

/* --------------- 判断函数 --------------- */

/* --------------- 动画 --------------- */

/**
 * @brief 推拉门
 * @param (x0, y0) 几何中心
 * @param (a, b) 长和宽，默认上下边是长
 */
void OLED_Show_Sliding_Door(u8 const x0, u8 const y0, u8 const a, u8 const b, u8 mode)
{
	u8 i, j;
	u8 flag = (a>b)?(0):(1);  // 0表示a>b
	u8 a_b_min = (a<b)?(a):(b);
	u8 a_b_max = (a>b)?(a):(b);
	
	for(i=0; i<a_b_min/2; i++)
	{
		OLED_Draw_Rounded_Cube_Erasure_Method(x0, y0, a, b, i, mode);
		OLED_Refresh_Gram();
		OLED_Draw_Rounded_Cube_Erasure_Method(x0, y0, a, b, i, !mode);
	}
	//使用OLED_Draw_Rounded_Cube_Erasure_Method清除痕迹会因为画缺角时的反转而留下痕迹
	for(j=a_b_max-1; j>a_b_min/2; j--)
	{
		OLED_Draw_Rounded_Cube_Erasure_Method(x0, y0, (flag)?(a):(j), (flag)?(j):(b), i, mode);
		OLED_Refresh_Gram();
		OLED_Draw_Rounded_Cube_Erasure_Method(x0, y0, (flag)?(a):(j), (flag)?(j):(b), i, !mode);
	}
	for(; j<a_b_max; j++)
	{
		OLED_Draw_Rounded_Cube_Erasure_Method(x0, y0, (flag)?(a):(j), (flag)?(j):(b), i, mode);
		OLED_Refresh_Gram();
		OLED_Draw_Rounded_Cube_Erasure_Method(x0, y0, (flag)?(a):(j), (flag)?(j):(b), i, !mode);
	}
	for(; i>0; i--)
	{
		OLED_Draw_Rounded_Cube_Erasure_Method(x0, y0, a, b, i, mode);
		OLED_Refresh_Gram();
		OLED_Draw_Rounded_Cube_Erasure_Method(x0, y0, a, b, i, !mode);
	}
}

/*************** 暂时没有做 a<b 的 ***************/

void OLED_Show_Boncing_Ball(void)
{
	static Circle_Dir circle_1 = {
		.c.x0 = OLED_COL_MAX/2-1,
		.c.y0 = OLED_ROW_MAX/2-1,
		.c.r = 10,
		.dir = 0
	};
	static Circle_Dir circle_2 = {
		.c.x0 = OLED_COL_MAX/4-1,
		.c.y0 = OLED_ROW_MAX/4-1,
		.c.r = 10,
		.dir = 0
	};
	
	OLED_DrawCircle(circle_1.c.x0, circle_1.c.y0, circle_1.c.r, CLEAR);
	OLED_DrawCircle(circle_2.c.x0, circle_2.c.y0, circle_2.c.r, CLEAR);
	// 移动 circle_1
	circle_1.dir = rand() % Circle_Dir_Max;
	switch(circle_1.dir)
	{
		case Circle_Dir_Up:
			circle_1.c.y0--;
			if(Judge_Two_Balls_is_Embed(&circle_1, &circle_2) || Judge_Ball_is_Out_of_Bounds(&circle_1))
				circle_1.c.y0++;
		break;
		case Circle_Dir_Down:
			circle_1.c.y0++;
			if(Judge_Two_Balls_is_Embed(&circle_1, &circle_2) || Judge_Ball_is_Out_of_Bounds(&circle_1))
				circle_1.c.y0--;
		break;
		case Circle_Dir_Left:
			circle_1.c.x0--;
			if(Judge_Two_Balls_is_Embed(&circle_1, &circle_2) || Judge_Ball_is_Out_of_Bounds(&circle_1))
				circle_1.c.x0++;
		break;
		case Circle_Dir_Right:
			circle_1.c.x0++;
			if(Judge_Two_Balls_is_Embed(&circle_1, &circle_2) || Judge_Ball_is_Out_of_Bounds(&circle_1))
				circle_1.c.x0--;
		break;
		default: break;
	}
	// 移动 circle_2
	circle_2.dir = rand() % Circle_Dir_Max;
	switch(circle_2.dir)
	{
		case Circle_Dir_Up:
			circle_2.c.y0--;
			if(Judge_Two_Balls_is_Embed(&circle_1, &circle_2) || Judge_Ball_is_Out_of_Bounds(&circle_2))
				circle_2.c.y0++;
		break;
		case Circle_Dir_Down:
			circle_2.c.y0++;
			if(Judge_Two_Balls_is_Embed(&circle_1, &circle_2) || Judge_Ball_is_Out_of_Bounds(&circle_2))
				circle_2.c.y0--;
		break;
		case Circle_Dir_Left:
			circle_2.c.x0--;
			if(Judge_Two_Balls_is_Embed(&circle_1, &circle_2) || Judge_Ball_is_Out_of_Bounds(&circle_2))
				circle_2.c.x0++;
		break;
		case Circle_Dir_Right:
			circle_2.c.x0++;
			if(Judge_Two_Balls_is_Embed(&circle_1, &circle_2) || Judge_Ball_is_Out_of_Bounds(&circle_2))
				circle_2.c.x0--;
		break;
		default: break;
	}
	OLED_DrawCircle(circle_1.c.x0, circle_1.c.y0, circle_1.c.r, FILL);
	OLED_DrawCircle(circle_2.c.x0, circle_2.c.y0, circle_2.c.r, FILL);
	OLED_Refresh_Gram();
}

void OLED_Show_Live_of_Circle(u8 const x0, u8 const y0, u8 const r, u8 mode)
{
	OLED_Draw_Filling_Circle(x0, y0, r, Fill_Out, mode);
	OLED_Draw_Filling_Circle(x0, y0, r, Fill_Out, !mode);
	OLED_Draw_Filling_Circle(x0, y0, r, Fill_Into, mode);
	OLED_Draw_Filling_Circle(x0, y0, r, Fill_Into, !mode);
}

void OLED_Show_Filling_Circle_to_Whole_Screen(u8 const x0, u8 const y0, u8 const r, u8 mode)
{
	uint8_t i;
	uint8_t const R = sqrt((pow(OLED_ROW_COL_MAX,2)+pow(OLED_ROW_COL_MIN,2))/4);
	
	for(i=0; i<=R; i++)
	{
		OLED_DrawCircle(x0, y0, i, mode);
		OLED_Refresh_Gram();
	}
}

void OLED_Show_Rotating_Ellipse(u8 const x0, u8 const y0, u8 const a, u8 const b, u16 const angle, u8 mode)
{
	uint16_t i;
	
	for(i=0; i<angle; i++)
	{
		OLED_DrawEllipse_Rotate(x0, y0, a, b, i, mode);
		OLED_Refresh_Gram();
		OLED_DrawEllipse_Rotate(x0, y0, a, b, i, !mode);
	}
}

void OLED_Show_Rotating_Two_Vertical_Ellipses(u8 const x0, u8 const y0, u8 const a, u8 const b, u16 const angle, u8 mode)
{
	uint16_t i;
	
	for(i=0; i<angle; i++)
	{
		OLED_Draw_Two_Vertical_Ellipses_Rotate(x0, y0, a, b, i, mode);
		OLED_Refresh_Gram();
		OLED_Draw_Two_Vertical_Ellipses_Rotate(x0, y0, a, b, i, !mode);
	}
}

void OLED_Show_Rotating_Telescoping_Two_Vertical_Ellipses(u8 const x0, u8 const y0, u8 const a, u8 const b, u16 const angle, u8 mode)
{
	uint8_t i, j;
	uint8_t dir = 1;  // 1表示伸，0表示缩
	uint16_t k;
	
	i = a-b; j = 0;
	for(k=0; k<angle; k++)
	{
		if(j==0) dir = 1;
		else if(j==b) dir = 0;
		if(dir) i++, j++;
		else i--, j--;
		OLED_Draw_Two_Vertical_Ellipses_Rotate(x0, y0, i, j, k, mode);
		OLED_Refresh_Gram();
		OLED_Draw_Two_Vertical_Ellipses_Rotate(x0, y0, i, j, k, !mode);
	}
}

/* --------------- 动画 --------------- */

/* --------------- 组合 --------------- */

void Demo_01(void)
{
	OLED_Show_Shrinking_Cube(63, 31, 60, 30, FILL);
	OLED_Show_Cube_to_Circle(63, 31, 60, 30, FILL);
	OLED_Show_Cube_Catched_by_Elliipse(63, 31, 30, 20, FILL);
	OLED_Show_Rotating_Ellipse(63, 31, 30, 20, 180, FILL);
	OLED_Show_Rotating_Two_Vertical_Ellipses(63, 31, 30, 20, 180, FILL);
	OLED_Show_Rotating_Telescoping_Two_Vertical_Ellipses(63, 31, 30, 20, 180, FILL);
	OLED_Show_Sliding_Door(63, 31, 60, 30, FILL);
	OLED_Show_Cube_to_LikeCircle(63, 31, 60, 30, FILL);
	OLED_Show_DNA(63, 31, 60, 30, FILL);
}

/* --------------- 组合 --------------- */
