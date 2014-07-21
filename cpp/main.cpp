#include <iostream>
#include "ossie/ossieSupport.h"

#include "PSKBasebandMod.h"
int main(int argc, char* argv[])
{
    PSKBasebandMod_i* PSKBasebandMod_servant;
    Resource_impl::start_component(PSKBasebandMod_servant, argc, argv);
    return 0;
}

