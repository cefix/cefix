/*
 *  Serial.cpp
 *  cefix
 *
 *  Created by Stephan Huber on 04.05.06.
 *  Copyright 2006 digitalmind.de // Stephan Maximilian Huber. All rights reserved.
 *
 */

#include "SerialCommunication.h"  
#include <cefix/Log.h>
#include <OpenThreads/Thread>
#include <iostream>

#ifdef __APPLE__
extern "C" {
	#include <fcntl.h>
	#include <sys/ioctl.h>
	#include <errno.h>
	#include <paths.h>
	#include <termios.h>
	#include <sysexits.h>
	#include <sys/param.h>
	#include <sys/select.h>
	#include <sys/time.h>
	#include <time.h>
	#include <AvailabilityMacros.h>
	#include <CoreFoundation/CoreFoundation.h>

	#include <IOKit/IOKitLib.h>
	#include <IOKit/serial/IOSerialKeys.h>
	#if defined(MAC_OS_X_VERSION_10_3) && (MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_3)
	#include <IOKit/serial/ioss.h>
	#endif
	#include <IOKit/IOBSD.h>



}

#endif

#ifdef WIN32
#include <windows.h>
#include "Serial.h"
#include <tchar.h>
#endif



namespace cefix {
#define BUFFER_SIZE 10240
#define NULL_REPLACEMENT 64
    class SerialImplementation {
        public:
            SerialImplementation(SerialCommunication* parent);
			~SerialImplementation();
            static SerialCommunication::PortList getPortList();
            bool connect(const std::string& port, unsigned int baud, unsigned int data, SerialCommunication::Parity parity, unsigned int stop, SerialCommunication::Handshake handshake);
            void disconnect();
			
			void write(const std::string& str);
			inline void raw_read();
			std::string read(unsigned int toToken = 0xFFFF); 
			std::string readLine() { 
                std::string result = read(10);
                if ((result.length()>0) &&(result[result.length()-1] == 13)) 
                    return result.substr(0,result.length()-1);
                else
                return result;
            }
			
            inline unsigned int bytesAvailable();
		
		protected:
			#ifdef __APPLE__
			int				fileDescriptor;
			struct termios	saved_options;
			#endif
			
			unsigned char buffer[BUFFER_SIZE];
			unsigned int readNdx, writeNdx;
			SerialCommunication* _parent;
			
#ifdef WIN32
			CSerial serial;
#endif

            
    };
    
#pragma  mark Mac OS X Implementation 


    #ifdef __APPLE__
	
	static int s_used_descriptor = 0;
	extern "C" {
	
		void catch_int_signal(int sig_num)
		{
			/* re-set the signal handler again to catch_int, for next time */
			signal(SIGINT, catch_int_signal);
			
			if (s_used_descriptor != 0) {
				printf("Closing stale serial connection ...");
				close(s_used_descriptor);
				s_used_descriptor = 0;
			}
			
			exit(0);
		}
	
	}
	
	SerialImplementation::SerialImplementation(SerialCommunication* parent) : fileDescriptor(0),_parent(parent) 
	{
		for(int i=0; i < BUFFER_SIZE; i++) buffer[i] = 0; 
	}
	
	SerialImplementation::~SerialImplementation() {
		if (fileDescriptor!=0)
			disconnect();
	}
    
