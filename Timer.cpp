#include "Timer.h"
#include "Schedule.h"
#include "List.h"
#include "PCB.h"
#include "Thread.h"
#include <dos.h>
#include <iostream.h>
#include "IVTEntry.h"

Timer* timer=new Timer();//Inicijalizacija timer-a
PCB* tempPCB;//Pokazivac na nit za koristenje u funkcijama
volatile unsigned prviDispatch=1;//Flag za inicijalnu promjenu konteksta

/*Ove promjenjive imaju u Joldzicevom kodu*/
/*Koristimo ih za inicijalizaciju i terminaicju prekidne rutine*/
static volatile unsigned tempsp, tempss, mainss, mainsp;
volatile unsigned adresaPrekidnogVektora = 0x08;
volatile unsigned adresaSlobodnogMjestaZaPrekid = 0x60;
volatile unsigned brojacPrekida=0;
volatile PCB *aktivnaNit;
volatile unsigned redniBrojAktivneNiti=0;
volatile unsigned brojZavrsenihNiti=0;
volatile int obaveznaPromjenaKonteksta=0;//Ovo je realizovano u PCB-u kao mustSwitchContext

volatile unsigned adresaPomjeraja;
volatile unsigned adresaSegmenta;
volatile unsigned prazanOff;
volatile unsigned prazanSeg;

void interrupt Timer::ISR()
{
	asm cli;
	if(!PCB::mustSwitchContext)//Ako nismo usli u rutinu preko dispatch()-a
	{							//tj. ako je timerski prekid
		PCB::runningPCB->timeToRun--;//Umanjimo vrijeme za izvrsavanje trenutnoj niti
		for(int i=0; i<PCB::blockedPCB->numOfElem(); ++i)//Prolazimo kroz sve 
		{												//blokirane niti
			tempPCB=PCB::blockedPCB->elem(i);//Pamtimo nit u privremenu
			tempPCB->timeToSleep--;//Smanjujemo joj vrijeme spavanja
			if(tempPCB->timeToSleep<=0)//Ukoliko vrijeme postane 0
			{
				tempPCB->status=PCB::READY; //Mijenjamo joj status u READY
				Scheduler::put(tempPCB);//Stavljamo je u Scheduler
				PCB::blockedPCB->remove(tempPCB->id);//Uklanjamo je iz niza blokiranih
			}
		}
		tick();//Ovu funkciju definise korisnik i onda se izvrsava svakim otkucajem timer-a
	}
	
	if(PCB::runningPCB->timeSlice != 0 && !PCB::mustSwitchContext)//Ako rutina nije pozvana
	{			//iz dispatch()-a i ako joj timeSlice nije 0 tj. beskonacan
		if(PCB::runningPCB->timeToRun == 0)//Ako je vrijeme za izvrsavanje niti isteklo
		{
			PCB::noMoreTime = 1;//Setujemo flag noMoreTime na 1
			PCB::runningPCB->timeToRun = PCB::runningPCB->timeSlice;
			//Dodjeljujemo niti novi timeSlice
		}
	} 
	if(PCB::mustSwitchContext||PCB::noMoreTime)//Ako je rutina pozvana iz dispatch()-a
	{						//ili je niti isteklo vrijeme
		//Izlazak iz main-a prvim dispatch-om
		//tj. ulazak u mod sa prekidima
		if(prviDispatch)
		{
			systemsp=_SP;
			systemss=_SS;
			
			prviDispatch=0;
		}
		
		//Preuzimanje konteksta
		PCB::runningPCB->stackPointer = _SP;
		PCB::runningPCB->stackSegment = _SS;
		
		//Postavljanje statusa niti iz koje izlazimo
		if(PCB::runningPCB->status==PCB::NEW)
			PCB::runningPCB->status=PCB::READY;
		if(PCB::runningPCB->status==PCB::RUNNING)
			PCB::runningPCB->status=PCB::READY;
		if(PCB::runningPCB->status==PCB::NEW)
			PCB::runningPCB->status=PCB::READY;
		
		//Prazna, idle ili blokirana nit se 
		//ne stavlja u Scheduler
		if(PCB::runningPCB!=0&&
			PCB::runningPCB!=PCB::idlePCB&&
			PCB::runningPCB->status!=PCB::BLOCKED)
			Scheduler::put(PCB::runningPCB);
		
		//Preuzimamo sljedecu nit
		PCB::runningPCB = Scheduler::get();

		//Provjera statusa niti
		if(PCB::runningPCB)
		{
			//Ako je nit zavrsila
			if(PCB::runningPCB->status==PCB::TERMINATED)
				PCB::runningPCB=PCB::idlePCB;
			//Ako nit spava ili je blokirana
			if(PCB::runningPCB->status==PCB::BLOCKED)
				PCB::runningPCB=PCB::idlePCB;
		}
		//Ako je Scheduler prazan
		if(!PCB::runningPCB||PCB::runningPCB==PCB::idlePCB)
		{
			//Povratak u main
			if(PCB::allPCB->numOfElem()==2&&
				PCB::mainPCB->status==PCB::TERMINATED)
			{				
				delete PCB::idlePCB;
				delete PCB::mainPCB;
				PCB::runningPCB=0;
				
				_SP=systemsp;
				_SS=systemss;
				asm sti;
				return;
			}
			PCB::runningPCB = PCB::idlePCB;
		}
		
		//Dodjelimo ponovo timeSlice niti
		PCB::runningPCB->timeToRun = PCB::runningPCB->timeSlice;
		
		//Dodjelimo adresu konteksta
		_SP=PCB::runningPCB->stackPointer;
		_SS=PCB::runningPCB->stackSegment;
	}
	
	asm sti;
	if(!PCB::mustSwitchContext) asm int 60h;//Poziv prethodne rutine (valja se)
	asm cli;
	/*Setujemo flag-ove za sljedeci ulazak u prekidnu rutinu*/
	PCB::mustSwitchContext = 0;
	PCB::noMoreTime = 0;
	asm sti;
}

