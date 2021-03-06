#ifndef SNW_MANAGER_HH
#define SNW_MANAGER_HH

#include "defines.hh"

class StopNWaitManager
{
public:
    /// The id of the destination node.
    uint32_t destination;

    // ---------------------------------------------------------------
    // SNW Variables used to Send.

    /// Structure used to store information about a transmitted packet.
    struct PacketStatusInformation
    {
        char seqNToData;
        sc_core::sc_time sendingTime;
        sc_core::sc_time ackReceivedTime;
    } PacketStatus;

    /// The ID of the task to which this is associated.
    uint32_t taskId;
    /// Numero di sequenza che verrà assegnato ai pacchetti inviati
    int seqNumber = 0;
    /// Variabile utilizzata per tenere traccia degli ack
    int nextAck = 0;
    /// Variabile per indicare che si è verificato un timeout
    bool timeoutOccured = false;
    /// Variabile settata acknowledge true, usata per gestire l'invio di pacchetti
    bool sending = true;
    /// Pacchetto utilizzato per l'invio dei dati.
    Payload_t * packet;
    /// Pacchetto utilizzato per l'invio degli ack.
    Payload_t * acknowledge;

    // ---------------------------------------------------------------
    // SNW Variables used to Receive.
    /// Variabile per tenere traccia del SN dei pacchetti che devono arrivare
    int nextSN = 0;
    /// Variabile usata per controllare la fase di invio dell'ack
    bool stop = false;
    /// Pacchetto utillizzato per la ricezione.
    Payload_t * ack;

    /// Routing table.
    std::map<uint32_t, uint32_t> routing;
    /// The queue of packets.
    std::queue<Payload_t> payloads;
    /// The queue of acks
    std::queue<Payload_t> acks;

    /// @brief Constructor.
    StopNWaitManager(const uint32_t & _taskId);

    /// @brief Destructor.
    ~StopNWaitManager();

    Payload_t * handlePacketToSend(
        uint32_t &          task_proxy,
        sc_core::sc_time    timeOut,
        const char *        nome);

    void handlePacketReceive(
        const char *        nome,
        Payload_t *         pack);

};
#endif
