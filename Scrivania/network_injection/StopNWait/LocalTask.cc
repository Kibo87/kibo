/// @file
/// A TLM task.

#include "LocalTask.hh"

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
        //Timeout fissato a 341600 us
        sc_core::sc_time timeOut (341600, sc_core::SC_US);
        //Creo il pacchetto che verrà inizializzato più avanti, al momente dell'invio
        Payload_t * p = static_cast<Payload_t *>(malloc(sizeof(Payload_t) * sizeof(p)));

        while (true)
        {
                //Condizione per verificare la condizione di timeout
                if(sc_core::sc_time_stamp() > (PacketStatus.sendingTime + timeOut))
                {

                        timeoutOccured = true;

                        std::cout << "\n##### TIMEOUT " << sc_core::sc_time_stamp() << " ############" << std::endl;
                        std::cout << "##### resend packet with SN : " << p->seqNumber << " #####" << std::endl;
                        //Stampo le informazione del pacchetto che devo reinviare
                        std::cout << "\n" << std::endl;
                        std::cout << "[" << name() << "]   SENDING ";
                        std::cout << "\t";
                        std::cout << "\tTo " << p->targetId;
                        std::cout << "\tData " << p->data;
                        std::cout << "\tSize " << sizeof(Payload_t);
                        std::cout << "\t  Sequence Number " << p->seqNumber;
                        std::cout << "\t(" << sc_core::sc_time_stamp() << ")" << std::endl;
                        PacketStatus.sendingTime = sc_core::sc_time_stamp();
                        const tlm_taskproxy_id_t tp = 0;
                        TlmTask_if_t::send(tp, reinterpret_cast<byte_t *>(p), sizeof(Payload_t));

                }
                if(sending == true && timeoutOccured == false)//Se ho ricevuto l'ack allora posso inviare un nuovo pacchetto
                {
                        //Inizializzo il pacchetto
                        p->type = 'p';
                        p->sourceId = taskId;
                        p->targetId = 3;
                        p->data = '!' + (rand() % 14);
                        p->seqNumber = seqNumber;

                        std::cout << "\n" << std::endl;
                        std::cout << "[" << name() << "]   SENDING ";
                        std::cout << "\t";
                        std::cout << "\tTo " << p->targetId;
                        std::cout << "\tData " << p->data;
                        std::cout << "\tSize " << sizeof(Payload_t);
                        std::cout << "\t  Sequence Number " << p->seqNumber;
                        std::cout << "\t(" << sc_core::sc_time_stamp() << ")" << std::endl;

                        PacketStatus.seqNToData = p->data;
                        PacketStatus.seqNToAcked = false;
                        PacketStatus.sendingTime = sc_core::sc_time_stamp();
                        //Numero di sequenza del prossimo pacchetto da inviare
                        seqNumber++;
                        const tlm_taskproxy_id_t tp = 0;
                        TlmTask_if_t::send(tp, reinterpret_cast<byte_t *>(p), sizeof(Payload_t));
                        sending = false;

                }
                wait(100, sc_core::SC_US);
        }
}

void LocalTask::b_transport(tlm::tlm_generic_payload & payload, sc_core::sc_time & t)
{

        if (payload.get_command() == Scnsl::Tlm::tlm_command_t(Scnsl::Tlm::PACKET_COMMAND))
        {
                Ack_t * a =  reinterpret_cast<Ack_t *>(payload.get_data_ptr());//Recupero il pacchetto di ack
                if(a->seqNumber == nextAck )
                {
                        std::cout << "[" << name() << "]   RECEIVED ";
                        std::cout << "\tFrom" << a->sourceId;
                        std::cout << "\t";
                        std::cout << "\tACK ";
                        std::cout << "\tSize " << sizeof(Ack_t);
                        std::cout << "\t  Sequence Number " << a->seqNumber;
                        std::cout << "\t(" << sc_core::sc_time_stamp() << ")" << std::endl;
                        //Memorizzo il tempo di ricezione dell'ack
                        PacketStatus.ackReceivedTime = sc_core::sc_time_stamp();
                        std::cout << "\n                        RTT : " << PacketStatus.ackReceivedTime - PacketStatus.sendingTime << "\n" << std::endl;
                        nextAck++;
                        sending = true;//Riabilito invio dei pacchetti
                }
                else//Nel caso in cui arrivasse un pacchetto duplicato non viene gestito
                {
                        timeoutOccured = false;
                        std::cout << "\n                RTT : " << sc_core::sc_time_stamp() - PacketStatus.sendingTime << "\n" << std::endl;
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
