
#include <sstream>
#include <systemc>
#include <tlm.h>
#include <exception>

#include <scnsl.hh>
#include "MyTask_t.hh"



using namespace Scnsl;
using namespace Scnsl::Core;
using namespace Scnsl::BuiltinPlugin;
using namespace Scnsl::Utils;
using namespace Scnsl::Setup;
using namespace std;
using namespace Scnsl::Tlm;
using Scnsl::Tracing::Traceable_base_t;

int sc_main( int argc, char * argv[] )
{
	try {

        // Which testcase? (from 1 to 3)
        unsigned int TESTCASE = 0;
        if ( argc == 2 )
        {
            std::stringstream ss;
            ss << argv[ 1 ];
            ss >> TESTCASE;
        }
        if ( TESTCASE > 3 || TESTCASE < 1 )
        {
            TESTCASE = 1;
        }

        // Singleton.
        Scnsl::Setup::Scnsl_t * scnsl = Scnsl::Setup::Scnsl_t::get_instance();

        // Nodes creation:
        Scnsl::Core::Node_t * n0 = scnsl->createNode();
        Scnsl::Core::Node_t * n1 = scnsl->createNode();
        Scnsl::Core::Node_t * n2 = scnsl->createNode();


        CoreChannelSetup_t ccs;

        ccs.extensionId = "core";
        ccs.channel_type = CoreChannelSetup_t::SHARED;
        ccs.name = "SharedChannel";
        ccs.alpha = 0.1;
        ccs.delay = sc_core::sc_time( 1.0, sc_core::SC_US );
        ccs.nodes_number = 2;

        Scnsl::Core::Channel_if_t * ch = scnsl->createChannel( ccs );
        
         CoreChannelSetup_t ccs2;

        ccs2.extensionId = "core";
        ccs2.channel_type = CoreChannelSetup_t::SHARED;
        ccs2.name = "SharedChannel2";
        ccs2.alpha = 0.1;
        ccs2.delay = sc_core::sc_time( 1.0, sc_core::SC_US );
        ccs2.nodes_number = 2;

        Scnsl::Core::Channel_if_t * ch2 = scnsl->createChannel( ccs2 );
        
        
        Scnsl::Utils::DefaultEnvironment_t::createInstance(ccs.alpha);

        const bool IS_SENDER = true;
        const Scnsl::Core::task_id_t id0 = 0;
        const Scnsl::Core::task_id_t id1 = 1;
        const Scnsl::Core::task_id_t id2 = 2;
        const Scnsl::Core::size_t PROXIES = 1;

        MyTask_t t0( "Task0", IS_SENDER, TESTCASE, id0, n0, 2 );
        MyTask_t t1( "Task1", ! IS_SENDER, TESTCASE, id1, n1, PROXIES );
        MyTask_t t2( "Task2", ! IS_SENDER, TESTCASE, id2, n2, PROXIES );

        // Creating the protocol 802.15.4:
        CoreCommunicatorSetup_t ccoms;
        ccoms.extensionId = "core";
        ccoms.ack_required = true;
        ccoms.short_addresses = true;
        ccoms.type = CoreCommunicatorSetup_t::MAC_802_15_4;
        // First instance:
        ccoms.name = "Mac0";
        ccoms.node = n0;
        Scnsl::Core::Communicator_if_t * mac0 = scnsl->createCommunicator( ccoms );
        // Second instance:
        ccoms.name = "Mac1";
        ccoms.node = n1;
        Scnsl::Core::Communicator_if_t * mac1 = scnsl->createCommunicator( ccoms );
        // Second instance:
        ccoms.name = "Mac12";
        ccoms.node = n2;
        Scnsl::Core::Communicator_if_t * mac2 = scnsl->createCommunicator( ccoms );
          // Second instance:
        ccoms.name = "Mac3";
        ccoms.node = n0;
        Scnsl::Core::Communicator_if_t * mac3 = scnsl->createCommunicator( ccoms );
        // SABOTEUR
         CoreCommunicatorSetup_t ccoms1;
        ccoms1.extensionId = "core";
        ccoms1.ack_required = true;
        ccoms1.short_addresses = true;
        ccoms1.type = CoreCommunicatorSetup_t::SABOTEUR;
        
         // SABOTEUR
         CoreCommunicatorSetup_t ccoms2;
        ccoms2.extensionId = "core";
        ccoms2.ack_required = true;
        ccoms2.short_addresses = true;
        ccoms2.type = CoreCommunicatorSetup_t::SABOTEUR;
        // First instance:
        ccoms1.name = "Sab1";
        ccoms1.node = n1;
            // First instance:
        ccoms2.name = "Sab2";
        ccoms2.node = n2;
         Scnsl::Core::Communicator_if_t * sab0 = scnsl->createCommunicator( ccoms1 );
        Scnsl::Core::Communicator_if_t * sab1 = scnsl->createCommunicator( ccoms2 );
        mac1->stackDown(sab0);
        sab0->stackUp(mac1);
        Communicator_if_t * macStack = new Scnsl::Utils::CommunicatorStack_t(mac1, sab0);
        
        mac2->stackDown(sab1);
        sab1->stackUp(mac2);
        Communicator_if_t * macStack1 = new Scnsl::Utils::CommunicatorStack_t(mac2, sab1);
        

        // Adding tracing features:
        CoreTracingSetup_t cts;
        cts.extensionId = "core";
        // - Setting the formatter:
        cts.formatterExtensionId = "core";
        cts.formatterName = "basic";
        // Setting the filter:
        cts.filterExtensionId = "core";
        cts.filterName = "basic";
        // Core formatter specific option:
        // printing also the type of trace:
        cts.print_trace_type = true;
        // - Setting to trace only user-like infos:
        cts.info = 5;
        cts.debug = 0;
        cts.log = 5;
        cts.error = 0;
        cts.warning = 0;
        cts.fatal = 0;
        // - Creating:
        Scnsl_t::Tracer_t * tracer1 = scnsl->createTracer( cts );
        // - Setting the output stream:
        tracer1->addOutput( & std::cout );
        // - Adding to trace:
        //tracer1->trace( & t0 );
        //tracer1->trace( & t1 );
        //tracer1->trace( ch );
        tracer1->trace( dynamic_cast<Traceable_base_t*>(sab0) );
        tracer1->trace( dynamic_cast<Traceable_base_t*>(sab1) );
        //tracer1->trace( dynamic_cast<Traceable_base_t*>(mac1) );
        // - Setting to trace backend-like infos:
        cts.info = 0;
        cts.debug = 5;
        cts.log = 0;
        cts.error = 5;
        cts.warning = 5;
        cts.fatal = 5;
        // - Creating:
       // Scnsl_t::Tracer_t * tracer2 = scnsl->createTracer( cts );
        // - Setting the output stream:
       // tracer2->addOutput( & std::cerr );
        // - Adding to trace:
       // tracer2->trace( & t0 );
       // tracer2->trace( & t1 );
       // tracer2->trace( ch );
       // tracer2->trace( Scnsl::Utils::EventsQueue_t::get_instance() );
       // tracer2->trace( dynamic_cast<Traceable_base_t*>(mac0) );
       // tracer2->trace( dynamic_cast<Traceable_base_t*>(mac1) );


        // Binding:
        BindSetup_base_t bsb0;
        bsb0.extensionId = "core";
        bsb0.destinationNode = n1;
        bsb0.node_binding.x = 0;
        bsb0.node_binding.y = 0;
        bsb0.node_binding.z = 0;
        bsb0.node_binding.bitrate = Scnsl::Protocols::Mac_802_15_4::BITRATE;
        bsb0.node_binding.transmission_power = 100;
        bsb0.node_binding.receiving_threshold = 10;
        
                BindSetup_base_t bsb3;
        bsb3.extensionId = "core";
        bsb3.destinationNode = n1;
        bsb3.node_binding.x = 1;
        bsb3.node_binding.y = 0;
        bsb3.node_binding.z = 1;
        bsb3.node_binding.bitrate = Scnsl::Protocols::Mac_802_15_4::BITRATE;
        bsb3.node_binding.transmission_power = 100;
        bsb3.node_binding.receiving_threshold = 10;

        
        scnsl->bind( n0, ch, bsb0 );
        
        scnsl->bind( & t0, & t1, ch, bsb0, mac0 );
        
        scnsl->bind( n0, ch2, bsb3 );
        
        scnsl->bind( & t0, & t2, ch2, bsb3, mac3 );
        

        BindSetup_base_t bsb1;
        bsb1.extensionId = "core";
        bsb1.destinationNode = NULL; // don't care: it receives only...
        bsb1.node_binding.x = 1;
        bsb1.node_binding.y = 1;
        bsb1.node_binding.z = 1;
        bsb1.node_binding.bitrate = Scnsl::Protocols::Mac_802_15_4::BITRATE;
        bsb1.node_binding.transmission_power = 1000;
        bsb1.node_binding.receiving_threshold = 1;

        scnsl->bind( n1, ch, bsb1 );
        scnsl->bind( & t1, & t0, ch, bsb1, macStack );
        
          BindSetup_base_t bsb2;
        bsb2.extensionId = "core";
        bsb2.destinationNode = NULL; // don't care: it receives only...
        bsb2.node_binding.x = 1;
        bsb2.node_binding.y = 0;
        bsb2.node_binding.z = 1;
        bsb2.node_binding.bitrate = Scnsl::Protocols::Mac_802_15_4::BITRATE;
        bsb2.node_binding.transmission_power = 1000;
        bsb2.node_binding.receiving_threshold = 1;
        
         scnsl->bind( n2, ch2, bsb2 );
        scnsl->bind( & t2, & t0, ch2, bsb2, macStack1 );


        sc_core::sc_start( sc_core::sc_time( 2000, sc_core::SC_MS ) );
        sc_core::sc_stop();
	}
    catch ( std::exception & e)
    {
        std::cerr << "Exception in sc_main(): " << e.what() << std::endl;
        return 1;
	}
	return 0;
}
