/// @file
/// A TLM task.

#include "RemoteTask.hh"


RemoteTask::RemoteTask(
    sc_core::sc_module_name modulename,
    const task_id_t id,
    Scnsl::Core::Node_t * n,
    const size_t proxies) :
        Scnsl::Tlm::TlmTask_if_t(modulename, id, n, proxies),
        taskId(id)
{
    SC_THREAD(writingProcess);
}

RemoteTask::~RemoteTask() throw ()
{
    // Nothing to do.
}

void RemoteTask::writingProcess()
{
        while (true)
        {
                if(stop){//Invio l'ack

                        std::cout << "[" << name() << "]  SENDING ";
                        std::cout << "\t";
                        std::cout << "\tTo " << ack->targetId;
                        std::cout << "\tACK ";
                        std::cout << "\tSize " << sizeof(Ack_t);
                        std::cout << "\t  Sequence Number " << ack->seqNumber;
                        std::cout << "\t(" << sc_core::sc_time_stamp() << ")" << std::endl;
                        wait(125, sc_core::SC_MS);
                        const tlm_taskproxy_id_t tp = 0;
                        TlmTask_if_t::send(tp, reinterpret_cast<byte_t *>(ack), sizeof(Ack_t));
                        stop = false;

                        wait(50, sc_core::SC_MS);
                }else{
                        wait(50, sc_core::SC_MS);
                }
    }
}

void RemoteTask::b_transport(tlm::tlm_generic_payload & payload, sc_core::sc_time & t)
{
        if (payload.get_command() == Scnsl::Tlm::tlm_command_t(Scnsl::Tlm::PACKET_COMMAND))
        {
                //Recupero il pacchetto ricevuto
                Payload_t * p = reinterpret_cast<Payload_t *>(payload.get_data_ptr());
                std::cout << "[" << name() << "]  RECEIVED ";
                std::cout << "\tFrom " << p->sourceId;
                std::cout << "\t";
                std::cout << "\tData " << p->data;
                std::cout << "\tSize " << sizeof(Payload_t);
                std::cout << "\t  Sequence Number " << p->seqNumber;
                std::cout << "\t(" << sc_core::sc_time_stamp() << ")" << std::endl;
                //Se il pacchetto ricevuto è nell'ordine corretto
                if(p->seqNumber == nextSN)
                {
                        //Preparo il pacchetto di tipo Ack_t
                        ack = static_cast<Ack_t *>(malloc(sizeof(Ack_t) * sizeof(ack)));
                        ack->type = 'a';
                        ack->seqNumber = nextSN;
                        ack->sourceId = 3;
                        ack->targetId = 1;
                        //nextSn viene incrementato solamente quando si riceve i pacchetti nell'ordine corretto. Quindi (nextSN-1) rappresenta il valore del SN dell'ultimo pacchetto ricevuto correttamente
                        nextSN++;
                        stop = true;

                }
                else
                {
                        //Il SeqNumber del pacchetto ricevuto non è corretto.
                        //in tal caso si ritrasmette un ACK per il pacchetto più recentemente ricevuto in ordine
                        std::cout << "\n\t\tError - Packet not expected --- Expected : " << nextSN << ", Actual : " << p->seqNumber << "\n" << std::endl;
                        if (nextSN != 0)
                        {
                                ack = static_cast<Ack_t *>(malloc(sizeof(Ack_t) * sizeof(ack)));
                                ack->type = 'a';
                                ack->seqNumber = nextSN - 1;
                                ack->sourceId = 3;
                                ack->targetId = 1;
                                stop = true;
                        }
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
