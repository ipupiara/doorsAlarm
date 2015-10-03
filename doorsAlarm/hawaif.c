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

///////////////////////   begin alarm code  /////////////////////////////////////

uint16_t  tickCnt;
uint16_t  ticksNeeded;


void initAlarm()
{
	// timer 1 for servomotor pwm interface code
	//  and and servomotor value update   
	
	TCCR1A = 0;
	
	// and for timer 1 for buzzer pwm code
}


void startAlarm()
{
	
}

void stopAlarm()
{
	
}

void startBuzzer()
{
	
}

void stopBuzzer()
{
	
}

///////////////////////    end alarm code   /////////////////////////////////

//////////////////////      begin durationtimer code  ///////////////////

int16_t remainingTriacTriggerDelayCounts;

int16_t triacTriggerTimeTcnt2;

int16_t secondsDurationTimerRemaining;

int16_t secondsInDurationTimer;


void startDurationTimer(int16_t secs)
{
	
}

void stopDurationTimer()
{
	
}

int16_t getSecondsDurationTimerRemaining()
{
	int16_t res;
	cli();
	res = secondsDurationTimerRemaining;
	sei();
	return res;
}


int16_t getSecondsInDurationTimer()
{
	int16_t res;
	cli();
	res = secondsInDurationTimer;
	sei();
	return res;
}



//////////////////////       end durationtimer code    ///////////////////////

/////////////////////        begin   doorsenosr   code   ////////////////////


/////////////////////        end doorsensor code  /////////////////////////////

ISR(PCINT0_vect)
{

}


ISR(TIMER1_COMPA_vect)
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