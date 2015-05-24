/*
 *  test_serial.cpp
 *  cefix
 *
 *  Created by Stephan Huber on 05.05.06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */
#include <iostream>
#include <sstream>
#include <cefix/Log.h>
#include <osg/Timer>
#include <osg/ref_ptr>
#include <cefix/SerialCommunication.h>

int main(int argc, char *argv[])
{
    osg::setNotifyLevel(osg::INFO);
    osg::notify(osg::ALWAYS) << "starting serial comm test" << std::endl;
	osg::ref_ptr<cefix::SerialCommunication> serial = new cefix::SerialCommunication();
	
	// get list of available ports
	cefix::SerialCommunication::PortList portlist = cefix::SerialCommunication::getPortList();
	for (cefix::SerialCommunication::PortList::iterator i = portlist.begin(); i != portlist.end(); i++) {
		std::string str = (*i);
		std::cout << "found serial port: " << str << std::endl;
	}
	
    std::string myport;
	// find port named usbserial
#ifdef __APPLE__
    if (0) 
        myport = serial->findPort("modem");
    else 
        myport = serial->findPort("usbserial");
#else
	myport = "COM3";

#endif
        
	if (myport.empty()) {
		osg::notify(osg::ALWAYS) << "could not find desired port" << std::endl;
		return 0;
	}
	// set the port
	serial->setPort(myport);
	// set the communication parameters
	serial->setParameters(9600, 8, cefix::SerialCommunication::SC_PARITY_NONE, 1);
    
	// connect
	if (!serial->connect()) {
		osg::notify(osg::ALWAYS) << "could not connect to port " << serial->getPort() << std::endl;
		return 0;
	}
	
    // test modem, sends "AT" and waits for "OK"
    if (0) {
        serial->write("AT\r\n");
        bool fok = false;
        
        while (!fok) {
            std::string input = serial->readLine();
            fok = (input.substr(0,2) == "OK");
            if (!input.empty())
                osg::notify(osg::ALWAYS) << input << std::endl;
        }
    }
    
	else {
        std::string last;
        std::string r;
        int i = 0;
        while (i < 500) {
            
            unsigned int b = serial->bytesAvailable();
            if (b>6) {
                
                r = serial->readTillToken(64);
                if (!r.empty() && (r != last))  {
                  
                    if (1 || (last != r)) {
                         i++;
                        std::cout << ">" << r << "< " << b << std::endl;
                        last = r;
                    }
                }
            }
        }  
    
    }
    
    
    /*{
        osg::Timer t;
        float timeout = 0.1;
        osg::Timer_t last_successful_read = 0;
		unsigned int readCount = 0;
    
        // test arduino-board
        // communication is a bit flaky, bec
        for (int i = 0; i < 2000; i++) {
            std::stringstream ss;
            ss << i;
            serial->writeLine(ss.str());
            std::string read = "";
			readCount = 0;
            while ((read.empty()) && (t.delta_s(last_successful_read, t.tick()) < timeout)) {
                read = serial->readLine();
				readCount++;
            }
            if (read.empty()) {
                // timeout missed?
                read = "** MISSED **";
            }
            last_successful_read = t.tick();
            osg::notify(osg::ALWAYS) << i << ": " << read << " (" << readCount << ")" << std::endl;
        }
    }*/
	
	// disconnect
	serial->disconnect();
	
	return 0;
}
