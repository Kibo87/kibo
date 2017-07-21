#ifndef DEFINES_HH
#define DEFINES_HH

#include <scnsl.hh>
#include <systemc>
#include <tlm.h>
#include <queue>
#include <iostream>
#include <chrono>
#include <map>
#include <thread>

//Struttura dei pacchetti "normali"
struct Payload_t
{
        char type;
        unsigned int sourceId;
        unsigned int lastSender;
        unsigned int targetId;
        unsigned int seqNumber;
        char data;
};


#endif
