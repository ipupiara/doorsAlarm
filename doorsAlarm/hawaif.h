/*
 * hawaif.h
 *
 * Created: 1/20/2015 10:31:44 AM
 *  Author: ixchel
 */ 


#ifndef HAWAIF_H_
#define HAWAIF_H_

int8_t  timerStarted;
int8_t  timerReachedEvent;
int8_t   doorsOpenEvent;
int8_t	 doorsClosedEvent;
int8_t fatalErrorOccurred;
char lastFatalErrorString [20];

void startDurationTimer(int16_t secs);
void stopDurationTimer();

void startAlarm();

void stopAlarm();

void initHW();

void enterIdleSleepMode();

#endif /* HAWAIF_H_ */