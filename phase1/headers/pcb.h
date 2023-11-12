#ifndef PCB_H_INCLUDED
#define PCB_H_INCLUDED

#include "../../headers/const.h"
#include "../../headers/types.h"
#include "../../headers/listx.h"


/*
Inizializza la lista pcbFree in modo che contenga tutti gli elementi dell'array statico dei MAXPROC PCB
La funzione viene chiamata solo una volta, nell'inizializzazione della struttura
*/
void initPcbs();


/*
@param 
    pcb_t *p: puntatore che punta a un processo
Inserisco l'elemento PCB puntato da p nella lista free_pcb
*/
void freePcb(pcb_t *p);


/*
@return
    type: pcb_t
    puntatore a processo allocato o NULL

Prende un PCB dalla lista free_pcb, assegna un valore iniziale a tutti i campi del PCB
*/
pcb_t *allocPcb();


/*
@param
    list_head pointer head - puntatore alla testa della list_head
Viene inizializzata la variabile head come testa della coda dei processi
*/
void mkEmptyProcQ(struct list_head *head);


/*
@param
    list_head pointer head - lista da analizzare
@return
    int - in realtà booleano - 0 se piena, 1 se vuota
*/
int emptyProcQ(struct list_head *head);


/*
@param
    list_head pointer head - coda di PCB
    pcb_t pointer p - processo da aggiunger in coda
Viene aggiunto il processo p alla coda di processi che comincia con head
*/
void insertProcQ(struct list_head *head, pcb_t *p);


/*
@param
    list_head pointer head - sentinella della coda di PCB
@return 
    pcb_t pointer - puntatore al primo elemento della coda
Restituisce il primo elemento della coda (il primo PCB) se esiste, NULL altrimenti
*/
pcb_t *headProcQ(struct list_head *head);


/*
@param
    list_head pointer head - sentinella della coda di PCB
@return 
    pcb_t pointer - puntatore al primo elemento della coda
Rimuove il primo elemento della coda e lo restiuisce. Se la coda è vuota, restituisce NULL
*/
pcb_t *removeProcQ(struct list_head *head);


/*
@param 
    list_head pointer head - sentinella della lista di PCB
    pcb_t pointer p - elemento da rimuovere
@return
    pcb_t pointer - puntatore all'elemento rimosso
Rimuove un elemento p dalla lista puntata dalla sentinella head
Se l'elemento è presente, viene rimosso e restituito il suo puntatore
altrimenti ritorna NULL
*/
pcb_t *outProcQ(struct list_head *head, pcb_t *p);


/*
@param
    pcb_t pointer p - puntatore a un PCB
@return 
    int - TRUE se p non ha figli, FALSE altrimenti
*/
int emptyChild(pcb_t *p);


/*
@param
    pcb_t pointer prnt - puntatore al PCB padre
    pcb_t pointer p - puntatore al PCB che diventerà figlio
Inserisce p come figlio di prnt
*/
void insertChild(pcb_t *prnt, pcb_t *p);


/*
@param
    pcb_t pointer p - puntatore a un PCB
@return 
    pcb_t pointer - puntatore al primo figlio di p
Rimuove il primo figlio di p e ne restituisce il puntatore
Ritorna NULL se p non ha figli
*/
pcb_t *removeChild(pcb_t *p);


/*
@param
    pcb_t pointer p - PCB da rimuovere dall'albero
@return 
    pcb_t pointer - PCB rimosso
Il PCB puntato da p non avrà più suo padre
Se p ha un padre, viene ritornato p
Altrimenti ritorna NULL
*/
pcb_t *outChild(pcb_t *p);

#endif
