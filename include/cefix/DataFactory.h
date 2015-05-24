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

#ifndef DATA_FACTORY_HEADER
#define DATA_FACTORY_HEADER

#include <string>
#include <list>
#include <osg/Referenced>
#include <osg/Image>
#include <cefix/PropertyList.h>
#include <OpenThreads/Thread>
#include <OpenThreads/Mutex>
#include <cefix/AbstractDataContainer.h>
#include <osg/Image>

/**
 * this singleton class encapsulates the data-handling. Objects can retrieve preferences and data from this class
 * the singleton caches previously loaded data
 */

namespace cefix {

class AppController;
class Serializer;
class LoadQueue;
class DataCache;

class CEFIX_EXPORT DataFactory : public osg::Referenced {

    public:
		typedef std::vector< void(*)() > CleanupHandlerVector;
                         
        /** returns the instance of this singleton */
        static DataFactory* instance();
        
        /** 
         * this method should be called in your main-method, so the object knows from where it can retrieve the data.
         * It sets the osgDB::dataPaths accordingly
         */
        void setApplicationPath(const std::string& path, const std::string& mediaFolderName = "media");
		
		std::string getApplicationName() { return _appName; }
		
		void setRootPath(const std::string& path);
        
        /** get the Property for a given preferences.key
         * @param key the key can be a path describing the hierarchy to get the property-value /tag1/tag2/tag3
         * @return NULL if the preferences could not be found or the prefs-value is an AbstractProperty 
         */
        cefix::AbstractProperty* getPreferencesFor(std::string key) {
            if (!_prefs.valid())
                _loadPreferences();
            if (_prefs.valid())
                return _prefs->find(key);
            else
                return NULL;
        }
		
		/** 
		 * get a propertylist from a preferencesFile for a given key 
		 */
		cefix::PropertyList* getPreferencesPropertyListFor(std::string key) {
			cefix::AbstractProperty* p = getPreferencesFor(key);
			return (p) ? p->asPropertyList() : NULL;
		}
        
        /**
         * gets a preferences for a specifc key, casts it to the right type. If the pref-key cannot be found, then a 
         * defaultvalue gets returned
         * @param key key of the prefernece to look for
         * @param defaultValue value to return, if the specified key cannot be found
         */ 
        template <class T> T getPreferencesFor(const std::string& key, T defaultValue) {
            cefix::AbstractProperty* p = getPreferencesFor(key);
            if (NULL != p) {
                // osg::notify(osg::ALWAYS) << key << " type: " << p->getType() << " value: " << p->asString() << std::endl;
                cefix::PropertyT<T>* casted_p = dynamic_cast< cefix::PropertyT<T>* >(p);
                if (casted_p) {
                    return casted_p->data();
				}
				else{
					T value;
					if (tryCastPreferenceValue<T>(p, value)) 
						return value;
					else 
						osg::notify(_notifyLevel) << "could not cast " << key << "  " << std::endl;
				}
            }
            
            //fallback:
            osg::notify(_notifyLevel) << "DataFactory::getPreferencesFor :: using defaults for key " << key << std::endl;
            return defaultValue;
        }
		
		/** 
		 * gets the value for a specific key, the value is used to look up the real value in a std::map, where the value is used as a key
		 * if the value is found in the map the corresponding value is returned, otherwise the default value
		 */
		template <typename T> T getPreferencesFor(const std::string& key, const std::map<std::string, T>& m, T defaultValue) 
		{
			std::string value = getPreferencesFor(key, "");
			typename std::map<std::string, T>::const_iterator itr = m.find(cefix::strToUpper(value));
			return (itr == m.end()) ? defaultValue : itr->second;
		}
		
		/** small helper method for getting prefs with a const char default value */
		std::string getPreferencesFor(const std::string& key, const char* defaultValue) {
			return getPreferencesFor<std::string>(key, defaultValue);
		}
		
		/** sets a key-value pair, don't forget to call savePreferences to save the changed preferences */
		template <typename T>bool setPreferencesFor(const std::string& key, T value, const std::string& xmlType = "", bool allowTypeChanges = false) 
		{
			cefix::AbstractProperty* p = getPreferencesFor(key);
			if (NULL != p) 
			{
				// wir haben ne prefs gefunden
				cefix::PropertyT<T>* castedP = dynamic_cast<cefix::PropertyT<T>*>(p);
				if (castedP) 
				{
					castedP->setData(value);
					return true;
				} else {
					if (!allowTypeChanges) 
					{
						osg::notify(_notifyLevel) << "AppController::setPreferencesFor :: could not set prefs for key '" << key << "' because of different types" << std::endl;
						return false;
					}
				}
			}
			
			// remove the old value and add the new value to the prefs-list
			removePreferencesFor(key);
			osg::ref_ptr<  cefix::PropertyT<T> > newprop = new cefix::PropertyT<T>(value);
			newprop->setXmlType(xmlType);
			return addPreferencesFor(key, newprop.get());
				
		}
		
		
		bool setPreferencesFor(const std::string& key, const char* value, const std::string& xmlType = "", bool allowTypeChanges = false) 
		{
			return setPreferencesFor<std::string>(key, value, xmlType, allowTypeChanges);
		}
		
		
		
