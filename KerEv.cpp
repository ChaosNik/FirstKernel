#include "KerEv.h"
#include "Schedule.h"
#include "PCB.h"
#include "Thread.h"

KernelEvent::KernelEvent(unsigned char ivtNumber)
{
	asm cli;
	flag=0;//Analogno vrijednosti semafora
	this->ivtNumber=ivtNumber;//Pamtivo IVT broj
	ivtEntry=IVTEntry::ivtTable[ivtNumber];//Uzimamo ulaz iz tabele ulaza
	ivtEntry->myKernelEvent=this;//Setujemo da je dogadjaj za taj ulaz ovaj dogadjaj
	myPCB=PCB::runningPCB;//Blokirana nit postaje trenutna running nit
	asm sti;
}
KernelEvent::~KernelEvent()
{
	asm cli;
	flag=0;
	if(ivtEntry->myKernelEvent)//Brisemo dogadjaj iz ulaza
		ivtEntry->myKernelEvent=0;
	signal();//Posljednji signal
	asm sti;
}
void KernelEvent::wait()
{
	asm cli;
	if(myPCB==PCB::runningPCB)//Ako je trenutna nit ista kao i prije
	{
		flag=1;//Setujemo flag
		myPCB->status=PCB::BLOCKED;//Stanje je blokirano
		if(ivtNumber==9)//Ovo sam vidio negdje da treba, bez ovoga nije radilo
		{
			asm{
				mov al,20h
				out 20h,al
			}
		}
		dispatch();//Preotimanje
	}
	asm sti;
	if(myPCB!=PCB::runningPCB)//Ako nit nije ista kao prije, onda nista
		return;
}
void KernelEvent::signal()
{
	asm cli;
	if(flag&&myPCB)//Ukoliko je flag setovan i postoji nit
	{
		myPCB->status=PCB::READY;//Stanje niti je spremno
		Scheduler::put(myPCB);//Ubacujemo nit u red spremnih
		flag=0;//Resetujemo flag
	}
	asm sti;
}