
#include <avr/io.h>
#include <stdio.h>
//#include <iostream.h>
//#include <assert.h>
#include <string.h>

#include "TStatechart.h"
#include "StateClass.h"
#include "hawaif.h"

extern const uStInt uStIntHandlingDone;
extern const uStInt uStIntNoMatch;


CDoorsAlarmEvent* currentEvent;

int8_t wishCounter;

CDoorsAlarmEvent* getNextEvent(CDoorsAlarmEvent* pev)
{
	CDoorsAlarmEvent* res = NULL;
	if (tangoPressedEvent ==  1) {
		tangoPressedEvent = 0;
		pev->evType = evTangoPressed;
		res = pev;
	}
	if (tangoReleasedEvent ==  1) {
		tangoReleasedEvent = 0;
		pev->evType = evTangoReleased;
		res = pev;
	}
	if (timerReachedEvent ==  1) {
		timerReachedEvent = 0;
		pev->evType = evTimerExpired;
		res = pev;
	}
	if (indiaLineOnEvent ==  1) {
		indiaLineOnEvent = 0;
		pev->evType = evIndiaSwitchedOn;
		res = pev;
	}
	if (indaLineOffEvent ==  1) {
		indaLineOffEvent = 0;
		pev->evType = evIndiaSwitchedOff;
		res = pev;
	}
	if (evMotorOutput53sSwitchedHighEvent ==  1) {
		evMotorOutput53sSwitchedHighEvent = 0;
		pev->evType = evMotorOutput53sSwitchedHigh;
		res = pev;
	}
	if (evMotorOuput53sSwitchedLowEvent ==  1) {
		evMotorOuput53sSwitchedLowEvent = 0;
		pev->evType = evMotorOutput53sSwitchedLow;
		res = pev;
	}
	return res;
}


uStInt evDoorsAlarmChecker(void)
{
	uStInt res = uStIntNoMatch;
	return res;
}

void entryIdleState(void)
{
	switchRelay53ToMoterOutput();
	if(isTangoLineOn()) {
		tangoPressedEvent = 1;
	} else if (isIndiaLineOn()) {
		indiaLineOnEvent = 1;		
	}
	setState("idl");
}

uStInt evIdleChecker(void)
{
	uStInt res = uStIntNoMatch;

	if (currentEvent->evType == evMotorOutput53sSwitchedLow) 
	{	
//			BEGIN_EVENT_HANDLER(PJoesTriacStateChart, eStateTriacIdle);
				// No event action.
			switchRelay53ToMoterOutput();	
//			END_EVENT_HANDLER(PJoesTriacStateChart);
			res =  uStIntHandlingDone;
	}
	if (currentEvent->evType == evTangoPressed)
	{	
			BEGIN_EVENT_HANDLER(PDoorsAlarmStateChart, eStateTangoPressed);
				// No event action.
			END_EVENT_HANDLER(PDoorsAlarmStateChart);
			res =  uStIntHandlingDone;
	}
	if (currentEvent->evType == evIndiaSwitchedOn) 
	{	
			BEGIN_EVENT_HANDLER(PDoorsAlarmStateChart, eStateIndiaOn);
				// No event action.
			END_EVENT_HANDLER(PDoorsAlarmStateChart);
			res =  uStIntHandlingDone;
	}
	return (res); 
}

void entryTangoPressedState(void)
{
	switchRelayToPlusLine15();
	setState("tnP");
}

uStInt evTangoPressedChecker(void)
{
	uStInt res = uStIntNoMatch;

	if (currentEvent->evType == evTangoReleased){
	
		BEGIN_EVENT_HANDLER(PDoorsAlarmStateChart, eStateTangoReleased);
			// No event action.
		END_EVENT_HANDLER(PDoorsAlarmStateChart);

		res =  uStIntHandlingDone;
	}
	return (res); 
}

void entryTangoReleasedState(void)
{
	wishCounter = 0;
	setState("tnR");
}

uStInt evTangoReleasedChecker(void)
{
	int8_t res;
	
	res = uStIntNoMatch;

	if (currentEvent->evType == evMotorOutput53sSwitchedHigh)  {	
		wishCounter ++;
		if ( wishCounter >= 3 )
		{
			BEGIN_EVENT_HANDLER(PDoorsAlarmStateChart, eStateIdle);
				// No event action.
			END_EVENT_HANDLER(PDoorsAlarmStateChart);
			res =  uStIntHandlingDone;
		}
	}	
	
	return res;
}

void entryIndiaOnState(void)
{
	startADCPolling();
	switchRelayToPlusLine15();
	stopIntervalTimer();
	setState("ind");
}

void exitIndiaOnState(void)
{
//	printf("exit I\n");
	stopADCPolling();
	stopIntervalTimer();
}

uStInt evIndiaOnChecker(void)
{
//	printf("check for event in State evStateIdle\n");
	uStInt res = uStIntNoMatch;

	if ((currentEvent->evType == evTangoPressed) || (currentEvent->evType == evIndiaSwitchedOff))
	{	
		BEGIN_EVENT_HANDLER(PDoorsAlarmStateChart, eStateIdle);
		// No event action.

		END_EVENT_HANDLER(PDoorsAlarmStateChart);
		res =  uStIntHandlingDone;		
	}
	if (currentEvent->evType == evMotorOutput53sSwitchedHigh) 
	{	
		switchRelay53ToMoterOutput();
		res =  uStIntHandlingDone;		
	}
	if (currentEvent->evType == evMotorOutput53sSwitchedLow) 
	{	
		startIntervalTimer();
		res =  uStIntHandlingDone;		
	}
	if (currentEvent->evType == evTimerExpired)
	{
		stopIntervalTimer();
		switchRelayToPlusLine15();
		res =  uStIntHandlingDone;
	}
	return (res);
}



#ifndef  sdccNULL 

#define tfNull 0

#else

t_fvoid  tfNull;

#endif 

// attention: sequence must be the same as above enum eStates

xStateType xaStates[eNumberOfStates] = {
 	{eStateDoorsAlarm,    // name
 		-1,									//parent
 		eStateIdle,    // default substate
 		0,    // keep history
 		evDoorsAlarmChecker,    // event checking fu
 		tfNull,     //    entering state
 		tfNull},     // exiting state

 	{eStateIdle,
 		eStateDoorsAlarm,
 		-1,
 		0,
 		evIdleChecker,
 		entryIdleState,
 		tfNull
	},
 	{eStateTangoPressed,
 		eStateDoorsAlarm,
 		-1,
 		0,
 		evTangoPressedChecker,
 		entryTangoPressedState,
 		tfNull
	},
	{eStateTangoReleased,
 		eStateDoorsAlarm,
 		-1,
 		0,		 							
 		evTangoReleasedChecker,
 		entryTangoReleasedState,
 		tfNull
	}, 	 
	{eStateIndiaOn,
		eStateDoorsAlarm,
		-1,
		0,
		evIndiaOnChecker,
		entryIndiaOnState,
		exitIndiaOnState
	} 
};


void startStateCharts()
{

#ifdef  sdccNULL 

	tfNull = (t_fvoid ) NULL;

#endif 

 	PDoorsAlarmStateChart = & SDoorsAlarmStateChart; 
	createTStatechart (& SDoorsAlarmStateChart, xaStates, eNumberOfStates, eStartState);
	
}


void stopStateCharts()
{
	destructTStatechart(&SDoorsAlarmStateChart);
}


bool processEvent(TStatechart* ts,CDoorsAlarmEvent* ev)
{
	currentEvent = ev;
	return ProcessEvent(ts);
}

