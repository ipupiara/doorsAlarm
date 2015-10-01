/*
 * hawaif.c
 *
 * Created: 1/20/2015 10:32:23 AM
 *  Author: ixchel
 */ 

#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

#include "hawaif.h"

enum doorStates  {
		doorsOpen,
		doorsClosed
	};


int8_t		lastDoorState;

uint16_t  tickCnt;
uint16_t  ticksNeeded;



ISR(PCINT0_vect)
{

}


ISR(TIMER1_COMPA_vect)
{   

}


void startDurationTimer(int16_t secs)
{
	
}

void stopDurationTimer()
{
	
}

void startAlarm()
{
	
}

void stopAlarm()
{
	
}


void initHW()
{
	cli();
	
	// init interval timer for statecharts using timer 0, because we will need timer 1 for servomotor pwm input
	timerStarted = 0;
	timerReachedEvent = 0;
	
	// init servo pwm interface using timer 1
	
	// init pwm interface for buzzer using timer 2
	// also use this timer for fast value setting on servomotor interface 
	

	sei();   
}


void enterIdleSleepMode()
{
	
}