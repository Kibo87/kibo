- STRUTTURE DATI DI UN TASK
    - Coda dei pacchetti da inviare.
    - Coda degli ack da inviare.
    - Mappa che tiene traccia dello stato di comunicazione con gli altri task, mediante il protocollo selezionato.

FUNCTION 1:
    - Misura la temperatura e la invia a Pippo. -> Butta un packet_t nella coda dei DATI.

SEND
    - Se c'è un ACK da inviare nella coda degli ACK da inviare, invia l'ACK.
    - Se c'è un DATO da inviare nella coda dei dati da inviare, invia il dato.

RECEIVE
    - Se ricevo un ACK,
        - reperisco nella mappa che mi tiene traccia delle comunicazioni con i miei Vicini e eseguo tutto quello che va fatto quando ricevo un ACK.
    - Se ricevo un pacchetto DATI,
        - ma questo pacchetto non è per me:
            - Aggiungo l'ACK a chi mi aveva mando il pacchetto alla coda degli ACK.
            - Aggiungo il DATO alla lista dei DATI da inviare.
        - allora devo:
            - Aggiungo l'ACK a chi mi aveva mando il pacchetto alla coda degli ACK.
            - Me ne frego del pacchetto.
			
------------------------------------------------------------------------------------------------


FUNCTION 1 (specifica per la prova):
        -Preparo dei dati random da inviare con intervallo regolare.
                -metto mittente, destinatario e i dati
                -metto un packet_t nella coda dei DATI del nodo in cui sono da inviare.

SEND
        -Se c'è un ACK da inviare nella coda degli ACK da inviare, invia l'ACK:
                -"apro" in Ack_t * a
                -metto destinatario/mittente corretti (uso proxy e uso mappa)
                -spedisco
        -Se c'è un DATO da inviare nella coda dei dati da inviare, invia il dato:
                -"apro" in Payload_t * p
                -metto destinatario/mittente corretti (uso proxy e uso mappa)
                -spedisco
                
RECEIVE
        -"apro" in Payload_t * p
        -Se ricevo un ACK:
                -aggiorno variabili del caso per continuare a inviare dati succesivi
        -Se ricevo un pacchetto DATI:
                -se in ordine corretto di ricezione, aggiorno variabili del caso
                -metto un ack in coda Ack (nuovo o  vecchio a seconda se avevo riccevuto un dato sbagliato)
                -se i dati sono per me                 
                        -in teoria li uso ( lascio vuoto )
                -dati per altri
                        -mi setto come ultimo mittente (in realtà lo faccio nel sender)
                        -metto dati in coda Dati

