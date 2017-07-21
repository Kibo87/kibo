/// @file
/// A TLM task.

#include "CentralTask.hh"

//bool stopC = false;



CentralTask::CentralTask(
    sc_core::sc_module_name modulename,
    const task_id_t id,
    Scnsl::Core::Node_t * n,
    const size_t proxies) :
        Scnsl::Tlm::TlmTask_if_t(modulename, id, n, proxies),
        taskId(id)
{
    SC_THREAD(writingProcess);
}

CentralTask::~CentralTask() throw ()
{
    // Nothing to do.
}

void CentralTask::writingProcess()
{
        while (true)
        {

                if (!acks.empty())//Se ci sono degli ack da inviare
                {
                        Ack_t received = acks.front();
                        acks.pop();
                        tlm_taskproxy_id_t tp = 0;

                        Ack_t * a = static_cast<Ack_t *>(malloc(sizeof(Ack_t) * sizeof(a)));
                        a->sourceId = received.sourceId;
                        a->targetId = received.targetId;
                        a->seqNumber = received.seqNumber;
                        a->type = received.type;

                        /*std::cout << "[" << name() << "] SENDING ";
                        std::cout << "\t";
                        std::cout << "\tTo " << a->targetId;
                        std::cout << "\tACK ";
                        std::cout << "\tSize " << sizeof(Ack_t);
                        std::cout << "\t  Sequence Number " << a->seqNumber;
                        std::cout << "\t(" << sc_core::sc_time_stamp() << ")" << std::endl;*/

                        TlmTask_if_t::send(tp, reinterpret_cast<byte_t *>(a), sizeof(Ack_t));
                }
                if (payloads.empty())
                {
                        wait(1, sc_core::SC_MS);
                }
                else
                {
                        // Retrive the payload.
                        Payload_t received = payloads.front();
                        // Pop the payload from the queue.
                        payloads.pop();
                        tlm_taskproxy_id_t tp = 0;

                        Payload_t * p = static_cast<Payload_t *>(malloc(sizeof(Payload_t) * sizeof(p)));
                        p->sourceId = received.sourceId;
                        p->targetId = received.targetId;
                        p->data = received.data;
                        p->seqNumber = received.seqNumber;

                        if (p->targetId == 1)
                        {
                                // The p has been sended to MyTask
                                tp = 0;
                        }
                        else if (p->targetId == 3)
                        {
                                // The p has been sended to MyTask2
                                tp = 1;
                        }
                        else
                        {
                                std::cerr << "Wrong destination id (" << p->targetId << ").\n";
                                continue;
                        }

                                /*std::cout << "[" << name() << "] SENDING ";
                                std::cout << "\t";
                                std::cout << "\tTo " << p->targetId;
                                std::cout << "\tData " << p->data;
                                std::cout << "\tSize " << sizeof(Payload_t);
                                std::cout << "\t  Sequence Number " << p->seqNumber;
                                std::cout << "\t(" << sc_core::sc_time_stamp() << ")" << std::endl;*/

                                TlmTask_if_t::send(tp, reinterpret_cast<byte_t *>(p), sizeof(Payload_t));
                                wait(50, sc_core::SC_MS);
                        }
        }
}

void CentralTask::b_transport(tlm::tlm_generic_payload & payload, sc_core::sc_time & t)
{
    if (payload.get_command() == Scnsl::Tlm::tlm_command_t(Scnsl::Tlm::PACKET_COMMAND))
    {
        Payload_t * p = reinterpret_cast<Payload_t *>(payload.get_data_ptr());
        //Sul pacchetto che arriva faccio un controllo per verificare se si tratta di un payload normale o di un ack
        if (p->type == 'a')
        {
                Ack_t * ackC =  reinterpret_cast<Ack_t *>(payload.get_data_ptr());
                /*std::cout << "[" << name() << "] RECEIVED ";
                std::cout << "\tFrom " << ackC->sourceId;
                std::cout << "\t";
                std::cout << "\tACK ";
                std::cout << "\tSize " << sizeof(Ack_t);
                std::cout << "\t  Sequence Number " << ackC->seqNumber;
                std::cout << "\t(" << sc_core::sc_time_stamp() << ")" << std::endl;*/
                acks.push(*ackC);
        }else{
                /*std::cout << "[" << name() << "] RECEIVED ";
                std::cout << "\tFrom " << p->sourceId;
                std::cout << "\t";
                std::cout << "\tData:" << p->data;
                std::cout << "\tSize " << sizeof(Payload_t);
                std::cout << "\t  Sequence Number " << p->seqNumber;
                std::cout << "\t(" << sc_core::sc_time_stamp() << ")" << std::endl;*/
                payloads.push(*p);
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
