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

#ifndef ERROR_DISPATCHER_HEADER
#define ERROR_DISPATCHER_HEADER

#include <vector>
#include <string>
#include <cefix/Export.h>

namespace cefix {

/** 
 * small helper class, which will dispatch errormessages and sends them via NotificationCenter
 */
class CEFIX_EXPORT ErrorDispatcher {
	public:
		enum Mode { FASTEST, THREAD_SAFE};
		typedef std::vector<std::string> ErrorVector;
		
		/**ctor @param identifier an identifier, the sending object for example */
		ErrorDispatcher(std::string identifier, Mode mode = FASTEST) : _identifier(identifier), _mode(mode) {}
		
		/** handle @param msg sends the error-text */
		void handle(std::string msg) const;		
		/** handles a vector of error-messages */
		void handle(std::string msg, const ErrorVector& vec) const {
			for(ErrorVector::const_iterator i = vec.begin(); i != vec.end(); i++) 
				handle( (msg.empty()) ? (*i) : msg+": "+(*i) );
		}
		
	private:
		std::string _identifier;
		Mode		_mode;
};

}

#endif