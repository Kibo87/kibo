#include <sstream>
#include <systemc>
#include <tlm.h>
#include <exception>

#include <scnsl.hh>

#include "CentralTask.hh"
#include "LocalTask.hh"

using namespace Scnsl::BuiltinPlugin;
using namespace Scnsl::Setup;
using Scnsl::Tracing::Traceable_base_t;

namespace /*anon*/ {

double inject(const Scnsl::Core::node_properties_t & /*sp*/, const Scnsl::Core::node_properties_t & /*rp*/)
{
    return 0.0001;
}

} // anon

// ////////  NETWORK SCENARIO  ////////////////////
//                                               //
//  **********                 **********        //
//  *        *                 *        *        //
//  *   N1   *<--------------->*   NC   *        //
//  *        *   FULLDUPLEX    *        *        //
//  **********    CHANNEL      **********        //
//                                               //
// ////////////////////////////////////////////////

int sc_main(int argc, char * argv[])
{
        try
        {
                std::cout << "\nLocal   <-->  Central with FullDuplex channel\n" << std::endl;
                std::cout << "+----------------------------------------------------------------------------------------------+" << std::endl;
                std::cout << "|TASK     ACTION        FROM    TO      DATA    SIZE      SEQUENCE NUMBER       TIME STAMP     |" << std::endl;
                std::cout << "+----------------------------------------------------------------------------------------------+" << std::endl;

                //////////////////////////////  SINGLETON  //////////////////////////////
                Scnsl::Setup::Scnsl_t * sim = Scnsl::Setup::Scnsl_t::get_instance();

                ////////////////////////////  NODE CREATION  ////////////////////////////
                Scnsl::Core::Node_t * n1 = sim->createNode();
                Scnsl::Core::Node_t * nC = sim->createNode();
                //////////////////////////  END NODE CREATION  //////////////////////////

                ////////////////////////////  CHANNEL SETUP  ////////////////////////////
                //Primo canale FULL-DUPLEX
                Scnsl::BuiltinPlugin::CoreChannelSetup_t csbCh1;
                csbCh1.extensionId = "core";
                csbCh1.channel_type = Scnsl::BuiltinPlugin::CoreChannelSetup_t::FULL_DUPLEX;
                csbCh1.capacity = 1000;
                csbCh1.capacity2 = 1000;
                csbCh1.delay = sc_core::sc_time(1000, sc_core::SC_US);
                csbCh1.name = "channel_fullduplex_ch1";
                Scnsl::Core::Channel_if_t * ch1 = sim->createChannel(csbCh1);

                //////////////////////////  END CHANNEL SETUP  ///////////////////////

                ////////////////////////// COMUNICATOR SETUP//////////////////////////
                ///
                ////////////////////////// END COMUNICATOR SETUP//////////////////////

                ////////////////////////// SABOTEUR SETUP/////////////////////////////
                Scnsl::BuiltinPlugin::CoreCommunicatorSetup_t ccoms1;
                ccoms1.extensionId = "core";
                ccoms1.ack_required = true;
                ccoms1.short_addresses = true;
                //ccoms1.bitErrorRateFunction = inject;
                ccoms1.type = Scnsl::BuiltinPlugin::CoreCommunicatorSetup_t::SABOTEUR;

                // First instance:
                ccoms1.name = "Sab1";
                ccoms1.node = nC;
                Scnsl::Core::Communicator_if_t * sab0 = sim->createCommunicator( ccoms1 );
//---------------------------------------

                Scnsl::BuiltinPlugin::CoreCommunicatorSetup_t ccoms2;
                ccoms2.extensionId = "core";
                ccoms2.ack_required = true;
                ccoms2.short_addresses = true;
                //ccoms2.bitErrorRateFunction = inject;
                ccoms2.type = Scnsl::BuiltinPlugin::CoreCommunicatorSetup_t::SABOTEUR;

                // First instance:
                ccoms2.name = "Sab2";
                ccoms2.node = n1;
                Scnsl::Core::Communicator_if_t * sab1 = sim->createCommunicator( ccoms2 );

                ////////////////////////// END SABOTEUR SETUP//////////////////////////

                ////////////////////////////  BINDING SETUP  //////////////////////////
                Scnsl::Setup::BindSetup_base_t bsb1; // N1 in Ch1
                Scnsl::Setup::BindSetup_base_t bsb2; // N2 in Ch1

                bsb1.extensionId = "core";
                bsb2.extensionId = "core";


                //////////////////////////
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
                Scnsl_t::Tracer_t * tracer1 = sim->createTracer( cts );
                // - Setting the output stream:
                tracer1->addOutput( & std::cout );
                // - Adding to trace:

                tracer1->trace( dynamic_cast<Traceable_base_t*>(sab0) );
                tracer1->trace( dynamic_cast<Traceable_base_t*>(sab1) );
                // - Setting to trace backend-like infos:
                cts.info = 0;
                cts.debug = 5;
                cts.log = 0;
                cts.error = 5;
                cts.warning = 5;
                cts.fatal = 5;
                //////////////////////////

                sim->bind(n1, ch1, bsb1);
                sim->bind(nC, ch1, bsb2);

                //////////////////////////  END BINDING SETUP  //////////////////////////
                LocalTask tLocal(    "Local",   1, n1, 1);
                CentralTask tCentral("Central", 2, nC, 1); //ultimo numero era 2


                sim->bind(&tLocal, &tCentral, ch1, bsb1, sab1);//NULL dove macStack
                tLocal.manager.routing[2] = 0;
                sim->bind(&tCentral, &tLocal, ch1, bsb2, sab0);
                tCentral.manager.routing[1] = 0;

                sc_core::sc_start(sc_core::sc_time(8, sc_core::SC_SEC));
                sc_core::sc_stop();

        }
        catch (std::exception & e)
        {
                std::cerr << e.what() << "\n";
                std::cerr << argc << "\n";
        }
return 0;
}
