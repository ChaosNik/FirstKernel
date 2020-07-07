#include "Thread.h"
#include "PCB.h"
#include "Timer.h"
#include "iostream.h"

/*Samo pozivamo metode iz PCB-a*/

void Thread::start()
{
	asm cli;
	myPCB->start();
	asm sti;
}

void Thread::waitToComplete()
{
	myPCB->waitToComplete();
}

Thread::~Thread()
{
	delete myPCB;
}

void Thread::sleep(unsigned int timeToSleep)
{
	myPCB->sleep(timeToSleep);
}

Thread::Thread(unsigned long stackSize, unsigned int timeSlice)
{
	asm cli;
	myPCB = new PCB(stackSize,timeSlice,this);
	asm sti;
}

/*Nasilna, korisnicka promjena konteksta*/
void dispatch()
{
	//lock();
	//asm cli;
	PCB::mustSwitchContext=1;//Setujemo flag da se zna da je pozvan dispatch()
	Timer::ISR();//Pozivamo ISR
	//asm sti;
	//unlock();
}