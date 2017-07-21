#include "gbn_manager.hh"

GoBackNManager::GoBackNManager(const uint32_t & _taskId) :
    taskId(_taskId),
    destination(),
    window_size(4),
    statusMap(),
    base(1),
    nextSN(1),
    nextAck(1),
    nextSNTemp(1),
    maxSN(1),
    packet(static_cast<Payload_t *>(malloc(sizeof(Payload_t) * sizeof(packet)))),
    acknowledge(static_cast<Payload_t *>(malloc(sizeof(Payload_t) * sizeof(acknowledge)))),
    ack(static_cast<Payload_t *>(malloc(sizeof(Payload_t) * sizeof(ack)))),
    nextSNack(1),
    payloads(),
    acks(),
    routing()
{
    // Nothing to do.
}

GoBackNManager::~GoBackNManager()
{
    // Nothing to do.
}

Payload_t * GoBackNManager::handlePacketToSend(uint32_t & task_proxy, sc_core::sc_time timeOut, const char* nome)
{

    PacketStatusInformation status;

    // Gestione ACK
    if (!acks.empty())
    {
        //Se ci sono degli ack da inviare, nvio l'ack

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

        return acknowledge;

    }

    // Gestione DATI

    //controllo timeout
    if(sc_core::sc_time_stamp() > (statusMap[base].sendingTime + timeOut))
    {
        std::cout << "\n\t\t@@@@ timeout " << base << "@@@@" << std::endl;
        nextSN = base;
        nextSNTemp = base + window_size;
        std::cout << "\t\tnextSNTemp : " << nextSNTemp << ", nextSN : " << nextSN << "\n" << std::endl;
        //Aggiorno il tempo dopo il time out
        statusMap[base].sendingTime = sc_core::sc_time_stamp();

    }

    //Preparo il pacchetto per l'invio
    if(nextSN < base + window_size)
    {
        if((nextSN <= maxSN) && (nextAck < maxSN))
        {

            packet->lastSender = taskId;
            packet->data = statusMap[nextSN].seqNToData;
            packet->seqNumber = nextSN;

            //Stampo le informazioni relative al pacchetto che sto per inviare
            std::cout << "[" << nome << "]   SENDING ";
            std::cout << "\t";
            std::cout << "\tTo " << packet->targetId;
            std::cout << "\tData " << packet->data;
            std::cout << "\tSize " << sizeof(Payload_t);
            std::cout << "\t  Sequence Number " << packet->seqNumber;
            std::cout << "\t(" << sc_core::sc_time_stamp() << ")" << std::endl;
            statusMap[nextSN].sendingTime = sc_core::sc_time_stamp();

            //Invio il pacchetto creato
            task_proxy = routing[packet->targetId];
            nextSN++;

            return packet;

        }
        else
        {
            //Se ci sono dei dati da inviare
            if (!payloads.empty() && maxSN <= nextSNTemp)
            {
                // Retrive the payload.
                Payload_t received = payloads.front();
                // Pop the payload from the queue.
                payloads.pop();

                // Prepare the payload.
                packet->sourceId = received.sourceId;
                packet->lastSender = taskId;
                packet->targetId = received.targetId;
                packet->seqNumber = received.seqNumber;
                packet->data = received.data;
                packet->type = received.type;

                //Inizializzo la struct che conterrà le informazioni relative al pacchetto da inviare
                status.seqNToData = packet->data;
                status.sendingTime = sc_core::sc_time_stamp();
                statusMap.insert(std::make_pair(packet->seqNumber, status));
#if 0
                std::cout << "\tAAA nextSN          '" << nextSN << "'\n";
                std::cout << "\tAAA nextAck         '" << nextAck << "'\n";
                std::cout << "\tAAA nome            '" << nome << "'\n";
                std::cout << "\tAAA sendingTime     '" << status.sendingTime << "'\n";


                std::cout << "\tAAA last sender        '" << packet->lastSender << "'\n";
                std::cout << "\tAAA numero maxSN        '" << maxSN << "'\n";
                std::cout << "\tAAA numero del nextSN   '" << packet->seqNumber << "'\n";
                std::cout << "\tAAA Dato nel seqNToData '" << status.seqNToData << "'\n";
                std::cout << "\tAAA Dato nel map        '" << statusMap[packet->seqNumber].seqNToData << "'\n\n";
#endif
                if (received.seqNumber>maxSN)
                {
                    maxSN = received.seqNumber;
                }

                //Stampo le informazioni relative al pacchetto che sto per inviare
                std::cout << "[" << nome << "]   SENDING ";
                std::cout << "\t";
                std::cout << "\tTo " << packet->targetId;
                std::cout << "\tData " << packet->data;
                std::cout << "\tSize " << sizeof(Payload_t);
                std::cout << "\t  Sequence Number " << packet->seqNumber;
                std::cout << "\t(" << sc_core::sc_time_stamp() << ")" << std::endl;

                //Invio il pacchetto creato
                task_proxy = routing[packet->targetId];
                nextSN++;

                return packet;
                std::cout << "\n\t\tBASE : " << base << "\tNEXTSN : " << nextSN << "\n" << std::endl;

            }

        }

    }
    return NULL;

}

