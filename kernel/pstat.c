#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"
#include "pstat.h"

extern struct proc proc[NPROC];

/*
struct pstat {
  int inuse[NPROC];   // whether this slot of the process table is in use (1 or 0)
  int tickets[NPROC]; // the number of tickets this process has
  int pid[NPROC];     // the PID of each process 
  int ticks[NPROC];   // the number of ticks each process has accumulated 
};
*/


struct spinlock pstat_lock;
struct pstat pstat;

void
pstatinit(void)
{
    initlock(&pstat_lock, "pstat");
    acquire(&pstat_lock);

    //printk("pstat lock acquired\n");
    for (int i = 0; i < NPROC; i++) {
        pstat.inuse[i] = 0;
        pstat.tickets[i] = 0;
        pstat.pid[i] = -1;
        pstat.ticks[i] = 0;
    }
    //printk("for termintated\n");
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
        pstat.inuse[proc_idx] = 1;
        pstat.pid[proc_idx] = p->pid;
        pstat.tickets[proc_idx] = 1;
        pstat.ticks[proc_idx] = 0;
        break;
    
    case NEWPROC_KFORK:
        int parent_idx = (p->parent) - proc;
        int parent_tickets = pstat.tickets[parent_idx];

        pstat.inuse[proc_idx] = 1;
        pstat.pid[proc_idx] = p->pid;
        pstat.tickets[proc_idx] = parent_tickets; // Inherit parent's tickets
        pstat.ticks[proc_idx] = 0;

        //printk("**fork pstat initializing check**\n");
        //printk("parend pid: %d, process pid: %d\n", p->parent->pid, p->pid);
        //printk("parent tickets: %d, process tickets: %d\n", parent_tickets, pstat.tickets[proc_idx]);
        // printk("\n");

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

    if (tickets <= 0 || tickets > 100) return -1;
    pstat.tickets[p - proc] = tickets;

    release(&pstat_lock);
    return 0;
}


void
pstatfreeproc(struct proc *p)
{
    acquire(&pstat_lock);
    int proc_idx = p - proc;
    pstat.inuse[proc_idx] = 0;
    pstat.pid[proc_idx] = -1;
    pstat.tickets[proc_idx] = 0;
    pstat.ticks[proc_idx] = 0;
    release(&pstat_lock);
}

// Increment the tick count for the given process
// Called when the timer expired
void
pstattick(struct proc *p)
{
    acquire(&pstat_lock);
    //printk("proc=%p p=%p\n", proc, p);
    int proc_idx = p - proc;
    //printk("tick updated idx: %d\n", proc_idx);
    pstat.ticks[proc_idx]++;
    release(&pstat_lock);
}


int
pstatgetinfo(uint64 buf)
{
    acquire(&pstat_lock);
    if (copyout(myproc()->pagetable, buf, (char*)&pstat, sizeof(pstat)) < 0) {
        release(&pstat_lock);
        return -1;
    }
    release(&pstat_lock);
    return 0;
}
