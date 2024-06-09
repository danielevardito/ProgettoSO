#ifndef PANDOS_TYPES_H_INCLUDED
#define PANDOS_TYPES_H_INCLUDED

/****************************************************************************
 *
 * This header file contains utility types definitions.
 *
 ****************************************************************************/

#include <umps/types.h>
#include "./const.h"
#include "./listx.h"

typedef signed int cpu_t;
typedef unsigned int memaddr;

/* Page Table Entry descriptor */
typedef struct pteEntry_t {
    unsigned int pte_entryHI;
    unsigned int pte_entryLO;
} pteEntry_t;

/* Support level context */
typedef struct context_t {
    unsigned int stackPtr;
    unsigned int status;
    unsigned int pc;
} context_t;

/* Support level descriptor */
typedef struct support_t {
    int        sup_asid;                        /* process ID					*/
    state_t    sup_exceptState[2];              /* old state exceptions			*/
    context_t  sup_exceptContext[2];            /* new contexts for passing up	*/
    pteEntry_t sup_privatePgTbl[USERPGTBLSIZE]; /* user page table				*/
    struct list_head s_list;
} support_t;

/* process table entry type */
typedef struct pcb_t
{
    /* process queue  */
    struct list_head p_list;

    /* process tree fields */
    struct pcb_t *p_parent;   /* ptr to parent	*/
    struct list_head p_child; /* children list */
    struct list_head p_sib;   /* sibling list  */

    /* process status information */
    state_t p_s;  /* processor state */
    cpu_t p_time; /* cpu time used by proc */

    /* First message in the message queue */
    struct list_head msg_inbox;

    /* Pointer to the support struct */
    support_t *p_supportStruct;

    /* process id */
    int p_pid;

    /* process state (Running, Ready, Blocked) */
    int state;

} pcb_t, *pcb_PTR;

/* message entry type */
typedef struct msg_t
{
    /* message queue */
    struct list_head m_list;

    /* thread that sent this message */
    struct pcb_t *m_sender;

    /* the payload of the message */
	unsigned int m_payload;
} msg_t, *msg_PTR;

typedef struct ssi_payload_t
{
    int service_code;
    void *arg;
} ssi_payload_t, *ssi_payload_PTR;

typedef struct ssi_create_process_t
{
    state_t *state;
    support_t *support;
} ssi_create_process_t, *ssi_create_process_PTR;

typedef struct ssi_do_io_t
{
    memaddr *commandAddr;
    unsigned int commandValue;
} ssi_do_io_t, *ssi_do_io_PTR;

typedef struct passupvector {
    unsigned int tlb_refill_handler;
    unsigned int tlb_refill_stackPtr;
    unsigned int exception_handler;
    unsigned int exception_stackPtr;
} passupvector_t;

/* Device register type for disks, flash and printers */
typedef struct dtpreg {
unsigned int status;
unsigned int command;
unsigned int data0;
unsigned int data1;
} dtpreg_t;
/* Device register type for terminals */
typedef struct termreg {
unsigned int recv_status;
unsigned int recv_command;
unsigned int transm_status;
unsigned int transm_command;
} termreg_t;
typedef union devreg {
dtpreg_t dtp;
termreg_t term;
} devreg_t;
/* Bus register area */
typedef struct devregarea {
unsigned int rambase;
unsigned int ramsize;
unsigned int execbase;
unsigned int execsize;
unsigned int bootbase;
unsigned int bootsize;
unsigned int todhi;
unsigned int todlo;
unsigned int intervaltimer;
unsigned int timescale;
unsigned int TLBFloorAddr;
unsigned int inst_dev[5];
unsigned int interrupt_dev[5];
devreg_t devreg[5][8];
} devregarea_t;

#endif
