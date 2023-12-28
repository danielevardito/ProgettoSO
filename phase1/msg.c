#include "./headers/msg.h"

static msg_t msgTable[MAXMESSAGES];
LIST_HEAD(msgFree_h); //initializes msgFree_h as an empty linked list

void initMsgs() {

    for (int i = 0; i < MAXMESSAGES; i++) { 
        freeMsg(&msgTable[i]);
    }

}

void freeMsg(msg_t *m) {  
    INIT_LIST_HEAD(&m->m_list);
    list_add_tail(&m->m_list, &msgFree_h); 
}

msg_t *allocMsg() {
    
    if (list_empty(&msgFree_h)) return NULL;
    
    msg_t * entry = container_of(msgFree_h.next, msg_t, m_list);
   
    list_del(&entry->m_list); //Rimuove l'elemento entry dalla lista in cui e' contenuto.

    INIT_LIST_HEAD(&entry->m_list); 
    entry->m_sender = NULL;
    entry->m_payload = 0;

    return entry;

 }

void mkEmptyMessageQ(struct list_head *head) {
    INIT_LIST_HEAD(head); 
}

int emptyMessageQ(struct list_head *head) {
    return list_empty(head);
}

void insertMessage(struct list_head *head, msg_t *m) {
    list_add_tail(&m->m_list, head);
}

void pushMessage(struct list_head *head, msg_t *m) {
    list_add(&m->m_list, head);
}

msg_t *popMessage(struct list_head *head, pcb_t *p_ptr) {

    if (list_empty(head)) {
        return NULL;    
    }

    if (p_ptr == NULL) {
        msg_t * m = container_of(head->next, msg_t, m_list); //Il nome della lista dovrebbe ritornare il primo valore
        list_del(&m->m_list);
        INIT_LIST_HEAD(&m->m_list);
        return m;
    }
    
    struct list_head * iter;
    list_for_each(iter, head){
        msg_t * item = container_of(iter, msg_t, m_list);
        //se p_ptr è in lista, rimuovilo e ritorna il messaggio corrispondente
        if (item->m_sender == p_ptr) {
            list_del(&item->m_list); //(*elem) è un puntatore
            INIT_LIST_HEAD(&item->m_list);
            return item;
        }   
    }

    //il PCB (diverso da NULL) di cui va rimosso il messaggio non è in lista
    return NULL;

}

msg_t *headMessage(struct list_head *head) {

    if (list_empty(head))
        return NULL;
    else 
        return container_of(head->next, msg_t, m_list);
    
}