		bool setPreferencesFor(const std::string& key, cefix::PropertyList* value, const std::string& xmlType = "", bool allowTypeChanges = false) 
		{
			return setPreferencesFor<osg::Referenced*>(key, value, xmlType, allowTypeChanges);
		} 
		
		/** removes a preference-value with specified key */
		bool removePreferencesFor(const std::string& key);
		
		/** save the preferences back to a file */
		void savePreferencesFile();
		
		/** creates a serializer-object referencing the prefs */
		Serializer createSerializerForPreferences();
		
        /** @return  the root-path, where the application/media-folder resides */
        const std::string& getRootPath() const { return _rootPath; }
		
		const std::string& getApplicationPath() const { return _appPath; }
		
        /** @return the media-path, where all media-files are located */
        std::string getMediaPath() const { return _mediaPath; }
		
		/** set the media path */
		void setMediaPath(const std::string& path);
        
		/** @return a place holder image */
		osg::Image* getPlaceHolderImage();
        
        /** @return the count of items to load */
        unsigned int getRemainingItemsToLoad();
        
		/** pauses the loadqueue */
		void pauseLoadQueue(bool flag);
		
		void setLoadQueuePriority(OpenThreads::Thread::ThreadPriority p);
		
        /** loads the propertylist from file and returns the found property,otherwise NULL */
        cefix::AbstractProperty* getDataProperty(std::string key, std::string file = "");
                
		template<class T>T* getDataFor(std::string key, std::string file = "") {  
			_mutex.lock();
			T* t = _getDataFor<T>(key, file);
			_mutex.unlock();
			return t;
		}
		
		/** @return the complete property-list of a XML-file */
		cefix::PropertyList* getPropertyListForFile(std::string filename = "");
		
		
		/** cleans up, don't use the datafactory afterwards, you'll get a crash */
		void cleanUp();
		
		/** releases the dataContainer, and removes it from the internal cache. 
		 *  If the container is in the load-queue, it is NOT removed from there */
		bool releaseData(const std::string& key, const std::string& file = "");
		
		/**
		 * add a prefs-key to check for inheritance. With this feature you can "include" prefs from another key
		 * After the prefs are loaded all registered keys are processed, and if one of the subkeys has an
		 * "inheritFrom" key the referenced xml-part is merged with the current
		 */
		void addPrefsKeyToCheckForInheritance(std::string s) { _prefsKeyToCheckInheritance.push_back(s); }
		
		void resolveInheritedPreferences() { 
			if (!_prefs.valid()) 
				_loadPreferences();
			else
				resolveInheritedPreferences(_prefs.get()); 
		}
		
		AppController* getAppController() { return _appController;}
		void setAppController(AppController* app) { _appController = app; }
		
		void setReadXmlFileCallbackForData(PropertyList::ReadXmlFileCallback* cb) { _readXmlFileCallback = cb; }
		PropertyList::ReadXmlFileCallback* getReadXmlFileCallbackForData() const { return _readXmlFileCallback.get(); }
		
		void setNotifyLevel(osg::NotifySeverity level) { _notifyLevel = level; }
		
		/// add a clenup-handler, this handler gets called, when the app is about to quit and the general cleanup is done
		void addCleanupHandler(void(*fpt)() ) { _cleanupHandler.push_back(fpt); }
		
		/// read a bunch of xml-files and add it as prefs, the name of the xml-file is used as the key
		bool addPreferencesFromFolderContents(const std::string folder);
		
		/// get the datacache
		DataCache* getCache() const { return _cache; }
    private:
        DataFactory();

    protected:
		void initLoadQueue();
		
		/** adds a new value to the preferences for a specified key */
		bool addPreferencesFor(const std::string& key,  cefix::AbstractProperty* property);

