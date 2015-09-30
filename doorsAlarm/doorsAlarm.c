/*
 * wishWash.c
 *
 * Created: 1/2/2015 1:26:04 PM
 *  Author: ixchel
 */ 


#include <avr/io.h>
#include <stddef.h>
#include "StateClass.h"
#include "hawaif.h"


#ifndef testMethods

	CWishWashEvent curEvent;

	int main(void)
	{
		CWishWashEvent* ev = NULL;
		initHW();
		startStateCharts();
		while(1)
		{
			ev=getNextEvent(&curEvent);
			if (ev != NULL) {
				processEvent(PWishWashStateChart,ev);
			}

		}
	}
	
#else 


enum motorRelaySimulatorStates
{
	Idlde_WaitForPlus,
	WaitForGoUpTimer,
	WaitForGoDownTimer
};


enum motorRelaySimulatorEvents
{
	MotorSwitchedToPlus = 1,
	TimerReached
};

int8_t currentState= Idlde_WaitForPlus;



void switchOutputHigh()
{
	switchRelayToPlusLine15();
}

void switchOutputLow()
{
	switchRelay53ToMoterOutput();
}

int8_t isInputHigh()
{
	return isTangoLineOn();
}

int8_t nextEvent()
{
	int8_t res = 0;
	if (currentState== Idlde_WaitForPlus)  {
		if (isInputHigh()) {
			res = MotorSwitchedToPlus;
		}
	}
	if (res == 0) {
		if (timerReachedEvent) {
			timerReachedEvent =0;
			res = TimerReached;
		}
	}
	return res;
}


int main(void)
{
	int8_t evt;
	while(1)
	{
		evt = nextEvent();
 		if (evt != 0) {
			if (currentState == Idlde_WaitForPlus) {
				if (evt == MotorSwitchedToPlus) {
					currentState = WaitForGoUpTimer;
					startTimerIntervalSecs(2);
				}
			}
		
			if (evt == TimerReached)  {
				if (currentState == WaitForGoUpTimer )  {
					currentState = WaitForGoDownTimer;
					startTimerIntervalSecs(4);
					switchOutputHigh();
				} else
				if (currentState == WaitForGoDownTimer) {
					currentState = Idlde_WaitForPlus;
					switchOutputLow();
				}
			}  // TimerReached 
		}   // if evt != 0
	}   // while 
}


#endif