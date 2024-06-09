#include <stdio.h>
#include <stdlib.h>
#include <umps/libumps.h>
#include <umps/const.h>
#include <umps/types.h>
#include "headers/exceptions.h"

#include "../phase1/headers/pcb.h"
#include "../headers/types.h"
#include "../headers/const.h"
#include "headers/ssi.h"
#include "headers/scheduler.h"
#include "headers/exceptions.h"

// Definizioni delle costanti mnemoniche per le interruzioni
#define DEVICE_INTERRUPT_BASE 3
#define PLT_INTERRUPT 1
#define INTERVAL_TIMER_INTERRUPT 2
#define GPR_LEN 32

#define RAMBASEADDR 0x10000000
#define ACK 1

extern pcb_t* current_process;
extern struct list_head blocked_pcbs;



int Tiniziale;

//Funzioni ausiliarie
int read_device_register(int address) {
    int *regPtr = (int *)address;
    int valore = *regPtr;
    return valore;
}

void write_device_register(int address, int value) {
    int *regPtr = (int *)address;
    *regPtr = value;
}

void update_time(pcb_t *process) {
    int Tfinale;
    STCK(Tfinale);                      
    process->p_time += (Tfinale - Tiniziale);
    Tiniziale = Tfinale;
}

void copy_state(state_t *to_process, state_t *from_process) {
    to_process->entry_hi = from_process->entry_hi;
    to_process->cause = from_process->cause;
    to_process->status = from_process->status;
    to_process->pc_epc = from_process->pc_epc;
    to_process->hi = from_process->hi;
    to_process->lo = from_process->lo;
    for (int i = 0; i < GPR_LEN; i++)
    {
        to_process->gpr[i] = from_process->gpr[i];
    }
}

int highest_priority_device(int bits) {
   int mask = 1;
   for(int i = 0; i < 8; i++)
   {
        if(bits & mask)
        {
            return i;
        }
    mask <<= 1;
   }
   
}  

void PLT_interrupt_handler(state_t *exception_state) {
    setTIMER(-1);
    update_time(current_process);
    copy_state(&(current_process->p_s), exception_state);
    insertProcQ(ready_queue, current_process);
    scheduler();
 }


void deviceInterruptHandler(int interrupt_line, int cause, state_t *exception_state) {

    pcb_t* unblockedPCB;
    devregarea_t *dev_reg_area = (devregarea_t *)RAMBASEADDR;
    
    unsigned int interrupt_device_bitmap = dev_reg_area->interrupt_dev[interrupt_line - DEVICE_INTERRUPT_BASE];
    int device_num = highest_priority_device(interrupt_device_bitmap);
    int devAddrBase = (int)&dev_reg_area->devreg[interrupt_line - DEVICE_INTERRUPT_BASE][device_num];

    unsigned int device_state;
    int isTerminal = (interrupt_line == 7);

    if (isTerminal) {
        termreg_t *terminal_reg = (termreg_t *)devAddrBase;
        if (terminal_reg->recv_status & 0x00000001) { // Check if the interrupt was caused by the receive part
            device_state = terminal_reg->recv_status;
            terminal_reg->recv_command = ACK;
        } else { // The interrupt was caused by the transmit part
            device_state = terminal_reg->transm_status;
            terminal_reg->transm_command = ACK;
        }
    } else {
        device_state = read_device_register(devAddrBase);
        write_device_register(devAddrBase, ACK);
    }

    LDST(exception_state); 
}

void IT_interruptHandler(state_t *exception_state) { 
  
    LDIT(PSECOND);
    pcb_t* currentPCB;
    while ((currentPCB = removeProcQ(&blocked_pcbs)) != NULL) 
    {
        send_message(0, currentPCB, SSI_ADDRESS, exception_state);
        insertProcQ(ready_queue, currentPCB);
    }

    if (current_process != NULL)
        LDST(exception_state); 
    else
        scheduler(); 
}


// Funzione per gestire le interruzioni
void handleInterrupt(int cause, state_t *exception_state) {
    
    int interrupt_cause = CAUSE_IP_GET(cause, 8);

    if (interrupt_cause & CAUSE_IP(PLT_INTERRUPT)) {
        PLT_interrupt_handler(exception_state);
    } else if (interrupt_cause & CAUSE_IP(INTERVAL_TIMER_INTERRUPT)) {
        IT_interruptHandler(exception_state);
    } else {
        for (int line = DEVICE_INTERRUPT_BASE; line < 8; line++) {
            if (interrupt_cause & CAUSE_IP(line)) {
                deviceInterruptHandler(line, cause, exception_state);
                return;
            }
        }
    }
}