		/** @return a new/cached data-container for key */
        template<class T>T* _getDataFor(std::string key, std::string file = "") {
            
            std::string filename = (file.empty()) ? _defaultDataFile : file;
            T* container = NULL;
            AbstractDataContainer* ac = getFromCache(filename, key);
            if (ac) {
                //container found in cache, cast it to T
                container = dynamic_cast<T*>(ac);
                if (!container) {
                        osg::notify(_notifyLevel)  << "DataFactory::getDataFor mismatching type for datacontainer for key " 
                                                << key << " in file " << filename << std::endl;
                        return NULL;
                }
            }
            else {
                //try to load xml-file into propertylist and return propertylist describing key
                cefix::PropertyList* pl = _loadDataFromXmlForKey(key, filename);
                if (pl) {
                    //data found in xml
                    container = new T();
                    container->setKey(key);
                    if (container->setPropertyList(pl)) {
						// add to cache
						putIntoCache(filename, key, container);
					}
					else {
						osg::ref_ptr<T> tmp = container; // dieser ref-ptr kümmert sich dann um das ggf. löschen
						container = NULL;
					}
                }
                else { 
                    osg::notify(_notifyLevel) << "DataFactory::getDataFor nothing found for key " << key << " in file " << filename << std::endl;
                }
            }
            if (container) {
				addToLoadQueue(container);
			}
            return container;
        }

        virtual ~DataFactory();
		
        /** read the preferences-file into memory */
        void _loadPreferences();
        
        /** adds an empty DataContainer to the load-queue */
        void addToLoadQueue(AbstractDataContainer* data);		
		void resolveInheritedPreferences(cefix::PropertyList* pl);
		
		AbstractDataContainer* getFromCache(const std::string& filename, const std::string& key);
		void putIntoCache(const std::string& filename, const std::string& key, AbstractDataContainer* container); 

		
		template<typename T> inline bool tryCastPreferenceValue(AbstractProperty* p, T& value) { return false; }
		       
        cefix::PropertyList* _loadDataFromXmlForKey(const std::string& key, const std::string& filename);
        
        std::string                                 _rootPath, _mediaPath, _appPath;
        std::string                                 _defaultDataFile;
        osg::ref_ptr<cefix::PropertyList>			_prefs;
        osg::ref_ptr<LoadQueue>                     _loadQueue;
        osg::ref_ptr<DataCache>                     _cache;
        OpenThreads::Mutex                          _mutex;
		
		std::vector<std::string>					_prefsKeyToCheckInheritance;
		AppController*								_appController;
		osg::ref_ptr<PropertyList::ReadXmlFileCallback>	_readXmlFileCallback;
		osg::NotifySeverity							_notifyLevel;
		std::string									_appName, _mediaFolderName;
		
		CleanupHandlerVector						_cleanupHandler;
        
    friend class AbstractDataContainer;
        
};

template<> inline bool DataFactory::tryCastPreferenceValue<int>(AbstractProperty* p, int& value) { value = p->asInt(); return true; }
template<> inline bool DataFactory::tryCastPreferenceValue<std::string>(AbstractProperty* p, std::string& value) { value = p->asString(); return true; }
template<> inline bool DataFactory::tryCastPreferenceValue<float>(AbstractProperty* p, float& value) { value = p->asFloat(); return true; }
template<> inline bool DataFactory::tryCastPreferenceValue<double>(AbstractProperty* p, double& value) { value = p->asDouble(); return true; }
template<> inline bool DataFactory::tryCastPreferenceValue<bool>(AbstractProperty* p, bool& value) { value = p->asBool(); return true; }
template<> inline bool DataFactory::tryCastPreferenceValue<osg::Vec2f>(AbstractProperty* p, osg::Vec2f& value) { value = p->asVec2f(); return true; }
template<> inline bool DataFactory::tryCastPreferenceValue<osg::Vec3f>(AbstractProperty* p, osg::Vec3f& value) { value = p->asVec3f(); return true; }
template<> inline bool DataFactory::tryCastPreferenceValue<osg::Vec4f>(AbstractProperty* p, osg::Vec4f& value) { value = p->asVec4f(); return true; }
template<> inline bool DataFactory::tryCastPreferenceValue<osg::Vec2d>(AbstractProperty* p, osg::Vec2d& value) { value = p->asVec2d(); return true; }
template<> inline bool DataFactory::tryCastPreferenceValue<osg::Vec3d>(AbstractProperty* p, osg::Vec3d& value) { value = p->asVec3d(); return true; }
template<> inline bool DataFactory::tryCastPreferenceValue<osg::Vec4d>(AbstractProperty* p, osg::Vec4d& value) { value = p->asVec4d(); return true; }
template<> inline bool DataFactory::tryCastPreferenceValue<osg::Matrixf>(AbstractProperty* p, osg::Matrixf& value) { value = p->asMatrixf(); return true; }
template<> inline bool DataFactory::tryCastPreferenceValue<osg::Matrixd>(AbstractProperty* p, osg::Matrixd& value) { value = p->asMatrixd(); return true; }
  
}

#endif