void GoBackNManager::handlePacketReceive(const char* nome, Payload_t * pack, sc_core::sc_time timeOut)
{

    //Sul pacchetto che arriva faccio un controllo per verificare se si tratta di un payload normale o di un ack

    //Se è Ack
    if (pack->type == 'a')
    {

        if(nextAck <= pack->seqNumber && base <= pack->seqNumber )
        {
            std::cout << "[" << nome << "]   RECEIVED ";
            std::cout << "\tFrom " << pack->sourceId;
            std::cout << "\t";
            std::cout << "\tACK ";
            std::cout << "\tSize " << sizeof(Payload_t);
            std::cout << "\t  Sequence Number " << pack->seqNumber;
            std::cout << "\t(" << sc_core::sc_time_stamp() << ")" << std::endl;
            //Viene confermato il pacchetto che mi aspettavo, quindi posso incrementare la variabile base

            base++;
            nextAck=pack->seqNumber;
            statusMap[nextAck].ackReceivedTime = sc_core::sc_time_stamp();

            std::cout << "\n\t\tRTT of packet with SN " << pack->seqNumber << " : " << statusMap[pack->seqNumber].ackReceivedTime - statusMap[pack->seqNumber].sendingTime << std::endl;
            std::cout << "\t\tBASE : " << base << "\tNEXTSN : " << nextSN << "\n" << std::endl;
            if ((statusMap[nextAck].ackReceivedTime-statusMap[nextAck].sendingTime)>timeOut)
                std::cout << "\n\t\t#### timeout "<< nextAck <<"#### "<<statusMap[pack->seqNumber].ackReceivedTime<<std::endl;
        }
        else
        {
            //L'ack arrivato non è quello che il mittente si aspettava e viene scartato
            std::cout << "\n\t\tError - Wrong ACK --- Expected : " << base << ", Actual : " << pack->seqNumber << " -- the ACK will be discarded\n"<< std::endl;
        }
    }

    //Se è Dato
    if (pack->type == 'p')
    {

#if 0
                std::cout << "\tAAA last sender            '" << pack->lastSender << "'\n";
                std::cout << "\tAAA pack->sourceId;        '" << pack->sourceId << "'\n";
                std::cout << "\tAAA numero del seqNumber   '" << pack->seqNumber << "'\n";
                std::cout << "\tAAA Dato pack->data        '" << pack->data << "'\n";
                std::cout << "\tAAA Dato nel map           '" << pack->targetId << "'\n\n";
#endif

        std::cout << "[" << nome << "]  RECEIVED ";
        std::cout << "\tFrom " << pack->sourceId;
        std::cout << "\t";
        std::cout << "\tData " << pack->data;
        std::cout << "\tSize " << sizeof(Payload_t);
        std::cout << "\t  Sequence Number " << pack->seqNumber ;
        std::cout << "\t(" << sc_core::sc_time_stamp() << ")" << std::endl;
        //Caso in cui arriva il pacchetto con SN che mi aspettavo
        if(nextSNack == pack->seqNumber)
        {
            //Setup ack packet
            ack = static_cast<Payload_t *>(malloc(sizeof(Payload_t) * sizeof(ack)));
            ack->type = 'a';
            ack->seqNumber = nextSNack;
            ack->sourceId = taskId;
            ack->targetId = pack->lastSender;

            nextSNack++;
            acks.push(*(ack));

        }
        //Caso in cui arriva un pacchetto duplicato
        else
        {
            ack = static_cast<Payload_t *>(malloc(sizeof(Payload_t) * sizeof(ack)));
            ack->type = 'a';
            ack->seqNumber = nextSNack-1;
            ack->sourceId = taskId;
            ack->targetId = pack->lastSender;
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
