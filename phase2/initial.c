#include <stdio.h>
#include <stdlib.h>
#include <umps/libumps.h>
#include "../phase1/headers/pcb.h"
#include "../phase1/headers/msg.h"
#include "headers/ssi.h"
#include "headers/exceptions.h"
#include "headers/scheduler.h"

extern void test();

// Passo 1: Dichiarare le variabili globali
// Variabili globali del Nucleo
int process_count = 0;
int soft_block_count = 0;
LIST_HEAD(ready_queue);

pcb_t* current_process = NULL;

LIST_HEAD(blocked_pcbs);


// Funzione principale
int main() {
    passupvector_t * passup_vector;
    passup_vector->tlb_refill_handler = (memaddr)uTLB_RefillHandler;
    passup_vector->exception_handler = (memaddr)exceptionHandler;

    passup_vector->tlb_refill_stackPtr = KERNELSTACK;

    // Passo 3: Inizializzare le strutture dati del Livello 2
    initPcbs();
    initMsgs();

    // Passo 4: Inizializzare le variabili dichiarate
    // (Inizializzazione di altre variabili globali)
    process_count = 0;
    soft_block_count = 0;
    INIT_LIST_HEAD(&ready_queue);
    INIT_LIST_HEAD(&blocked_pcbs);

    // Passo 5: Caricare il timer di intervallo di sistema
    // (Configurazione del timer con il valore appropriato)
    setTIMER(PSECOND);

    // Passo 6: Istanziare il primo processo
    // (Assegnare memoria per il PCB, inizializzare il suo stato, ecc.)
    pcb_t* firstProcess = allocPcb();
    list_add(&firstProcess->p_list, &ready_queue);
    process_count++;

    //imposta lo stato del processo su READY
    firstProcess->state = READY;

     // Abilita gli interrupt
    firstProcess->p_s.status = firstProcess->p_s.status | IEPON;

    // Attiva la modalità kernel
    firstProcess->p_s.status = firstProcess->p_s.status & ~USERPON;

    // Imposta lo SP su RAMTOP
    firstProcess->p_s.reg_sp = RAMSTART;

    // Imposta il PC sull'indirizzo di SSI_function_entry_point
    firstProcess->p_s.pc_epc = (memaddr)SSI_function_entry_point;

    //Impostare tutti i campi della struttura dei processi su NULL

    firstProcess->p_parent = NULL;
    INIT_LIST_HEAD(&firstProcess->p_child);
    INIT_LIST_HEAD(&firstProcess->p_sib);
    
    //Impostare il campo del tempo accumulato (p_time) su zero.

    firstProcess->p_time = 0;

    //Impostare il puntatore della struttura di supporto (p_supportStruct) su NULL

    firstProcess->p_supportStruct = NULL;

    // Passo 7: Istanziare il secondo processo
    // (Simile al passo precedente)

     pcb_t* scndProcess = allocPcb();
    list_add(&scndProcess->p_list, &ready_queue);
    process_count++;

    //imposta lo stato del processo su READY
    scndProcess->state = READY;

     // Abilita gli interrupt
    scndProcess->p_s.status = scndProcess->p_s.status | IEPON;

    // Attiva la modalità kernel
    scndProcess->p_s.status = scndProcess->p_s.status & ~USERPON;

    // Imposta lo SP su RAMTOP
    scndProcess->p_s.reg_sp = RAMSTART - (2 * PAGESIZE);

    // Imposta il PC sull'indirizzo di SSI_function_entry_point
    scndProcess->p_s.pc_epc = (memaddr)test;

    //Impostare tutti i campi della struttura dei processi su NULL

    scndProcess->p_parent = NULL;
    INIT_LIST_HEAD(&scndProcess->p_child);
    INIT_LIST_HEAD(&scndProcess->p_sib);
    
    //Impostare il campo del tempo accumulato (p_time) su zero.

    scndProcess->p_time = 0;

    //Impostare il puntatore della struttura di supporto (p_supportStruct) su NULL

    scndProcess->p_supportStruct = NULL;

    // Passo 8: Chiamare il pianificatore (Scheduler)
    scheduler(); // Questo assumerebbe che esista già una funzione di pianificazione

    return 0;
}