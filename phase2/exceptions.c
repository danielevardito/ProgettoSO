#include <stdio.h>
#include <stdlib.h>
#include <umps/libumps.h>
#include "headers/pcb.h"
#include "headers/msg.h"
#include <umps/const.h>
#include <umps/types.h>

// Definizione delle costanti per i codici di eccezione
#define IOINTERRUPTS 0
#define TLBEXCEPTION_MIN 1
#define TLBEXCEPTION_MAX 3
#define PROGRAMTRAP_MIN 4
#define PROGRAMTRAP_MAX 7
#define SYSEXCEPTION 8

extern handleInterrupt;
extern pcbFree_h;
extern ready_queue;
extern blocked_pcbs;

extern current_process;

void uTLB_RefillHandler()
{
    setENTRYHI(0x80000000);
    setENTRYLO(0x00000000);
    TLBWR();
    LDST((state_t *)BIOSDATAPAGE);
}

// TODO
void tlb_exception_handler();

// TODO
void program_trap_handler();

// TODO
void syscall_exception_handler(state_t *prev_processor_state)
{
    unsigned int a0 = prev_processor_state->gpr[4];
    pcb_t *a1 = prev_processor_state->gpr[5];       // registro a1 - destinazione
    unsigned int a2 = prev_processor_state->gpr[6]; // registro a2 - payload

    unsigned int kernelMode = getSTATUS() & KUp_MASK;
    /*
        Caso SENDMESSAGE
    */
    if (kernelMode && (a0 >= -2 && a0 <= -1))
    {

        pcb_t *destination = a1;
        unsigned int payload = a2;

        msg_t *m = allocMsg();
        m->m_payload = payload;
        m->m_sender = current_process;

        if (destination->state == IDLE)
        {
            prev_processor_state->gpr[1] = DEST_NOT_EXIST; // v0 = DEST_NOT_EXIST
        }
        else if (destination->state == READY)
        {
            insertMessage(&destination->msg_inbox, m);
        }
        else if (destination->state == BLOCKED)
        {
            list_del(&destination->p_list);
            list_add_tail(&destination->p_list, &ready_queue);
            insertMessage(&destination->msg_inbox, m);
        }

        int result = SYSCALL(SENDMESSAGE, (unsigned int)destination, (unsigned int)payload, 0);
        if (result)
        {
            prev_processor_state->gpr[1] = 0; // v0 = 0
        }
        else
        {
            prev_processor_state->gpr[1] = MSGNOGOOD; // v0 = MSGNOGOOD
        }
    }
    /*
        Caso RECEIVEMESSAGE
    */
    else if (kernelMode && a0 == -2)
    {
        SYSCALL(RECEIVEMESSAGE, (unsigned int)a1, (unsigned int)a2, 0);

        pcb_t *sender = prev_processor_state->gpr[5];        // registro a1 - mittente
        unsigned int payload = prev_processor_state->gpr[6]; // registro a2 - payload

        msg_t *m = allocMsg();

        int found = 0;

        if ((unsigned int)sender != ANYMESSAGE){
            struct list_head *iter;
            list_for_each(iter, &current_process->msg_inbox)
            {
                m = container_of(iter, msg_t, m_list);
                if (m->m_sender == sender)
                {
                    found = 1;
                    break;
                }
            }
        }

        if (list_empty(&sender->msg_inbox || !found))
        {
            current_process->state = BLOCKED;
            list_add_tail(&sender->p_list, &blocked_pcbs);
        }
    }
    else if(!kernelMode){
        unsigned int ExcCode_value = current_process->p_s->cause->ExcCode;
        current_process->p_s->ri = ExcCode_value;
        current_process->p_s->cause->ExcCode = ri;

        program_trap_handler();
        //dubbio da specifiche
    LDST((state_t *)BIOSDATAPAGE)
    }
}



void exceptionHandler()
{
    // Salvo lo stato del processo in BIOSDATAPAGE prima dell'eccezione
    state_t *prev_processor_state = (state_t *)BIOSDATAPAGE;
    STST(prev_processor_state);
    prev_processor_state->gpr[4];

    // Prendo l'eccezione da gestire
    unsigned int cause_register = getCause();
    unsigned int exc_code = (cause_register >> CAUSESHIFT) & GETEXECCODE;

    switch (exc_code)
    {
    case IOINTERRUPTS:
        handleInterrupt(exc_code, IOINTERRUPTS); // TODO
        break;
    case 1 ... 3: // TLB Exceptions
        tlb_exception_handler();
        break;
    case 4 ... 7:
    case 9 ... 12: // Program Traps
        program_trap_handler();
        break;
    case SYSEXCEPTION: // Syscall
        syscall_exception_handler(prev_processor_state);
        break;
    }
}

// Prototipi delle funzioni di gestione delle eccezioni
void device_interrupt_handler();

/*Este código define una función exception_handler que recibe un código de excepción y utiliza una instrucción
switch para determinar qué función de manejo de excepciones llamar, dependiendo del código de excepción proporcionado.
Cada función de manejo de excepciones implementa el comportamiento adecuado para el tipo de excepción
correspondiente, como interrupciones de dispositivo, excepciones de TLB, trampas de programa y excepciones
de SYSCALL.*/

// Funzione per gestire le interruzioni del dispositivo
void device_interrupt_handler()
{
    // Implementazione della gestione delle interruzioni del dispositivo
}

// Funzione per gestire le eccezioni di TLB
void tlb_exception_handler()
{
    // Implementazione della gestione delle eccezioni di TLB
}

// Funzione per gestire le trappole del programma
void program_trap_handler()
{
    // Implementazione della gestione delle trappole del programma
}

// Funzione per gestire le eccezioni di SYSCALL
void syscall_exception_handler()
{
    // Implementazione della gestione delle eccezioni di SYSCALL
}