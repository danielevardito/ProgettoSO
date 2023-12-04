#include "./headers/msg.h"
#include "../klog.c"

static msg_t msgTable[MAXMESSAGES];
LIST_HEAD(msgFree_h); //initializes msgFree_h as an empty linked list

void initMsgs() {

    /*initialize the msgFree list to contain all the elements of the static array of MAXMESSAGES messages.
    This method will be called only once during data structureinitialization.*/

   //Il contemuto della linked list sono liste
    for (int i = 0; i < MAXMESSAGES; i++) { //Si puo sicuramente simplificare con un for each
        
        INIT_LIST_HEAD(&msgTable[i].m_list);
        list_add_tail(&msgTable[i].m_list, &msgFree_h); //Aggiunge in coda tutti gli elementi di m_list

    }
}

void freeMsg(msg_t *m) {  //insert the element pointed to by m onto the msgFree list.
    list_add_tail(&m->m_list, &msgFree_h); //aggiunge il nuovo elemento m (m_list di msg_t) in coda a msgFree 
}

msg_t *allocMsg() {

    /*return NULL if the msgFree list is empty. 
    Otherwise, remove an element from the msgFree list,
    provide initial values for ALL of the messages fields and then
    return a pointer to the removed element. 
    Messages get reused, so it is important that no previous value persist in amessage when it gets reallocated.*/

    if (list_empty(&msgFree_h)) return NULL;
    
    msg_t * entry = container_of(msgFree_h.next, msg_t, m_list);
   
    list_del(&entry->m_list); //Rimuove l'elemento entry dalla lista in cui e' contenuto.

    INIT_LIST_HEAD(&entry->m_list); 
    entry->m_sender = NULL;
    entry->m_payload = 0;

    return entry;

 }



void mkEmptyMessageQ(struct list_head *head) {
	/* used to initialize a variable to be head pointer to a message queue;
    returns a pointer to the head of an empty message queue.*/

    //MESSAGGIO NUOVO? --> No

    INIT_LIST_HEAD(head); //INIT_LIST_HEAD(struct list_head* list){}
    //return head; Avanza
}

int emptyMessageQ(struct list_head *head) {
	/*TRUE if the queue whose tail is pointed to by head is empty,
    FALSE otherwise.*/
    //Se la coda della lista su cui punta head é vuota?Quindi se la lista é vuota?
    return list_empty(head);
}

void insertMessage(struct list_head *head, msg_t *m) {
	/*: insert the message pointed to by m at the end (tail) of
    the queue whose head pointer is pointed to by head.*/

    list_add_tail(&m->m_list, head);
}

void pushMessage(struct list_head *head, msg_t *m) {
	/* insert the message pointed to by m at the head of the
    queue whose head pointer is pointed to by head.*/

    list_add(&m->m_list, head);
}

msg_t *popMessage(struct list_head *head, pcb_t *p_ptr) {
/*remove the first element (starting by the head) from
the message queue accessed via head whose sender is p ptr.

If p ptr is NULL, return the first message in the queue.
if the message queue was empty or if no
message from p ptr was found Return NULL; 
otherwise return the pointer
to the removed message*/

    if (list_empty(head)) {
        return NULL;    
    }

    if (p_ptr == NULL) {
        return container_of(head->next, msg_t, m_list); //Il nome della lista dovrebbe ritornare il primo valore
    }
    
    struct list_head* iter;
    list_for_each(iter, head){
        msg_t * item = container_of(iter, msg_t, m_list);
        if (item->m_sender == p_ptr) {
            list_delm_list(&item->m_list); //(*elem) è un puntatore
            return item;
        }   
    }
    
    return NULL;

}

msg_t *headMessage(struct list_head *head) {
	/* return a pointer to the first message from the queue whose head is pointed to by head. 
    Do not remove the message from the queue. 
    Return NULL if the queue is empty*/

    if (list_empty(head))
        return NULL;
    else 
        return head->next;
    
}