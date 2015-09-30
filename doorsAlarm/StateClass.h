
#if !defined(CCtateClassHeader)
#define CCtateClassHeader

#include "TStatechart.h"

//  this statechart models the behaviour of an alarm system in a armoire, where an alarm is needed when the doors are closed
//  but the light has beed forgotten to be switched off. It beeps shortly whith a buzzer as a backup for the little bells
//  which get ringed by a servomotor at the same time and somewhat longer for their nice sound (cf. carneval at schwyz).
//  the alarm system requires that when the light is off, also on the PCB of the CPU, current is switched off.
//  This condition greatly simplifies the behaviour model.

enum eEventTypes
{
	evNone = 0,
	evDoorsOpen,
	evDoorsClosed,
	evTimerExpired,
	evFatalError
};

// This defines and names the states the class has.
// attention: sequence must be the same as in xaStates (below) !!!
enum eStates
{
	eStateDoorsAlarm,
	eStartState = eStateDoorsAlarm,
	eStateIdle,
	eStateLightOnAlarm,
	eStateLightOnAlarmActive,
	eStateLightOnAlarmPaused,
	eStateFatalError,
	eNumberOfStates
};

TStatechart SDoorsAlarmStateChart;
TStatechart* PDoorsAlarmStateChart;

typedef struct  {
	int evType;
//	int evId;
/*	union {
		int8_t keyCode;
		struct {			// currently not in use
			float   voltage;  
			int8_t  potiPos;
			int8_t  jobType;
		} zeroAdjustingState;
	}  evData;
	*/
} CDoorsAlarmEvent ;


CDoorsAlarmEvent* getNextEvent(CDoorsAlarmEvent* pev);

void startStateCharts();

void stopStateCharts();

bool processEvent(TStatechart* t,CDoorsAlarmEvent* ev);

#endif


