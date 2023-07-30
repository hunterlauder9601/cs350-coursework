#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "traps.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int lazyIndicate;

extern int free_frame_cnt; // CS350 for project3
int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return proc->pid;
}


int
sys_sbrk(void) {
	uint size;
	int addr;
	int n;
	argint(0, &n);
	addr = proc->sz;
	if(lazyIndicate == 1) {
		if((proc->sz + n) >= 0xfffffff) {
			cprintf("Allocating pages failed!\n");
			return -1;
		}
		proc->prevsz = proc->sz;
		proc->sz = proc->sz + n;
		if(n < 0) {
			if((size = deallocuvm(proc->pgdir, proc->prevsz, proc->sz)) == 0) {
				cprintf("Deallocating pages failed!\n");
				return -1;
			}
		}
	}
	else {
  	if(growproc(n) < 0)
    	return -1;
	}
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;
  
  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(proc->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;
  
  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// CS 350 print out free frames
int sys_print_free_frame_cnt(void)
{
    cprintf("free-frames %d\n", free_frame_cnt);
    return 0;
}

// CS 350 set page allocator
extern int page_allocator_type;
int sys_set_page_allocator(void)
{
    if(argint(0,&page_allocator_type) < 0){
        return -1;
    }
    lazyIndicate = page_allocator_type;
    return 0;
}

// CS 350 shared memory
int sys_shmget(void)
{
    int shm_id;
    if(argint(0, &shm_id) < 0){
        return -1;
    }
    cprintf("Your shared memory mechanism has not been implemented!\n");    
    return 0;
}

// delete a shared page
int sys_shmdel(void)
{
    int shm_id;
    if(argint(0, &shm_id) < 0){
        return -1;
    }
    cprintf("Your shared memory mechanims has not been implemented!\n");
    return 0;
}
