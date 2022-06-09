#ifndef __SCREEN_H
#define __SCREEN_H

#include "sys.h"

typedef struct{
	Circle c;
	u8 dir;
}Circle_Dir;

typedef enum{
	Circle_Dir_Up,
	Circle_Dir_Down,
	Circle_Dir_Left,
	Circle_Dir_Right,
	Circle_Dir_Max
}Circle_Dir_Typedef;

typedef enum{
	Fill_Into,
	Fill_Out
}Fill_Dir_Typedef;

void OLED_Show_Picture(void);
void OLED_Draw_Rotating_Line_Angle(u8 const x0, u8 const y0, u8 const lenght, u8 mode);
void OLED_Draw_Filling_Circle(u8 const x0, u8 const y0, u8 const r, u8 dir, u8 mode);

static bool Judge_Ball_is_Out_of_Bounds(Circle_Dir *c);
static bool Judge_Two_Balls_is_Embed(Circle_Dir *c1, Circle_Dir *c2);

void OLED_Show_Sliding_Door(u8 const x0, u8 const y0, u8 const a, u8 const b, u8 mode);
void OLED_Show_Boncing_Ball(void);

void OLED_Show_Shrinking_Cube(u8 const x0, u8 const y0, u8 const a, u8 const b, u8 mode);
void OLED_Show_Live_of_Circle(u8 const x0, u8 const y0, u8 const r, u8 mode);
void OLED_Show_Filling_Circle_to_Whole_Screen(u8 const x0, u8 const y0, u8 const r, u8 mode);
void OLED_Show_Rotating_Ellipse(u8 const x0, u8 const y0, u8 const a, u8 const b, u16 const angle, u8 mode);
void OLED_Show_Rotating_Two_Vertical_Ellipses(u8 const x0, u8 const y0, u8 const a, u8 const b, u16 const angle, u8 mode);
void OLED_Show_Rotating_Telescoping_Two_Vertical_Ellipses(u8 const x0, u8 const y0, u8 const a, u8 const b, u16 const angle, u8 mode);
void OLED_Show_Cube_to_Circle(u8 const x0, u8 const y0, u8 const a, u8 const b, u8 mode);
void OLED_Show_Cube_to_LikeCircle(u8 const x0, u8 const y0, u8 const a, u8 const b, u8 mode);
void OLED_Show_Cube_to_LikeCircle_Fillet_Overflow(u8 const x0, u8 const y0, u8 const a, u8 const b, u8 mode);
void OLED_Show_Cube_Catched_by_Elliipse(u8 const x0, u8 const y0, u8 const a, u8 const b, u8 mode);
void OLED_Show_DNA(u8 const x0, u8 const y0, u8 const a, u8 const b, u8 mode);

void Demo_01(void);

#endif
