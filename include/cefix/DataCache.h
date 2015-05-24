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

#ifndef DATA_CACHE_HEADER_
#define DATA_CACHE_HEADER_

#include <string>
#include <map>

#include <osg/Referenced>
#include <cefix/AbstractDataContainer.h>

namespace cefix {
/** this class implements a caching structure for laoded data-containers */

class CEFIX_EXPORT DataCache: public osg::Referenced {

    public:
        typedef std::map<std::string, osg::ref_ptr< AbstractDataContainer > > DataCacheMap;
        /** default constructor */
        DataCache() : osg::Referenced(), _verboseLogging(false) {}
        
        /** @return true if the cache has an item of file dataFile and key */
        bool hasKey(const std::string& dataFile, const std::string& key) const {
            return (_map.find(_getKey(dataFile, key)) != _map.end());
        }
        /** @return NULL if the item could not be found or the found item */
        AbstractDataContainer* get(const std::string& dataFile, const std::string& key);
        
        /** adds an item to the cache
         * @param dataFile filename of the datafile
         * @param key key of the object
         * @param container the data-container
         */
        void put(const std::string& dataFile, const std::string& key, AbstractDataContainer* container);
		
		/** removes an item from the cache */
		bool remove(const std::string& dataFile, const std::string& key);
		
		/** clears the cache */
		void clear() { _map.clear(); }
		
		void setVerboseLogging(bool f) { _verboseLogging = f; }
        
    protected:
		virtual ~DataCache() {}
        /** computes the key for the map */
        inline std::string _getKey(const std::string& filename, const std::string&key) const { return filename+"::"+key; }
        DataCacheMap    _map;
		bool _verboseLogging;

};

}


#endif