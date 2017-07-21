#ifndef SCNSL__MY_TASK_C_HH
#define SCNSL__MY_TASK_C_HH

/// @file
/// Central node.

#include "defines.hh"
#include "gbn_manager.hh"

class CentralTask: public Scnsl::Tlm::TlmTask_if_t
{
    public:
        SC_HAS_PROCESS( CentralTask );
        /// @brief Constructor.
        ///
        /// @param name This module name.
        /// @param id this module unique ID.
        /// @param n The node on which this task is placed.
        /// @param proxies The number of connected task proxies.
        /// @throw std::invalid_argument If proxies is zero.
        ///
        CentralTask(sc_core::sc_module_name name,
            const task_id_t id,
            Scnsl::Core::Node_t * n,
            const size_t proxies);

        virtual ~CentralTask() throw ();

        virtual void b_transport(tlm::tlm_generic_payload & payload, sc_core::sc_time & t);

        GoBackNManager manager;


        protected:
            /// The id of this task.
            task_id_t taskId;

            /// @brief Processes.
            void writingProcess();
            //ESEMPIO funzione del nodo che invia
            //void temperatureProcess();

        private:
                CentralTask(CentralTask &);
                CentralTask & operator =(CentralTask &);
};
#endif
