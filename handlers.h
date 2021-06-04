#ifndef HANDLERS_H
#define HANDLERS_H

#include "stm32f4xx.h"

extern volatile uint32_t msTicks;
extern volatile uint32_t SevenSegDisplay;

void __attribute__((interrupt)) SysTick_Handler(void);
void __attribute__((interrupt)) TIM7_IRQHandler(void);

#endif
