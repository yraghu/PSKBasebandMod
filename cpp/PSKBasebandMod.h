#ifndef BASEBANDMOD_IMPL_H
#define BASEBANDMOD_IMPL_H

#include "PSKBasebandMod_base.h"
using std::cout;
using std::endl;
using std::complex;
using std::vector;
using std::string;
#include <stdlib.h>
using std::srand;
using std::rand;
#include <time.h>
using std::time;
#include <math.h>
using std::sin;
using std::cos;
using std::exp;


typedef float 				value;
typedef complex<value> 		complex_value;

class PSKBasebandMod_i : public PSKBasebandMod_base
{
    ENABLE_LOGGING
    public:
        PSKBasebandMod_i(const char *uuid, const char *label);
        ~PSKBasebandMod_i();
        int serviceFunction();

    private:
        void generateRandomData(vector<unsigned short> &data);
        void checkInput(vector<unsigned short> &in, unsigned short maxVal);
        unsigned long old_packet_size;
        unsigned short old_level;
        unsigned short old_oversample;
        bool updateSRI;
        vector<complex_value> dataOut;


        vector<unsigned short> randomInputData;
        BULKIO::StreamSRI sri;
};

#endif // BASEBANDMOD_IMPL_H
