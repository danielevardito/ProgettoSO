#include "./headers/pcb.h"
#include "../klog.c"

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
    if(emptyChild(prnt)){
        prnt->p_child.next = &p->p_child;
    }
    else{
        pcb_t * fstChild = container_of(&prnt->p_child.next, pcb_t, p_child);
        struct list_head * tmp = &fstChild->p_sib;

        while(tmp != tmp->next) tmp =  tmp->next;

        tmp->next = &p->p_sib;
        p->p_sib.prev = tmp;
    }

    p->p_parent = prnt;
}

pcb_t *removeChild(pcb_t *p) {
    if(emptyChild(p)) return NULL;

    pcb_t * child = container_of(&p->p_child.next, pcb_t, p_child);
    child->p_parent = NULL;

    if(list_empty(&child->p_sib))
        //il padre non punta più al suo unico figlio    
        INIT_LIST_HEAD(&p->p_child); 
    else{
        //il padre punta come primo figlio al secondo figlio
        p->p_child.next = child->p_sib.next;
        INIT_LIST_HEAD(&child->p_sib);
    }

    return child;
}

pcb_t *outChild(pcb_t *p) {
    if(p->p_parent == NULL) return NULL;

    if(p->p_sib.prev == &p->p_sib){ //È il primo figlio
        pcb_t * prnt = p->p_parent; 
        if(list_empty(&p->p_sib)) //Se non ha fratelli
            INIT_LIST_HEAD(&prnt->p_child);
        else{  //se ha fratelli
            prnt->p_child.next = &p->p_sib.next; //il padre punta al secondo figlio
            p->p_sib.next->prev = p->p_sib.next; //il secondo fratello punterà con prev a se stesso, diventando il primo
        }
    }
    else if(p->p_sib.next != &p->p_sib) //È un figlio intermedio
        list_del(&p->p_sib);
    else //È l'ultimo figlio
        p->p_sib.prev->next = p->p_sib.prev; //Il penultimo figlio punta a se stesso per diventare l'ultimo

    INIT_LIST_HEAD(&p->p_sib);
    p->p_parent = NULL;

    return p;
}
