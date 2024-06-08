#include <umps/libumps.h>

void uTLB_RefillHandler();

void syscall_exception_handler(state_t *prev_processor_state);

void exceptionHandler();

void pass_up_or_die(unsigned int index_value, state_t *exception_state);

void tlb_exception_handler(state_t *exception_state);

void program_trap_handler(state_t *exception_state);