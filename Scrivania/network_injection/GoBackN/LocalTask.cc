/// @file
/// A TLM task.

#include "LocalTask.hh"
#define Ws 4//Dimensione della finestra di invio

LocalTask::LocalTask(
        sc_core::sc_module_name modulename,
        const task_id_t id,
        Scnsl::Core::Node_t * n,
        const size_t proxies) :
        Scnsl::Tlm::TlmTask_if_t(modulename, id, n, proxies),
        taskId(id)
{
        SC_THREAD(writingProcess);
}

LocalTask::~LocalTask() throw ()
{
    // Nothing to do.
}

void LocalTask::writingProcess()
{
        sc_core::sc_time timeOut (600000, sc_core::SC_US);
        //Creo il pacchetto che verrà inizializzato più avanti, al momente dell'invio
        Payload_t * p = static_cast<Payload_t *>(malloc(sizeof(Payload_t) * sizeof(p)));

        while (true)
        {
                //controllo timeout
                if(sc_core::sc_time_stamp() > (StatusMap[base].sendingTime + timeOut)){
                        std::cout << "\n\t\t@@@@ timeout " << base << "@@@@" << std::endl;
                        nextSNTemp = nextSN;
                        nextSN = base;
                        std::cout << "\t\tnextSNTemp : " << nextSNTemp << ", nextSN : " << nextSN << "\n" << std::endl;
                }

                //Preparo il pacchetto per l'invio
                if(nextSN < base + Ws)
                {
                        if(nextSN <= nextSNTemp - 1)
                        {
                                Payload_t * p = static_cast<Payload_t *>(malloc(sizeof(Payload_t) * sizeof(p)));
                                p->type = 'p';
                                p->sourceId = taskId;
                                p->targetId = 3;
                                p->data = StatusMap[nextSN].seqNToData;
                                p->seqNumber = nextSN;

                                //Stampo le informazioni relative al pacchetto che sto per inviare
                                std::cout << "[" << name() << "]   SENDING ";
                                std::cout << "\t";
                                std::cout << "\tTo " << p->targetId;
                                std::cout << "\tData " << p->data;
                                std::cout << "\tSize " << sizeof(Payload_t);
                                std::cout << "\t  Sequence Number " << p->seqNumber;
                                std::cout << "\t(" << sc_core::sc_time_stamp() << ")" << std::endl;
                                StatusMap[nextSN].sendingTime = sc_core::sc_time_stamp();

                                //Invio il pacchetto creato
                                const tlm_taskproxy_id_t tp = 0;
                                TlmTask_if_t::send(tp, reinterpret_cast<byte_t *>(p), sizeof(Payload_t));
                                nextSN++;
                        }
                        else{
                                // Prepare the payload.

                                p->type = 'p';
                                p->sourceId = taskId;
                                p->targetId = 3;
                                p->data = '!' + (rand() % 14);
                                p->seqNumber = nextSN;
                                //Inizializzo la struct che conterrà le informazioni relative al pacchetto da inviare
                                PacketStatus.seqNToData = p->data;
                                PacketStatus.seqNToAcked = false;
                                PacketStatus.sendingTime = sc_core::sc_time_stamp();
                                StatusMap.insert(std::pair<int,PacketStatusInformation>(nextSN, PacketStatus));
                                //Stampo le informazioni relative al pacchetto che sto per inviare
                                std::cout << "[" << name() << "]   SENDINg ";
                                std::cout << "\t";
                                std::cout << "\tTo " << p->targetId;
                                std::cout << "\tData " << p->data;
                                std::cout << "\tSize " << sizeof(Payload_t);
                                std::cout << "\t  Sequence Number " << p->seqNumber;
                                std::cout << "\t(" << sc_core::sc_time_stamp() << ")" << std::endl;

                                //Invio il pacchetto creato
                                const tlm_taskproxy_id_t tp = 0;
                                TlmTask_if_t::send(tp, reinterpret_cast<byte_t *>(p), sizeof(Payload_t));
                                nextSN++;
                                std::cout << "\n\t\tBASe : " << base << "\tNEXTSN : " << nextSN << "\n" << std::endl;
                        }
                }
                wait(100, sc_core::SC_US);//Attendo 100us
        }
}

void LocalTask::b_transport(tlm::tlm_generic_payload & payload, sc_core::sc_time & t)
{
        if (payload.get_command() == Scnsl::Tlm::tlm_command_t(Scnsl::Tlm::PACKET_COMMAND))
        {
                sc_core::sc_time timeOut (600000, sc_core::SC_US);
                Ack_t * a =  reinterpret_cast<Ack_t *>(payload.get_data_ptr());//Recupero il pacchetto di ack

                if(nextAck == a->seqNumber)
                {
                        std::cout << "[" << name() << "]   RECEIVED ";
                        std::cout << "\tFrom" << a->sourceId;
                        std::cout << "\t";
                        std::cout << "\tACK ";
                        std::cout << "\tSize " << sizeof(Ack_t);
                        std::cout << "\t  Sequence Number " << a->seqNumber;
                        std::cout << "\t(" << sc_core::sc_time_stamp() << ")" << std::endl;
                        //Viene confermato il pacchetto che mi aspettavo, quindi posso incrementare la variabile base
                        /*std::map<int,PacketStatusInformation>::iterator it;
                        it = StatusMap.find(a->seqNumber);
                        it->second.seqNToAcked = true;
                        it->second.ackReceivedTime = sc_core::sc_time_stamp();*/
                        StatusMap[nextAck].seqNToAcked = true;
                        StatusMap[nextAck].ackReceivedTime = sc_core::sc_time_stamp();

                        base++;
                        nextAck++;
                        std::cout << "\n\t\tRTT of packet with SN " << a->seqNumber << " : " << StatusMap[a->seqNumber].ackReceivedTime - StatusMap[a->seqNumber].sendingTime << std::endl;
                        std::cout << "\t\tBASE : " << base << "\tNEXTSN : " << nextSN << "\n" << std::endl;
                        if ((StatusMap[nextAck].ackReceivedTime-StatusMap[nextAck].sendingTime)>timeOut) std::cout << "\n\t\t#### timeout "<< nextAck <<"#### "<<StatusMap[a->seqNumber].ackReceivedTime<<std::endl;

                }else{
                        //L'ack arrivato non è quello che il mittente si aspettava e viene scartato
                        std::cout << "\n\t\tError - Wrong ACK --- Expected : " << nextAck << ", Actual : " << a->seqNumber << " -- the ACK will be discarded\n"<< std::endl;
                }

        }
        else if (payload.get_command() == Scnsl::Tlm::tlm_command_t(Scnsl::Tlm::CARRIER_COMMAND))
        {
        //std::cout << "[" << name() << "]  RECEIVED CARRIER_COMMAND\n";
        }
        else
        {
        //std::cout << "[" << name() << "]  RECEIVED SOMETHING\n";
        }
}
