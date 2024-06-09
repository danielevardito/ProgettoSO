#include "../../phase1/headers/pcb.h"

int read_device_register(int indirizzo);

void write_device_register(int indirizzo, int valore);

void update_time(pcb_t *p);

void copy_state(state_t *to_process, state_t *from_process);

int highest_priority_device(int bits);

void PLT_interrupt_handler(state_t *exception_state);

void unblock_waiting_process();

void deviceInterruptHandler(int linea_interruzione, int cause, state_t *exception_state);

void IT_interruptHandler(state_t *exc_state);

void handleInterrupt(int cause, state_t *exception_state);