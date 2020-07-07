#ifndef _IVTENTRY_H_
#define _IVTENTRY_H_

#include "PCB.h"

typedef void interrupt (*Handler)(...);

class PCB;
class KernelEvent;

class IVTEntry
{
public:
	static IVTEntry* ivtTable[256];
	IVTEntry(unsigned char ivtNumber,
				Handler newHandler,
				int callOldHandlerFlag);
	~IVTEntry();
	static unsigned usedTable[256];

	void signal();
//private:
	Handler oldHandler,newHandler;
	int callOldHandlerFlag;
	unsigned char ivtNumber;
	KernelEvent* myKernelEvent;
	Thread* myLifetimeThread;//------
};

/*Makro samo poziva signal odredjenog ulaza*/
#define PREPAREENTRY(ivtNumber,callOldHandlerFlag)\
void interrupt interruptRoutine##ivtNumber(...);\
IVTEntry entry##ivtNumber(ivtNumber,interruptRoutine##ivtNumber,callOldHandlerFlag);\
void interrupt interruptRoutine##ivtNumber(...)\
{\
		IVTEntry::ivtTable[ivtNumber]->signal();\
}


#endif