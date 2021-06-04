#include "stm32f4xx.h"
#include "handlers.h"
#include "retarget.h"

#define RGB_MASK 	(GPIO_MODER_MODE13_Msk | GPIO_MODER_MODE14_Msk | GPIO_MODER_MODE15_Msk)
#define RGB_MODER (GPIO_MODER_MODE13_0 | GPIO_MODER_MODE14_0 | GPIO_MODER_MODE15_0)
#define R_ON 			(GPIO_BSRR_BS13 | GPIO_BSRR_BR14 | GPIO_BSRR_BR15)
#define G_ON 			(GPIO_BSRR_BR13 | GPIO_BSRR_BR14 | GPIO_BSRR_BS15)
#define B_ON 			(GPIO_BSRR_BR13 | GPIO_BSRR_BS14 | GPIO_BSRR_BR15)
#define ALL_OFF 	(GPIO_BSRR_BR13 | GPIO_BSRR_BR14 | GPIO_BSRR_BR15)

#define MODER_SEG_MASK 	(0xFFFFUL << GPIO_MODER_MODER0_Pos)
#define MODER_SEG_OUT 	(0x5555UL << GPIO_MODER_MODER0_Pos)
#define SEG_SEL_MASK 		(GPIO_MODER_MODE0_Msk | GPIO_MODER_MODE3_Msk)
#define SEG_SEL_OUT			(GPIO_MODER_MODE0_0 | GPIO_MODER_MODE3_0)



 // A => PA3 
 // B => PA2 A 
 // C => PA4 F B 
 // D => PA7 G 
 // E => PA5 E C 
 // F => PA0 D 
 // G => PA1 
 // DP => PA6 
 
 #define Seg_A (1<<3) 
 #define Seg_B (1<<2) 
 #define Seg_C (1<<4) 
 #define Seg_D (1<<7) 
 #define Seg_E (1<<5) 
 #define Seg_F (1<<0) 
 #define Seg_G (1<<1) 
 #define Seg_DP (1<<6) 

extern const unsigned char znak[10];
const unsigned char znak[10] = 
 {	Seg_A | Seg_B | Seg_C | Seg_D | Seg_E | Seg_F , 				// 0
		Seg_B | Seg_C , 																				// 1
		Seg_A | Seg_B | Seg_D | Seg_E | Seg_G, 									// 2
		Seg_A | Seg_B | Seg_C | Seg_D | Seg_G, 									// 3
		Seg_B | Seg_C | Seg_F | Seg_G, 													// 4
		Seg_A | Seg_C | Seg_D | Seg_F | Seg_G, 									// 5
		Seg_A | Seg_C | Seg_D | Seg_E | Seg_F | Seg_G, 					// 6
		Seg_A | Seg_B | Seg_C , 																// 7
		Seg_A | Seg_B | Seg_C | Seg_D | Seg_E | Seg_F | Seg_G, 	// 8
		Seg_A | Seg_B | Seg_C | Seg_D | Seg_F | Seg_G, 				 	// 9
}; 

void paliH4(void);

void Delay(uint32_t NoOfTicks);
void initRGB(void);
void paliRGB(void);

void initZvucnik(void);
 
void init7SEG(void);
void foreverLoop(void);
 
int main(void){
	paliH4();
	
	initRGB();
	paliRGB();
	
	printf("Redefinicija sistemskih poziva uspjesno napravljena !\r\n");
	
	initZvucnik();
	FILE *fMorse;
	fMorse = fopen ("Morse","w");
	fprintf(fMorse,"SOS - SOS - PURS");
	fclose(fMorse);
	
	init7SEG();
	foreverLoop();
}

void paliH4(void){
	// PD13 LED3
	// PD12 LED4
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
	while(!(RCC->AHB1ENR & RCC_AHB1ENR_GPIODEN)); // potrebna 2 ciklusa da takt dode
	
	GPIOD->MODER = (GPIOD->MODER & (~(GPIO_MODER_MODE12_Msk | GPIO_MODER_MODE13_Msk))) 
									| GPIO_MODER_MODE12_0 
									| GPIO_MODER_MODE13_0; // 13 i 12 output
	
	GPIOD->BSRR = (GPIO_BSRR_BR13 | GPIO_BSRR_BS12); // upali h4 ugasi h3
}

void Delay(uint32_t NoOfTicks){
	uint32_t curTicks = msTicks;
	
	while((msTicks-curTicks) < NoOfTicks);
}

void initRGB(void){
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; // dovedi takt na periferiju
	while(!(RCC->AHB1ENR & RCC_AHB1ENR_GPIOBEN_Msk)); // cekaj da takt dode
	
	GPIOB->MODER = (GPIOB->MODER & (~(RGB_MASK))) | RGB_MODER; // rgb output
}

void paliRGB(void){
	volatile uint32_t arr[3] = {R_ON, G_ON, B_ON};
	for(uint8_t i = 0; i < 9; i++){
		GPIOB->BSRR = arr[i % 3];
		Delay(1000);
	}
	GPIOB->BSRR = ALL_OFF;
}

void initZvucnik(void){
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
	while(!(RCC->AHB1ENR & RCC_AHB1ENR_GPIOEEN_Msk));
	
	GPIOE->MODER = (GPIOE->MODER & (~GPIO_MODER_MODE11_Msk)) | GPIO_MODER_MODE11_0;
}

// PA0-7 i PC0 = SEL_DISP2, PC3 = SEL_DISP1
void init7SEG(void){
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
	while(!(RCC->AHB1ENR & RCC_AHB1ENR_GPIOCEN_Msk)); // dovoljno je pricekati samo 1
	
	GPIOA->MODER = (GPIOA->MODER & ~(MODER_SEG_MASK)) | MODER_SEG_OUT;
	GPIOC->MODER = (GPIOC->MODER & ~(SEG_SEL_MASK)) | SEG_SEL_OUT;
}

__attribute__((noreturn)) void foreverLoop(void){
	uint32_t cnt_loop;
	for(cnt_loop = 0; 1; cnt_loop++){
		printf("Broj prolaza = %d\r\n", cnt_loop);
		GPIOD->BSRR = (((cnt_loop % 2) == 0) ? GPIO_BSRR_BS13 : GPIO_BSRR_BR13); // mijenjaj stanje
		SevenSegDisplay = cnt_loop % 100;
		Delay(1000);
	}
}
