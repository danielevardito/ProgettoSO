// Definizioni delle costanti mnemoniche per le interruzioni
#define DEVICE_INTERRUPT_BASE 3
#define PLT_INTERRUPT 1
#define INTERVAL_TIMER_INTERRUPT 2


// Funzione per gestire le interruzioni
void HandleInterrupt(int linea_interruzione, int numero_dispositivo) {
    // Verificare il tipo di interruzione
    switch (linea_interruzione) {
    case PLT_INTERRUPT:
        // Gestire l'interruzione del Timer Locale del Processore (PLT)
        // Qui eseguirai le operazioni necessarie per gestire questa interruzione
        break;
    case INTERVAL_TIMER_INTERRUPT:
        // Gestire l'interruzione del Timer Intervallo a livello di sistema
        // Qui eseguirai le operazioni necessarie per gestire questa interruzione
        break;
    default:
        // Gestire un'interruzione del dispositivo
        // Calcolare l'indirizzo del registro del dispositivo
        int devAddrBase = 0x10000054 + ((linea_interruzione - DEVICE_INTERRUPT_BASE) * 0x80) + (numero_dispositivo * 0x10);
        // Salvare lo stato del dispositivo
        int stato_dispositivo = LeggiRegistroDispositivo(devAddrBase);
        // Riconoscere l'interruzione in sospeso scrivendo un comando di ACK nel registro del dispositivo
        ScriviRegistroDispositivo(devAddrBase, ACK);
        // Sbloccare il processo (PCB) che ha avviato questa operazione di I/O e ha richiesto la risposta dello stato
        SbloccaProcessoInAttesa();
        // Inserire il codice di stato nel registro v0 del PCB sbloccato
        ImpostaStatoInPCB(stato_dispositivo);
        // Inserire il PCB sbloccato nella coda dei pronti per passarlo dallo stato "bloccato" allo stato "pronto"
        InserisciCodaPronti();
        break;
    }
}