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

#ifndef ABSTRACT_DATA_CONTAINER_
#define ABSTRACT_DATA_CONTAINER_

#include <vector>
#include <string>
#include <cefix/Export.h>
#include <osg/Referenced>
#include <osg/ref_ptr>

namespace cefix {

class PropertyList;
/** 
 * this class defines a abstract structure for data-containers loaded by the data-factory.
 * the data is loaded asynchronously, so request the loading and wait until the loading-process is finished. 
 * You can monitor the progress with loadSucceeded()
 */
class CEFIX_EXPORT AbstractDataContainer: public osg::Referenced {

    public:
	
		typedef std::vector<std::string>  ErrorVector;
        
        /** default constructor */
        AbstractDataContainer(); 
        
        /** requst a load of the data, without that call no data gets loaded */
        void requestLoad();
        
        /** @return true if all data has loaded */
        bool loadSucceeded() const { return _loadFinished; }
        
        /** you can discard the loaded data with this call, you have to implement this method in you derived class. 
         *  Be sure to reset the LoadFlag to false */
        virtual void discard() = 0;
        
        /** @return a unique ID of this object */
        unsigned int getID() const { return _id; }
        
        /** @return the key of this object */
        std::string getKey() const { return _key; }
        
        /** sets the key of this object */
        void setKey(std::string key) { _key = key; }
        
        /** cancel the loading */
        void cancel() { _cancelled = true; }
        
        /** @return true, if the load should be cancelled */
        bool isCancelled() const { return _cancelled; }
		
		/** returns the filename */
		std::string getFileName() const;
    
    protected:
        
        /** implement the loading stuff with this method. it is called from the LoadQueue, when the data should get loaded */
        virtual void load() = 0;
       
         /** sets the loadFlag to true, marking this object as loaded completedly */
        void setLoadFlag() { _loadFinished = true; }
        
        /** clears the load-flag, so this object gets reloaded, when requested */
        void clearLoadFlag() { _loadFinished = false; }
        
        /** sets the queued-flag (called from LoadQueue) */
        void setQueued(bool flag) { _queued = flag; }
       
        /** check if container is already in laodqueue */
        bool isQueued() const { return _queued; }
        
        /** @return true if this container needs to be loaded asynchronously (default) */
        virtual bool needsQueued() { return true; }
        
        /** sets the propertylist for this container */
        bool setPropertyList(cefix::PropertyList* pl);
        
		
		/** sends a errormessage to the errorlog */
        void error(std::string err_msg);  
		
		typedef std::vector<std::string> ErrorMessageVector;
		void error(std::string msg, const ErrorMessageVector& err_msg_vec);
		
		/** @return # errors */
		unsigned int getErrorCount() {
			return _errors.size();
		}
                
		/** @return returns the vector with all error-messages */
		ErrorVector& getErrors() { return _errors; }
		
		/** clear any pending error-messages */
		void clearErrors() { _errors.clear(); }
		

		      
    protected:
	
		virtual ~AbstractDataContainer();
	
		/** implement this method to check your propertylist-data. If you return false from here, the container 
		 *  gets deleted and the caller gets an empty container.
		 */
		virtual bool isPropertyListApplicable(cefix::PropertyList* pl) { return true; }
	
        bool            _loadFinished;
        osg::ref_ptr<cefix::PropertyList> _pl;
    private:
        bool            _queued;
        unsigned int    _id;
        std::string     _key;
		ErrorVector		_errors;
        bool            _cancelled;
        
        
    friend class LoadQueue;
    friend class DataFactory;
    

};

}

#endif