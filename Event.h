#ifndef _event_h_
#define _event_h_

#include "IVTEntry.h"

typedef unsigned char IVTNo;

class KernelEvent;

class Event
{
public:
	Event (IVTNo ivtNo);
	~Event ();
	void wait();
protected:
	friend class KernelEvent;
	void signal(); // can call KernelEv
private:
	KernelEvent* myImpl;
};
#endif