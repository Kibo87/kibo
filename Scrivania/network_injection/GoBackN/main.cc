#include <sstream>
#include <systemc>
#include <tlm.h>
#include <exception>

#include <scnsl.hh>

#include "CentralTask.hh"
#include "LocalTask.hh"
#include "RemoteTask.hh"

// ////////////////////////  NETWORK SCENARIO  /////////////////////////
//                                                                    //
//  **********                 **********                 **********  //
//  *        *                 *        *                 *        *  //
//  *   N1   *<--------------->*   NC   *<--------------->*   N2   *  //
//  *        *   FULLDUPLEX    *        *     SHARED      *        *  //
//  **********    CHANNEL      **********     CHANNEL     **********  //
//                                                                    //
// /////////////////////////////////////////////////////////////////////

int sc_main(int argc, char * argv[])
{
        try
        {
                std::cout << "\nLocal   <-->  Central with FullDuplex channel\nCentral <-->  Remote  with Shared channel" << std::endl;
                std::cout << "+----------------------------------------------------------------------------------------------+" << std::endl;
                std::cout << "|TASK     ACTION        FROM    TO      DATA    SIZE      SEQUENCE NUMBER       TIME STAMP     |" << std::endl;
                std::cout << "+----------------------------------------------------------------------------------------------+" << std::endl;

                //////////////////////////////  SINGLETON  //////////////////////////////
                Scnsl::Setup::Scnsl_t * sim = Scnsl::Setup::Scnsl_t::get_instance();

                ////////////////////////////  NODE CREATION  ////////////////////////////
                Scnsl::Core::Node_t * n1 = sim->createNode();
                Scnsl::Core::Node_t * n2 = sim->createNode();
                Scnsl::Core::Node_t * nC = sim->createNode();
                //////////////////////////  END NODE CREATION  //////////////////////////

                ////////////////////////////  CHANNEL SETUP  ////////////////////////////
                //Primo canale FULL-DUPLEX
                Scnsl::BuiltinPlugin::CoreChannelSetup_t csbCh1;
                csbCh1.extensionId = "core";
                csbCh1.channel_type = Scnsl::BuiltinPlugin::CoreChannelSetup_t::FULL_DUPLEX;
                csbCh1.capacity = 1000;
                csbCh1.capacity2 = 1000;
                csbCh1.delay = sc_core::sc_time(1.0, sc_core::SC_MS);
                csbCh1.name = "channel_fullduplex_ch1";
                Scnsl::Core::Channel_if_t * ch1 = sim->createChannel(csbCh1);

                //Secondo canale SHARED
                Scnsl::BuiltinPlugin::CoreChannelSetup_t csbCh2;
                csbCh2.extensionId = "core";
                csbCh2.channel_type = Scnsl::BuiltinPlugin::CoreChannelSetup_t::SHARED;
                csbCh2.capacity = 1000;
                csbCh2.capacity2 = 1000;
                csbCh2.nodes_number = 2;
                csbCh2.alpha = 0.1;
                csbCh2.delay = sc_core::sc_time(1.0, sc_core::SC_MS);
                csbCh2.propagation = Scnsl::BuiltinPlugin::CoreChannelSetup_t::EM_SPEED;
                csbCh2.name = "channel_fullduplex_ch2";
                Scnsl::Core::Channel_if_t * ch2 = sim->createChannel(csbCh2);
                //////////////////////////  END CHANNEL SETUP  //////////////////////////

                ////////////////////////////  BINDING SETUP  ////////////////////////////
                Scnsl::Setup::BindSetup_base_t bsb1; // N1 in Ch1
                Scnsl::Setup::BindSetup_base_t bsb2; // N2 in Ch1
                Scnsl::Setup::BindSetup_base_t bsb3; // N2 in Ch2
                Scnsl::Setup::BindSetup_base_t bsb4; // N3 in Ch2
                bsb1.extensionId = "core";
                bsb2.extensionId = "core";
                bsb3.extensionId = "core";
                bsb4.extensionId = "core";

                bsb3.destinationNode = n2;
                bsb3.node_binding.x = 0;
                bsb3.node_binding.y = 0;
                bsb3.node_binding.z = 0;
                bsb3.node_binding.bitrate = Scnsl::Protocols::Mac_802_15_4::BITRATE;
                bsb3.node_binding.transmission_power = 100;
                bsb3.node_binding.receiving_threshold = 10;

                bsb4.destinationNode = nC;
                bsb4.node_binding.x = 1;
                bsb4.node_binding.y = 1;
                bsb4.node_binding.z = 1;
                bsb4.node_binding.bitrate = Scnsl::Protocols::Mac_802_15_4::BITRATE;
                bsb4.node_binding.transmission_power = 100;
                bsb4.node_binding.receiving_threshold = 10;

                sim->bind(n1, ch1, bsb1);
                sim->bind(nC, ch1, bsb2);
                sim->bind(nC, ch2, bsb3);
                sim->bind(n2, ch2, bsb4);
                //////////////////////////  END BINDING SETUP  //////////////////////////
                LocalTask tLocal("Local", 1, n1, 1);
                CentralTask tCentral("Central", 2, nC, 2);
                RemoteTask tRemote("Remote", 3, n2, 1);

                sim->bind(&tLocal, &tCentral, ch1, bsb1, NULL);
                sim->bind(&tCentral, NULL, ch1, bsb2, NULL);
                sim->bind(&tCentral, &tRemote, ch2, bsb3, NULL);
                sim->bind(&tRemote, NULL, ch2, bsb4, NULL);

                sc_core::sc_start(sc_core::sc_time(4, sc_core::SC_SEC));
                sc_core::sc_stop();

        }
        catch (std::exception & e)
        {
                std::cerr << e.what() << "\n";
                std::cerr << argc << "\n";
        }
        return 0;
}
