#include <sstream>
#include <systemc>
#include <tlm.h>
#include <exception>

#include <scnsl.hh>

#include "CentralTask.hh"
#include "LocalTask.hh"

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
                //Scnsl::Core::Node_t * n2 = sim->createNode();
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

                //////////////////////////  END CHANNEL SETUP  //////////////////////////

                ////////////////////////////  BINDING SETUP  ////////////////////////////
                Scnsl::Setup::BindSetup_base_t bsb1; // N1 in Ch1
                Scnsl::Setup::BindSetup_base_t bsb2; // N2 in Ch1

                bsb1.extensionId = "core";
                bsb2.extensionId = "core";

                sim->bind(n1, ch1, bsb1);
                sim->bind(nC, ch1, bsb2);

                //////////////////////////  END BINDING SETUP  //////////////////////////
                LocalTask tLocal(    "Local",   1, n1, 1);
                CentralTask tCentral("Central", 2, nC, 1);

                sim->bind(&tLocal, &tCentral, ch1, bsb1, NULL);
                tLocal.manager.routing[2] = 0;
                sim->bind(&tCentral, NULL, ch1, bsb2, NULL);
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
