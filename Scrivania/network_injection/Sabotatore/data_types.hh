// Copyright (C) 2008
// by all contributors..

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


#ifndef SCNSL_CORE_DATA_TYPES_HH
#define SCNSL_CORE_DATA_TYPES_HH

#include <map>
#include <utility>

#include <systemc>
#include<tlm.h>

#include "../scnslConfig.hh"


namespace Scnsl { namespace Core {

struct data_extension_t;
class RadiationPattern_if_t;

} } // Scnsl::Core


namespace tlm {

/// @brief Tirck to allow exporting of static member ID.
template<>
class SCNSL_EXPORT tlm_extension<Scnsl::Core::data_extension_t> :
        public tlm_extension_base
{
public:
    tlm_extension() = default;
    tlm_extension(const tlm_extension & other) = default;
    virtual ~tlm_extension();
    virtual tlm_extension_base* clone() const = 0;
    virtual void copy_from(tlm_extension_base const &ext) = 0;
    const static unsigned int ID;
};

} // tlm

namespace Scnsl { namespace Core {

class Node_t;
class Channel_if_t;

} }


namespace Scnsl { namespace Core {

/// @name General types.
//@{

/// @brief The size type.
typedef unsigned int size_t;

/// @brief The counter type.
typedef unsigned int counter_t;

/// @brief A byte type.
typedef unsigned char byte_t;

/// @brief An error code type.
/// By convention, 0 means no error.
/// Other codes mean some error.
typedef unsigned int errorcode_t;

/// @brief The identifier number.
typedef int IdentifierNumber_t;

//@}


/// @brief General simulation types.
//@{

/// @brief A node ID.
typedef unsigned int node_id_t;

/// @brief A node properties ID.
typedef unsigned int node_properties_id_t;

/// @brief A task ID.
typedef unsigned int task_id_t;

/// @brief The carrier flag type.
typedef bool carrier_t;

/// @brief A packet ID.
typedef unsigned int packet_id_t;

/// @brief A label type for packets.
typedef unsigned int label_t;

/// @brief A position coordinate type.
typedef double position_t;

/// @brief The bitrate type.
typedef double bitrate_t;

/// @brief A delay time.
typedef sc_core::sc_time delay_t;

/// @brief The transmission power type.
typedef double power_t;

/// @brief The propagation speed.
typedef double propagation_t;

/// @brief The event ID.
typedef unsigned int event_id_t;

/// @brief The routing table type.
typedef
std::map< Node_t *, std::pair < Channel_if_t *, Node_t * > > RoutingTable_t;

/// @brief The distance between node
typedef double distance_t;

typedef std::vector<RadiationPattern_if_t*> Lobes;

/// @brief Node properties.
/// Such properties are:
/// - Position (x,y,z) in meters.
/// - Transmission bitrate.
/// - Transmission power in Watt.
/// - Receiving threshold in Watt.
/// - Vector of lobes used by directional nodes.
///
struct SCNSL_EXPORT node_properties_t
{
    /// @brief The node properties id.
    node_properties_id_t id;
    /// @brief The source node.
    Node_t * sourceNode;
    /// @brief The x coordinate (in meters).
    position_t x;
    /// @brief The y coordinate (in meters).
    position_t y;
    /// @brief The z coordinate (in meters).
    position_t z;
    /// @brief The transmission bitrate (in bit/sec).
    bitrate_t bitrate;
    /// @brief The transmission power (in Watt).
    power_t transmission_power;
    /// @brief The receiving threshold (in Watt).
    power_t receiving_threshold;
    /// @brief The lobes composing the radiation pattern
    Lobes lobes;

    /// @brief Empty struct constructor
    node_properties_t();

    /// @brief Copy struct constructor
    node_properties_t(const node_properties_t& np);

    /// @brief Struct overriding of the operator=
    node_properties_t & operator=(const node_properties_t & np);
};

/// @brief Distances between nodes in meters and number of nodes for each coordinate used in mesh topology.
struct SCNSL_EXPORT node_factory_infos_t
{
    /// @brief The distance on x coordinate.
    distance_t nodeStepX;
    /// @brief The distance on y coordianate.
    distance_t nodeStepY;
    /// @brief The distance on z coordinate.
    distance_t nodeStepZ;
    /// @brief The number of nodes on x coordinate.
    counter_t nodeNumberX;
    /// @brief The number of nodes on y coordinate.
    counter_t nodeNumberY;
    /// @brief The number of nodes on z coordinate.
    counter_t nodeNumberZ;

    node_factory_infos_t();
};

/// @brief Data extension for tlm_generic_payload.
/// Contains:
/// - packet label
///
struct SCNSL_EXPORT data_extension_t:
        public tlm::tlm_extension< data_extension_t >
{
    data_extension_t();
    virtual ~data_extension_t();
    data_extension_t(const data_extension_t & other);
    data_extension_t & operator =(data_extension_t other);
    void swap(data_extension_t & other);

    tlm::tlm_extension_base * clone() const;

    virtual void copy_from( tlm::tlm_extension_base const &ext );

    /// @brief The packet label that represents the packet priority.
    label_t label;
    /// @brief The power sensed when receiving the packet.
    power_t power;
};

struct SCNSL_EXPORT saboteur_methods
{
    typedef double (*DoubleMethod)(const Scnsl::Core::node_properties_t & /*sp*/, const Scnsl::Core::node_properties_t & /*rp*/);
    typedef sc_core::sc_time (*TimeMethod)(const Scnsl::Core::node_properties_t & /*sp*/, const Scnsl::Core::node_properties_t & /*rp*/);

    DoubleMethod getPacketLossRate;
    DoubleMethod getBitErrorRate;
    TimeMethod getDelay;
};

//@}

} }

#include "RadiationPattern_if_t.hh"

#endif

