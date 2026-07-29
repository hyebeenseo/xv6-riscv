#include "pstat.h"
#include "spinlock.h"
#include "proc.c"

/*
struct pstat {
  int inuse[NPROC];   // whether this slot of the process table is in use (1 or 0)
  int tickets[NPROC]; // the number of tickets this process has
  int pid[NPROC];     // the PID of each process 
  int ticks[NPROC];   // the number of ticks each process has accumulated 
};
*/


struct spinlock pstat_lock;
struct pstat* pstat;

void
pstatinit(void)
{
    struct proc *p;
    initlock(&pstat_lock, "pstat");
    aquire(&pstat_lock);

    for (p = proc; p < &proc[NPROC]; p++) {
        int idx = p - proc;
        pstat->inuse[idx] = 0;
        pstat->tickets[idx] = 0;
        pstat->pid[idx] = -1;
        pstat->ticks[idx] = 0;
    }
    
    release(&pstat_lock);
}

void
pstatnewproc(struct proc *p, int mode)
{
    acquire(&pstat_lock);
    int proc_idx = p - proc;
    switch (mode)
    {
    case NEWPROC_USERINIT:
        pstat->inuse[proc_idx] = 1;
        pstat->pid[proc_idx] = p->pid;
        pstat->tickets[proc_idx] = 1;
        pstat->ticks[proc_idx] = 0;
        break;
    
    case NEWPROC_KFORK:
        int parent_idx = (p->parent) - proc;
        int parent_tickets = pstat->tickets[parent_idx];

        pstat->inuse[proc_idx] = 1;
        pstat->pid[proc_idx] = p->pid;
        pstat->tickets[proc_idx] = parent_tickets; // Inherit parent's tickets
        pstat->ticks[proc_idx] = 0;
        break;

    default:
        panic("pstatnewproc");
    }
    release(&pstat_lock);
}

int
raiseticket(struct proc *p, int tickets)
{
    acquire(&pstat_lock);

    if (tickets <= 0) return -1;
    pstat->tickets[p - proc] = tickets;

    release(&pstat_lock);
}


void
pstatkillproc(struct proc *p)
{
    acquire(&pstat_lock);
    int proc_idx = p - proc;
    pstat->inuse[proc_idx] = 0;
    pstat->pid[proc_idx] = -1;
    pstat->tickets[proc_idx] = 0;
    pstat->ticks[proc_idx] = 0;
    release(&pstat_lock);
}

// Increment the tick count for the given process
// Called when the timer expired
void
pstattick(struct proc *p)
{
    acquire(&pstat_lock);
    int proc_idx = p - proc;
    pstat->ticks[proc_idx]++;
    release(&pstat_lock);
}
