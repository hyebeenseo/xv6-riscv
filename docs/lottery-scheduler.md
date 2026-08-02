# xv6 Lottery Scheduler

## Overview

This project implements a Lottery Scheduler in xv6.

It provides two new system calls:
- `setticket()`, which changes the number of lottery tickets assigned to a process.
- `pgetinfo()`, which retrives process information such as the number of tickets and accumlated CPU ticks.


## Features

The Lottery scheduler selects next process to run by holding a lottery.

The probability of a process being selected is proportional to the number of tichets it owns.

Each process starts with one ticket by default, and child processes inherit their parent's ticket count.

Two new system calls are provided:

    1. **`setticket`**
        - Sets the number of tickets for the calling process.
        - Valid range: **1-100**
        - Returns `0` on success and `-1` if the given value is invalid.

    2.  **`pgetinfo`**
        - Retrieces process information related to the Lottery Scheduler.
        - Returns information including `inuse`, `pid`, `tickets`, and `ticks`.
        - Primarily intended for debugging and testing.

## Implementation

### `pstat`

User programs can retrieve a snapshot of this information by calling `pgetinfo()`.

The kernel maintains a global `pstat` structure containing information about every process, inclding whether the process is in use, its PID, ticket count, and acculated CPU ticks.

The structure is updated whenever relevant event occurs, such as timer interrupts or calls to `setticket()` or creation and termination of a process.

### Lottery Scheduler

The scheduler selects the next process as follows:

1. Iterate through a process table and collect all `RUNNABLE` processes.
2. Compute the total number of tickets owned by all runnable processes.
3. Generate a random number in the range `[0, total_tickets - 1]`.
4. Select the process whose ticket range contains the generated number.

#### Example

```
RUNNABLE process   : p1        | p2    | p3
Tickets held       : 3         | 2     | 5 
                     --------------------------------------
random number range  0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9     
                     --------------------------------------
```
If the generated random number is between **0** and **2**, process **p1** is selected.

If it is between **3** and **4**, process **p2** is selected.

If it is between **5** and **9**, process **p3** is selected.

### xorshift32 RNG

Implemented a pseudo random number generator (PRNG) using the xorshift algorithm proposed by George Marsaglia.

### setticket()

`setticket()` changes the number of tickets assigned to the calling process.

The valid range is **1-100**. If an invalid value is provided, the system call returns `-1` and leaves the current ticket count unchanged.

### pgetinfo()

`pgetinfo()` copies the kernel's `pstat` structure to a user-provided buffer.

The caller must allocate a `struct pstat` object and pass its pointer to the system call.

The kernel obtains the user-space pointer using `argaddr()`, and transfers the data to the user's virtual address space using `copyout()`.


## Test
- Test methodology

To verify the Lottery Scheduler, five CPU-bound child processes were created.

Each child process was assigned a different number of tickets:
| Process | Tickets |
|---------|--------:|
| p1 | 1 |
| p2 | 2 |
| p3 | 4 |
| p4 | 8 |
| p5 | 16 |

Each child continuously executed an infinite loop to remain CPU-bound.

Meanwhile, the parent process periodically called `pgetinfo()` to retrieve the accumulated CPU ticks of each child process.

The scheduler was considered to be functioning correctly if the measured CPU ticks incresed approximately in proportion to the assigned number of tickets.

- Results
```
===== sample 10 =====
pid=5  ticket=1  ticks=34
pid=6  ticket=2  ticks=91
pid=7  ticket=4  ticks=157
pid=8  ticket=8  ticks=322
pid=9  ticket=16  ticks=661
```

| Tickets | CPU Ticks |
| ------: | --------: |
|       1 |        34 |
|       2 |        91 |
|       4 |       157 |
|       8 |       322 |
|      16 |       661 |


## Files Modified

### Scheduler

| File | Description |
|------|-------------|
| `kernel/pstat.h` | Added the definition of `struct pstat`. |
| `kernel/pstat.c` | Implemented `pstat` management functions. |
| `kernel/prng.c` | Implemented the xorshift32 pseudo-random number generator. |
| `kernel/proc.c` | Replaced the Round-Robin scheduler with a Lottery Scheduler. |
| `kernel/defs.h` | Added declarations for newly implemented kernel functions. |
| `kernel/main.c` | Initialized the global `pstat` structure and its lock. |
| `Makefile` | Added newly created object files to the build. |

### Syscall

Modified the following files to add the `setticket()` and `pgetinfo()` system calls.

- `kernel/sysproc.c`
- `kernel/syscall.c`
- `kernel/syscall.h`
- `user/usys.pl`
- `user/user.h`


### Testing

- `user/usertests.c` — Added test cases for the Lottery Scheduler.


## Challenges

### 1. Kernel Page Fault

While implementing `pstat`, the kernel panicked due to a null pointer dereference.
The issue was caused by accessing the global `pstat` structure before it had been properly initialized.
This was resolved by allocating and initializing the structure before it was used.

### 2. Transferring Data Between User Space and Kernel Space

Implementing `pgetinfo()` required safely copying process information from the kernel to a user-provided buffer.
This involved understanding how xv6 retrieves user-space pointers with `argaddr()` and copies data to user virtual memory using `copyout()`.

### 3. Verifying the Lottery Scheduler

Since Lottery Scheduling is probabilistic, a single execution cannot reliably verify its correctness.
To evaluate the scheduler, multiple CPU-bound processes with different ticket counts were created, and their accumulated CPU ticks were periodically collected using `pgetinfo()`.
The observed CPU time increased approximately in proportion to the assigned number of tickets.


## What I Learned

- How system calls transfer data between user space and kernel space using `argaddr()` and `copyout()`.

- How timer interrupts can be used to account for CPU usage.

- How a probabilistic scheduler differs from Round-Robin scheduling.

- Gained a solid understanding of the relationship between virtual addresses and physical addresses, and how the kernel accesses user-space memory.

- Learned why synchronization is necessary in the kernel, and how to use spinlocks to protect shared data structures.

- How to debug kernel panics using `scause` and `addr2line`.

- How process information can be safely exposed to user space through a dedicated interface.


## Reference

- OSTEP Projects - Lottery Scheduling

- RISC-V Privileged Specification
