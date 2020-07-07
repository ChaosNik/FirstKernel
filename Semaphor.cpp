#include "Semaphor.h"
/*Samo pozivamo metode it KernelSemaphore-a*/

Semaphore::Semaphore(int init)
{
	myImpl = new KernelSemaphore(init);
}
Semaphore::~Semaphore()
{
	delete myImpl;
}
void Semaphore::wait()
{
	myImpl->wait();
}
void Semaphore::signal()
{
	myImpl->signal();
}
int Semaphore::val() const // Returns the current value of the semaphore
{
	return myImpl->val();
}