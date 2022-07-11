/**
 * @file dma.c
 * @author YEWANhub
 * @brief ����DMA1->CH3
 * @version V1.0.0
 * @date 2022-07-11
 * @encoding GB2312
 */

#include "sys.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_dma.h"
#include "dma.h"

// ʹ���ļ��ɼ�oled.h����
#include "oled.h"
extern u8 OLED_GRAM[OLED_COL_MAX][OLED_PAGE_MAX];
#define DMA1_CH3_BUFF_SIZE (OLED_COL_MAX*OLED_ROW_MAX/8)

void MyDMA_Init(void)
{
    DMA_InitTypeDef DMA_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    DMA_DeInit(DMA1_Channel3);
    DMA_InitStructure.DMA_BufferSize = DMA1_CH3_BUFF_SIZE;  // ��ͨ���Ļ�������С
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;  // �Ӵ洢����ȡ���͵�����
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  // ���ڴ浽�ڴ洫������
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)OLED_GRAM;  // �ڴ����ַ
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;  // 8λ
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  // �ڴ��ַ����
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  // ��ѭ��ģʽ
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&SPI1->DR;  // �������ַ
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  // 8λ
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  // �����ַ����
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;  // ��ͨ�����������ȼ�
    DMA_Init(DMA1_Channel3, &DMA_InitStructure);

    DMA_Cmd(DMA1_Channel3, DISABLE);
}

void MyDMA_Enable(void)
{
    if(DMA_GetFlagStatus(DMA1_FLAG_TC3)!=RESET)  // �ж�DMA1->CH3�������
        DMA_ClearFlag(DMA1_FLAG_TC3);  // ���DMA1->CH3������ɱ�־λ
    DMA_Cmd(DMA1_Channel3, DISABLE);
    DMA_SetCurrDataCounter(DMA1_Channel3, DMA1_CH3_BUFF_SIZE);
    DMA_Cmd(DMA1_Channel3, ENABLE);
}
