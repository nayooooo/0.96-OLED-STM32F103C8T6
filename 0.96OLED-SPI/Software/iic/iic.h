#ifndef __IIC_H
#define __IIC_H
#include "sys.h"
/**********************************************
//		GND -> GND
//		VCC -> VCC
//		SCL -> GPIOB6
//		SDA -> GPIOB7
**********************************************/
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ���������ɣ��������������κ���;
//ALIENTEKս��STM32������
//IIC���� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/9
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) �������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////

#define SLAVE_ADDRESS 0x78

//IO��������
 
#define SDA_IN()  {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=(u32)8<<28;} // PB7���������������ģʽ
#define SDA_OUT() {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=(u32)3<<28;}

//IO��������	 
#define IIC_SCL    PBout(6) //SCL
#define IIC_SDA    PBout(7) //SDA	 
#define READ_SDA   PBin(7)  //����SDA 

//IIC���в�������
static void IIC_Start(void);				//����IIC��ʼ�ź�
static void IIC_Stop(void);	  			//����IICֹͣ�ź�
static void IIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�
static u8 IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
static u8 IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
static void IIC_Ack(void);					//IIC����ACK�ź�
static void IIC_NAck(void);				//IIC������ACK�ź�

static void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
static u8 IIC_Read_One_Byte(u8 daddr,u8 addr);	  

void IIC_GPIO_Init(void);                //��ʼ��IIC��IO��	
void IIC_WR_Byte(u8 dat, u8 cmd);

#endif
















