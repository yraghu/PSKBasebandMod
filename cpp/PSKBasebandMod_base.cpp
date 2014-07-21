#include "PSKBasebandMod_base.h"

/*******************************************************************************************

    AUTO-GENERATED CODE. DO NOT MODIFY

    The following class functions are for the base class for the component class. To
    customize any of these functions, do not modify them here. Instead, overload them
    on the child class

******************************************************************************************/

PSKBasebandMod_base::PSKBasebandMod_base(const char *uuid, const char *label) :
    Resource_impl(uuid, label),
    ThreadedComponent()
{
    loadProperties();

    symbols_in = new bulkio::InUShortPort("symbols_in");
    addPort("symbols_in", symbols_in);
    symbols_out = new bulkio::OutUShortPort("symbols_out");
    addPort("symbols_out", symbols_out);
    mod_out = new bulkio::OutFloatPort("mod_out");
    addPort("mod_out", mod_out);
}

PSKBasebandMod_base::~PSKBasebandMod_base()
{
    delete symbols_in;
    symbols_in = 0;
    delete symbols_out;
    symbols_out = 0;
    delete mod_out;
    mod_out = 0;
}

/*******************************************************************************************
    Framework-level functions
    These functions are generally called by the framework to perform housekeeping.
*******************************************************************************************/
void PSKBasebandMod_base::start() throw (CORBA::SystemException, CF::Resource::StartError)
{
    Resource_impl::start();
    ThreadedComponent::startThread();
}

void PSKBasebandMod_base::stop() throw (CORBA::SystemException, CF::Resource::StopError)
{
    Resource_impl::stop();
    if (!ThreadedComponent::stopThread()) {
        throw CF::Resource::StopError(CF::CF_NOTSET, "Processing thread did not die");
    }
}

void PSKBasebandMod_base::releaseObject() throw (CORBA::SystemException, CF::LifeCycle::ReleaseError)
{
    // This function clears the component running condition so main shuts down everything
    try {
        stop();
    } catch (CF::Resource::StopError& ex) {
        // TODO - this should probably be logged instead of ignored
    }

    Resource_impl::releaseObject();
}

void PSKBasebandMod_base::loadProperties()
{
    addProperty(levels,
                4,
                "levels",
                "",
                "readwrite",
                "",
                "external",
                "configure");

    addProperty(generate,
                true,
                "generate",
                "",
                "readwrite",
                "",
                "external",
                "configure");

    addProperty(packet_size,
                1000,
                "packet_size",
                "",
                "readwrite",
                "",
                "external",
                "configure");

    addProperty(stream_id,
                "PSKBasebandMod Stream",
                "stream_id",
                "",
                "readwrite",
                "",
                "external",
                "configure");

    addProperty(xdelta,
                1.0,
                "xdelta",
                "",
                "readwrite",
                "",
                "external",
                "configure");

    addProperty(oversample_num,
                4,
                "oversample_num",
                "",
                "readwrite",
                "",
                "external",
                "configure");

}


