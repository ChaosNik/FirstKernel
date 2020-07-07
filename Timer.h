#ifndef _Timer_h_
#define _timer_h_

#include "Thread.h"

void tick();//Ovu funkciju definise korisnik
static volatile unsigned systemsp, systemss;//SP i SS main() funkcije
extern Thread* idleThread;//Nit koja se vrti kada su sve ostale blokirane
class Timer
{
public:
	Timer() {};//Ne treba se nigdje ni koristiti posto je pozvan u .cpp fajlu
	static void setISR();//Postavlja nas ISR na mjesto tajmerskog ISR-a
	static void interrupt ISR();//Nas ISR
	static void restoreISR();//Vraca stari tajmerski ISR
	
	static void lockISR();//Nebitno, ne koristi se
	static void unlockISR();//Nebitno, ne koristi se
	static unsigned locked;//Nebitno, ne koristi se
};
#endif