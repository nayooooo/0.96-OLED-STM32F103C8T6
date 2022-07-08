#include "sys.h"

void WFI_SET(void)
{
	__ASM volatile("WFI;");
}

//关闭所有中断(但是不包括fault和NMI中断)
void INTX_DISABLE(void)
{
	__ASM volatile("CPSID   I");
	__ASM volatile("BX      LR");
}

//开启所有中断
void INTX_ENABLE(void)
{
	__ASM volatile("CPSIE   I");
	__ASM volatile("BX      LR");
}

//设置栈顶地址
//addr:栈顶地址
void MSR_MSP(u32 addr)
{
	__ASM volatile("MSR MSP, r0"); 			//set Main Stack value
	__ASM volatile("BX r14");
}
