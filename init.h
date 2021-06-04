#ifndef INIT_H
#define INIT_H

#include "stm32f4xx.h"

void init_CLOCK(void);
void init_FlashAccess(void);
void init_FPU(void);
void init_SysTick(void);
void init_USART2(void);
void init_TIM(void);

#endif
