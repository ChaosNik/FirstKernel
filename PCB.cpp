#include "PCB.h"
#include <dos.h>
#include <iostream.h>//samo test
#include "Semaphor.h"
#include "Thread.h"
#include "Schedule.h"

/*Setovanja i deklaracije*/
volatile int PCB::noMoreTime = 0;
volatile int PCB::mustSwitchContext = 0;
volatile unsigned temporaryRegister;

PCB* PCB::runningPCB = 0;
PCB* PCB::mainPCB = 0;
PCB* PCB::systemPCB = 0;
PCB* PCB::idlePCB = 0;
//List* readyPCB = new List(); je Scheduler
List* PCB::blockedPCB = new List();
List* PCB::terminatedPCB = new List();
List* PCB::allPCB = new List();
unsigned PCB::globalID=0;
static volatile unsigned newsp, newss, oldsp, oldss;
static volatile unsigned newip, newcs;

PCB::PCB(unsigned long stackSize,
	unsigned int timeSlice,
	Thread* myThread):
	stackSize(stackSize),
	timeToSleep(0),
	timeToRun(timeSlice),
	timeSlice(timeSlice),
	myThread(myThread)
{
	asm cli;
	id=globalID;
	if(globalID==0)//Prva nit koja se konstruise je userMain()
		mainPCB = this;
	if(globalID==1)//Druga nit koja se konstruise je idle()
		idlePCB = this;
   	status = NEW;//Stanje niti je da je nova
	semaphore = new Semaphore(0);//Konstruisemo semafor koji nije blokiran
	stack = new unsigned[stackSize];//Rezervisemo mjesto za stek
	newsp=stackPointer=FP_OFF(stack+stackSize-1);//Namjestamo offset steka
	newss=stackSegment=FP_SEG(stack+stackSize-1);//Namjestamo segment steka
	
	newip=FP_OFF(&(wrapperRun));//IP funkcije niti
	newcs=FP_SEG(&(wrapperRun));//CS funkcije niti
	
	asm{
		/*Setujemo vrijednosti SP-a i SS-a*/
		mov oldsp,sp
		mov oldss,ss
		
		mov sp,newsp
		mov ss,newss
		
		/*Zapamtimo statusni registar na stek*/
		mov ax,0x200
		push ax
		
		/*Zapamtimo nove CS i IP na stek*/
		mov ax,newcs
		push ax
		mov ax,newip
		push ax
		
		/*Zapamtimo sve registe, tj. za svaki registar pushujemo 0*/
		mov ax,0
		push ax
		push ax
		push ax
		push ax
		push ax
		push ax
		push ax
		push ax
		push ax
		
		/*Vratimo stare SP i SS*/
		mov newsp,sp
		mov newss,ss
		
		mov sp,oldsp
		mov ss,oldss
	}
	
	/*Setujemo SS i SP ove niti*/
	stackPointer=newsp;
	stackSegment=newss;
	
	globalID++;//Inkrementujemo globalni ID
	allPCB->add(this);//Dodajemo ovu nit u listu svih zivih niti
	asm sti;
}
PCB::~PCB()
{
	lock();
	delete stack;//Dealociramo stek ove niti
	delete semaphore;//Dealociramo semafor
	allPCB->remove(id);//Brisemo ovu nit iz liste zivih
	unlock();
}
void PCB::start()
{
	lock();
	if(id==1)//Idle nit se posmatra kao zavrsena i ne stavlja se u Scheduler
		status=TERMINATED;
    else if(status==NEW)//Nova nit se stavlja u Scheduler
	{
		status=READY;//Stanje joj postaje READY
		Scheduler::put(this);//Stavimo je u Scheduler
	}
	unlock();
}
void PCB::waitToComplete()
{
	lock();
    if(runningPCB->status!=NEW&&//Ako running nit nije nova ili zavrsena
		runningPCB->status!=TERMINATED)
	{
		semaphore->wait();//Semafor date niti treba da saceka dok data nit ne zavrsi
	}
	unlock();
}
void PCB::sleep(unsigned int timeToSleep)
{
	lock();
	this->timeToSleep=timeToSleep;//Setujemo vrijeme za spavanje
	status=BLOCKED;//Stanje se mijenja u blokirano
	if(blockedPCB->find(id)==0)//Ako nit nije vec blokirana od neke druge
		blockedPCB->add(this);//dodajemo je u listu blokiranih
   	unlock();
	if(runningPCB==this)//Ukoliko nit blokira sama sebe
		dispatch();//Radimo preotimanje
}
void PCB::run()
{
	myThread->run();//Pokrecemo myThread->run()
	lock();
	status=TERMINATED;//Kada se run zavrsi stanje niti je da je zavrsena
	semaphore->signal();//Signalizujemo semafor
	unlock();
	dispatch();//Preotimamo procesor
}
void PCB::wrapperRun()//Sluzi nam za konstruktor
{
	runningPCB->run();
}