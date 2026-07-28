#ifndef _PSTAT_H_
#define _PSTAT_H_

#include "param.h"

#define NEWPROC_USERINIT 0
#define NEWPROC_KFORK 1

extern struct spinlock pstat_lock;
extern struct pstat* pstat;

struct pstat {
  int inuse[NPROC];   // whether this slot of the process table is in use (1 or 0)
  int tickets[NPROC]; // the number of tickets this process has
  int pid[NPROC];     // the PID of each process 
  int ticks[NPROC];   // the number of ticks each process has accumulated 
};

#endif // _PSTAT_H_