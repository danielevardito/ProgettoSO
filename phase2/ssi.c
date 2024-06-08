#include <umps3/umps/libumps.h>
#include "../phase1/headers/pcb.h"
#include "headers/ssi.h"

extern pcb_t* current_process;
extern struct list_head blocked_pcbs;

void SSI()
{
    while (1)
    {
        // Receive request

        state_t * current_processor_state;
        STST(current_processor_state);
        pcb_t * sender = current_processor_state->gpr[4]; // registro a1 - mittente

        ssi_payload_t * payload = current_processor_state->gpr[5];

        SSIRequest(sender, payload->service_code, payload->arg);

        // Send response
    }
}

void SSIRequest(pcb_t *sender, int service, void *arg)
{

    // Handle request
    switch (service)
    {
    case CREATEPROCESS:
        handleCreateProcess(sender, (ssi_create_process_t *)arg);
        break;
    case DOIO:
        handleDoIO((ssi_do_io_t *)arg);
        break;
    // Add cases for other service requests here
    default:
        // Caso in cui non si tratta di un servizio valido
        // Termina il processo chiamante e tutta la sua progenie
        handleTerminateProcess((pcb_t *)arg);
    }
}

pcb_t *createProcess(ssi_create_process_t *create_args)
{
    // Allocate new PCB
    pcb_t *new_process = allocPcb();
    if (new_process == NULL)
    {
        // Return error if PCB allocation failed
        return (pcb_t *)NOPROC;
    }

    // Initialize PCB fields
    //new_process->state = *(create_args->state);
    // Initialize other fields as needed

    // Add new process to ready queue, process tree, etc.
    // ...

    return new_process;
}

void terminateProcess(pcb_t *process)
{
    process->state = IDLE;
    // Terminate process
    freePcb(process);
}

void performIO(ssi_do_io_t *do_io_args)
{
    // Perform I/O operation
    // ...
    // Send response to waiting process
    // ...
}

void receive_request(ssi_payload_t *payload)
{
    // Receive request using RECEIVEMESSAGE syscall
    SYSCALL(RECEIVEMESSAGE, (unsigned int)payload, 0, 0);
}

void send_response(void *response)                                          //come fa a sapere a chi mandarla?
{
    // Send response using SENDMESSAGE syscall
    SYSCALL(SENDMESSAGE, (unsigned int)response, 0, 0);
}

void handleCreateProcess(pcb_t * sender, ssi_create_process_t *create_args)
{
    pcb_t *new_process = createProcess(create_args);
    send_response(new_process);
}

void handleTerminateProcess(pcb_t *process)
{
    if (process == NULL)
        return;

    /*
    Finchè ci sono figli, rimuovili
    */
    while (!emptyChild(process))
    {
        handleTerminateProcess(container_of(process->p_child.next, pcb_t, p_child));
    }
    outChild(process);
    terminateProcess(process);

    // Send response indicating termination completion
}

void handleDoIO(ssi_do_io_t *do_io)
{
    current_process->state = BLOCKED;
    //metto il processo nella blocked list
    list_add(&current_process->p_list, &blocked_pcbs);
    ssi_payload_t payload = {
        .service_code = DOIO,
        .arg = do_io,
    };
    // Send the request with the payload
    SYSCALL(SENDMESSAGE, (unsigned int)ssi_pcb, (unsigned int)(&payload), 0);
    // Wait for SSI’s response
    SYSCALL(RECEIVEMESSAGE, (unsigned int) ssi_pcb, (unsigned int)(&response), 0);

    performIO(do_io);
    // Send response indicating I/O completion
}
