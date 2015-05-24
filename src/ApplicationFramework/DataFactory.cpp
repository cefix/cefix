/*
 *  DataDactory.cpp
 *  AudiHeritage
 *
 *  Created by Stephan Huber on 11.05.06.
 *  Copyright 2006 digitalmind.de // Stephan Maximilian Huber. All rights reserved.
 *
 */

#include "DataFactory.h"
#include "ErrorDispatcher.h"
#include <cefix/Log.h>

#include <osgDB/FileNameUtils>
#include <osgDB/FileUtils>

#include <cefix/MissingImagePlaceHolderContainer.h>
#include <cefix/Notify.h>
#include <cefix/Serializer.h>
#include <cefix/DataCache.h>
#include <cefix/LoadQueue.h>

namespace cefix {

/** small error-handler class for the propertylist-class, so that errors while parsing the xml-files are forwarded to the right classes */
class DataErrorHandler : public cefix::PropertyList::ErrorHandler {

	virtual void handle(std::string msg) {
		
		ErrorDispatcher dispatcher("Data",ErrorDispatcher::THREAD_SAFE);
		dispatcher.handle(msg);
		log::error("DataFactory") << msg << std::endl;
	}
};


// --------------------------------------------------------------------------------------------------------------------
// Konstruktor
// --------------------------------------------------------------------------------------------------------------------

DataFactory::DataFactory() : 
	osg::Referenced(),
	_mediaFolderName("media") 
{
    _notifyLevel = osg::DEBUG_INFO;
    _cache = new DataCache();
	
	// unseren ErrorHandler installieren
	cefix::PropertyList::ErrorHandler::instance(new DataErrorHandler());
}

void DataFactory::initLoadQueue() 
{
	_loadQueue = new LoadQueue();
    _loadQueue->start();
}

void DataFactory::addToLoadQueue(AbstractDataContainer* data) 
{
	if(!data->needsQueued()) {
        // this datacontainer does not want to get queued, load it immeadetly
        if (!data->loadSucceeded()) {
            data->load();
            data->setLoadFlag();
        }
        return;
    }
	if (!_loadQueue.valid())
		initLoadQueue();
	_loadQueue->add(data);
}


unsigned int DataFactory::getRemainingItemsToLoad() 
{ 
	return _loadQueue.valid() ? _loadQueue->getRemainingItems() : 0; 
}
        
void DataFactory::pauseLoadQueue(bool flag) 
{ 
	if (_loadQueue.valid()) _loadQueue->setPauseFlag(flag); 
}

void DataFactory::setLoadQueuePriority(OpenThreads::Thread::ThreadPriority p) 
{ 
	if (_loadQueue.valid()) _loadQueue->setSchedulePriority(p); 
}

bool DataFactory::releaseData(const std::string& key, const std::string& file) {
	std::string filename = (file.empty()) ? _defaultDataFile : file;
	_mutex.lock();
	bool result = _cache->remove(filename, key);
	_mutex.unlock();
	return result;
}

AbstractDataContainer* DataFactory::getFromCache(const std::string& filename, const std::string& key) 
{
	return _cache->get(filename, key);
}

void DataFactory::putIntoCache(const std::string& filename, const std::string& key, AbstractDataContainer* container) 
{
	return _cache->put(filename, key, container);
}

// --------------------------------------------------------------------------------------------------------------------
// Destruktor
// --------------------------------------------------------------------------------------------------------------------

DataFactory::~DataFactory() {
    cleanUp();
}

void DataFactory::cleanUp() 
{
	_mutex.lock();
	
	for(CleanupHandlerVector::iterator i = _cleanupHandler.begin(); i != _cleanupHandler.end(); ++i) 
	{
		(*i)();
	}
	_cleanupHandler.clear();
	
	if (_loadQueue.valid()) 
	{
        _loadQueue->quit();
		_loadQueue = NULL;
    }
	
	if (_cache.valid()) 
		_cache->clear();
	_mutex.unlock();
}


// --------------------------------------------------------------------------------------------------------------------
// instance -- liefert die Instanz zurück
// --------------------------------------------------------------------------------------------------------------------

DataFactory* DataFactory::instance() {
    static osg::ref_ptr<DataFactory> s_data_factory = new DataFactory();
    
    return s_data_factory.get();
}


// --------------------------------------------------------------------------------------------------------------------
// richtet osg soweit ein, das bilder tex im media-ordner gefunden werden können
// --------------------------------------------------------------------------------------------------------------------

void DataFactory::setApplicationPath(const std::string& app_path, const std::string& mediaFolderName) {

	_mediaFolderName = mediaFolderName;
    _rootPath = _appPath = _mediaPath = "";
    
	std::string path = osgDB::getFilePath(app_path);
	
	_appName = osgDB::getSimpleFileName(osgDB::getNameLessExtension(app_path));
    
    #ifdef __APPLE__
        path = osgDB::getFilePath(path);
        path = osgDB::getFilePath(path);
        path = osgDB::getFilePath(path);
    #endif
	
	_rootPath = _appPath = path;
	
    std::string mediaPath = osgDB::findFileInDirectory(_mediaFolderName, path);
    
	// ok, sehen wir mal im bundle nach.
	if (mediaPath.empty()) 
		mediaPath = osgDB::findDataFile(_mediaFolderName);
		
	// auch nichts? dann gehen wir mal davon aus, dass wir in einem build-directory sitzen
    
    if (mediaPath.empty()) 
    {
        // ok, media-ordner befindet sich nicht auf gleicher ebene wie die applikation
		std::string ppath = path;
        
        #ifdef __APPLE__
        int max_depth = 6;
        #else
        int max_depth = 4;
        #endif
        
        bool media_path_found(false);
        
        while(!media_path_found && (max_depth >= 0)) 
        { 
            ppath = osgDB::getFilePath(ppath);
            mediaPath = osgDB::findFileInDirectory(_mediaFolderName, ppath);
            
            if (mediaPath.empty()) 
            {
                --max_depth;
            } else {
                media_path_found = true;
            }
        }
            
        if (mediaPath.empty()) {
            osg::notify(_notifyLevel) << "DataFactory:: could not locate media-folder! " << _mediaFolderName << std::endl;
        } else {
			// wir haben einen media-path gefunden, deswegen passen wir den Rootpath an
			_rootPath = ppath;
		}
	}
            
    if (!mediaPath.empty()) {
          
        setMediaPath(mediaPath);
		
    }  
	
	osgDB::Registry::instance()->getDataFilePathList().push_back(_rootPath);
    
    cefix::log::info("DataFactory") << "app-path   : " << _appPath << std::endl;
    cefix::log::info("DataFactory") << "root-path  : " << _rootPath << std::endl;
    cefix::log::info("DataFactory") << "media-path : " << _mediaPath << std::endl;
    
}

void DataFactory::setMediaPath(const std::string& mediaPath)
{
	_mediaPath = mediaPath;
	osgDB::Registry::instance()->getDataFilePathList().push_back(_mediaPath); 
}


// ----------------------------------------------------------------------------------------------------------
// setRootPath
// ----------------------------------------------------------------------------------------------------------

void DataFactory::setRootPath(const std::string& path) 
{
	osgDB::FilePathList& dl = osgDB::Registry::instance()->getDataFilePathList();
	if (!_rootPath.empty()) {
		// alten rootpath entsorgen
		for(osgDB::FilePathList::iterator i = dl.begin(); i != dl.end();) {
			if (*i == _rootPath)
				i =dl.erase(i);
			else
				++i;
		}
		
	}
	
	if (!_mediaPath.empty()) {
		for(osgDB::FilePathList::iterator i = dl.begin(); i != dl.end();) {
			if (*i == _mediaPath)
				i =dl.erase(i);
			else
				++i;
		}
	}
	
	osgDB::Registry::instance()->getDataFilePathList().push_back(_rootPath);
	
	_rootPath = path;
	std::string mediaPath = osgDB::findFileInDirectory(_mediaFolderName, path);
	
	if (!mediaPath.empty()) {
		_mediaPath = mediaPath;
		osgDB::Registry::instance()->getDataFilePathList().push_back(_mediaPath); 
	}

}


// --------------------------------------------------------------------------------------------------------------------
// _readPreferences -- liest die Prefs-Datei
// --------------------------------------------------------------------------------------------------------------------

void DataFactory::_loadPreferences() {
    
    _prefs = new cefix::PropertyList();
    if (!_prefs->loadFromXML("preferences.xml")) {
        osg::notify(_notifyLevel) << "DataFactory:: could not locate preferences.xml" << std::endl;
    }

    resolveInheritedPreferences(_prefs.get());
    _defaultDataFile = getPreferencesFor<std::string>("dataFile", "index.xml");
}


void DataFactory::resolveInheritedPreferences(cefix::PropertyList* pl) {

	for(std::vector<std::string>::iterator key = _prefsKeyToCheckInheritance.begin();key != _prefsKeyToCheckInheritance.end(); ++key) {
	
		cefix::AbstractProperty* p = _prefs->find((*key));
		if (p && (p->getType() == AbstractProperty::PROPERTY_LIST)) {
			cefix::PropertyList* pl = p->asPropertyList();
			for(unsigned int i = 0; i < pl->size(); ++i) {
				
				if (pl->get(i)->getType() == AbstractProperty::PROPERTY_LIST) {
					
					cefix::PropertyList* subpl = pl->get(i)->asPropertyList();
					if (subpl->hasKey("inheritFrom")) {
						std::string sub_pl_key = *key + "/" + subpl->get("inheritFrom")->asString();
						cefix::AbstractProperty* other_pl = _prefs->find(sub_pl_key);
						if (other_pl) 
							subpl->addNonExistingKeys(other_pl->asPropertyList());
					}
					
				}
			}
				
		}
		else
			osg::notify(_notifyLevel) << "DataFactory:: could not check inheritance for prefs-key " << *key << std::endl;
	
	}

}



/** 
 * an datacontainer for the xml-files to be loaded. this container does net get queued, the content of the xml-files is
 * loaded immeadetly.
 */
class XmlFileDataContainer : public AbstractDataContainer {
    public:
        /** @return the loaded propertylist */
        cefix::PropertyList* getData() { return _data.get(); }
        /** discard the loaded data -- not implemented and needed */
        virtual void discard() {}
        
