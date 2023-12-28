#ifndef MSG_H_INCLUDED
#define MSG_H_INCLUDED

#include "../../headers/const.h"
#include "../../headers/types.h"
#include "../../headers/listx.h"

/*
Nei commenti "*m" denota "l'elemento puntato da m"
*/

/*
COMPUTAZIONE: ogni messaggio dell'array msgTable viene "liberato", ovvero viene aggiunto 
              alla lista dei messaggi liberi
*/
void initMsgs();

/*
PARAMETRI: (msg_t *) m: messaggio da liberare
COMPUTAZIONE: *m viene aggiunto alla lista dei messaggi liberi
*/
void freeMsg(msg_t *m);

/*
RITORNO: (msg_t *): messaggio allocato
COMPUTAZIONE: se la lista dei messaggi liberi è vuota ritorna NULL
              altrimenti il primo messaggio della lista dei messaggi liberi viene rimosso,
              vengono inizializzati tutti i campi e viene ritornato il messaggio allocato
*/
msg_t *allocMsg();

/*
PARAMETRI: (list_head *) head: nuova coda di messaggi
COMPUTAZIONE: head viene inizializzata
*/
void mkEmptyMessageQ(struct list_head *head);

/*
PARAMETRI: (list_head *) head: coda da verificare vuota
RITORNO: (int): booleano 1 vero, 0 falso
COMPUTAZIONE: se head è vuota ritorna vero, altrimenti falso
*/
int emptyMessageQ(struct list_head *head);

/*
PARAMETRI: (list_head *) head: coda di messaggi 
           (msg_t *) m: messaggio da inserire in coda
COMPUTAZIONE: inserisci m come ultimo elemento di head
*/
void insertMessage(struct list_head *head, msg_t *m);

/*
PARAMETRI: (list_head *) head: coda di messaggi
           (msg_t *) m: messaggio da inserire in testa
COMPUTAZIONE: aggiungi m come primo elemento di head
*/
void pushMessage(struct list_head *head, msg_t *m);

/*
PARAMETRI: (list_head *) head: coda di messaggi
           (pcb_t *) p_ptr: PCB il cui messaggio va rimosso 
RITORNO: (msg_t *): messaggio rimosso
COMPUTAZIONE: se la coda di messaggi è vuota, ritorna NULL - non c'è nessun messaggio da rimuovere
              se p_ptr è NULL, rimuovi il primo elemento in coda e ritornalo
              altrimenti, scorro head e se trovo un messaggio di p_ptr, lo rimuovo e lo ritorno
              altrimenti sono di sicuro nel caso in cui il head e p_ptr sono diversi da NULL ma non c'è
              nessun messaggio di p_ptr, quindi ritorna NULL
*/
msg_t *popMessage(struct list_head *head, pcb_t *p_ptr);

/*
PARAMETRI: (list_head *) head: coda di messaggi
RITORNO: (msg_t *): primo messaggio in coda
COMPUTAZIONE: se head è vuota, ritorna NULL
              altrimenti, ritorna il primo elemento in coda
*/
msg_t *headMessage(struct list_head *head);

#endif
