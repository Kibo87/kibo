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


#ifndef SCNSL_PROTOCOLS_Saboteur_T_HH
#define SCNSL_PROTOCOLS_Saboteur_T_HH

/// @file
/// The routing protocol communicator..

#include <cstdlib>
#include "../../scnslConfig.hh"
#include "../../core/data_types.hh"
#include "../../core/Communicator_if_t.hh"
#include "../../core/Node_t.hh"
#include "../../core/Packet_t.hh"
#include "../../core/Channel_if_t.hh"
#include "../../utils/Environment_if_t.hh"
#include "../../tracing/Traceable_base_t.hh"


namespace Scnsl { namespace Protocols { namespace Saboteur {

    class SCNSL_EXPORT Saboteur_t:
        public Scnsl::Core::Communicator_if_t,
        public Scnsl::Tracing::Traceable_base_t
        {
        public:

            typedef Scnsl::Core::Packet_t::byte_t byte_t;
            typedef Scnsl::Core::Packet_t Packet_t;
            typedef Scnsl::Core::Channel_if_t Channel_if_t;
            typedef Scnsl::Core::Node_t Node_t;
            typedef Scnsl::Core::Node_t::node_properties_t node_properties_t;
            typedef Scnsl::Utils::Environment_if_t Environment_if_t;
            typedef double (*bitErrorRateFunction_t)(
                const node_properties_t & sp, const node_properties_t & rp);

            /// @brief Constructor.
            ///
            /// @param name The name of the communicator.
            /// @param function The bit error rate function.
            ///
            Saboteur_t(const char* name, bitErrorRateFunction_t function);

            /// @brief Virtual destructor.
            virtual
            ~Saboteur_t();

            /// @name Communicator interface reimplemented methods.
            //@{

            virtual
            errorcode_t send( const Packet_t & p );

            virtual
            errorcode_t receive( const Packet_t & p );

            //@}

        protected:

            // Last error rate
            double _bitErrorRate;
            // Index of the next bit error
            size_t _nextError;
            // Error bit rate function
            bitErrorRateFunction_t _getBitErrorRate;

        private :

            /// @brief Disabled copy constructor.
            Saboteur_t( const Saboteur_t & );

            /// @brief Disabled assignement operator.
            Saboteur_t & operator = ( const Saboteur_t & );

            /// @brief The default bit error rate function.
            ///
            /// The communicator uses this function if the user does not
            /// provide any bit error rate function.
            ///
            /// @param sp source node properties.
            /// @param rp destination node properties.
            /// @return the bit error rate
            ///
            static double _bitErrorRateDefaultFunction(
                const node_properties_t & sp, const node_properties_t & rp);

            Scnsl::Core::Packet_t::byte_t _changeBit( byte_t byte, size_t index);

            int _getBit(unsigned char data, int i);
};

} } }

#endif
