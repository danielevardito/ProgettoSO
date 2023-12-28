#include "./headers/pcb.h"

static pcb_t pcbTable[MAXPROC];
LIST_HEAD(pcbFree_h);
static int next_pid = 1; 

void initPcbs() {
    for(int i = 0; i < MAXPROC; i++){
        freePcb(&pcbTable[i]);
    }
}

void freePcb(pcb_t *p) {
    list_add_tail(&p->p_list, &pcbFree_h);
}

pcb_t *allocPcb() {
    if(list_empty(&pcbFree_h)) return NULL;

    //rimozione del primo PCB in lista
    struct pcb_t * p = container_of(pcbFree_h.next, pcb_t, p_list);
    list_del(&p->p_list);
    /*
    inizializzazione dei campi
    */
    INIT_LIST_HEAD(&p->p_list);
    p->p_parent = NULL;
    INIT_LIST_HEAD(&p->p_child);
    INIT_LIST_HEAD(&p->p_sib);
    p->p_time = 0;
    INIT_LIST_HEAD(&p->msg_inbox);
    p->p_supportStruct = NULL;
    p->p_pid = next_pid;
    next_pid++;
    p->p_time = 0;

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
    //se è vuota ritorna NULL
    if(emptyProcQ(head)) return NULL;

    /*
    Altrimenti rimuovi il primo elemento (il next della sentinella)
    e ritorna il pcb corrispondente
    */
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

    //altrimenti rimuovi p dalla lista, svuota il campo p_list e ritorna p
    list_del(l);
    INIT_LIST_HEAD(l);
    return p;
}

int emptyChild(pcb_t *p) {
    if(list_empty(&p->p_child)) return 1;

    return 0;
}

void insertChild(pcb_t *prnt, pcb_t *p) {

    list_add_tail(&p->p_sib, &prnt->p_child);
    p->p_parent = prnt;
    
}

pcb_t *removeChild(pcb_t *p) {
    //se non ci sono figli, ritorna NULL
    if(emptyChild(p)) return NULL;

    /*
    se ci sono figli, elimina il primo figlio
    Settando a NULL suo padre ed eliminandolo dalla lista dei fratelli
    */
    pcb_t * son = container_of(p->p_child.next, pcb_t, p_sib);
    son->p_parent = NULL;
    list_del(&son->p_sib);

    return son;
}

pcb_t *outChild(pcb_t *p) {
    //se non ha un padre, ritorna NULL
    if(p->p_parent == NULL) return NULL;

    /*
    altrimenti setta come padre NULL e rimuovilo dalla lista dei fratelli
    */
    p->p_parent = NULL;
    list_del(&p->p_sib);

    return p;
}
