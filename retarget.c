#include "retarget.h"

extern void send_char_morse(char c);

int sendChars(const unsigned char *buf, unsigned len, void (*char_receiver)(char));

FILEHANDLE _sys_open(const char *name, int openmode) {
	if(strncmp(name, ":STDIN", 6) == 0){
		return FH_STDIN;
	}
	else if(strncmp(name, ":STDOUT", 7) == 0){
		return FH_STDOUT;
	}
	else if(strncmp(name, ":STDERR", 7) == 0){
		return FH_STDERR;
	}
	else if(strncmp(name, "Morse", 5) == 0 && openmode == 4){
		return FH_MORSE;
	}
	else{
		return (-1);
	}
}

int _sys_istty(FILEHANDLE fh) { // je li interaktivna
	switch(fh){
		case FH_STDIN:
			return (1);
		case FH_STDOUT:
			return (1);
		case FH_STDERR:
			return (1);
		case FH_MORSE:
			return (1);
		default:
			return (-1);
	}
}

int _sys_write(FILEHANDLE fh, const unsigned char *buf, unsigned len, int mode) { // vraca koliko nije uspjelo poslati...
	(void)mode;
	
	if(fh == FH_STDOUT || fh == FH_MORSE){
		
		void (*char_receiver)(char) = ((fh == FH_STDOUT) ? &sendchar_USART2 : &send_char_morse);
		return sendChars(buf, len, char_receiver);
		
	}
	else{
		return (-1);
	}
}

void _ttywrch(int ch) {
	sendchar_USART2(ch & 0xFF);
}

__attribute__((noreturn)) void _sys_exit(int returncode) {
	(void)returncode;
	while(1);
}

int _sys_close(FILEHANDLE fh){ // vraca 0 ako uspješno; razlicito inace
	switch(fh){
		case FH_STDIN:
			return (0);
		case FH_STDOUT:
			return (0);
		case FH_STDERR:
			return (0);
		case FH_MORSE:
			return (0);
		default:
			return (-1);
	}
}

void sendchar_USART2(char c){
	while(!(USART2->SR & USART_SR_TXE));
	USART2->DR = c; //(c & 0xFF);
}

int sendChars(const unsigned char *buf, unsigned len, void (*char_receiver)(char)){
	while(len){
			char_receiver(*buf);
			//sendchar_USART2(*buf);
			len--;
			buf++;
	}
	return (0);
}
