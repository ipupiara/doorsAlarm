
#if !defined(CCtateClassHeader)
#define CCtateClassHeader

#include "TStatechart.h"


enum eEventTypes
{
	evNone = 0,
	evTangoPressed,
	evTangoReleased,
	evIndiaSwitchedOn,
	evIndiaSwitchedOff,
	evMotorOutput53sSwitchedLow,
	evMotorOutput53sSwitchedHigh,
	evTimerExpired
};

// This defines and names the states the class has.
// attention: sequence must be the same as in xaStates (below) !!!
enum eStates
{
	eStateDoorsAlarm,
	eStartState = eStateDoorsAlarm,
	eStateIdle,
	eStateTangoPressed,
	eStateTangoReleased,
	eStateIndiaOn,
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


