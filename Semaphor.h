#ifndef _Semaphor_h_
#define _Semaphor_h_

#include "KerSem.h"

class KernelSemaphore;

class Semaphore {
public:
	Semaphore(int init=1);
	virtual ~Semaphore();
	virtual void wait();
	virtual void signal();
	int val() const; // Returns the current value of the semaphore
private:
	KernelSemaphore* myImpl;
};

#endif