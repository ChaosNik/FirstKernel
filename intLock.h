#ifndef _INTLOCK_H_
#define _INTLOCK_H_

#include <thread.h>

extern int mutex_glb;
void mutex_glb_wait();

#define intLock mutex_glb_wait();
#define intUnlock mutex_glb = 1;

#endif