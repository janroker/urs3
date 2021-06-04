#include "init.h"

#define wait_sklop(x, y) do {} while(!((x) & (y)))

//====================
// 8MHz / M => 1 * N => 200 / P => 50 MHz
#define RCC_PLLCFGR_50MHz (RCC_PLLCFGR_PLLSRC_HSE | (0x1U << RCC_PLLCFGR_PLLP_Pos) | 0x3208U)  // setiraj PLLSRC, P = 01 -> 4, M i N; 
														// 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
														// 0 1 1 0 0 1 0 0 0 0 0 1 0 0 0  --> N = 0xC8 = 200, M = 0x8 = 8
														// 0x3208U
#define HPRE_0_PPRE_MAX (RCC_CFGR_PPRE1_DIV16 | RCC_CFGR_PPRE2_DIV16 | (~RCC_CFGR_HPRE_Msk))
#define PPRE_MASK 			(RCC_CFGR_PPRE1_Msk | RCC_CFGR_PPRE2_Msk)

// ===================
#define BITS_PER_S 	9600
														
// AHB 50
// APB1 25
// APB2 50
void init_CLOCK(void) {
	RCC->CR |= RCC_CR_HSEON; // ukljuci hse
	wait_sklop(RCC->CR, RCC_CR_HSERDY); // cekaj hse
	
	RCC->CR &= (~(RCC_CR_PLLON)); // mora biti iskljucen da se moze pisati (default...)
	
	RCC->PLLCFGR = (RCC_PLLCFGR_RST_VALUE & (~0x37FFFU)) |  RCC_PLLCFGR_50MHz; // resetiraj M N, P value i setiraj 50MHz
	
	RCC->CR |= RCC_CR_PLLON; // ukljuci pll
	wait_sklop(RCC->CR, RCC_CR_PLLRDY); // cekaj pll
	
	RCC->CFGR |= HPRE_0_PPRE_MAX; // postavi u cfgr PPRE1 i PPRE2 na max i HPRE na 0000 ... ovako je svejedno koje su vrijednosti bile;
	
	RCC->CFGR = (RCC->CFGR & (~RCC_CFGR_SW_Msk)) | RCC_CFGR_SW_PLL; // odaberi CLKPLL na muxu
	while((RCC->CFGR & RCC_CFGR_SWS_Msk) != RCC_CFGR_SWS_PLL); // cekaj da mux prebaci
	
	RCC->CFGR = (RCC->CFGR & (~(PPRE_MASK))) | RCC_CFGR_PPRE1_DIV2 | RCC_CFGR_PPRE2_DIV1; // postavi bitove za PPRE1 i PPRE2 sve u 0 pa ih postavi na div2 i div1...
}

void init_FlashAccess(void){
	SCB->VTOR = FLASH_BASE; // premjesti vektore
	
	FLASH->ACR |= ( // postavi ART acc
									FLASH_ACR_LATENCY_1WS 
									| FLASH_ACR_PRFTEN 
									| FLASH_ACR_ICEN 
									| FLASH_ACR_DCEN
								);
	while((FLASH->ACR & 0x7U) != FLASH_ACR_LATENCY_1WS); // cekaj da se postavi
}

void init_FPU(void){
	SCB->CPACR |= ((3UL << 20) | (3UL << 22)); // full access;
}

void init_SysTick(void){
	
	SCB->SHP[11] = 0x2 << 4; // prioritet sysTick
	SCB->AIRCR = (SCB->AIRCR & 0xF8FFUL) | 0x05FA0000UL; // PRIGROUP
	
	SysTick->LOAD = 0xC34FU;// N = 50000 => LOAD = N - 1
	SysTick->VAL = 0x0U;
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk; // omoguci interupt, clksource /1, pocni brojat
	
}

// alternativna funkcija na pd6, pd5
void init_USART2(void){
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN; // omoguci takt na periferiji
	// wait_sklop(RCC->APB1ENR, RCC_APB1ENR_USART2EN_Msk); ne treba zbog cekanja ispod
	
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
	wait_sklop(RCC->AHB1ENR, RCC_AHB1ENR_GPIODEN); // omoguci takt na gpiod
	
	// =========== alternate function // USART2 TX => PD5 || USART2 RX => PD6
	GPIOD->AFR[0] |= (7UL << 24) | (7UL << 20); // AFRL6 = AF7 --> pinu 6 daj AF7 || AFRL5 = AF7 --> pinu 5 daj AF7
	GPIOD->MODER = (GPIOD->MODER & (~GPIO_MODER_MODE5_Msk)) | GPIO_MODER_MODE5_1;
	GPIOD->MODER = (GPIOD->MODER & (~GPIO_MODER_MODE6_Msk)) | GPIO_MODER_MODE6_1;
	
	// fapb1 = 25MHz
	
	USART2->CR1 = 0x0UL; 
	USART2->CR2 = 0x0UL; 
	USART2->CR3 = 0x0UL; 
	
	USART2->CR1 |= (1UL << 13); // UE => USART enable
	USART2->CR1 |= (0UL << 12); // M => 8 bits
	USART2->CR1 |= (0UL << 10); // PCE => No parity
	USART2->CR2 |= (0UL << 12); // STOP => 1 stop bit
	USART2->BRR = 0x0A2CUL; // Baud rate generator
	USART2->CR1 |= (1UL << 3); // TE => TX enable
}

void init_TIM(void){
	RCC->APB1ENR |= RCC_APB1ENR_TIM7EN; // dovedi takt
	wait_sklop(RCC->APB1ENR, RCC_APB1ENR_TIM7EN_Msk);
	
	TIM7->PSC = (TIM7->PSC & (~0xFFFFUL)) | 0x4UL; // prescaler == 4+1... => 5MHz * 10^-2 => 50000
	TIM7->CNT = (TIM7->CNT & (~0xFFFFUL)) | 0UL;
	TIM7->ARR = (TIM7->ARR & (~0xFFFFUL)) | 0xC350UL; // 50000
	TIM7->DIER |= TIM_DIER_UIE; // interrupt enable
	TIM7->CR1 |= TIM_CR1_CEN; // counter enable
	
	NVIC_SetPriority(TIM7_IRQn, 6);
	NVIC_EnableIRQ(TIM7_IRQn);
}
