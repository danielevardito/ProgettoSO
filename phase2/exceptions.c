#include <stdio.h>
#include <stdlib.h>
#include <umps/libumps.h>
#include <umps/const.h>
#include <umps/types.h>
#include "headers/exceptions.h"


#include "../phase1/headers/msg.h"
#include "../headers/types.h"
#include "../headers/const.h"

// Definizione delle costanti per i codici di eccezione
#define IOINTERRUPTS 0
#define TLBEXCEPTION_MIN 1
#define TLBEXCEPTION_MAX 3
#define PROGRAMTRAP_MIN 4
#define PROGRAMTRAP_MAX 7
#define SYSEXCEPTION 8
#define GPR_LEN 32

extern void handleInterrupt();
extern struct list_head ready_queue;
extern struct list_head blocked_pcbs;
extern void scheduler();
extern void handleTerminateProcess();

extern pcb_t* current_process;

void uTLB_RefillHandler()
{
    setENTRYHI(0x80000000);
    setENTRYLO(0x00000000);
    TLBWR();
    LDST((state_t *)BIOSDATAPAGE);
}




void syscall_exception_handler(state_t *prev_processor_state)
{
    //gpr è la struttura con i registri
    unsigned int a0 = prev_processor_state->gpr[4]; // registro a0 - selettore
    pcb_t *a1 = prev_processor_state->gpr[5];       // registro a1
    unsigned int a2 = prev_processor_state->gpr[6]; // registro a2 - payload

    unsigned int kernelMode = getSTATUS() & KUp_MASK;
    /*
        Caso Pass up or die    
    */
    if (kernelMode && (a0 >= 1))
    {
        pass_up_or_die(GENERALEXCEPT,prev_processor_state);
    }
    /*
        Caso SENDMESSAGE
    */
    if (kernelMode && (a0 == SENDMESSAGE))
    {

        pcb_t *destination = a1;
        unsigned int payload = a2;

        msg_t *m = allocMsg();
        if(m==NULL)                                   // se non c'è più spazio
        {
            prev_processor_state->gpr[1] = MSGNOGOOD; // v0 = MSGNOGOOD
        }
        m->m_payload = payload;
        m->m_sender = current_process;

        if (destination->state == IDLE)
        {
            prev_processor_state->gpr[1] = DEST_NOT_EXIST; // v0 = DEST_NOT_EXIST
        }
        else if (destination->state == READY)
        {
            insertMessage(&destination->msg_inbox, m);
            prev_processor_state->gpr[1] = 0; // v0 = 0
        }
        else if (destination->state == BLOCKED)
        {
            list_del(&destination->p_list);
            list_add_tail(&destination->p_list, &ready_queue);
            insertMessage(&destination->msg_inbox, m);
            prev_processor_state->gpr[1] = 0; // v0 = 0
        }

        state_t * newState = (state_t *)BIOSDATAPAGE;
        newState->pc_epc += WORDLEN;                   // Aumento il program counter
        LDST(newState);
    }
    /*
        Caso RECEIVEMESSAGE
    */
    else if (kernelMode && a0 == RECEIVEMESSAGE)
    {
        pcb_t *sender = prev_processor_state->gpr[5];        // registro a1 - mittente
        unsigned int payload = prev_processor_state->gpr[6]; // registro a2 - payload

        msg_t *m;
        if (sender==ANYMESSAGE)
        {
            m = popMessage(&current_process->msg_inbox,NULL);
        }
        else {
            m = popMessage(&current_process->msg_inbox,sender);
        }

        if (m==NULL) // se non c'è alcun messagio che soddisfi i requisiti
        {
            current_process->state = BLOCKED;
            list_add_tail(&current_process->p_list, &blocked_pcbs);  
        }

        current_process->p_s = prev_processor_state;
        scheduler();
    }
    else if(!kernelMode){
        unsigned int ExcCode_value = current_process->p_s->cause->ExcCode;
        current_process->p_s->ri = ExcCode_value;
        current_process->p_s->cause->ExcCode = 10; // RI = 10

        program_trap_handler(prev_processor_state);
    }
}



void exceptionHandler()
{
    // Salvo lo stato del processo in BIOSDATAPAGE prima dell'eccezione
    state_t *prev_processor_state = (state_t *)BIOSDATAPAGE;
    STST(prev_processor_state);  // Carico lo stato precedente

    // Prendo l'eccezione da gestire
    unsigned int cause_register = getCause();
    unsigned int exc_code = (cause_register >> CAUSESHIFT) & GETEXECCODE;

    switch (exc_code)
    {
    case IOINTERRUPTS:
        handleInterrupt(exc_code, IOINTERRUPTS);
        break;
    case 1 ... 3:      // TLB Exceptions
        tlb_exception_handler(prev_processor_state);
        break;
    case 4 ... 7:
    case 9 ... 12:     // Program Traps
        program_trap_handler(prev_processor_state);
        break;
    case SYSEXCEPTION: // Syscall
        syscall_exception_handler(prev_processor_state);
        break;
    }
}

void pass_up_or_die(unsigned int index_value, state_t *exception_state)
{
    if(current_process!=NULL)
    {
    
        if(current_process->p_supportStruct == NULL)
        {
            handleTerminateProcess(current_process);
            scheduler();
        }
        else
        {
            current_process->p_supportStruct->sup_exceptState[index_value] = *exception_state;
            context_t saved_context = current_process->p_supportStruct->sup_exceptContext[index_value];  //salvo il contesto perché sia disponibile
            LDCXT(saved_context.stackPtr, saved_context.status, saved_context.pc);  //cambia lo stato del processo attuale
        }
    }

}

// Funzioni di gestione delle eccezioni

// Funzione per gestire le eccezioni di TLB
void tlb_exception_handler(state_t *exception_state)
{
    pass_up_or_die(PGFAULTEXCEPT, exception_state);
}

// Funzione per gestire le trap del programma
void program_trap_handler(state_t *exception_state)
{
    pass_up_or_die(GENERALEXCEPT, exception_state);
}

