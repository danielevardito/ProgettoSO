#include <umps3/umps/libumps.h> // Assicurati che questo percorso sia corretto
#include "../phase1/headers/pcb.h"
//#include "../phase1/headers/msg.h"
#include "headers/ssi.h"

// Indirizzo del processo SSI
#define SSI_ADDRESS 0x12345678 // Questo dovrebbe essere un indirizzo "fittizio" unico

extern struct list_head *ready_queue;
extern pcb_t *current_process;

void SSI()
{
    while (1)
    {
        // Ricevi la richiesta
        ssi_payload_t payload;
        receive_request(&payload);

        // Ottieni lo stato del processore corrente
        state_t current_processor_state;
        STST(&current_processor_state);

        // Ottieni il PCB del mittente
        pcb_t *sender = (pcb_t *)current_processor_state.gpr[4]; // Registro a1 - mittente

        // Gestisci la richiesta
        SSIRequest(sender, payload.service_code, payload.arg);
    }
}

void SSIRequest(pcb_t *sender, int service, void *arg)
{
    switch (service)
    {
    case CREATEPROCESS:
        handleCreateProcess(sender, (ssi_create_process_t *)arg);
        break;
    case DOIO:
        handleDoIO((ssi_do_io_t *)arg);
        break;
    // Aggiungi casi per altre richieste di servizio qui
    default:
        // Termina il processo e la sua progenie se il servizio non è valido
        handleTerminateProcess(sender);
    }
}

pcb_t *createProcess(ssi_create_process_t *create_args)
{
    // Alloca un nuovo PCB
    pcb_t *new_process = allocPcb();
    if (new_process == NULL)
    {
        // Restituisci errore se l'allocazione del PCB è fallita
        return (pcb_t *)NOPROC;
    }

    // Inizializza i campi del PCB
    new_process->p_s = *(create_args->state);
    new_process->p_supportStruct = create_args->support;

    // Aggiungi il nuovo processo alla coda dei processi pronti e all'albero dei processi
    insertProcQ(ready_queue, new_process);
    insertChild(current_process, new_process);

    return new_process;
}

void terminateProcess(pcb_t *process)
{
    // Termina ricorsivamente la progenie
    while (!emptyChild(process))
    {
        handleTerminateProcess(removeChild(process));
    }

    // Termina il processo
    outProcQ(ready_queue, process);
    freePcb(process);
}

void performIO(ssi_do_io_t *do_io_args)
{
    // Esegui l'operazione di I/O
    devreg_t *device = (devreg_t *)do_io_args->commandAddr;

    // Supponiamo che il campo corretto sia `data0`, `data1`, ecc.
    // Modifica in base alla tua struttura `devreg_t`


    // Attendi il completamento dell'I/O
    
    

    // Invia la risposta al processo in attesa
    send_response((void *)(device));
}

void receive_request(ssi_payload_t *payload)
{
    SYSCALL(RECEIVEMESSAGE, SSI_ADDRESS, (unsigned int)payload, 0);
}

void send_response(void *response)
{
    SYSCALL(SENDMESSAGE, SSI_ADDRESS, (unsigned int)response, 0);
}

void handleCreateProcess(pcb_t *sender, ssi_create_process_t *create_args)
{
    pcb_t *new_process = createProcess(create_args);
    send_response((void *)new_process);
}

void handleTerminateProcess(pcb_t *process)
{
    if (process == NULL)
        return;

    while (!emptyChild(process))
    {
        handleTerminateProcess(container_of(process->p_child.next, pcb_t, p_child));
    }
    outChild(process);
    terminateProcess(process);

    // Invia risposta completamento
}

void handleDoIO(ssi_do_io_t *do_io_args)
{
    performIO(do_io_args);
    // Invia risposta completamento
}