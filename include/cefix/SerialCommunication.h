/*
 *
 *      (\(\
 *     (='.')           cefix --
 *    o(_")")           a multipurpose library on top of OpenSceneGraph
 *  
 *
 *                      Copyright 2001-2011, stephanmaximilianhuber.com 
 *                      Stephan Maximilian Huber. 
 *
 *                      All rights reserved.
 *
 */

#ifndef SERIAL_HEADER_
#define SERIAL_HEADER_

#include <cefix/Export.h>

#include <string>
#include <vector>
#include <osg/Referenced>

namespace cefix {

    class SerialImplementation;
    class CEFIX_EXPORT SerialCommunication : public osg::Referenced {
    
        public:
            enum Parity {SC_PARITY_NONE = 1, SC_PARITY_ODD, SC_PARITY_EVEN};
            enum Handshake { SC_HANDSHAKE_RTS, SC_HANDSHAKE_CTS, SC_HANDSHAKE_RTS_CTS, SC_HANDSHAKE_XON_XOFF, SC_HANDSHAKE_BOTH, SC_HANDSHAKE_NONE };
            typedef std::vector<std::string> PortList;
            
            SerialCommunication();            
            SerialCommunication(const std::string& port, unsigned int baud, unsigned int databits=8, Parity parity=SC_PARITY_NONE, 
                            unsigned int stopbits = 1, Handshake handshake = SC_HANDSHAKE_XON_XOFF, bool non_blocking = true);
                                        
            void setParameters(
				unsigned int baud, 
				unsigned int databits = 8 , 
				Parity parity = SC_PARITY_NONE, 
				unsigned int stopbits = 1,
				Handshake handshake = SC_HANDSHAKE_XON_XOFF
			) {
                setBaudrate(baud);
                setDataBits(databits);
                setParity(parity);
                setStopBits(stopbits);
				setHandShakeMode(handshake);
            }
            
            inline void setPort(const std::string& port) { _port = port; }
            inline const std::string& getPort() const { return _port; }
            
            static PortList getPortList(); 
			static std::string findPort(const std::string& partialName);
            
            inline void setBaudrate(unsigned int rate) { _baudRate = rate; }
            inline unsigned int getBaudrate() const { return _baudRate; }
            
            inline void setParity(Parity parity) { _parity = parity; }
            inline Parity getParity() const { return _parity; }
            
            inline void setDataBits(unsigned int bits) { _dataBits = bits; }
            inline unsigned int getDataBits() const { return _dataBits; }
            
            inline Handshake getHandshakeMode() const { return _handshake; }
            inline void setHandShakeMode(Handshake flag) { _handshake = flag; }
            
            inline void setStopBits(unsigned int bits) { _stopBits = bits; }
            inline unsigned int getStopBits() const { return _stopBits; }
            
            inline bool isConnected() const { return _connected; }
            
            bool connect();
            void disconnect();
            
            void write(const std::string& str);
            void writeLine(const std::string& str) { write(str + "\n"); }
            unsigned int bytesAvailable();
            std::string readLine();
            std::string read();
            std::string readTillToken(unsigned char token);
            
			bool errorOccurred() { return _errorOccurred; }

			void setErrorOccured(bool f) { _errorOccurred = f; }
            
            bool isNonBlocking() { return _nonBlocking; }
        
        protected:
            virtual ~SerialCommunication();
            bool                    _connected;
            unsigned int            _baudRate;
            unsigned int            _dataBits, _stopBits;
            Parity                  _parity;
            std::string             _port;
            SerialImplementation*   _implementation;
            Handshake               _handshake;
			bool					_errorOccurred, _nonBlocking;
    };


}

std::ostream& operator<<(std::ostream& os, const cefix::SerialCommunication& comm);

#endif