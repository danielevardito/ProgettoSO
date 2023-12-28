#ifndef PCB_H_INCLUDED
#define PCB_H_INCLUDED

#include "../../headers/const.h"
#include "../../headers/types.h"
#include "../../headers/listx.h"

/*
Nei commenti "*p" denota "l'elemento puntato da p"
*/

/*
COMPUTAZIONE: ogni elemento dell'array pcbTable viene "liberato", ossia viene inserito nella lista pcbFree_h
*/
void initPcbs();


/*
PARAMETRI: (pcb_t *) p: puntatore al PCB da liberare
COMPUTAZIONE: *p viene inserito nella lista pcbFree_h
*/
void freePcb(pcb_t *p);


/*
RITORNO: (pcb_t *): puntatore al PCB allocato
COMPUTAZIONE: il primo elemento della lista pcbFree_h viene rimosso dalla lista, per allocare il PCB corrispondente.
              L'allocazione consiste nell'assegnare a 0 o NULL tutti i campi del PCB, tranne che per il pid, che 
              viene assegnato in ordine crescente
*/
pcb_t *allocPcb();


/*
PARAMETRI: (list_head *) head: puntatore alla coda da inizializzare
COMPUTAZIONE: una nuova coda è una lista appena inizializzata
*/
void mkEmptyProcQ(struct list_head *head);


/*
PARAMETRI: (list_head *) head: puntatore alla coda da verificare 
RITORNO: (int): valore di verità: 0 Falso, 1 Vero
COMPUTAZIONE: se *head risulta vuota, viene ritornato falso, altrimenti vero
*/
int emptyProcQ(struct list_head *head);


/*
PARAMETRI: (list_head *) head: puntatore alla coda in cui va inserito *p
           (pcb_t *) p: puntatore al PCB da inserire
COMPUTAZIONE: *p viene inserito alla fine di *head
*/
void insertProcQ(struct list_head *head, pcb_t *p);


/*
PARAMETRI: (list_head *) head: puntatore alla coda di PCB
RITORNO: (pcb_t *): puntatore al processo in testa a head
COMPUTAZIONE: se la coda è vuota, ritorna NULL.
              Altrimenti, ritorna il PCB in testa
*/
pcb_t *headProcQ(struct list_head *head);


/*
PARAMETRI: (list_head *) head:
RITORNO: (pcb_t *):
COMPUTAZIONE: se la coda è vuota ritorna NULL
              altrimenti rimuovi il primo PCB e ritornalo
*/
pcb_t *removeProcQ(struct list_head *head);


/*
PARAMETRI: (list_head *) head: puntatore alla coda in cui va rimosso l'elemento
           (pcb_t *) p: elemento da rimuovere
RITORNO: (pcb_t *): elemento rimosso
COMPUTAZIONE: verifica se *p è in *head
              se p non è in head ritorna NULL
              altrimenti ritona p
*/
pcb_t *outProcQ(struct list_head *head, pcb_t *p);


/*
PARAMETRI: (pcb_t *) p: PCB padre
RITORNO: (int): booleano: 1 vero, 0 falso
COMPUTAZIONE: se la lista corrispondente ai figli è vuota, ritorna vero
              altrimenti falso
*/
int emptyChild(pcb_t *p);


/*
PARAMETRI: (pcb_t *) prnt: PCB padre
           (pcb_t *) p: PCB figlio
COMPUTAZIONE: *p viene inserito come ultimo figlio di *prnt - questo copre tutti i casi 
                (ovvero se *prnt ha figli o no)
              poi a *p viene assegnato prnt come padre
*/
void insertChild(pcb_t *prnt, pcb_t *p);


/*
PARAMETRI: (pcb_t *) p: PCB padre in cui va rimosso il primo figlio
RITORNO: (pcb_t *): PCB figlio rimosso
COMPUTAZIONE: se *p non ha figli, ritorna NULL
              altrimenti rimuovi il figlio dalla lista, assegna NULL al campo p_parent del figlio
              e ritorna il figlio
*/
pcb_t *removeChild(pcb_t *p);


/*
PARAMETRI: (pcb_t *) p: PCB da rimuovere
RITORNO: (pcb_t *): PCB rimosso
COMPUTAZIONE: se p non ha un padre, ritorna NULL
              altrimenti rimuovi p dalla lista dei fratelli (e quindi non è accessibile dal padre anche se figlio unico)
              assegna NULL al campo p_parent di *p e ritorna p
*/
pcb_t *outChild(pcb_t *p);

#endif
