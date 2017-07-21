#include "snw_manager.hh"

StopNWaitManager::StopNWaitManager(const uint32_t & _taskId) :
        taskId(_taskId),
        packet(static_cast<Payload_t *>(malloc(sizeof(Payload_t) * sizeof(packet)))),
        acknowledge(static_cast<Payload_t *>(malloc(sizeof(Payload_t) * sizeof(acknowledge)))),
        ack(static_cast<Payload_t *>(malloc(sizeof(Payload_t) * sizeof(ack))))
{
    // Nothing to do.
}

StopNWaitManager::~StopNWaitManager()
{
    // Nothing to do.
}

Payload_t * StopNWaitManager::handlePacketToSend(
    uint32_t &          task_proxy,
    sc_core::sc_time    timeOut,
    const char *        nome)
{

    // Gestione ACK
    if (!acks.empty())//Se ci sono degli ack da inviare
    {
        if(stop)//Invio l'ack
        {
            Payload_t received = acks.front();
            acks.pop();

            //preparo dati destinatario/mittente
            acknowledge->targetId = received.targetId;
            acknowledge->sourceId = taskId;
            acknowledge->lastSender = taskId;
            acknowledge->seqNumber = received.seqNumber;
            acknowledge->type = received.type;

            //Stampe
            std::cout << "[" << nome << "]  SENDING ";
            std::cout << "\t";
            std::cout << "\tTo " << received.targetId;
            std::cout << "\tACK ";
            std::cout << "\tSize " << sizeof(Payload_t);
            std::cout << "\t  Sequence Number " << received.seqNumber;
            std::cout << "\t(" << sc_core::sc_time_stamp() << ")" << std::endl;


            task_proxy = routing[acknowledge->targetId];

            stop = false;

            return acknowledge;

        }
    }

    // Gestione DATI
    if(sc_core::sc_time_stamp() > (PacketStatus.sendingTime + timeOut) & nextAck != seqNumber)
    {
        timeoutOccured = true;

        //Stampo timeout
        std::cout << "\n##### TIMEOUT " << sc_core::sc_time_stamp() << " ############" << std::endl;
        std::cout << "##### resend packet with SN : " << packet->seqNumber << " #####" << std::endl;
        //Stampo le informazione del pacchetto che devo reinviare
        std::cout << "\n" << std::endl;
        std::cout << "[" << nome << "]   SENDING ";
        std::cout << "\t";
        std::cout << "\tTo " << packet->targetId;
        std::cout << "\tData " << packet->data;
        std::cout << "\tSize " << sizeof(Payload_t);
        std::cout << "\t  Sequence Number " << packet->seqNumber;
        std::cout << "\t(" << sc_core::sc_time_stamp() << ")" << std::endl;
        PacketStatus.sendingTime = sc_core::sc_time_stamp();

        task_proxy = routing[packet->targetId];

        return packet;
    }

    // Se ho ricevuto l'ack e sono nei tempi, allora posso inviare un nuovo pacchetto
    if(sending == true && timeoutOccured == false)
    {
        if (!payloads.empty())//Se ci sono dei dati da inviare
        {
            // Retrive the payload.
            Payload_t received = payloads.front();
            // Pop the payload from the queue.
            payloads.pop();

            //preparo dati destinatario/mittente del pacchetto che devo reinviare
            packet->sourceId = received.sourceId;
            packet->lastSender = taskId;
            packet->targetId = received.targetId;
            packet->seqNumber = received.seqNumber;
            packet->data = received.data;
            packet->type = received.type;

            std::cout << "\n" << std::endl;
            std::cout << "[" << nome << "]   SENDING ";
            std::cout << "\t";
            std::cout << "\tTo " << packet->targetId;
            std::cout << "\tData " << packet->data;
            std::cout << "\tSize " << sizeof(Payload_t);
            std::cout << "\t  Sequence Number " << packet->seqNumber;
            std::cout << "\t(" << sc_core::sc_time_stamp() << ")" << std::endl;

            PacketStatus.seqNToData = packet->data;
            PacketStatus.sendingTime = sc_core::sc_time_stamp();

            //Spengo l'invio di nuovi pacchetti (SNW invia uno per volta)
            if(nextAck==packet->seqNumber)
            {
                sending = false;
            }

            //Numero di sequenza del prossimo pacchetto da inviare
            seqNumber++;

            task_proxy = routing[packet->targetId];

            return packet;
        }
    }
    return NULL;
}

void StopNWaitManager::handlePacketReceive(
    const char *    nome,
    Payload_t *     pack)
{

    //Sul pacchetto che arriva faccio un controllo per verificare se si tratta di un payload normale o di un ack

    //Se è Ack
    if (pack->type == 'a')
    {

        if(pack->seqNumber == nextAck)
        {
            std::cout << "[" << nome << "]   RECEIVED ";
            std::cout << "\tFrom " << pack->sourceId;
            std::cout << "\t";
            std::cout << "\tACK ";
            std::cout << "\tSize " << sizeof(Payload_t);
            std::cout << "\t  Sequence Number " << pack->seqNumber;
            std::cout << "\t(" << sc_core::sc_time_stamp() << ")" << std::endl;
            //Memorizzo il tempo di ricezione dell'ack
            PacketStatus.ackReceivedTime = sc_core::sc_time_stamp();
            std::cout << "\n                        RTT : " << PacketStatus.ackReceivedTime - PacketStatus.sendingTime << "\n" << std::endl;
            nextAck++;
            sending = true;//Riabilito invio dei pacchetti

        }
        else
        {
            timeoutOccured = false;
            std::cout << "\n                RTT : " << sc_core::sc_time_stamp() - PacketStatus.sendingTime << "\n" << std::endl;
        }

    }
    //Se è Dato
    if (pack->type == 'p')
    {
        //Ricevo un dato quindi mando un Ack consono
        std::cout << "[" << nome << "]  RECEIVED ";
        std::cout << "\tFrom " << pack->sourceId;
        std::cout << "\t";
        std::cout << "\tData " << pack->data;
        std::cout << "\tSize " << sizeof(Payload_t);
        std::cout << "\t  Sequence Number " << pack->seqNumber ;
        std::cout << "\t(" << sc_core::sc_time_stamp() << ")" << std::endl;
        //Caso in cui arriva il pacchetto con SN che mi aspettavo
        if(nextSN == pack->seqNumber)
        {
            //Setup ack packet
            ack->type = 'a';
            ack->seqNumber = nextSN;
            ack->sourceId = taskId;
            ack->targetId = pack->lastSender;
            nextSN++;
            stop = true;
            //Metto in coda l'ack
            acks.push(*(ack));
        }
        //Caso in cui arriva un pacchetto duplicato
        else
        {
            ack->type = 'a';
            ack->seqNumber = nextSN-1;
            ack->sourceId = taskId;
            ack->targetId = pack->lastSender;

            stop = true;
            //Metto in coda l'ack
            acks.push(*(ack));
        }
        //Se il dato è per me
        if (pack->targetId==taskId)
        {
            //Faccio quello che dovrei fare
        }
        else
        {
            //Se il dato non è per me
            //Lo metto in coda da spedire
            payloads.push(*pack);
        }
    }
}