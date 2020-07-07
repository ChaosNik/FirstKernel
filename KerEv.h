#ifndef _KerEv_h_
#define _KerEv_h_

#include "Event.h"

class IVTEntry;
class PCB;

class KernelEvent
{
public:
	KernelEvent(unsigned char ivtNumber);
	~KernelEvent();

	void wait();
	void signal();

//private:
	unsigned char ivtNumber;
	IVTEntry* ivtEntry;
	unsigned flag;
	PCB* myPCB;
	List* blockedList;
};

#endif