    protected:
        /** disable queuing the load */
        virtual bool needsQueued() { return false; }
        
        /** load the xmlfile into a propertylist */
        virtual void load() {
            
            _data = new cefix::PropertyList();
			_data->setReadXmlFileCallback(cefix::DataFactory::instance()->getReadXmlFileCallbackForData());
            bool success = _data->loadFromXML(getKey());
            if (!success) {
                error("Could not load xml-file: " + getKey());
            }
            
        }
        
        osg::ref_ptr<cefix::PropertyList> _data;
};


// --------------------------------------------------------------------------------------------------------------------
// 
// --------------------------------------------------------------------------------------------------------------------

cefix::AbstractProperty* DataFactory::getDataProperty(std::string key, std::string file) {
    std::string filename = (file.empty()) ? _defaultDataFile : file;
    cefix::AbstractProperty* p = NULL;
    
    _mutex.lock();
    XmlFileDataContainer* filecontainer = _getDataFor<XmlFileDataContainer>(filename, "__INTERNAL");
    if ((filecontainer) && filecontainer->getData()) {
        cefix::PropertyList* pl = filecontainer->getData();
        p = pl->find(key);
    }
    _mutex.unlock();
    return p;
    
}



cefix::PropertyList* DataFactory::getPropertyListForFile(std::string file) {
	
	std::string filename = (file.empty()) ? _defaultDataFile : file;
	
	// here is the filename the key, and the filename an internal value ...  
    XmlFileDataContainer* filecontainer = _getDataFor<XmlFileDataContainer>(filename, "__INTERNAL");

    if ((filecontainer) && filecontainer->getData()) {
        cefix::PropertyList* pl = filecontainer->getData();
        return pl; 
	}
    
    return NULL;
}




// --------------------------------------------------------------------------------------------------------------------
// _loadDataFromXmlForKey
// --------------------------------------------------------------------------------------------------------------------

cefix::PropertyList* DataFactory::_loadDataFromXmlForKey(const std::string& key, const std::string& filename) { 
    
    if (filename == "__INTERNAL") {
        // we want to load a xml-file, so return an empty propertylist.. (we are called recursively)
        return new cefix::PropertyList();
    }
    // here is the filename the key, and the filename an internal value ...  
    XmlFileDataContainer* filecontainer = _getDataFor<XmlFileDataContainer>(filename, "__INTERNAL");

    if ((filecontainer) && filecontainer->getData()) {
        cefix::PropertyList* pl = filecontainer->getData();
        
        if (pl) {
            cefix::AbstractProperty* p = pl->find(key);
            if ((p) && (p->getType() == cefix::AbstractProperty::PROPERTY_LIST)) {
                cefix::PropertyList* spl = dynamic_cast<cefix::PropertyList*>(p->asObject());
                return spl;
            }
            
        }        
        
    }
    
    return NULL;
}

osg::Image* DataFactory::getPlaceHolderImage() {
	osg::Image* image = NULL;
	MissingImagePlaceHolderContainer* miphc = _getDataFor<MissingImagePlaceHolderContainer>("MissingImagePlaceHolder", "preferences.xml");
	if (miphc)
		image = miphc->getPlaceholder();
	
	return image;
}

// ----------------------------------------------------------------------------------------------------------
// removePreferences
// ----------------------------------------------------------------------------------------------------------
bool DataFactory::removePreferencesFor(const std::string& key) {

	std::string path = osgDB::getFilePath(key);
	std::string lastkey = osgDB::getSimpleFileName(key);
	cefix::AbstractProperty* p = _prefs->find(path);
	if (p && p->getType()==cefix::AbstractProperty::PROPERTY_LIST) {
		p->asPropertyList()->remove(lastkey);
		return true;
	} else
		return false;
			
	return true;
}

// ----------------------------------------------------------------------------------------------------------
// addPreferences
// ----------------------------------------------------------------------------------------------------------

bool DataFactory::addPreferencesFor(const std::string& key, cefix::AbstractProperty* property) 
{
	std::string path = osgDB::getFilePath(key);
	std::string lastkey = osgDB::getSimpleFileName(key);
	
	typedef std::vector<std::string> PathVector;
	PathVector paths;
	cefix::strTokenize(path, paths, "/");

	cefix::PropertyList* pl = _prefs.get();
	for (PathVector::iterator i = paths.begin(); i != paths.end(); ++i) 
	{
		if (pl->hasKey(*i)) 
		{
			cefix::AbstractProperty* p = pl->get(*i);
			if (p->getType() == cefix::AbstractProperty::PROPERTY_LIST) 
			{
				pl = p->asPropertyList();
			} else {
				// ups, was ist da jetzt passiert?
				log::error("DataFactory") << "fuck, schau in den code " << std::endl;
				return false;
			}
		} 
		else 
		{
			cefix::PropertyList* new_pl = new cefix::PropertyList();
			pl->add(*i, new_pl);
			pl = new_pl; 
		}
	}
	
	pl->addValue(lastkey, property);
	return true;
}

void DataFactory::savePreferencesFile() 
{
	cefix::notify("savePreferences");	
	_prefs->saveAsXML(_prefs->getXmlFileName());
}

Serializer DataFactory::createSerializerForPreferences() 
{
	Serializer serializer(_prefs.get());
	return serializer;
}


bool DataFactory::addPreferencesFromFolderContents(const std::string folder) 
{
	std::string filepath = osgDB::findDataFile(folder);
	if (filepath.empty())
		return false;
		
	osgDB::DirectoryContents contents = osgDB::getDirectoryContents(filepath);
	for(osgDB::DirectoryContents::iterator i = contents.begin(); i != contents.end(); ++i) 
	{
		std::string file = filepath + "/" + (*i);
		if (osgDB::getFileExtension(file) == "xml") {
			osg::ref_ptr<cefix::PropertyList> pl = new cefix::PropertyList(file);
			if (pl->size() > 0) {
				std::string key = osgDB::getSimpleFileName(osgDB::getNameLessAllExtensions(file));
				cefix::log::info("DataFactory") << "added file " << file << " with key " << key << "as preferences" << std::endl;
				_prefs->add(key, pl.get());
			}
		}
	}
	
	return true;
}


}

    
