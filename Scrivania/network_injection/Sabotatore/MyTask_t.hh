// Copyright (C) 2008
// by all contributors.

// This file is part of SCNSL.

// SCNSL is free software:
// you can redistribute it and/or modify it under the terms of the
// GNU Lesser General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option)
// any later version.

// SCNSL is distributed in the hope
// that it will be useful, but WITHOUT ANY WARRANTY;
// without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.
// See the GNU Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public
// License along with SCNSL,
// in a file named LICENSE.txt.
// If not, see <http://www.gnu.org/licenses/>.


#ifndef MYTASK_T_HH
#define MYTASK_T_HH

#include <systemc>
#include <scnsl.hh>

/// @file
/// A simple TLM task.


/// @brief A simple TLM task.
///
/// Design patterns:
///
/// - Non copyable.
/// - Non assignable.
///
class MyTask_t:
    public Scnsl::Tlm::TlmTask_if_t
{
public:

    SC_HAS_PROCESS( MyTask_t );

    /// @brief Constuctor.
    ///
    /// @param modulename This module name.
    /// @param is_sender True if is sender.
    /// @param test The kind of test.
    /// @param id this task ID.
    /// @param n The relative host node.
    /// @param proxies The number of bounded proxies.
    ///
    MyTask_t( const sc_core::sc_module_name modulename,
              const bool is_sender,
              const task_id_t id,
              const unsigned int test,
              Scnsl::Core::Node_t * n,
              const size_t proxies )
    throw ();

    /// @brief Virtual destructor.
    virtual
    ~MyTask_t()
    throw ();

    /// @name Inherited interface methods.
    //@{


    virtual
    void b_transport( tlm::tlm_generic_payload & p, sc_core::sc_time & t );

    //@}

    /// @brief Signals when a packet is arrived.
    sc_core::sc_event _packetArrivedEvent;

private:

    /// @name Processes.
    //@{

    /// @brief Sender process.
    void _sender()
        throw ();

    /// @brief Sender with synchronization process.
    void _senderSynch()
        throw ();

    /// @brief Interference process.
    void _interference()
        throw ();

    /// @brief Receiver with synchronization process.
    void _receiverSynch()
        throw ();

    //@}

    /// @brief Disabled copy constructor.
    MyTask_t( const MyTask_t & );

    /// @brief Disabled assignment operator.
    MyTask_t & operator = ( MyTask_t & );
};



#endif

