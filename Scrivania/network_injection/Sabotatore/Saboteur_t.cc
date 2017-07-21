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

/// @file
///

#include "scnsl/protocols/saboteur/Saboteur_t.hh"

using namespace Scnsl::Protocols::Saboteur;

// ////////////////////////////////////////////////////////////////
// Constructor and destructor.
// ////////////////////////////////////////////////////////////////

Saboteur_t::Saboteur_t(const char* name, bitErrorRateFunction_t function):
    Scnsl::Tracing::Traceable_base_t( name ),
    _bitErrorRate( -1 ),
    _nextError( 0 ),
    _getBitErrorRate( function != nullptr? function : _bitErrorRateDefaultFunction)
{
    // Nothing to do.
}

Saboteur_t::~Saboteur_t()
{
    // Nothing to do.
}

// ////////////////////////////////////////////////////////////////
// Private methods.
// ////////////////////////////////////////////////////////////////

Scnsl::Core::Packet_t::byte_t Saboteur_t::_changeBit(byte_t b, size_t i)
{
    byte_t mask = static_cast<byte_t>(1 << i);
    return mask ^ b;
}

int Saboteur_t::_getBit(unsigned char data, int i)
{
    return (data >> i) & 0x01;
}

double Saboteur_t::_bitErrorRateDefaultFunction(const node_properties_t & sp,
                                                const node_properties_t & rp)
{

    Environment_if_t* env = Environment_if_t::getInstance();
    return env->getBitErrorRate(sp, rp);
}

// ////////////////////////////////////////////////////////////////
// Interface methods.
// ////////////////////////////////////////////////////////////////

Saboteur_t::errorcode_t Saboteur_t::send( const Packet_t & p)

{
    double new_value_delay = 0; // da 16000 fa 34
    if (new_value_delay > 0)
    {
        sc_core::wait(new_value_delay, sc_core::SC_US);
    }

    return Scnsl::Core::Communicator_if_t::send(p);
}

Saboteur_t::errorcode_t Saboteur_t::receive( const Packet_t & p)

{

// ////////////////////////////////////////////////////////////////
// Opzione attivazione Delay - controllare dove implementarlo perchè non blocchi il canale
// ////////////////////////////////////////////////////////////////

    double new_value_delay = 0; // da 16000 fa 34
    if (new_value_delay > 0)
    {
        sc_core::wait(new_value_delay, sc_core::SC_US);
    }

    const Channel_if_t *ch = p.getChannel();
    const Node_t *destNode = p.getDestinationNode();
    node_properties_t sp = p.getSourceProperties();
    node_properties_t rp = destNode->getProperties(ch);

    // BIT ERROR ALGORITHM
    double new_bit_error_rate = _getBitErrorRate(sp, rp);
    double dd = 2/_bitErrorRate;

    // if bit error changes or it is the thirst packet, reset index and parameter
    if(new_bit_error_rate < _bitErrorRate || new_bit_error_rate > _bitErrorRate)
    {
        _bitErrorRate = new_bit_error_rate;
        dd = 2/_bitErrorRate;
        size_t randd = static_cast<size_t>(rand()) % (static_cast<size_t>(dd)-1);
        _nextError = randd;

    }
    Packet_t injectedPacket(p);
    Packet_t* nestedPacket = &injectedPacket;

    while(!nestedPacket->hasBufferPayload())
    {
        nestedPacket = nestedPacket->getInnerPayload();
    }
    const size_t packetSize = nestedPacket->getPayloadSizeInBytes();

    byte_t * payload = nullptr;

    nestedPacket->getPayload(payload);

    std::stringstream ss0;
    ss0 << "ORIGINAL Packet: " << nestedPacket->getId();
    SCNSL_TRACE_LOG( 1, ss0.str().c_str() );

    std::stringstream ss1;
    for(int j=0;j < static_cast<int>(nestedPacket->getPayloadSizeInBytes());++j)
    {
        for (int i=0; i<8; ++i)
        {
            ss1 << _getBit(payload[j], i);
        }
    }

    SCNSL_TRACE_LOG( 1, ss1.str().c_str() );

    for (;;)
    {

        if (_nextError >= packetSize*8) break;

        std::stringstream ss;
        ss<< "CORRUPTING bit: " << _nextError << " of Packet: " <<
             nestedPacket->getId() << " with ERROR_BIT: " << _getBitErrorRate(sp, rp);
        SCNSL_TRACE_LOG( 1, ss.str().c_str() );


        // find byte and offset
        const size_t by = _nextError / 8;
        const size_t offset = _nextError % 8;
        byte_t byte = payload[by];
        byte_t corrupted_byte = _changeBit(byte, offset);
        payload[by] = corrupted_byte;

        size_t randd = static_cast<size_t>(rand()) % (static_cast<size_t>(dd)-1);
        _nextError = _nextError + 1 + randd;
    }

    _nextError = _nextError - (packetSize*8);


    std::stringstream ss2;
    ss2 << "CORRUPTED Packet: " << nestedPacket->getId();
    SCNSL_TRACE_LOG( 1, ss2.str().c_str() );

    std::stringstream ss3;
    for(int j=0;j < static_cast<int>(nestedPacket->getPayloadSizeInBytes());++j)
    {
        for (int i=0; i<8; ++i)
        {
            ss3 << _getBit(payload[j], i);

        }
    }
    SCNSL_TRACE_LOG( 1, ss3.str().c_str() );

// ////////////////////////////////////////////////////////////////
// definire quanti pacchetti vanno persi in percentuale
// ////////////////////////////////////////////////////////////////

    double new_packet_loss_rate = 0;
    double r = (rand() % 10) + 1;
    r=r/10;
    // nel caso vanno persi pacchetti
    if (new_packet_loss_rate  > 0 && r < new_packet_loss_rate)
    {
        return 0;
    }
    else
    {
        return Scnsl::Core::Communicator_if_t::receive(injectedPacket);
    }
}
