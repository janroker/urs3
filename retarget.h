#ifndef RETARGET_H
#define RETARGET_H

#include <stdio.h>   // FILE, __stdin, __stdout
#include <rt_misc.h> // razne funkcije za redeficiciju
#include <rt_sys.h>  // _sys_open, _sys_write itd.
#include <string.h>  // strncmp i sl.
#include "stm32f4xx.h"

#define FH_STDIN    0x8001
#define FH_STDOUT   0x8002
#define FH_STDERR   0x8003
#define FH_MORSE 		0x1234

__asm(".global __use_no_semihosting");

extern void sendchar_USART2(char c);

#endif
