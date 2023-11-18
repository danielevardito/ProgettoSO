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
    /*
    inizializzazione dei campi
    */
    /*p_list*/
    INIT_LIST_HEAD(&p->p_list);
    /* process tree fields */
    p->p_parent = NULL;
    INIT_LIST_HEAD(&p->p_child);
    INIT_LIST_HEAD(&p->p_sib);
    /*cpu_t*/
    p->p_time = 0;
    /*msg_inbox*/
    INIT_LIST_HEAD(&p->msg_inbox);
    //pid
    p->p_pid = next_pid;
    next_pid++;

    return p;
}

void mkEmptyProcQ(struct list_head *head) {
    INIT_LIST_HEAD(head);
}

int emptyProcQ(struct list_head *head) {
    return list_empty(head);
}

void insertProcQ(struct list_head *head, pcb_t *p) {
    list_add_tail(&p->p_list, head);
}

pcb_t *headProcQ(struct list_head *head) {
    if(emptyProcQ(head)) return NULL;
    return container_of(head->next, pcb_t, p_list);
}

pcb_t *removeProcQ(struct list_head *head) {
    if(emptyProcQ(head)) return NULL;

    struct list_head * l = head->next;
    pcb_t *p = container_of(l, pcb_t, p_list);
    list_del(l);

    return p;
}

pcb_t *outProcQ(struct list_head *head, pcb_t *p) {
    struct list_head * l = &p->p_list;
    int found = 0;

    //itera sulla lista e trova l'elemento in p
    struct list_head* iter;
    list_for_each(iter,head) {
        if(iter == l){
            found = 1;
            break;
        }
    }

    //se l'elemento non è stato trovato, ritorna NULL
    if(!found) return NULL;

    //altrimenti rimuovi p dalla lista e ritornalo
    list_del(l);
    return p;
}

int emptyChild(pcb_t *p) {
    if(list_empty(&p->p_child)) return NULL;
}

void insertChild(pcb_t *prnt, pcb_t *p) {
}

pcb_t *removeChild(pcb_t *p) {
}

pcb_t *outChild(pcb_t *p) {
}
