
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

typedef char stateString [10];
stateString * wwState;
int16_t  stateEntryCnt;

void setState(char* sta)
{
	wwState = (stateString *) sta;
	++stateEntryCnt;
}

CDoorsAlarmEvent* getNextEvent(CDoorsAlarmEvent* pev)
{
	CDoorsAlarmEvent* res = NULL;
	if (fatalErrorOccurred) {     // do this with highest priority (at the beginning)
		//			fatalErrorOccurred = 0;	  // once occurred state stays until restart/reset
		pev->evType = evFatalError;
		res = pev;
	}
	if (doorsOpenEvent ==  1) {
		doorsOpenEvent = 0;
		pev->evType = evDoorsOpen;
		res = pev;
	}
	if (doorsClosedEvent ==  1) {
		doorsClosedEvent = 0;
		pev->evType = evDoorsClosed;
		res = pev;
	}
	if (timerReachedEvent ==  1) {
		timerReachedEvent = 0;
		pev->evType = evTimerExpired;
		res = pev;
	}
	if (runningSecondsTick){
		//		cli();    // 8-bit access is already atomic
		runningSecondsTick = 0;
		//		sei();
		pev->evType = evSecondsTick;
		res  = pev;
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
	setState("idl");
	startDurationTimer(maxSecsPossible);   // enable secondsTick
}

void exitIdleState(void)
{
	stopDurationTimer();
}

uStInt evIdleChecker(void)
{
	uStInt res = uStIntNoMatch;

	if (currentEvent->evType == evDoorsClosed)
	{	
			BEGIN_EVENT_HANDLER(PDoorsAlarmStateChart, eStateLightOnAlarm);
				// No event action.
			END_EVENT_HANDLER(PDoorsAlarmStateChart);
			res =  uStIntHandlingDone;
	}
	if (currentEvent->evType == evTimerExpired)
	{
		startDurationTimer(maxSecsPossible);   // enable secondsTick
		res =  uStIntHandlingDone;
	}
	if (currentEvent->evType == evSecondsTick)
	{
		if ( (getSecondsInDurationTimer() % 10) == 0)
		{
			retrieveDoorEvents ();
		}
		res =  uStIntHandlingDone;
	}
	return (res); 
}

void entryLightOnAlarmState(void)
{
	setState("LOA");
}

uStInt evLightOnAlarmChecker(void)
{
	uStInt res = uStIntNoMatch;

	if (currentEvent->evType == evDoorsOpen){
	
		BEGIN_EVENT_HANDLER(PDoorsAlarmStateChart, eStateIdle);
			// No event action.
		END_EVENT_HANDLER(PDoorsAlarmStateChart);

		res =  uStIntHandlingDone;
	}
	return (res); 
}

void entryLightOnAlarmActiveState(void)
{
	setState("LAA");
	retrieveDoorEvents();
	startDurationTimer(5);
	startAlarm();
	startBuzzer();
}

void exitLightOnAlarmActiveState(void)
{
	setState("noS");
	stopDurationTimer();
	stopAlarm();
	stopBuzzer();
}


uStInt evLightOnAlarmActiveChecker(void)
{
	int8_t res;
	
	res = uStIntNoMatch;

	if (currentEvent->evType == evTimerExpired)  {	
		BEGIN_EVENT_HANDLER(PDoorsAlarmStateChart, eStateLightOnAlarmPaused);
			// No event action.
		END_EVENT_HANDLER(PDoorsAlarmStateChart);
		res =  uStIntHandlingDone;
		
	}	
	if (currentEvent->evType == evSecondsTick)
	{
		if (getSecondsInDurationTimer() == 1)  {
			stopBuzzer();
			res =  uStIntHandlingDone;
		}
	}
	return res;
}

void entryLightOnAlarmPausedState(void)
{
	setState("LAP");
	startDurationTimer(15);
}

void exitLightOnAlarmPausedState(void)
{
	setState("noS");
	stopDurationTimer();
}

uStInt evLightOnAlarmPausedChecker(void)
{
//	printf("check for event in State evStateIdle\n");
	uStInt res = uStIntNoMatch;

	if (currentEvent->evType == evTimerExpired)
	{	
		BEGIN_EVENT_HANDLER(PDoorsAlarmStateChart, eStateLightOnAlarmActive);
		// No event action.

		END_EVENT_HANDLER(PDoorsAlarmStateChart);
		res =  uStIntHandlingDone;		
	}
	return (res);
}


void entryFatalErrorState(void)
{
	printf("entry FatalError\n");
	printf("**************fatal Error: %s *************************\n",lastFatalErrorString);
	setState("fEr");
}

void exitFatalErrorState(void)
{
	printf("exit FatalError\n");
	setState("noS");
}

uStInt evFatalErrorChecker(void)
{
	uStInt res = uStIntNoMatch;
	//	printf("check for event in State evStateIdle\n");

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
 		exitIdleState
	},
 	{eStateLightOnAlarm,
 		eStateDoorsAlarm,
 		eStateLightOnAlarmActive,
 		0,
 		evLightOnAlarmChecker,
 		entryLightOnAlarmState,
 		tfNull
	},
	{eStateLightOnAlarmActive,
 		eStateLightOnAlarm,
 		-1,
 		0,		 							
 		evLightOnAlarmActiveChecker,
 		entryLightOnAlarmActiveState,
		exitLightOnAlarmActiveState	
	}, 	 
	{eStateLightOnAlarmPaused,
		eStateLightOnAlarm,
		-1,
		0,
		evLightOnAlarmPausedChecker,
		entryLightOnAlarmPausedState,
		exitLightOnAlarmPausedState
	} ,

	{eStateFatalError,
		eStateDoorsAlarm,
		-1,
		0,
		evFatalErrorChecker,
		entryFatalErrorState,
		exitFatalErrorState
	}
};


void startStateCharts()
{

#ifdef  sdccNULL 

	tfNull = (t_fvoid ) NULL;

#endif 
	stateEntryCnt = 0;
	
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

