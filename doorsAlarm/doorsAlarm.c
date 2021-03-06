/*
 * wishWash.c
 *
 * Created: 1/2/2015 1:26:04 PM
 *  Author: ixchel
 */ 


#include <avr/io.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "StateClass.h"
#include "hawaif.h"


void USART_Init( unsigned int baud )
{
	// Set baud rate
	
	UBRR0H = (unsigned char)(baud>>8);
	UBRR0L = (unsigned char)baud;
	// Enable receiver and transmitter
	UCSR0A =  (UCSR0A & 0b11111100) ;
	
	UCSR0B=(1<<RXEN0)|(1<<TXEN0);
	//	UCSR0B = 0b00011000;  // rx compl intr ena - tx compl intr ena - dreg empty intr ena - rx ena - tx ena - sz2 (size bit 2)  - 9. bit rx - 9. tx

	UCSR0C = 0b00000110; // "00" async usart - "00" disa parity - 1 (or 2) stop bit - sz1 - sz0 (set t0 8 bit) - clk polarity (sync only)
}


static int uart_putchar(char c, FILE *stream);

static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);  // first opened will be stdout and errout

//stdout = fdevopen((FILE *)uart_putchar, NULL, 0);

static int uart_putchar(char c, FILE *stream)
{
	while (!(UCSR0A & (1<<UDRE0)));
	if (c == '\n')
	{
		uart_putchar('\r',&mystdout);
	}
	UDR0 = c;
	return 0;
}


int main(void)
{

	CDoorsAlarmEvent ev;
	CDoorsAlarmEvent* pev = &ev;
//	int16_t dummyI;


//	USART_Init( 143 );   // baud 4800 at 11.0592 mhz, single uart speed
//	USART_Init( 71 );   // baud 9600 at 11.0592 mhz, single uart speed
//	USART_Init (11 );   // baud 57.6k  at 11.0592 mhz, single uart speed
	USART_Init (5 );   // baud 115.2k  at 11.0592 mhz, single uart speed
	
	
	stdout = &mystdout;
	printf("\nSTARTUP\n");

	initHW();
	
	startStateCharts();	

	while (1)
	{
		pev=getNextEvent(&ev);
		if (pev != NULL)  {
			processEvent(&SDoorsAlarmStateChart,pev);	
		}
	}

}

