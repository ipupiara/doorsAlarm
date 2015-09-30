/*
 * hawaif.h
 *
 * Created: 1/20/2015 10:31:44 AM
 *  Author: ixchel
 */ 


#ifndef HAWAIF_H_
#define HAWAIF_H_

int8_t  timerReachedEvent;
int8_t  indiaLineOnEvent;
int8_t   indaLineOffEvent;
int8_t   tangoPressedEvent;
int8_t	 tangoReleasedEvent;
int8_t   evMotorOutput53sSwitchedHighEvent;
int8_t   evMotorOuput53sSwitchedLowEvent ;


//#define testMethods   //used for simulation of wishermotor with a second AtTiny85 on breadboard

typedef char stateString [3];

stateString * wwState;

int16_t  stateEntryCnt;

void setState(char* sta);

void switchRelayToPlusLine15();

void switchRelay53ToMoterOutput();

void startADCPolling();

void stopADCPolling();

#ifdef testMethods

	void startTimerIntervalSecs(int8_t secs);

	void stopTimerIntervalSecs();

#endif

void startIntervalTimer();
void stopIntervalTimer();

int isTangoLineOn();
int isIndiaLineOn();

void initHW();

void enterIdleSleepMode();

#endif /* HAWAIF_H_ */