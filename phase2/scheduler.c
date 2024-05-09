#include <stdio.h>
#include <stdlib.h>
#include <umps/libumps.h>
#include "headers/pcb.h"
#include "headers/msg.h"

extern int process_count;
extern int soft_block_count;
extern pcb_t* ready_queue;
extern pcb_t* current_process; 

// Funzione per lo Scheduler
void scheduler() {
    // Comportamento se la Coda dei Processi Pronti è vuota
    if (emptyProcQ(ready_queue)) {
        //HALT execution se la process count è 0
        if (process_count == 1) {
            HALT(); 
        }
        // Se ci sono processi in attesa, entrare in uno Stato di Attesa
        else if (soft_block_count > 0 && process_count > 0) {
            WAIT();
        }
        // Se c'è un blocco (deadlock) del sistema, chiamare il servizio PANIC di BIOS
        else if(process_count > 0 && soft_block_count == 0){
            PANIC();
        }
    }
    else{
        //Rimuovo il primo elemento della Coda dei Processi Pronti
        pcb_t * p = removeProcQ(ready_queue);

        //Assegno p a current process
        current_process = p;

        //Passo allo stato running
        p->state = RUNNING;

        //Carico il tempo nel PLT
        setTIMER(TIMESLICE);

        //Assegno lo stato del processo al processore
        LDST(&p->p_s);
    }
}