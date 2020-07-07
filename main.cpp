/*Moj kod*/
#include <iostream.h>
#include "Timer.h"
#include "PCB.h"//testiranje
#include "IVTEntry.h"//testiranje
#include <dos.h>//testiranje

/*Lokovi sa ugnjezdavanjem*/
#define lock() asm{\
					pushf;\
					cli;\
				}
#define unlock() asm popf;

/*Argumenti koje prosljedjujemo userMain-u*/
volatile int argcGlobal;
char** argvGlobal;
volatile int resultGlobal;//Rezultat userMain-a
volatile int globalIP, globalCS;//ne koriste se
unsigned system_sp,system_ss;//SP i SS main() funkcije

int userMain(int, char*[]);
class Thread;
/*Klasa korisnicke niti*/
class UserMain:public Thread
{
public:
	void run();
};

void UserMain::run()
{
	resultGlobal=userMain(argcGlobal,argvGlobal);
	dispatch();
}

/*Klasa prazne niti*/
class IdleThread:public Thread
{
public:
	void run();
};
void IdleThread::run()
{
	while(1);
}
int main(int argc, char* argv[])
{	
	argcGlobal=argc;
	argvGlobal=argv;
	int a;
	
	lock();
	/*Deklarisanje prve dvije niti*/
	Thread* userMain=new UserMain();
	userMain->start();
	idleThread=new IdleThread();
	idleThread->start();
	unlock();
	Timer::setISR();//Pocetak prekidnog koda
	dispatch();//Prvi dispatch()
	Timer::restoreISR();//Kraj prekidnog koda
	
	return resultGlobal;
}