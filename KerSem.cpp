#include "KerSem.h"
#include "Schedule.h"
#include "PCB.h"
#include "Thread.h"


KernelSemaphore::KernelSemaphore(int init)
{
	asm sti;
	value=init;//Vrijednost semafora
	blockedList=new List();//Lista blokiranih niti
	asm cli;
}
KernelSemaphore::~KernelSemaphore()
{
	lock();
	while(value<0)//Signaliziranje svim blokiranim nitima
		signal();
	delete blockedList;
	unlock();
}
void KernelSemaphore::wait()
{
	/*wait() operacija sa predavanja*/
	asm cli;
	value--;
	if(value<0)
	{
		PCB::runningPCB->status=PCB::BLOCKED;//Stanje niti je blokirano
		blockedList->add(PCB::runningPCB);//Ubacujemo nit u listu blokiranih na ovom semaforu
	}
	asm sti;
	dispatch();
}
void KernelSemaphore::signal()
{
	/*signal() operacija sa predavanja*/
	asm cli;
	value++;
	if(value<=0)
	{
		PCB* temp=blockedList->get();//Skidamo nit iz liste blokiranih
		if(PCB::allPCB->find(temp->id))
		{
			temp->status=PCB::READY;//Stanje niti je spremno
			Scheduler::put(temp);//Ubacujemo je u scheduler
		}
	}
	asm sti;
}
int KernelSemaphore::val()const
{
	return value;
}