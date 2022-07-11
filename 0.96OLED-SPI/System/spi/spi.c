/**
 * @file spi.c
 * @author YEWANhub
 * @brief ʵ��Ӳ��SPI
 * @version V1.0.0
 * @date 2022-06-10
 * @encoding GB2312
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
#include "sys.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_spi.h"
#include "spi.h"

/**
 * @fn SPI_GPIO_Init
 * @brief ��ʼ��SPI1
 * @return None
 */
void SPI_GPIO_Init(void)
{
#if USE_SPI_MODE == 0
	GPIO_InitTypeDef  GPIO_InitStructure;
 	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = SPI_CS_Pin|SPI_SCLK_Pin|SPI_SDIN_Pin;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOA,SPI_SCLK_Pin|SPI_SDIN_Pin|SPI_CS_Pin);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = SPI_RST_Pin|SPI_DC_Pin;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOB,SPI_RST_Pin|SPI_DC_Pin);
#elif USE_SPI_MODE == 1
	GPIO_InitTypeDef  GPIO_InitStructure;
	SPI_InitTypeDef SPI_InitStructure;
 	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_SPI1|\
							RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);

	// SCL��SDA
	GPIO_InitStructure.GPIO_Pin = SPI_SCLK_Pin|SPI_SDIN_Pin;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// CS
	GPIO_InitStructure.GPIO_Pin = SPI_CS_Pin;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOA,SPI_CS_Pin);

	//RES��DC
	GPIO_InitStructure.GPIO_Pin = SPI_RST_Pin|SPI_DC_Pin;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOB,SPI_RST_Pin|SPI_DC_Pin);

	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;  // 2��Ƶ
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;  // �ڵ�һ�������زɼ�����
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;  // ����ͬ��ʱ�ӿ���״̬Ϊ�͵�ƽ
	SPI_InitStructure.SPI_CRCPolynomial = 7;  // CRC����Ķ���ʽ
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;  // SPI���ͽ���8λ֡�ṹ
	SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;  // SPI����ֻ����
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;  // MSB�ȷ���
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;  // SPI����ģʽ
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;  // �������Ƭѡ��CS��
	SPI_Init(SPI1, &SPI_InitStructure);
#if USE_DMA
	SPI1->CR2 = 1 << 1;  // ����DMA���������ڷ�������
#endif
	SPI_Cmd(SPI1, ENABLE);
#endif
}

/**
 * @brief ������ӻ���SSD1306��д��һ���ֽ�
 * @param dat Ҫд�������/����
 * @param cmd ����/�����־ 0,��ʾ����;1,��ʾ����
 * @return None
 */
void SPI_WR_Byte(u8 dat, u8 cmd)
{
	if(cmd) SPI_DC_Set();
	else SPI_DC_Clr();
	SPI_CS_Clr();
#if USE_SPI_MODE == 0
	u8 i;
	for(i=0;i<8;i++)
	{
		SPI_SCLK_Clr();
		if(dat&0x80) SPI_SDIN_Set();
		else SPI_SDIN_Clr();
		SPI_SCLK_Set();
		dat<<=1;   
	}
#elif USE_SPI_MODE == 1
	u8 retry = 0;
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET)
	{
		retry++;
		if(retry>200) return;
	}
	SPI_I2S_SendData(SPI1, dat);
	retry = 0;
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE)==RESET)
	{
		retry++;
		if(retry>200) return;
	}
#endif
	SPI_CS_Set();
	SPI_DC_Set();
}
