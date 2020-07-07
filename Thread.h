#ifndef _Thread_h_
#define _Thread_h_

typedef unsigned long StackSize;
const StackSize defaultStackSize = 4096;
typedef unsigned int Time; // time, x 55ms
const Time defaultTimeSlice = 2; // default = 2*55ms
typedef int ID;

class PCB; // Kernel's implementation of a user's thread

class Thread
{
public:
	void start();
	void waitToComplete();
	virtual ~Thread();
	void sleep(unsigned int timeToSleep);
	friend void running();
//protected:
	friend class PCB;
	Thread(unsigned long stackSize = defaultStackSize,
		unsigned int timeSlice = defaultTimeSlice);
	virtual void run() {}
//private:
	PCB* myPCB;
};

void dispatch();

#endif