	SerialCommunication::PortList SerialImplementation::getPortList() {
		io_iterator_t			serialPortIterator;
		char					bsdPath[MAXPATHLEN];
		kern_return_t			kernResult; 
		CFMutableDictionaryRef	classesToMatch;
		SerialCommunication::PortList portlist;

		// Serial devices are instances of class IOSerialBSDClient
		classesToMatch = IOServiceMatching(kIOSerialBSDServiceValue);
		if (classesToMatch == NULL)
		{
			log::error("SerialCommunication") << "IOServiceMatching returned a NULL dictionary." << std::endl;
			
		}
		else
			CFDictionarySetValue(classesToMatch,
								 CFSTR(kIOSerialBSDTypeKey),
								 CFSTR(kIOSerialBSDAllTypes));
			
		
		

		kernResult = IOServiceGetMatchingServices(kIOMasterPortDefault, classesToMatch, &serialPortIterator);    
		if (KERN_SUCCESS != kernResult)
		{
			log::error("SerialCommunication") << "IOServiceGetMatchingServices returned " << kernResult << std::endl;
			return portlist;
		}
		
		io_object_t		modemService;
		kernResult = KERN_FAILURE;
				
		// Initialize the returned path
		
		
		// Iterate across all modems found. In this example, we bail after finding the first modem.
		
		while ((modemService = IOIteratorNext(serialPortIterator)))
		{
			CFTypeRef	bsdPathAsCFString;
			*bsdPath = '\0';

			// Get the callout device's path (/dev/cu.xxxxx). The callout device should almost always be
			// used: the dialin device (/dev/tty.xxxxx) would be used when monitoring a serial port for
			// incoming calls, e.g. a fax listener.
		
			bsdPathAsCFString = IORegistryEntryCreateCFProperty(modemService,
																CFSTR(kIOCalloutDeviceKey),
																kCFAllocatorDefault,
																0);
			if (bsdPathAsCFString)
			{
				Boolean result;
				
				// Convert the path from a CFString to a C (NUL-terminated) string for use
				// with the POSIX open() call.
			
				result = CFStringGetCString((CFStringRef)bsdPathAsCFString,
											bsdPath,
											MAXPATHLEN, 
											kCFStringEncodingUTF8);
				CFRelease(bsdPathAsCFString);
				
				if (result)
				{
					osg::notify(osg::DEBUG_INFO) << "found serial device: "<< bsdPath << std::endl;
					portlist.push_back(std::string(bsdPath));
				}
			}


			// Release the io_service_t now that we are done with it.
		
			(void) IOObjectRelease(modemService);
		}

		return portlist;
	}
	
