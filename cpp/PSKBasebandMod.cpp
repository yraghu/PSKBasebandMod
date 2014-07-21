/**************************************************************************

    This is the component code. This file contains the child class where
    custom functionality can be added to the component. Custom
    functionality to the base class can be extended here. Access to
    the ports can also be done from this class

**************************************************************************/

#include "PSKBasebandMod.h"

PREPARE_LOGGING(PSKBasebandMod_i)

PSKBasebandMod_i::PSKBasebandMod_i(const char *uuid, const char *label) :
    PSKBasebandMod_base(uuid, label)
{
	old_level = levels;
	old_packet_size = packet_size;
	old_oversample = oversample_num;
	sri = bulkio::sri::create(stream_id);
	sri.blocking = true;
	sri.mode = 1;
	sri.xdelta = xdelta;
	srand(time(NULL));
	updateSRI = true;
}

PSKBasebandMod_i::~PSKBasebandMod_i()
{
}

/***********************************************************************************************

    Basic functionality:

        The service function is called by the serviceThread object (of type ProcessThread).
        This call happens immediately after the previous call if the return value for
        the previous call was NORMAL.
        If the return value for the previous call was NOOP, then the serviceThread waits
        an amount of time defined in the serviceThread's constructor.
        
    SRI:
        To create a StreamSRI object, use the following code:
                std::string stream_id = "testStream";
                BULKIO::StreamSRI sri = bulkio::sri::create(stream_id);

	Time:
	    To create a PrecisionUTCTime object, use the following code:
                BULKIO::PrecisionUTCTime tstamp = bulkio::time::utils::now();

        
    Ports:

        Data is passed to the serviceFunction through the getPacket call (BULKIO only).
        The dataTransfer class is a port-specific class, so each port implementing the
        BULKIO interface will have its own type-specific dataTransfer.

        The argument to the getPacket function is a floating point number that specifies
        the time to wait in seconds. A zero value is non-blocking. A negative value
        is blocking.  Constants have been defined for these values, bulkio::Const::BLOCKING and
        bulkio::Const::NON_BLOCKING.

        Each received dataTransfer is owned by serviceFunction and *MUST* be
        explicitly deallocated.

        To send data using a BULKIO interface, a convenience interface has been added 
        that takes a std::vector as the data input

        NOTE: If you have a BULKIO dataSDDS or dataVITA49  port, you must manually call 
              "port->updateStats()" to update the port statistics when appropriate.

        Example:
            // this example assumes that the component has two ports:
            //  A provides (input) port of type bulkio::InShortPort called short_in
            //  A uses (output) port of type bulkio::OutFloatPort called float_out
            // The mapping between the port and the class is found
            // in the component base class header file

            bulkio::InShortPort::dataTransfer *tmp = short_in->getPacket(bulkio::Const::BLOCKING);
            if (not tmp) { // No data is available
                return NOOP;
            }

            std::vector<float> outputData;
            outputData.resize(tmp->dataBuffer.size());
            for (unsigned int i=0; i<tmp->dataBuffer.size(); i++) {
                outputData[i] = (float)tmp->dataBuffer[i];
            }

            // NOTE: You must make at least one valid pushSRI call
            if (tmp->sriChanged) {
                float_out->pushSRI(tmp->SRI);
            }
            float_out->pushPacket(outputData, tmp->T, tmp->EOS, tmp->streamID);

            delete tmp; // IMPORTANT: MUST RELEASE THE RECEIVED DATA BLOCK
            return NORMAL;

        If working with complex data (i.e., the "mode" on the SRI is set to
        true), the std::vector passed from/to BulkIO can be typecast to/from
        std::vector< std::complex<dataType> >.  For example, for short data:

            bulkio::InShortPort::dataTransfer *tmp = myInput->getPacket(bulkio::Const::BLOCKING);
            std::vector<std::complex<short> >* intermediate = (std::vector<std::complex<short> >*) &(tmp->dataBuffer);
            // do work here
            std::vector<short>* output = (std::vector<short>*) intermediate;
            myOutput->pushPacket(*output, tmp->T, tmp->EOS, tmp->streamID);

        Interactions with non-BULKIO ports are left up to the component developer's discretion

    Properties:
        
        Properties are accessed directly as member variables. For example, if the
        property name is "baudRate", it may be accessed within member functions as
        "baudRate". Unnamed properties are given a generated name of the form
        "prop_n", where "n" is the ordinal number of the property in the PRF file.
        Property types are mapped to the nearest C++ type, (e.g. "string" becomes
        "std::string"). All generated properties are declared in the base class
        (PSKBasebandMod_base).
    
        Simple sequence properties are mapped to "std::vector" of the simple type.
        Struct properties, if used, are mapped to C++ structs defined in the
        generated file "struct_props.h". Field names are taken from the name in
        the properties file; if no name is given, a generated name of the form
        "field_n" is used, where "n" is the ordinal number of the field.
        
        Example:
            // This example makes use of the following Properties:
            //  - A float value called scaleValue
            //  - A boolean called scaleInput
              
            if (scaleInput) {
                dataOut[i] = dataIn[i] * scaleValue;
            } else {
                dataOut[i] = dataIn[i];
            }
            
        Callback methods can be associated with a property so that the methods are
        called each time the property value changes.  This is done by calling 
        addPropertyChangeListener(<property name>, this, &PSKBasebandMod_i::<callback method>)
        in the constructor.

        Callback methods should take two arguments, both const pointers to the value
        type (e.g., "const float *"), and return void.

        Example:
            // This example makes use of the following Properties:
            //  - A float value called scaleValue
            
        //Add to PSKBasebandMod.cpp
        PSKBasebandMod_i::PSKBasebandMod_i(const char *uuid, const char *label) :
            PSKBasebandMod_base(uuid, label)
        {
            addPropertyChangeListener("scaleValue", this, &PSKBasebandMod_i::scaleChanged);
        }

        void PSKBasebandMod_i::scaleChanged(const float *oldValue, const float *newValue)
        {
            std::cout << "scaleValue changed from" << *oldValue << " to " << *newValue
                      << std::endl;
        }
            
        //Add to PSKBasebandMod.h
        void scaleChanged(const float* oldValue, const float* newValue);
        
        
************************************************************************************************/
int PSKBasebandMod_i::serviceFunction()
{
	bool gen = generate; //caching off generate boolean
    bulkio::InUShortPort::dataTransfer *port_in = NULL;
    if(!gen)
    {
    	port_in = symbols_in->getPacket(bulkio::Const::BLOCKING);
    	if(not port_in)
    		return NOOP;
    }

    vector<unsigned short> *input;
    vector<value> *output;

    if(old_level != levels)
    	old_level = levels; //caching off
    if(old_packet_size != packet_size)
    	old_packet_size = packet_size; //caching off
    if(old_oversample != oversample_num)
    {
    	old_oversample = oversample_num;
    	if(old_oversample == 0)	//checks if oversample num is zero
    	{						//changes to 1 if it is
    		oversample_num = 1;
    		old_oversample = 1;
    		cout << "WARNING - 'oversample_num' cannot be zero. changed to 1" << endl;
    	}
    	if(old_packet_size % old_oversample != 0)
    	{
    		unsigned short temp = old_oversample;
    		while(old_packet_size % temp != 0 && temp > 0)
    			temp--;
    		old_oversample = temp;
    		oversample_num = temp;
    		cout << "WARNING - 'oversample_num' not divisible by packet size" << endl;
    		cout << "Changed to " << old_oversample << endl;
    	}
    }
    if(sri.xdelta != xdelta)
    {
    	sri.xdelta = xdelta;
    	updateSRI = true;
    }



    if(gen)
    {
    	randomInputData.resize(old_packet_size);
    	generateRandomData(randomInputData);
    	input = &randomInputData;
    }
    else
    	input = (vector<unsigned short>*)&(port_in->dataBuffer);

    checkInput(*input, old_level); // warning log if any above the levels/push it down

    dataOut.resize(input->size());
    for(unsigned int i=0;i<input->size()/old_oversample;i++)
    {
    	value theta = (*input)[i] * (2*M_PI/old_level);
    	value real_part = cos(theta);
    	value imag_part = sin(theta);
    	complex_value temp(real_part, imag_part);
    	for(int k=0;k<old_oversample;k++)
    		dataOut[old_oversample*i+k] = temp;
    }
    output = (vector<value>*)&dataOut;
    BULKIO::PrecisionUTCTime tstamp;
    bool EOS;
    string streamID;
    if(gen)
    {
    	tstamp = bulkio::time::utils::now();
    	EOS = 0;
    	streamID = stream_id;
    	symbols_out->pushPacket(randomInputData, tstamp, 0, "Symbols Gen");
    }
    else
    {
    	tstamp = port_in->T;
    	EOS = port_in->EOS;
    	streamID = port_in->streamID;
    }

    if(updateSRI)
    	mod_out->pushSRI(sri);
    mod_out->pushPacket(*output, tstamp, EOS, streamID);

    return NORMAL;
}


void PSKBasebandMod_i::checkInput(vector<unsigned short> &in, unsigned short maxVal)
{
	for(unsigned int i=0;i<in.size();i++)
	{
		if(in[i] > maxVal)
		{
			cout << "Warning: input data has values that are higher than levels" << endl;
			cout << "\tChanging value to the maximum allowed value" << endl;
			in[i] = maxVal;
		}
	}
}


void PSKBasebandMod_i::generateRandomData(vector<unsigned short> &data)
{
	for(unsigned int i=0;i<old_packet_size;i++)
			data[i] = rand() % old_level;
}

