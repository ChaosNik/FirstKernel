#ifndef _PCB_h_
#define _PCB_h_

#include "List.h"
/*Definisanje lock-a i unlock-a sa mogucnoscu gnjiezdenja*/
#define lock() asm{\
					pushf;\
					cli;\
				}
#define unlock() asm popf;

class Semaphore;
class Thread;
class List;

class PCB
{
public:
	PCB(unsigned long stackSize,
		unsigned int timeSlice,
		Thread* myThread);
	~PCB();
    void start();
    void waitToComplete();
    void sleep(unsigned int timeToSleep);
	void goFromIdleThreadToMainFunction();//Ne koristi se
//private:
	void run();//Poziva myThread->run()
	static void wrapperRun();//wrapper za izvrsavanje
	
	friend class Timer;
	friend class List;
	friend class KernelSemaphore;
	
	/*Promjenjive za Timer*/
	static volatile int noMoreTime;//Nema vise vremena
	static volatile int mustSwitchContext;//Pozvana iz dispatch()-a

	/*Pokazivaci na bitnije niti*/
	static PCB* runningPCB;//Nit koja se izvrsava
	static PCB* mainPCB;//Nit koja izvrsava userMain()
	static PCB* systemPCB;//Nit koja izvrsava main(), ne koristi se
	static PCB* idlePCB;//Nit koja radi prazan posao
	//List* readyPCB; je Scheduler
	static List* blockedPCB;//Lista blokiranih niti (onih koje spavaju)
	static List* terminatedPCB;//Lista unistenih niti, ne koristi se
	static List* allPCB;//Lista svih niti koje su zive
	static unsigned int globalID;//Globalni ID na osnovu kojeg niti dobijaju svoj ID

	/*Tip stanja sa dijagrama stanja*/
	enum Status
	{
		NEW=1,
		RUNNING=2,
		BLOCKED=4,
		READY=8,
		TERMINATED=16,
	};

	/*Promjenjive*/
    unsigned long stackSize;//Velicina steka
    unsigned stackPointer;
	unsigned stackSegment;
    Status status;//Stanje
	unsigned int timeToSleep;//Vrijeme koje nit treba da spava
	unsigned int timeToRun;//Vrijeme koje nit treba da se pokrece
	unsigned int timeSlice;//Velicina timeSlice-a za timeToRun
	unsigned int* stack;//Pokazivac na stek
    Semaphore* semaphore;//Semafor blokiranih niti ove niti
	unsigned int id;//userMain-0, idle-1, threads>=2
	Thread* myThread;//Pokazivac na nit ovog PCB-a
};

#endif