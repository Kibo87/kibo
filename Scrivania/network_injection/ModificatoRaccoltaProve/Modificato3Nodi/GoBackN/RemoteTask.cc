/// @file
/// A TLM task.

#include "RemoteTask.hh"

RemoteTask::RemoteTask(
    sc_core::sc_module_name modulename,
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

RemoteTask::~RemoteTask() throw ()
{
    // Nothing to do.
}

#if 0
void RemoteTask::temperatureProcess()
{
    int seqNum =1;

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

        wait(50000, sc_core::SC_US);

    }
}
#endif

void RemoteTask::writingProcess()
{
    sc_core::sc_time timeOut (600000, sc_core::SC_US);

    while (true)
    {

        uint32_t task_proxy = 0;
        const char* nome = name();
        Payload_t * packet = manager.handlePacketToSend(task_proxy, timeOut, nome);
        if(packet != NULL)
        {
            TlmTask_if_t::send(task_proxy, reinterpret_cast<byte_t *>(packet), sizeof(Payload_t));
        }

        wait(50, sc_core::SC_US);
    }

}

void RemoteTask::b_transport(tlm::tlm_generic_payload & payload, sc_core::sc_time & t)
{

    if (payload.get_command() == Scnsl::Tlm::tlm_command_t(Scnsl::Tlm::PACKET_COMMAND))
    {

        sc_core::sc_time timeOut (600000, sc_core::SC_US);

        const char* nome = name();
        Payload_t * pack = reinterpret_cast<Payload_t *>(payload.get_data_ptr());
        manager.handlePacketReceive(nome, pack, timeOut);

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
