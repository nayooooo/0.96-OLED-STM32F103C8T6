/**
 * @file dma.c
 * @author YEWANhub
 * @brief 开启DMA1->CH3
 * @version V1.0.0
 * @date 2022-07-11
 * @encoding GB2312
 */

#include "sys.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_dma.h"
#include "dma.h"

// 使本文件可见oled.h内容
#include "oled.h"
extern u8 OLED_GRAM[OLED_COL_MAX][OLED_PAGE_MAX];
#define DMA1_CH3_BUFF_SIZE (OLED_COL_MAX*OLED_ROW_MAX/8)

void MyDMA_Init(void)
{
    DMA_InitTypeDef DMA_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    DMA_DeInit(DMA1_Channel3);
    DMA_InitStructure.DMA_BufferSize = DMA1_CH3_BUFF_SIZE;  // 该通道的缓存区大小
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;  // 从存储区读取发送到外设
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  // 非内存到内存传输数据
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)OLED_GRAM;  // 内存基地址
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;  // 8位
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  // 内存地址递增
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  // 非循环模式
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&SPI1->DR;  // 外设基地址
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  // 8位
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  // 外设地址不变
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;  // 该通道具有中优先级
    DMA_Init(DMA1_Channel3, &DMA_InitStructure);

    DMA_Cmd(DMA1_Channel3, DISABLE);
}

void MyDMA_Enable(void)
{
    if(DMA_GetFlagStatus(DMA1_FLAG_TC3)!=RESET)  // 判断DMA1->CH3传输完成
        DMA_ClearFlag(DMA1_FLAG_TC3);  // 清除DMA1->CH3传输完成标志位
    DMA_Cmd(DMA1_Channel3, DISABLE);
    DMA_SetCurrDataCounter(DMA1_Channel3, DMA1_CH3_BUFF_SIZE);
    DMA_Cmd(DMA1_Channel3, ENABLE);
}
