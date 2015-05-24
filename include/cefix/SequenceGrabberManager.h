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

#ifndef _SEQUENCE_GRABBER_MANAGER_HEADER_
#define _SEQUENCE_GRABBER_MANAGER_HEADER_
#include <cefix/Export.h>
#include <map>
#include <osg/Referenced>
#include <osg/NodeCallback>
#include <osg/NodeVisitor>
#include <cefix/SequenceGrabber.h>
#include <cefix/ObjectFactory.h>

namespace cefix {
	/** small helper-struct to store essential data to construct a seqence-grabber */
	struct SequenceGrabberCreateData {
		SequenceGrabberCreateData(const std::string& n, unsigned int w, unsigned int h, unsigned int r) : name(n), width(w), height(h), rate(r) {}
		std::string name;
		unsigned int width, height, rate; 
	};
	
	/** base class fpr creator-class for object factory */
	class SequenceGrabberCreatorBase : public osg::Referenced {
		public:
			virtual SequenceGrabber* operator()(SequenceGrabberCreateData data) const { return NULL; }
			virtual ~SequenceGrabberCreatorBase() {};
	};
	
	/** template class for creating specific classes from the object-factory */
	template <class T>
	class SequenceGrabberCreatorT : public SequenceGrabberCreatorBase {
		public:
			virtual SequenceGrabber* operator()(SequenceGrabberCreateData data) const { return new T(data.name, data.width, data.height, data.rate); }
			virtual ~SequenceGrabberCreatorT() {};
	};
	typedef std::map<std::string, void(*)(SequenceGrabberDeviceInfoList&)> PopulateDeviceListCallbackMap;
    typedef std::map< std::string, osg::ref_ptr<SequenceGrabber> > SequenceGrabberMap;
	typedef cefix::ObjectFactory<SequenceGrabber, std::string, SequenceGrabberCreatorBase, SequenceGrabberCreateData> SequenceGrabberFactory;
	/**
	 * this class manages all Sequence Grabbers and update them on a regular basis, use the IdleCallback-class for 
	 * this, or use the idle-method
	 * @author Stephan Maximilian Huber
	 */
	class CEFIX_EXPORT SequenceGrabberManager : public osg::Referenced {
    
		public:
			/// @return returns the manager-singleton-object
			static SequenceGrabberManager* instance();

			/// default constructor
			SequenceGrabberManager();

			/**
			 * get a named Sequencegrabber, use an empty string to get the default grabber
			 * @param name the name of the input device (iSight, for example)
			 * @param w width of the grabbing-rect
			 * @param h height og the grabbing-rect
			 * @param returns a (cached) instance of the specific sequence grabber
			 */
            SequenceGrabber* get(std::string name = "", unsigned int w = 320, unsigned int h = 240, unsigned int rate = 30);
             SequenceGrabber* get(const SequenceGrabber::DeviceInfo& di, unsigned int w = 320, unsigned int h = 240, unsigned int rate = 30)
             {
                return get(di.getKey(), w, h, rate);
             }
             
			/// remove all sequence grabbers
			inline void cleanup() {
				_map.clear();
			}
			
			
			template <class T> void registerClass() 
			{ 
				_factory->registerObjectClass(T::getGrabberId(), new SequenceGrabberCreatorT<T>()); 
				_deviceListCallbackMap[T::getGrabberId()] = &T::populateDeviceInfoList;
			}
			
			const std::string& getDefaultGrabberName() { return _defaultGrabberName; }
			void setDefaultGrabberName(const std::string& name) { _defaultGrabberName = name; }
			
			SequenceGrabber* getLastAcquiredDevice() { return _lastAcquiredDevice; }
            
            const SequenceGrabberDeviceInfoList& getDeviceInfoList()  { populateDeviceInfoList(); return _devices; }
 
		protected:
			virtual ~SequenceGrabberManager() {}
			
		private:
			void populateDeviceInfoList();
			/// map of all registered and used SequenceGrabbers
            SequenceGrabberMap _map;
			osg::ref_ptr<SequenceGrabberFactory> _factory;
			std::string _defaultGrabberName;
			SequenceGrabber* _lastAcquiredDevice;
			SequenceGrabberDeviceInfoList _devices;
            PopulateDeviceListCallbackMap _deviceListCallbackMap;
	};
	template<class T>
	class SequenceGrabberRegistrar  {
	public:
		SequenceGrabberRegistrar(bool isDefault = false)
		{
			cefix::SequenceGrabberManager::instance()->registerClass<T>();
			if (isDefault)
				cefix::SequenceGrabberManager::instance()->setDefaultGrabberName(T::getGrabberId());
		}
	};

}

std::ostream& operator<<(std::ostream& os, const cefix::SequenceGrabberDeviceInfoList& device_list);

#endif