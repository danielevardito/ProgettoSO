#include <stdio.h>
#include <stdlib.h>
#include <umps/libumps.h>
#include "headers/pcb.h"
#include "headers/msg.h"

extern int process_count;
extern int soft_block_count;
extern pcb_t* ready_queue;
extern pcb_t* current_process; 

void uTLB_RefillHandler(){ 
    setENTRYHI(0x80000000);
    setENTRYLO(0x00000000);
    TLBWR();
    LDST((state_t*) 0x0FFFF000);
}

// Funzione per lo Scheduler
void scheduler() {
    // Comportamento se la Coda dei Processi Pronti è vuota
    if (ready_queue == NULL) {
        // Se non ci sono processi in attesa e c'è solo un processo in esecuzione, interrompere l'esecuzione
        if (process_count == 1) {
            // Chiamare il servizio di HALT di BIOS
            // halt_bios_service();
            // Supponendo che halt_bios_service sia una funzione che chiama il servizio di HALT di BIOS
            return;
        }
        // Se ci sono processi in attesa, entrare in uno Stato di Attesa
        else if (soft_block_count > 0) {
            // Abilitare le interruzioni e disabilitare il PLT o caricarlo con un valore grande
            // set_interrupts_enabled(true);
            // disable_plt();
            // o
            // load_large_value_to_plt();
            // Supponendo che set_interrupts_enabled, disable_plt e load_large_value_to_plt siano funzioni appropriate
            // Eseguire l'istruzione WAIT
            // wait_instruction();
            return;
        }
        // Se c'è un blocco (deadlock) del sistema, chiamare il servizio PANIC di BIOS
        else {
            // panic_bios_service();
            // Supponendo che panic_bios_service sia una funzione che chiama il servizio PANIC di BIOS
            return;
        }
    }
}