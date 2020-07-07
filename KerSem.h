#ifndef _KerSem_h_
#define _KerSem_h_

#include "List.h"

class PCB;
class List;

class KernelSemaphore
{
public:
	KernelSemaphore(int init);
	virtual ~KernelSemaphore();
	virtual void wait ();
	virtual void signal();
	int val () const; // Returns the current value of the semaphore
private:
	int value;
	List* blockedList;
};

#endif