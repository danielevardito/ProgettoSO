#include "./headers/pcb.h"

static pcb_t pcbTable[MAXPROC];
LIST_HEAD(pcbFree_h);
static int next_pid = 1;

void initPcbs() {
    for(int i = 0; i < MAXPROC; i++){
        INIT_LIST_HEAD(&pcbTable[i].p_list);
        list_add_tail(&pcbTable[i].p_list, &pcbFree_h);
    }
}

void freePcb(pcb_t *p) {
    INIT_LIST_HEAD(&p->p_list);
    list_add_tail(&p->p_list, &pcbFree_h);
}

pcb_t *allocPcb() {
    if(list_empty(&pcbFree_h)) return NULL;

    //rimozione del primo PCB in lista
    pcb_t * p = container_of(&pcbFree_h, pcb_t, p_list);
    list_del(&p->p_list);
    //inizializzazione dei campi
    p->p_pid = next_pid;
    next_pid++;

    return p;
}

void mkEmptyProcQ(struct list_head *head) {
}

int emptyProcQ(struct list_head *head) {
}

void insertProcQ(struct list_head *head, pcb_t *p) {
}

pcb_t *headProcQ(struct list_head *head) {
}

pcb_t *removeProcQ(struct list_head *head) {
}

pcb_t *outProcQ(struct list_head *head, pcb_t *p) {
}

int emptyChild(pcb_t *p) {
}

void insertChild(pcb_t *prnt, pcb_t *p) {
}

pcb_t *removeChild(pcb_t *p) {
}

pcb_t *outChild(pcb_t *p) {
}
