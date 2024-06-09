#include <stdio.h>
#include <stdlib.h>
#include <umps/libumps.h>
#include "../../headers/types.h"

void SSI_function_entry_point();

void SSI();

void SSIRequest(pcb_t *sender, int service, void *arg);

pcb_t *createProcess(ssi_create_process_t *create_args);

void terminateProcess(pcb_t *process);

void performIO(ssi_do_io_t *do_io_args);

void receive_request(ssi_payload_t *payload);

void send_response(void *response);

void handleCreateProcess(pcb_t * sender, ssi_create_process_t *create_args);

void handleTerminateProcess(pcb_t *process);

void handleDoIO(ssi_do_io_t *do_io);