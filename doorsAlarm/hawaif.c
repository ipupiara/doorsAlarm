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

int8_t  TOVcnt;
int8_t  currentDirection;
int16_t TOVUpdateCnt;
int16_t cycleCnt;


void initAlarm()
{
	// timer 1 for servomotor pwm interface code
	//  and and servomotor value update   
	
	TCCR1A = (1 << COM1A1) | (1<< WGM11) ;    // waveform mode 14 using com1a pin, compare value on IC
	TCCR1B = (1<< WGM12)  | (1<< WGM13)   ;
	
	ICR1 = 22120;
	
	TIMSK1 =  ( 1<< TOIE1);
	
	// and for timer 1 for buzzer pwm code
}


void startBuzzer()
{
	
}

void stopBuzzer()
{
	
}

void setCOM1A(float pct)
{
	float ICR1F ;
	int16_t ICR1Debug;
	if (pct < 0.0) {
		pct = 0.0;
	}
	if (pct > 100) {
		pct = 100;
	}
	// ICR1 approx = 0.75 *(22120/20) + (pct /100) *  (2.25 - 0.75) * (22120 /20) ;
	ICR1F =  829.50 +  (pct  *  16.59);
	ICR1Debug  = (int16_t) (ICR1F);
	cli();
	ICR1 = ICR1Debug;
	sei();
}

ISR(TIMER1_OVF_vect) 
{
	cli();
	if (TOVcnt > 1) {
		if (TOVUpdateCnt == 50)  {
			++ cycleCnt;
			TOVUpdateCnt = 0;
		}
		TOVcnt = 0;
		++ TOVUpdateCnt;     // just try some kind of cycle movement
		if (TOVUpdateCnt < 10)  {
			setCOM1A ((float)TOVUpdateCnt /10);
		}  else {
			setCOM1A( (float) (1- ((TOVUpdateCnt - 10) / 40 )) );     
		}
	 } else {
		++TOVcnt;
	}
	sei();
}


void startAlarm()
{
	TOVcnt = 0;
	TOVUpdateCnt = 0;
	cycleCnt = 0;
	TCCR1B |= (1 << CS10);   // set no prescaler, run at clkIO (max cycle duration approx. 0.005 secs)
}

void stopAlarm()
{
	TCCR1B &= ~((1 << CS10 ) | (1 << CS11 )  | (1 << CS12 ) );   // remove any prescaler bits
}


///////////////////////    end alarm code   /////////////////////////////////

//////////////////////      begin durationtimer code  ///////////////////

int16_t remainingTriacTriggerDelayCounts;

int16_t triacTriggerTimeTcnt2;

int16_t secondsDurationTimerRemaining;

int16_t secondsInDurationTimer;

uint16_t  tickCnt;
uint16_t  ticksNeeded;


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