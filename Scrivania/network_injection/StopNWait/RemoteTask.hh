#ifndef SCNSL__MY_TASK2_HH
#define SCNSL__MY_TASK2_HH

/// @file
/// Source node.

#include "defines.hh"

class RemoteTask: public Scnsl::Tlm::TlmTask_if_t
{
    public:
        SC_HAS_PROCESS (RemoteTask);
        /// @brief Constructor.
        ///
        /// @param name This module name.
        /// @param id this module unique ID.
        /// @param n The node on which this task is placed.
        /// @param proxies The number of connected task proxies.
        /// @throw std::invalid_argument If proxies is zero.
        ///
        RemoteTask(sc_core::sc_module_name name,
            const task_id_t id,
            Scnsl::Core::Node_t * n,
            const size_t proxies);

        virtual ~RemoteTask() throw ();

        virtual void b_transport(tlm::tlm_generic_payload & payload, sc_core::sc_time & t);

    protected:
        /// The id of this task.
        task_id_t taskId;
        //variabile per tenere traccia del SN dei pacchetti che devono arrivare
        int nextSN = 0;
        //variabile usata per controllare la fase di invio dell'ack
        bool stop = false;

        /// @brief Processes.
        void writingProcess();

    private:
        RemoteTask(RemoteTask &);
        RemoteTask & operator =(RemoteTask &);
};
#endif