	bool SerialImplementation::connect(const std::string& port, unsigned int baud, unsigned int data, 
                                SerialCommunication::Parity parity, unsigned int stop, SerialCommunication::Handshake handshake) {
    
		int				second_handshake;
		struct termios	options;
		
		readNdx = writeNdx = 0;
		
        if (_parent->isNonBlocking())
            fileDescriptor = open(port.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
        else
            fileDescriptor = open(port.c_str(), O_RDWR | O_NOCTTY);
            
		if (fileDescriptor == -1)
		{
			log::error("SerialCommunication") << "Error opening serial port " << port << " - " <<  strerror(errno) << " (" << errno << ")" << std::endl;
			return false;
		}
		
		s_used_descriptor = fileDescriptor;
		// install signal handler, to catch abormal program termination
		signal(SIGINT, catch_int_signal);

		// Note that open() follows POSIX semantics: multiple open() calls to the same file will succeed
		// unless the TIOCEXCL ioctl is issued. This will prevent additional opens except by root-owned
		// processes.
		// See tty(4) ("man 4 tty") and ioctl(2) ("man 2 ioctl") for details.
		
		if (ioctl(fileDescriptor, TIOCEXCL) == -1)
		{
			log::error("SerialCommunication") << "Error setting TIOCEXCL on serial port " << port << " - " <<  strerror(errno) << " (" << errno << ")" << std::endl;
			close(fileDescriptor);
			return false;
		}
		
		// Now that the device is open, clear the O_NONBLOCK flag so subsequent I/O will block.
		// See fcntl(2) ("man 2 fcntl") for details.
		
		/*if (fcntl(fileDescriptor, F_SETFL, 0) == -1)
		{
			osg::notify(osg::WARN) << "Error setting fcntl on serial port " << port << " - " <<  strerror(errno) << " (" << errno << ")" << std::endl;
			close(fileDescriptor);
			return false;
		}
        
        if (fcntl(fileDescriptor, F_SETFL, O_NONBLOCK) == -1) {
            log::error("SerialCommunication") << "Error setting fcntl nodelay on serial port " << port << " - " <<  strerror(errno) << " (" << errno << ")" << std::endl;
		}*/
		
		// Get the current options and save them so we can restore the default settings later.
		if (tcgetattr(fileDescriptor, &saved_options) == -1)
		{
			log::error("SerialCommunication") << "Error getting tty attributes on serial port " << port << " - " <<  strerror(errno) << " (" << errno << ")" << std::endl;
			close(fileDescriptor);
			return false;
		}
		options = saved_options;
		
		// Set raw input (non-canonical) mode, with reads blocking until either a single character 
		// has been received or a one second timeout expires.
		// See tcsetattr(4) ("man 4 tcsetattr") and termios(4) ("man 4 termios") for details.
		
		cfmakeraw(&options);
		options.c_cc[VMIN] = 0;
		options.c_cc[VTIME] = 10;
			
		// The baud rate, word length, and handshake options can be set as follows:
		
		cfsetspeed(&options, baud);		// Set baud  
        
        options.c_cflag &= ~CSIZE;
        
		switch (data) {
			case 7:
				options.c_cflag |= CS7;
				break;
			default: 
				options.c_cflag |= CS8;
				break;
		}
		
		switch (parity) {
			case SerialCommunication::SC_PARITY_NONE:
				break;
			
			case SerialCommunication::SC_PARITY_ODD:
				options.c_cflag |= PARENB | PARODD;
				break;
			
			case SerialCommunication::SC_PARITY_EVEN:	
				options.c_cflag |= PARENB;
				break;
		}
		
        switch (handshake) {
            case SerialCommunication::SC_HANDSHAKE_RTS:
                options.c_cflag |= CRTS_IFLOW;
                break;
                
            case SerialCommunication::SC_HANDSHAKE_CTS:
                options.c_cflag |= CCTS_OFLOW;
                break;
                
            case SerialCommunication::SC_HANDSHAKE_RTS_CTS:
                options.c_cflag |= CCTS_OFLOW | CRTS_IFLOW;
                break;
            
            case SerialCommunication::SC_HANDSHAKE_XON_XOFF:
                options.c_cflag |= (IXON | IXOFF);
                break;
            
            case SerialCommunication::SC_HANDSHAKE_BOTH:
                options.c_cflag |= (IXON | IXOFF);
                options.c_cflag |= CCTS_OFLOW | CRTS_IFLOW;
                break;
                
            default:
                options.c_cflag &= ~(IXON | IXOFF);
                options.c_cflag &= ~(CCTS_OFLOW | CRTS_IFLOW);
        }
        
		#if defined(MAC_OS_X_VERSION_10_4) && (MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_4)
		// Starting with Tiger, the IOSSIOSPEED ioctl can be used to set arbitrary baud rates
		// other than those specified by POSIX. The driver for the underlying serial hardware
		// ultimately determines which baud rates can be used. This ioctl sets both the input
		// and output speed. 
		
		speed_t speed = baud; // Set 14400 baud
		if (ioctl(fileDescriptor, IOSSIOSPEED, &speed) == -1)
		{
			log::error("SerialCommunication") << "Error setting iospeed on serial port " << port << " - " <<  strerror(errno) << " (" << errno << ")" << std::endl;
			_parent->setErrorOccured(true);
		}
		#endif
		
		
		// Cause the new options to take effect immediately.
		if (tcsetattr(fileDescriptor, TCSANOW, &options) == -1)
		{
			log::error("SerialCommunication") << "Error setting tty attributes on serial port " << port << " - " <<  strerror(errno) << " (" << errno << ")" << std::endl;
			close(fileDescriptor);
			return false;
		}

		// To set the modem handshake lines, use the following ioctls.
		// See tty(4) ("man 4 tty") and ioctl(2) ("man 2 ioctl") for details.
		
		if (ioctl(fileDescriptor, TIOCSDTR) == -1) // Assert Data Terminal Ready (DTR)
		{
			log::error("SerialCommunication") << "Error asserting DSR on serial port " << port << " - " <<  strerror(errno) << " (" << errno << ")" << std::endl;
			_parent->setErrorOccured(true);
		}
		
		if (ioctl(fileDescriptor, TIOCCDTR) == -1) // Clear Data Terminal Ready (DTR)
		{
			log::error("SerialCommunication") << "Error clearing DTR on serial port " << port << " - " <<  strerror(errno) << " (" << errno << ")" << std::endl;
			_parent->setErrorOccured(true);
		}
		
		second_handshake = TIOCM_DTR | TIOCM_RTS | TIOCM_CTS | TIOCM_DSR;
		if (ioctl(fileDescriptor, TIOCMSET, &handshake) == -1)
		// Set the modem lines depending on the bits set in handshake
		{
			log::error("SerialCommunication") << "Error setting handshake lines on serial port " << port << " - " <<  strerror(errno) << " (" << errno << ")" << std::endl;
            _parent->setErrorOccured(true);
        }
		
		
		
		#if defined(MAC_OS_X_VERSION_10_3) && (MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_3)
			unsigned long mics = 1UL;

			// Set the receive latency in microseconds. Serial drivers use this value to determine how often to
			// dequeue characters received by the hardware. Most applications don't need to set this value: if an
			// app reads lines of characters, the app can't do anything until the line termination character has been
			// received anyway. The most common applications which are sensitive to read latency are MIDI and IrDA
			// applications.
			
			if (ioctl(fileDescriptor, IOSSDATALAT, &mics) == -1)
			{
				// set latency to 1 microsecond
				log::error("SerialCommunication") << "Error setting latency on serial port " << port << " - " <<  strerror(errno) << " (" << errno << ")" << std::endl;
				_parent->setErrorOccured(true);
			}
		#endif
		printf("Input baud rate changed to %d\n", (int) cfgetispeed(&options));
		printf("Output baud rate changed to %d\n", (int) cfgetospeed(&options));
		
		readNdx = writeNdx = 0;
		return true;
		
    }

    
    void SerialImplementation::disconnect() {
		
		if (tcdrain(fileDescriptor) == -1) {
			log::error("SerialCommunication") << "Error on drain "  <<  strerror(errno) << " (" << errno << ")" << std::endl;
            _parent->setErrorOccured(true);
		}
		
		if (tcsetattr(fileDescriptor, TCSANOW, &saved_options) == -1) {
			log::error("SerialCommunication") << "Error on resetting tty attributes "  <<  strerror(errno) << " (" << errno << ")" << std::endl;	
            _parent->setErrorOccured(true);	
		}

		close(fileDescriptor);
		fileDescriptor = 0;
		s_used_descriptor = 0;
    }
	
	
	void SerialImplementation::write(const std::string& data) 
    {
        osg::notify(osg::DEBUG_INFO) << "writing: " << data << std::endl;
        bool finished = false;
        unsigned int num_tries = 0;
        const char* buffer = data.c_str();
        std::size_t  data_size(data.size()) ,bytes_written_complete(0), num_bytes(data.length());
        
        while(!finished) 
        {
            int written_bytes = ::write(fileDescriptor, buffer, std::min<std::size_t>(_parent->getBaudrate(), num_bytes));
            
            if (written_bytes != -1) 
            {
                buffer += written_bytes;
                num_bytes -= written_bytes;
                bytes_written_complete += written_bytes;
                
                if (num_bytes == 0)
                    finished = true;
                else {
                    unsigned int ms = (written_bytes * 1000 * 1000) / _parent->getBaudrate();
                    OpenThreads::Thread::microSleep(ms);
                    //tcdrain(fileDescriptor);
                }
            }
            else 
            {
                if (errno == EAGAIN) {
                    std::cout << "trying again ..." << std::endl;
                    num_tries++;
                    if (num_tries > 10*30) 
                    {
                        log::error("SerialCommunication") << "Error writing to serial port "  <<  strerror(errno) << " (" << errno << ")" << std::endl;
                        _parent->setErrorOccured(true);
                        finished = true;
                    }
                } 
                else {
                    log::error("SerialCommunication") << "Error writing to serial port "  <<  strerror(errno) << " (" << errno << ")" << std::endl;
                    _parent->setErrorOccured(true);
                    finished = true;
                }
            } 
		}
	}
    
	void SerialImplementation::raw_read() {
		int numBytes = ::read(fileDescriptor, &(buffer[writeNdx]), BUFFER_SIZE-writeNdx-3);
		osg::notify(osg::DEBUG_INFO) << "(" << numBytes << ")";
		if (numBytes > 0) {
            // clean read \0 characters
            for(unsigned int i= writeNdx; i < writeNdx + numBytes; i++) {
                if (buffer[i] == 0) buffer[i] = NULL_REPLACEMENT;
            }
			writeNdx += numBytes;
			buffer[writeNdx] = 0;
		}
		
	}


	
    #endif

#pragma  mark WINDOWS implementation
 
#ifdef WIN32
//windows implementation

	SerialImplementation::SerialImplementation(SerialCommunication* parent) : _parent(parent) {
		writeNdx = readNdx = 0;
	}

	SerialImplementation::~SerialImplementation() {
	}


	bool SerialImplementation::connect(const std::string& port, unsigned int baud, unsigned int data, cefix::SerialCommunication::Parity parity, unsigned int stop, SerialCommunication::Handshake handshake) {
		
		LONG    lLastError = ERROR_SUCCESS;
		lLastError = serial.Open(_T(port.c_str()));

		if (lLastError != ERROR_SUCCESS) {
			log::error("SerialCommunication") << "SerialCommunication: can't open port: " << port << " " << serial.GetLastError() << std::endl;
			_parent->setErrorOccured(true);
			return false;
		}

		CSerial::EBaudrate sbaud;
		switch(baud) {
			case 110: 
				sbaud = CSerial::EBaud110;
				break;
			case 300:
				sbaud = CSerial::EBaud300;
				break;
			case 600:
				sbaud = CSerial::EBaud600;
				break;
			case 1200:
				sbaud = CSerial::EBaud1200;
				break;
			case 2400:
				sbaud = CSerial::EBaud2400;
				break;
			case 4800:
				sbaud = CSerial::EBaud4800;
				break;
			case 9600:
				sbaud = CSerial::EBaud9600;
				break;
			case 14400:
				sbaud = CSerial::EBaud14400;
				break;
			case 19200:
				sbaud = CSerial::EBaud19200;
				break;
			case 38400:
				sbaud = CSerial::EBaud38400;
				break;
			case 56000:
				sbaud = CSerial::EBaud56000;
				break;
			case 57600:
				sbaud = CSerial::EBaud57600;
				break;
			case 115200:
				sbaud = CSerial::EBaud115200;
				break;
			case 128000:
				sbaud = CSerial::EBaud128000;
				break;
			case 256000:
				sbaud = CSerial::EBaud256000;
			default:
				sbaud = CSerial::EBaud9600;
				log::error("SerialCommunication") <<  "unsupported baud rate: " << baud << std::endl;
				break;
		}

		CSerial::EDataBits sdata;
		switch (data) {
			case 7:
				sdata = CSerial::EData7;
				break;
			case 8:
				sdata = CSerial::EData8;
				break;
			default:
				sdata = CSerial::EData8;
				log::error("SerialCommunication") << "unsupported databits: " << data << std::endl;
				break;
		}

		CSerial::EStopBits sstop;
		switch (stop) {
			case 1:
				sstop = CSerial::EStop1;
				break;
			case 2:
				sstop = CSerial::EStop2;
				break;
			default:
				sstop = CSerial::EStop1;
				log::error("SerialCommunication") << "unsupported stopbits: " << stop << std::endl;
				break;
		}
		
		CSerial::EParity sparity;
		switch (parity) {
			case (SerialCommunication::SC_PARITY_NONE):
				sparity = CSerial::EParNone;
				break;

			case SerialCommunication::SC_PARITY_EVEN:
				sparity = CSerial::EParEven;
				break;

			case SerialCommunication::SC_PARITY_ODD:
				sparity = CSerial::EParOdd;
				break;
			
			default:
				sparity = CSerial::EParNone;
				log::error("SerialCommunication") << "unsupported parity: " << parity << std::endl;
		}

		CSerial::EHandshake shandshake;
		switch (handshake) {
			case SerialCommunication::SC_HANDSHAKE_NONE:
				shandshake = CSerial::EHandshakeOff;
				break;
			case SerialCommunication::SC_HANDSHAKE_RTS_CTS:
				shandshake = CSerial::EHandshakeHardware;
				break;
			case SerialCommunication::SC_HANDSHAKE_XON_XOFF:
				shandshake = CSerial::EHandshakeSoftware;
				break;
			default:
				shandshake = CSerial::EHandshakeOff;
				log::error("SerialCommunication") << "unsupported handshake: " << handshake << std::endl;
		}
		
		lLastError = serial.Setup(sbaud,sdata,sparity,sstop);
		if (lLastError != ERROR_SUCCESS) {
			log::error("SerialCommunication") << "can't open connection: " << serial.GetLastError() << std::endl;
			_parent->setErrorOccured(true);
			return false;
		}
		lLastError = serial.SetupHandshaking(CSerial::EHandshakeOff);
		if (lLastError != ERROR_SUCCESS) {
			log::error("SerialCommunication") << "can't setup handshake: " << serial.GetLastError() << std::endl;
			_parent->setErrorOccured(true);
			return false;
		}

		serial.Purge();

		return true;
	}

	void SerialImplementation::disconnect(){
		 serial.Close();
	}
	
	cefix::SerialCommunication::PortList SerialImplementation::getPortList() {
		cefix::SerialCommunication::PortList portlist;
		return portlist;
	}

	void SerialImplementation::raw_read() {
		LONG    lLastError = ERROR_SUCCESS;
		DWORD dwBytesRead = 0;
		lLastError = serial.Read(&(buffer[writeNdx]), BUFFER_SIZE-writeNdx-3, &dwBytesRead);
		if (lLastError != ERROR_SUCCESS) {
			log::error("SerialCommunication") << "error while reading from port: " << serial.GetLastError() << std::endl;
			_parent->setErrorOccured(true);
			return;
		}
		else {
            // clean read \0 characters
            for(unsigned int i= writeNdx; i < writeNdx + dwBytesRead; i++) {
                if (buffer[i] == 0) buffer[i] = NULL_REPLACEMENT;
            }
			writeNdx += dwBytesRead;
			buffer[writeNdx] = 0;
		}
	}

	void SerialImplementation::write(const std::string& str) {
		LONG    lLastError = ERROR_SUCCESS;
		lLastError = serial.Write(str.c_str(),str.length());
		if (lLastError != ERROR_SUCCESS) {
			log::error("SerialCommunication") << "error while writing to port: " << serial.GetLastError() << std::endl;
			_parent->setErrorOccured(true);
		}
	}





#endif

		std::string SerialImplementation::read(unsigned int tillToken) {
		if (readNdx >= writeNdx)
			raw_read();
			
		if (readNdx < writeNdx) {
			// there is something in our buffer, so return that first
			char* cptr =(char*)(&buffer[readNdx]);
			std::string result(cptr);
			if (tillToken == 0xFFFF) {
				if (readNdx > BUFFER_SIZE / 2) 
					readNdx = writeNdx = 0;
				else
					readNdx = writeNdx;
				return result;
			}
			else {
				std::string::size_type pos = result.find((char)tillToken);
				if (pos == std::string::npos) {
                    raw_read();
					//osg::notify(osg::DEBUG_INFO) << "["<< result << "]" << std::endl;
					return "";
				} 
				else {
					
					result = result.substr(0, pos);
										
                    if (readNdx >= writeNdx)  {
                        readNdx = writeNdx = 0;
                    }
                    else {
                        readNdx += pos + 1;
                        if (writeNdx > BUFFER_SIZE / 2) {
                            // buffer an den Anfang schieben, sodass readNdx = 0
                            memcpy(&buffer[0], &buffer[readNdx], writeNdx - readNdx);
                            writeNdx -= readNdx;
                            readNdx = 0;
                        }
					}
                    //osg::notify(osg::DEBUG_INFO) << "[" << readNdx  << "|" << writeNdx << "]" << std::endl;
					return result;
				}
			}
		}
		return "";
	}
	
	
	
	unsigned int SerialImplementation::bytesAvailable() {
		raw_read();
		return writeNdx - readNdx;
	}
    
#pragma  mark crossplattform class
    
    SerialCommunication::SerialCommunication() : osg::Referenced(), _connected(false), _handshake(SC_HANDSHAKE_XON_XOFF), _errorOccurred(false), _nonBlocking(true) {
        PortList portList = getPortList();
        if (portList.size() == 0) {
            log::error("SerialCommunication") << "no serial ports found" << std::endl;
        }
        else {
            setPort(portList[0]);
        }
		setParameters(9600);
        _implementation = new SerialImplementation(this);
    }
    
    SerialCommunication::SerialCommunication(const std::string& port, unsigned int baud, unsigned int databits, Parity parity, 
                    unsigned int stopbits, Handshake handshake, bool non_blocking) 
    :   osg::Referenced(), 
        _connected(false), 
        _handshake(handshake), 
        _errorOccurred(false),
        _nonBlocking(non_blocking)
    {
        _implementation = new SerialImplementation(this);
        setPort(port);
        setParameters(baud, databits, parity, stopbits, handshake);
        
    }
    
    bool SerialCommunication::connect() {
		_errorOccurred = false;
        _connected = _implementation->connect(getPort(), getBaudrate(), getDataBits(), getParity(), getStopBits(), getHandshakeMode());
        return _connected;
    }
    
    
    SerialCommunication::PortList SerialCommunication::getPortList() {
    
        return  SerialImplementation::getPortList(); 
    }
	
	std::string SerialCommunication::findPort(const std::string& partialName) {
		PortList portlist = getPortList();
		for(PortList::iterator i = portlist.begin(); i != portlist.end(); i++) {
			std::string::size_type pos  = (*i).find(partialName);
			if (pos != std::string::npos) {
				return (*i);
			}
		}
		return "";
	}
	
	unsigned int SerialCommunication::bytesAvailable() {
		if (_connected)
			return _implementation->bytesAvailable();
		else
			return 0;
	}
    
    std::string SerialCommunication::readTillToken(unsigned char token) {
        return _implementation->read(token);
    }
	
	std::string SerialCommunication::read() {
		return _implementation->read();
	}
	
	std::string SerialCommunication::readLine() {
		return _implementation->readLine();
    }
	
	void SerialCommunication::write(const std::string& data) {
		_implementation->write(data);
	}
	
    void SerialCommunication::disconnect() {
        if (_connected) {
            _implementation->disconnect();
            _connected = false;
			_errorOccurred = false;
        }
    }
    
    SerialCommunication::~SerialCommunication() { 
        if (_connected) 
            disconnect(); 
        delete _implementation;
    }
}

std::ostream& operator<<(std::ostream& os, const cefix::SerialCommunication& comm)
{
    os << comm.getPort() << " (" << comm.getBaudrate() << "/" << comm.getDataBits()<<"/";
    switch(comm.getParity()) {
        case  cefix::SerialCommunication::SC_PARITY_NONE:
            os << "NONE";
            break;
        case  cefix::SerialCommunication::SC_PARITY_ODD:
            os << "ODD";
            break;
        case  cefix::SerialCommunication::SC_PARITY_EVEN:
            os << "EVEN";
            break;
    }
    os  << "/" << comm.getStopBits() << "/";
    switch(comm.getHandshakeMode()) {
        case  cefix::SerialCommunication::SC_HANDSHAKE_RTS:
            os << "rts";
            break;
        case  cefix::SerialCommunication::SC_HANDSHAKE_CTS:
            os << "cts";
            break;
        case  cefix::SerialCommunication::SC_HANDSHAKE_RTS_CTS:
            os << "rts/cts";
            break;
        case  cefix::SerialCommunication::SC_HANDSHAKE_XON_XOFF:
            os << "xon/xoff";
            break;
        case  cefix::SerialCommunication::SC_HANDSHAKE_BOTH:
            os << "both handshake";
            break;
        case  cefix::SerialCommunication::SC_HANDSHAKE_NONE:
            os << "no handshake";
    }
    os << ")";
    return os;
}	
		

