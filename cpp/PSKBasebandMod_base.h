#ifndef PSKBASEBANDMOD_IMPL_BASE_H
#define PSKBASEBANDMOD_IMPL_BASE_H

#include <boost/thread.hpp>
#include <ossie/Resource_impl.h>
#include <ossie/ThreadedComponent.h>

#include <bulkio/bulkio.h>

class PSKBasebandMod_base : public Resource_impl, protected ThreadedComponent
{
    public:
        PSKBasebandMod_base(const char *uuid, const char *label);
        ~PSKBasebandMod_base();

        void start() throw (CF::Resource::StartError, CORBA::SystemException);

        void stop() throw (CF::Resource::StopError, CORBA::SystemException);

        void releaseObject() throw (CF::LifeCycle::ReleaseError, CORBA::SystemException);

        void loadProperties();

    protected:
        // Member variables exposed as properties
        unsigned short levels;
        bool generate;
        CORBA::ULong packet_size;
        std::string stream_id;
        double xdelta;
        unsigned short oversample_num;

        // Ports
        bulkio::InUShortPort *symbols_in;
        bulkio::OutUShortPort *symbols_out;
        bulkio::OutFloatPort *mod_out;

    private:
};
#endif // PSKBASEBANDMOD_IMPL_BASE_H
