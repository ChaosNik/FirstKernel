#include "Event.h"
#include "KerEv.h"
#include "PCB.h"
#include "Timer.h"
#include "IVTEntry.h"

/*Samo poziva metode iz KernelEvent-a*/

Event::Event(unsigned char ivtNumber)
{
	myImpl=new KernelEvent(ivtNumber);
}
Event::~Event()
{
	delete myImpl;
}
void Event::wait()
{
	myImpl->wait();
}
void Event::signal()
{
	myImpl->signal();
}