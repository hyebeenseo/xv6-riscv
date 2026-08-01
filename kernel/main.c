#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"
#include "pstat.h"

volatile static int started = 0;

// start() jumps here in supervisor mode on all CPUs.
void
main()
{
  if (cpuid() == 0) {
    consoleinit();
    printkinit();
    printk("\n");
    printk("xv6 kernel is booting\n");
    printk("\n");
    kinit();            // physical page allocator
    printk("1. kinit run\n");
    kvminit();          // create kernel page table
    printk("2. kvminit run\n");
    kvminithart();      // turn on paging
    printk("3. kvmininthart run\n");
    procinit();         // process table
    printk("4. procinit run\n");
    pstatinit();        // pstat for lottery scheduler
    printk("5. pstatinit run\n");
    trapinit();         // trap vectors
    printk("6. trapinit run\n");
    trapinithart();     // install kernel trap vector
    printk("7. trapinithart run\n");
    plicinit();         // set up interrupt controller
    printk("8. plincinit run\n");
    plicinithart();     // ask PLIC for device interrupts
    printk("9. plicinithart run\n");
    binit();            // buffer cache
    printk("10. binit run\n");
    iinit();            // inode table
    printk("11. iinit run\n");
    fileinit();         // file table
    printk("12. fileinit run\n");
    virtio_disk_init(); // emulated hard disk
    printk("13. virito_disk_init run\n");
    userinit();         // first user process
    printk("14. userinit run\n");
    __atomic_thread_fence(__ATOMIC_SEQ_CST);
    started = 1;
  } else {
    while (started == 0)
      ;
    __atomic_thread_fence(__ATOMIC_SEQ_CST);
    printk("hart %d starting\n", cpuid());
    kvminithart();  // turn on paging
    trapinithart(); // install kernel trap vector
    plicinithart(); // ask PLIC for device interrupts
  }
  //scheduler();
  //printk("lets call lottery scheduler!\n");
  lottery_scheduler();
}
