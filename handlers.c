#include "handlers.h"

volatile uint32_t msTicks;
volatile uint32_t SevenSegDisplay;

extern const unsigned char znak[10];

void __attribute__((interrupt)) SysTick_Handler(void){
	msTicks++;
}

// PC0 = SEL_DISP2, PC3 = SEL_DISP1
void __attribute__((interrupt)) TIM7_IRQHandler(void){
	if(GPIOC->ODR & GPIO_ODR_OD0){ // selectan je 2.
		GPIOA->ODR = (GPIOA->ODR & (~(0xFFFFUL << GPIO_ODR_OD0_Pos))) | znak[SevenSegDisplay / 10];
		GPIOC->BSRR = GPIO_BSRR_BR0 | GPIO_BSRR_BS3; // pali seg1
	}
	else if(GPIOC->ODR & GPIO_ODR_OD3){ // selectan je 1.
		GPIOA->ODR = (GPIOA->ODR & (~(0xFFFFUL << GPIO_ODR_OD0_Pos))) | znak[SevenSegDisplay % 10];
		GPIOC->BSRR = GPIO_BSRR_BS0 | GPIO_BSRR_BR3; // pali seg 2
	}
	else{ // selectan je nijedan
		GPIOA->ODR = (GPIOA->ODR & (~(0xFFFFUL << GPIO_ODR_OD0_Pos))) | znak[SevenSegDisplay / 10];
		GPIOC->BSRR = GPIO_BSRR_BR0 | GPIO_BSRR_BS3; // pali seg 1
	}
	// pending flag u NVIC se automatski brise posluživanjem prekida
	TIM7->SR &= (~0x1UL);
}
