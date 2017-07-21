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

                //Non serve tenere traccia dei pacchetti passati
                //std::map<int, PacketStatusInformation> StatusMap;


        protected:
                /// The id of this task.
                task_id_t taskId;
                //Numero di sequenza che verrà assegnato ai pacchetti inviati
                int seqNumber = 0;
                //Variabile utilizzata per tenere traccia degli ack
                int nextAck = 0;
                //variabile per indicare che si è verificato un timeout
                bool timeoutOccured = false;
                //Variabile settata a true, usata per gestire l'invio di pacchetti
                bool sending = true;

                /// @brief Processes.
                void writingProcess();

        private:
                LocalTask(LocalTask &);
                LocalTask & operator =(LocalTask &);
};

#endif
