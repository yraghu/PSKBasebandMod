#!/usr/bin/env python
import unittest
import ossie.utils.testing
import os
import scipy
import math
from scipy import fftpack
import matplotlib.pyplot
from ossie.utils import sb
from time import sleep
from omniORB import any
import random

def plotFFT(data,fftNum=2*1024, xdelta=1.0,subplot=None):
    f = scipy.fftpack.fftshift(fftpack.fft(data,fftNum))
    
    sampleRate = 1.0/xdelta
    fDb = [20*math.log10(abs(x)) for x in f]  
    freqs = scipy.fftpack.fftshift(fftpack.fftfreq(fftNum,xdelta))
    
    if subplot !=None:
        matplotlib.pyplot.subplot(subplot)
    
    matplotlib.pyplot.plot(freqs, fDb)
    if subplot==None:
        matplotlib.pyplot.show()


class ResourceTests(ossie.utils.testing.ScaComponentTestCase):
    """Test for all resource implementations in PSKBasebandMod"""

    def setUp(self):
        #set up 
        ossie.utils.testing.ScaComponentTestCase.setUp(self)
        self.src = sb.DataSource()
        self.sink = sb.DataSink()
        
        #connect 
        self.startComponent()
        #self.src.connect(self.comp)
        self.comp.connect(self.sink, 'floatIn')
        
        #starts sandbox
        sb.start()
        
        #variables
    
    def tearDown(self):
        #######################################################################
        # Simulate regular resource shutdown
        self.comp.releaseObject()
        
        self.comp.stop()
        sb.reset()
        sb.stop()
        ossie.utils.testing.ScaComponentTestCase.tearDown(self);

    def startComponent(self):
        #######################################################################
        # Launch the resource with the default execparams
        execparams = self.getPropertySet(kinds=("execparam",), modes=("readwrite", "writeonly"), includeNil=False)
        execparams = dict([(x.id, any.from_any(x.value)) for x in execparams])
        self.launch(execparams)

        #######################################################################
        # Verify the basic state of the resource
        self.assertNotEqual(self.comp, None)
        self.assertEqual(self.comp.ref._non_existent(), False)

        self.assertEqual(self.comp.ref._is_a("IDL:CF/Resource:1.0"), True)

        #######################################################################
        # Validate that query returns all expected parameters
        # Query of '[]' should return the following set of properties
        expectedProps = []
        expectedProps.extend(self.getPropertySet(kinds=("configure", "execparam"), modes=("readwrite", "readonly"), includeNil=True))
        expectedProps.extend(self.getPropertySet(kinds=("allocate",), action="external", includeNil=True))
        props = self.comp.query([])
        props = dict((x.id, any.from_any(x.value)) for x in props)
        # Query may return more than expected, but not less
        for expectedProp in expectedProps:
            self.assertEquals(props.has_key(expectedProp.id), True)

        #######################################################################
        # Verify that all expected ports are available
        for port in self.scd.get_componentfeatures().get_ports().get_uses():
            port_obj = self.comp.getPort(str(port.get_usesname()))
            self.assertNotEqual(port_obj, None)
            self.assertEqual(port_obj._non_existent(), False)
            self.assertEqual(port_obj._is_a("IDL:CF/Port:1.0"),  True)

        for port in self.scd.get_componentfeatures().get_ports().get_provides():
            port_obj = self.comp.getPort(str(port.get_providesname()))
            self.assertNotEqual(port_obj, None)
            self.assertEqual(port_obj._non_existent(), False)
            self.assertEqual(port_obj._is_a(port.get_repid()),  True)

        #######################################################################
        # Make sure start and stop can be called without throwing exceptions
        self.comp.packet_size = 40
        self.comp.start()
    # TODO Add additional tests here
    #
    # See:
    #   ossie.utils.bulkio.bulkio_helpers,
    #   ossie.utils.bluefile.bluefile_helpers
    # for modules that will assist with testing resource with BULKIO ports


    def testOne(self):
        outData = []
        count = 0
        while True:
            outData = self.sink.getData()
            if outData:
                break
            if count == 100:
                break
            sleep(.1)
            count+=1
        sb.stop()
        sleep(.5)
        for x in outData:
            print "%i " %x,
        print ""
        complexData = []
        for x in xrange(len(outData)/2):
            complexData.append(complex(outData[2*x], outData[x*2+1]))
        complexData = [x + 0.1*random.random() for x in complexData]
        plotFFT(complexData)
    




    

if __name__ == "__main__":
    ossie.utils.testing.main("../PSKBasebandMod.spd.xml") # By default tests all implementations
    
    
    
