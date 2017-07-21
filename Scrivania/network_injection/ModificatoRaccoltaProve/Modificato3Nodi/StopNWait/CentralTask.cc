/// @file
/// A TLM task.

#include "CentralTask.hh"

CentralTask::CentralTask(   sc_core::sc_module_name modulename,
                            const task_id_t id,
                            Scnsl::Core::Node_t * n,
                            const size_t proxies) :
        Scnsl::Tlm::TlmTask_if_t(modulename, id, n, proxies),
        manager(id),
        taskId(id)
{
        SC_THREAD(writingProcess);
        //SC_THREAD(temperatureProcess);
}

CentralTask::~CentralTask() throw ()
{
    // Nothing to do.
}

/*void CentralTask::temperatureProcess()
{
    int seqNum =0;

    while (true)
    {
        Payload_t * p = static_cast<Payload_t *>(malloc(sizeof(Payload_t) * sizeof(p)));

        //Inizializzo il pacchetto
        p->type = 'p';
        p->sourceId = taskId;
        p->lastSender = taskId;
        p->targetId = 1; //Scelta destinatario
        p->data = '!' + (rand() % 14);
        p->seqNumber = seqNum;

        seqNum++;

        manager.payloads.push(*p);

        wait(100000, sc_core::SC_US);

    }
}*/

void CentralTask::writingProcess()
{
    //Timeout fissato a 341600 us poi messo 392400
    sc_core::sc_time timeOut (390000, sc_core::SC_US);
    while (true)
    {
        // Crea una variable che verrà settata con il task proxy al quale
        //  dovrò inviare il pacchetto.
        uint32_t task_proxy = 0;
        // Call the handler.
        Payload_t * packet = manager.handlePacketToSend(task_proxy, timeOut, this->name());
        // Se mi viene ritornato un pacchetto da inviare, invialo.
        if(packet != NULL)
        {
            TlmTask_if_t::send(task_proxy, reinterpret_cast<byte_t *>(packet), sizeof(Payload_t));
        }
        wait(50, sc_core::SC_US);
    }
}

void CentralTask::b_transport(tlm::tlm_generic_payload & payload, sc_core::sc_time & t)
{
    if (payload.get_command() == Scnsl::Tlm::tlm_command_t(Scnsl::Tlm::PACKET_COMMAND))
    {
        // Ricevo il pacchetto e lo casto a Payload.
        Payload_t * pack = reinterpret_cast<Payload_t *>(payload.get_data_ptr());
        // Call the handler.
        manager.handlePacketReceive(this->name(), pack);
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
