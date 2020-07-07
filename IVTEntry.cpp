#include "IVTEntry.h"
#include "KerEv.h"
#include "Thread.h"
#include "Timer.h"
#include <dos.h>
#include <iostream.h>//test

unsigned IVTEntry::usedTable[256]={0};//Nebitno, ne koristi se
IVTEntry* IVTEntry::ivtTable[256]={0};//Tabela ulaza

IVTEntry::IVTEntry(unsigned char ivtNumber,
					Handler newHandler,
					int callOldHandlerFlag)
{
	asm cli;
	myKernelEvent=0;
	this->callOldHandlerFlag=callOldHandlerFlag;//flag iz PREPAREENTRY makroa
	
	this->ivtNumber=ivtNumber;
	this->newHandler=newHandler;
	oldHandler=getvect(ivtNumber);//Nabavljamo staru rutinu
	setvect(ivtNumber,newHandler);//Postavljamo novu rutinu
	ivtTable[ivtNumber]=this;//Postavljamo ovaj ulaz u tabelu
	asm sti;
}

IVTEntry::~IVTEntry()
{
	asm cli;
	setvect(ivtNumber, oldHandler);//Vracamo staru rutinu
	
	ivtTable[ivtNumber]=0;//Brisemo ulaz iz tabele
	if(ivtNumber==9)//Vidio sam negdje da ovako treba, nije radilo bez ovoga
	{
		asm{
			mov al,20h
			out 20h,al
		}
	}
	oldHandler=0;
	newHandler=0;
	asm sti;
}

void IVTEntry::signal()
{
	if(myKernelEvent)//Ako postoji myKernelEvent
		myKernelEvent->signal();//Pozivamo njegov signal
	if(callOldHandlerFlag)//Ako treba pozvati i staru rutinu
	{
		for(int i=0;i<30000;++i)//Ovo je mali workaround, posto negdje
			for(int j=0;j<30000;j++);//nisam dobro ispratio lock i unlock
		asm cli;
		(*oldHandler)();//Pozivamo staru rutinu
		asm sti;
	}
	dispatch();//Preotimanje
}