/*Samo prilagodimo Joldzicev kod*/
volatile unsigned staraRutinaOff, staraRutinaSeg, timerISRoff, timerISRseg;
void Timer::setISR()
{
	asm cli;
	adresaPomjeraja=adresaPrekidnogVektora*4;
	adresaSegmenta=adresaPrekidnogVektora*4+2;
	prazanOff=adresaSlobodnogMjestaZaPrekid*4;
	prazanSeg=adresaSlobodnogMjestaZaPrekid*4+2;
	
	timerISRoff=FP_OFF(&Timer::ISR);
	timerISRseg=FP_SEG(&Timer::ISR);
	
	asm {
		push es
		push ax
		push di
		mov ax,0
		mov es,ax

		mov di, word ptr adresaSegmenta
		mov ax, word ptr es:di
		mov word ptr staraRutinaSeg, ax
		mov ax, timerISRseg
		mov word ptr es:di, ax

		mov di, word ptr adresaPomjeraja
		mov ax, word ptr es:di
		mov word ptr staraRutinaOff, ax
		mov ax, timerISRoff
		mov word ptr es:di, ax

		mov di, word ptr prazanOff
		mov ax, word ptr staraRutinaOff
		mov word ptr es:di, ax
		mov di, word ptr prazanSeg
		mov ax, word ptr staraRutinaSeg
		mov word ptr es:di, ax

		pop di
		pop ax
		pop es
	}
	asm sti;
}

/*Isto samo prilagodjavamo*/
void Timer::restoreISR()
{
	asm cli;
	adresaPomjeraja=adresaPrekidnogVektora*4;
	adresaSegmenta=adresaPrekidnogVektora*4+2;
	asm {
		push es
		push ax
		push di

		mov ax,0
		mov es,ax

		mov di, word ptr adresaSegmenta
		mov ax, word ptr staraRutinaSeg
		mov word ptr es:di, ax

		mov di, word ptr adresaPomjeraja
		mov ax, word ptr staraRutinaOff
		mov word ptr es:di, ax

		pop di
		pop ax
		pop es
	}
	asm sti;
}