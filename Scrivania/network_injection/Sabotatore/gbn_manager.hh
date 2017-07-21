#ifndef GBN_MANAGER_HH
#define GBN_MANAGER_HH

#include "defines.hh"

class GoBackNManager
{
private:
    /// Structure used to store information about a transmitted packet.
    class PacketStatusInformation
    {
    public:
        PacketStatusInformation():
            seqNToData(),
            sendingTime(),
            ackReceivedTime()
        {
            //NTD
        }

        PacketStatusInformation(const PacketStatusInformation & other):
            seqNToData(other.seqNToData),
            sendingTime(other.sendingTime),
            ackReceivedTime(other.ackReceivedTime)
        {
            //NTD
        }

        char seqNToData;
        sc_core::sc_time sendingTime;
        sc_core::sc_time ackReceivedTime;
    };

public:
    /// The ID of the task to which this is associated.
    uint32_t taskId;

    // ------------------------------------------------------------------------
    // GBN Variables SEND
    /// The id of the destination node.
    uint32_t destination;
    // Dimensione della finestra di invio
    int window_size;
    /// Mappa per assocciare le caratteristiche del pacchetto al corrispondente SN
    std::map<int, PacketStatusInformation> statusMap;
    /// Variabile contenente il SN del più vecchio pacchetto non confermato
    int base;
    /// Variabile contenente il SN del prossimo pacchetto da trasmettere
    int nextSN;
    /// Variabile contenente il numero del prossimo ack atteso
    int nextAck;
    /// Variabile temporanea utilizzata nella procedura per gestire il reinvio della finestra di pacchetti
    int nextSNTemp;
    /// Massimo pacchetto che ho da spedire
    int maxSN;
    /// Pacchetto utilizzato per l'invio dei dati.
    Payload_t * packet;
    /// Pacchetto utilizzato per l'invio degli ack.
    Payload_t * acknowledge;

    // ------------------------------------------------------------------------
    // GBN Variables RECEIVE
    /// Pacchetto utillizzato per la ricezione.
    Payload_t * ack;
    /// Variabile contenente il numero del prossimo pacchetto atteso
    int nextSNack;

    // ------------------------------------------------------------------------
    /// The queue of packets.
    std::queue<Payload_t> payloads;
    /// The queue of acks
    std::queue<Payload_t> acks;

    // ------------------------------------------------------------------------
    // Routing table.
    std::map<uint32_t, uint32_t> routing;

    /// @brief Constructor.
    GoBackNManager(const uint32_t & _taskId);

    /// @brief Destructor.
    ~GoBackNManager();

    Payload_t * handlePacketToSend(uint32_t & task_proxy, sc_core::sc_time timeOut, const char* nome);

    void handlePacketReceive(const char* nome, Payload_t * pack, sc_core::sc_time timeOut);

};
#endif
