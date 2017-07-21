#ifndef SCNSL__MY_TASK_HH
#define SCNSL__MY_TASK_HH

/// @file
/// Source node.

#include "defines.hh"

class LocalTask: public Scnsl::Tlm::TlmTask_if_t
{
    public:
        SC_HAS_PROCESS( LocalTask );
        /// @brief Constructor.
        ///
        /// @param name This module name.
        /// @param id this module unique ID.
        /// @param n The node on which this task is placed.
        /// @param proxies The number of connected task proxies.
        /// @throw std::invalid_argument If proxies is zero.
        ///
        LocalTask(sc_core::sc_module_name name,
            const task_id_t id,
            Scnsl::Core::Node_t * n,
            const size_t proxies);

        virtual ~LocalTask() throw ();

        virtual void b_transport(tlm::tlm_generic_payload & p, sc_core::sc_time & t);

        private:
                struct PacketStatusInformation
                {
                        char seqNToData;
                        bool seqNToAcked;
                        sc_core::sc_time sendingTime;
                        sc_core::sc_time ackReceivedTime;
                } PacketStatus;
                //mappa per assocciare le caratteristiche del pacchetto al corrispondente SN
                std::map<int, PacketStatusInformation> StatusMap;
                //variabile contenente il SN del più vecchio pacchetto non confermato
                int base = 1;
                //variabile contenente il SN del prossimo pacchetto da trasmettere
                int nextSN = 1;

                int nextAck = 1;
                //Variabile settata a false, usata per gestire il reinvio di pacchetti
                bool resend = false;
                //variabile temporanea utilizzata nella procedura per gestire il reinvio della finestra di pacchetti
                int nextSNTemp = 1;

        protected:
        /// The id of this task.
                task_id_t taskId;

                int seqNumber;

        /// @brief Processes.
                void writingProcess();

        private:
                LocalTask(LocalTask &);
                LocalTask & operator =(LocalTask &);
};

#endif
