#include "../phase1/headers/pcb.h"
#include "../phase1/headers/msg.h"

int proc_count = 0; //processi in corso
int soft_block_count = 0; //processi in corso - stato di waiting

struct list_head ready_queue;
mkEmptyProcQ(ready_queue); //ready queue di processi - puntatore alla coda

pcb_t * current_pcb = NULL; //processo attualmente nello running state

pcb_t * sub_device_proc1[SEMDEVLEN-1];
pcb_t * sub_device_proc2[SEMDEVLEN-1]; //array di pcb per i subdevice

struct list_head pseudo_clock_supp_pcb;
mkEmptyProcQ(pseudo_clock_supp_pcb); //lista di pcb a supporto dello pseudo-clock

passupvector_t * passupvector;
passupvector->tlb_refill_handler = (memaddr)uTLB_RefillHandler; //passup vector poi si continua

unsigned int SP = KERNELSTACK;

passupvector->exception_handler = (memaddr)exceptionHandler;
