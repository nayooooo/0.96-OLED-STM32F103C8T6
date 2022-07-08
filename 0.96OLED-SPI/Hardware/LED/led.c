#include "sys.h"
#include "delay.h"
#include "led.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

void LED_GPIO_Config(uint32_t LED_GPIO_CLK, GPIO_TypeDef* LED_GPIO_PORT, uint16_t LED_GPIO_PIN)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_APB2PeriphClockCmd(LED_GPIO_CLK, ENABLE);
	
	GPIO_InitStruct.GPIO_Pin = LED_GPIO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(LED_GPIO_PORT, &GPIO_InitStruct);
	
	GPIO_SetBits(LED_GPIO_PORT,LED_GPIO_PIN);
}

void LED_Init(void)
{
	LED_GPIO_Config(RCC_APB2Periph_GPIOC, GPIOC, GPIO_Pin_13);
}

void LED_Flash(u16 ms)
{
	LED = LED_ON;
	delay_ms(ms);
	LED = LED_OFF;
	delay_ms(ms);
}
