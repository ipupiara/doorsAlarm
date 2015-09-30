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


#define     servoPin	PORTD5
#define		servoDDR	DDRD		// it is needed to set pin as output  for timer 1   (16 bit timer)

#define     doorsPort	PORTB
#define		doorsPin	PORTB1
#define		doorsPortInputPins	PINB
#define		doorsDDR	DDRB
#define		doorsPCIE	PCIE1
#define     doorsPCMSK	PCMSK1
#define		doorsPCINT	PCINT9
#define     doorsPCI	PCINT1

int8_t		lastDoorState;



int16_t lastADCVal;
uint16_t  tickCnt;
uint16_t  ticksNeeded;

int8_t  lastPinB;

int8_t timerStarted;



void setState(char* sta)
{
	wwState = (stateString *) sta;
	++stateEntryCnt; 
}


void switchRelay53ToMoterOutput()
{
	PORTB &=  ~(1<< RELAYSWITCH);
}

void  switchRelayToPlusLine15()
{
	PORTB |=  (1<< RELAYSWITCH);
}


ISR(PCINT0_vect)
{
	int8_t pinB = PINB;
	
	int8_t newLinePin =  (pinB & (1<< INDIALINEPIN));
	int8_t oldLinePin =  (lastPinB  & (1<< INDIALINEPIN));
	if ( newLinePin && (!oldLinePin) ) {
		indiaLineOnEvent = 1;
	}
	if ( (!newLinePin) && oldLinePin ) {
		indaLineOffEvent = 1;
	}
	newLinePin =  (pinB & (1<< TANGOLINEPIN));
	oldLinePin =  (lastPinB  & (1<< TANGOLINEPIN));
	if ( newLinePin && (!oldLinePin) ) {
		doorsOpenEvent = 1;
	}
	if ( (!newLinePin) && oldLinePin ) {
		doorsClosedEvent = 1;
	}
	newLinePin =  (pinB  & (1<< RELAYPIN));
	oldLinePin =  (lastPinB & (1<< RELAYPIN));
	if ( newLinePin && (!oldLinePin) ) {
		evMotorOutput53sSwitchedHighEvent = 1;
	}
	if ( (!newLinePin) && oldLinePin ) {
		evMotorOuput53sSwitchedLowEvent = 1;
	}
	lastPinB = pinB;	
}


ISR(ADC_vect)

{
	lastADCVal = ADC;
	if (lastADCVal < 0x200) {
		ticksNeeded = (lastADCVal >> 4);
	} else {
		ticksNeeded = (0x1FF >> 4) + ((lastADCVal - 0x1FF) >> 3);
	}
}


ISR(TIMER1_COMPA_vect)
{   
	int16_t ticksN;
	if (timerStarted) { ++tickCnt;}
	ticksN = ticksNeeded;
	if ((tickCnt > ticksN) && timerStarted) {
		timerReachedEvent = 1;
	}    
	if (((ADCSRA) & (1<<ADEN))!= 0  ) { ADCSRA  |= (1<<ADSC);}   // start one ADC cycle
}


void stopADCPolling()
{
	cli();
	TCCR1 &= ~( (1 << CS10) |  (1 << CS11) | (1 << CS12) | (1 << CS13)) ; 
	ADCSRA  &= 	~(1<< ADEN);
	sei();
}

void startADCPolling()
{
			// Timer 1    prepare  for ADC triggering 
	cli();
	tickCnt = 0;
	ticksNeeded = 20;   // 10 sec
	TCCR1 = 1 << CTC1; 
	OCR1A = 0xF4;  // counter top value means approx   2 ADC measures per sec
	GTCCR = 0x00;
	TIMSK  = 1 << OCIE1A;  //  interrupt needed for ADC trigger
	TCNT1 = 0x00 ;

	//  init ADC

	lastADCVal = -1;

	ADMUX = (1<<REFS0) | (1<<REFS1) | (1 << MUX0) | (1 << MUX1) ; //  VCC as ref.voltage, right adjust, ADC3 input (PB3)
//	DIDR0 = (1<<ADC3D);  // disable digital input

	ADCSRA  = (1<<ADEN) | (1<<ADIE) | (1<<ADPS1) | (1<<ADPS2);  // | (1<<ADPS0)  now 64 prescaler wihout ps0 approx 125 kHz
	// ena adc, set single conversion mode, int enable
	ADCSRB = 0x00 ;
	//  timer 1 comp match trigger

	// start Timer 1 and hence also ADC
	TCCR1 |=  (1 << CS10) |  (1 << CS11) | (1 << CS12) | (1 << CS13) ;      //   16384 prescaler... about 488 Hz at 8Mhz CLK
	sei();
}


#ifdef testMethods

	// method needed only for testing so far
	void startTimerIntervalSecs(int8_t secs)
	{
		stopIntervalTimer();
		ticksNeeded = secs * 2;
		startIntervalTimer();
	}


	// method only for testing so far
	void stopTimerIntervalSecs()
	{
		stopIntervalTimer();
	}

#endif

void startIntervalTimer()
{
	timerStarted = 1;
//	startADCPolling();
}

void stopIntervalTimer()
{
	timerStarted = 0;
	tickCnt = 0;
//	stopADCPolling();
}


int isIndiaLineOn()
{
	int8_t res = (PINB & (1 << INDIALINEPIN)) != 0;
	return res;
}


int isTangoLineOn()
{
		int8_t res = (PINB & (1 << TANGOLINEPIN)) != 0;
		return res;
}

/*

void enterIdleSleepMode()
{
	MCUCR &= ~((1<<SM0) | (1<<SM1)); // select idle sleep mode
	MCUCR |= (1<<SE) ; // enter idle sleep mode
	MCUCR &= ~(1<<SE); // disable sleep mode after wake up
}
*/


void initHW()
{
	stateEntryCnt = 0;
	
	cli();
	timerStarted = 0;
	timerReachedEvent = 0;
	indiaLineOnEvent = 0;
	indaLineOffEvent = 0;
	doorsOpenEvent = 0;
	doorsClosedEvent = 0;
	evMotorOutput53sSwitchedHighEvent = 0;
	evMotorOuput53sSwitchedLowEvent  = 0;
	
	// enable pcint0..2
	DDRB  &= ~((1<DDB0) | (1<< DDB1) | (1<<DDB2)  );            // one could also use above pin defines here.. it is the same sequence
	PORTB   &= ~((1<PB0) | (1<< PB1) | (1<<PB2)  );			// disable pullups
	PCMSK |= ((1 << PCINT0) |  (1 << PCINT1) | (1 << PCINT2));
	GIMSK |= (1 << PCIE);

	PORTB &= ~(1<<RELAYSWITCH);  // switch relay off to default = forward line coming from wish-motor
	DDRB |= (1<<RELAYSWITCH);   // output
	lastPinB = PINB;

	sei();   
}
