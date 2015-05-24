/*
 *  DataCache.cpp
 *  AudiHeritage
 *
 *  Created by Stephan Huber on 15.05.06.
 *  Copyright 2006 digitalmind.de // Stephan Maximilian Huber. All rights reserved.
 *
 */

#include "DataCache.h"
#include <cefix/PropertyList.h>

namespace cefix {

// --------------------------------------------------------------------------------------------------------------------
// get
// --------------------------------------------------------------------------------------------------------------------

AbstractDataContainer* DataCache::get(const std::string& dataFile, const std::string& key) {
    DataCacheMap::iterator itr = _map.find(_getKey(dataFile, key));
    //osg::notify(osg::ALWAYS) << "DataCache::get file: " << dataFile << " key: " << key << ((itr == _map.end()) ? " not found" : " found") << std::endl;
    return (itr == _map.end()) ? NULL : itr->second.get();
}


// --------------------------------------------------------------------------------------------------------------------
// put
// --------------------------------------------------------------------------------------------------------------------

void DataCache::put(const std::string& dataFile, const std::string& key, AbstractDataContainer* container) {
    //osg::notify(osg::ALWAYS) << "DataCache::put file: " << dataFile << " key: " << key << std::endl;
    _map[_getKey(dataFile, key)] = container;
}

/** removes an item from the cache */
bool DataCache::remove(const std::string& dataFile, const std::string& key) 
{
	DataCacheMap::iterator itr = _map.find(_getKey(dataFile, key));
	if (itr != _map.end()) {
		_map.erase(itr);
		return true;
	} else if (_verboseLogging) {
		cefix::log::info("DataCache") << "could not erase " << _getKey(dataFile, key) << ", key not found" << std::endl;
		
		{
			std::ostream& os = cefix::log::info("DataCache");
			for(DataCacheMap::iterator i = _map.begin(); i != _map.end(); ++i) {
				os << i->first << " ";
			}
			os << std::endl;
		}
	}
	return false;
}

}