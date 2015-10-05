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


////////////////////////    begin buzzer code     &/////////////////////////


void startBuzzer()
{
	TCCR2B  |=  (1 << CS22);      // set prescaler to 64
}

void stopBuzzer()
{
	TCCR2B  &=  ~((1 << CS22) |  (1 <<  CS21 ) | (1 << CS20) )   ;	//  clear any prescaler
}

void initBuzzer()
{
	// buzzer using pwm on timer 2
	TCCR2A = (1 << WGM20) |  (1 <<  WGM21)  |  (1 << COM2B1)  ;
	TCCR2B = (1  << WGM22)  ;
	OCR2A =  0x22;      // top value
	OCR2B =  0x11;      //  square wave of approx 5k  at prescaler  64
	DDRD  |=  (1 <<  PORTD6);
}

/////////////////////////    end  buzzer code  ////////////////////////////



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
	
	DDRD |= (1<< PORTD5);  //   set OC1A as output
	
	// and for timer 1 for buzzer pwm code
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
		TOVcnt = 0;
		if (TOVUpdateCnt > 50)  {
			++ cycleCnt;
			TOVUpdateCnt = 0;
		}
													    // just try some kind of trivial cycle movement
		if (TOVUpdateCnt <= 10)  {
			setCOM1A ((float)TOVUpdateCnt /10);
		}  else {
			setCOM1A( (float) (1- ((TOVUpdateCnt - 10) / 40 )) );     
		}
		++ TOVUpdateCnt; 
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


void stopDurationTimer()
{
	cli();
	TCCR0B &= ~((1 << CS00 ) | (1 << CS01 )  | (1 << CS02 ) );   // remove any prescaler bits
	sei();
}

void startDurationTimer(int16_t secs)
{
	cli();
	stopDurationTimer();    // clear all prescaler bits (= set CS01 to 0 !)
	secondsDurationTimerRemaining = secs;
	secondsInDurationTimer = 0;
	tickCnt = 0;
	TCCR0B |= ((1 << CS00 ) |  (1 << CS02 ) );   // prescaler 1024
	sei();
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

ISR(TIMER0_COMPA_vect)
{
	if (tickCnt >= (ticksNeeded -1) ){
		tickCnt = 0;
		runningSecondsTick = 1;
		++secondsInDurationTimer;
		--secondsDurationTimerRemaining;
		if (secondsDurationTimerRemaining <= 0) {
			timerReachedEvent = 1;
			stopDurationTimer();
		}
	}  else  {
		++ tickCnt;
	}
		
}

void initDurationTimer()
{
	// buzzer using pwm on timer 2
	TCCR0A = 0x00  ;
	TCCR0B = (1 << WGM01);  // CTC on OCRA
	TIMSK0  = (1<< OCIE0A);    // timer overflow interrupt enable
	OCR0A = 200 ;  // gives so far a frequency of 108 with prescaler 1024
	ticksNeeded =  108; 
	tickCnt = 0;
}


//////////////////////       end durationtimer code    ///////////////////////

/////////////////////        begin   doorsenosr   code   ////////////////////

void initDoorSensor()
{
	DDRB &=  ~(1<<PORTB1);      //  pb1 as input
	PORTB1  |=  (1<< PORTB1);    //  pull up switched on
								//  assumed that PUD in MCUCR will not be set ....
	PCICR |= (1<< PCIE1);		//  enable ex. pin change interrupt on port B
	PCMSK1 |=  (1<< PCINT9) ;   //  on pcint09  (= pb1)
}


void retrieveDoorEvents ()
{
	if (PINB & (1<< PORTB1)) {
		doorsClosedEvent = 1;
		} else {
		doorsOpenEvent = 1;			// todo : values to be tested, opened/closed might be vice-versa
	}
}


ISR (PCINT1_vect)
{
	retrieveDoorEvents();
}



/////////////////////        end doorsensor code  /////////////////////////////


void initHW()
{
	cli();
	
	// init interval timer for statecharts using timer 0, because we will need timer 1 for servomotor pwm input
	timerStarted = 0;
	timerReachedEvent = 0;
	
	initBuzzer();   // init pwm interface for buzzer using timer 2
	initAlarm();     // init servo pwm interface using timer 1
	initDurationTimer();  // using timer 0
	initDoorSensor();
	
	sei();   
}


void enterIdleSleepMode()
{
	//	MCUCR |= 0x00;    //((1<<SM0) | (1<<SM1)); // select idle sleep mode
	MCUCR |= (1<<SE) ; // enter idle sleep mode
	sleep_cpu();
	MCUCR &= ~(1<<SE); // disable sleep mode after wake up
						// tobe tested
}