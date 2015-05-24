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

#ifndef TEXTURE_ALLOCATION_OBSERVER_HEADER
#define TEXTURE_ALLOCATION_OBSERVER_HEADER

#include <list>
#include <map>

#include <osg/observer_ptr>
#include <osg/Referenced>
#include <osg/Texture>
#include <osg/Image>

#include <cefix/Particle.h>
#include <cefix/AnimationBase.h>
#include <cefix/StatisticsGroup.h>
#include <cefix/Video.h>

#include <cefix/AbstractDataContainer.h>
#include <cefix/Export.h>

namespace cefix{

/** the allocationobserver-class monitors the allocation of specific objects and do some statistics every second. If requested the results are updated via a statisticsgroup, or dumped to the console. 
 *  currently, images, movies, datacontainers and textures created by the quad2dgeometry can be tracked. You'll can enable the tracking via the preferences.xml-file of via cefix::Settings
 */
class CEFIX_EXPORT AllocationObserver : public cefix::AnimationBase{
	
	public:
		/** singleton-getter */
		static AllocationObserver* instance();
		
		/** observe a texture */
		inline void observe(osg::Texture* o) { observe("texture", o); }
		
		/** observe a particle */
		inline void observe(Particle* o) { observe("particle", o); }
		
		/** observe an image */
		inline void observe(osg::Image* o) { observe("image", o); }

		/** observe a quicktime-movie */
		inline void observe(cefix::Video* o) { observe("video", o); }

		/** observe a data-container */
		inline void observe(cefix::AbstractDataContainer* o) { observe("datacontainer", o); }
		
		/** observe an osg::object */
		inline void observe(osg::Object* o) { observe(o->className(), o); }
		
		/** observe an animation */
		inline void observe(cefix::AnimationBase* o) { observe("animation", o); }
		
		/** observe an object of type key, must be subclassed from referenced. You can use this method to track custom objects. 
		 *  After creating your object, register it with this method, all the magic is done behind the curtains. */
		void observe(const std::string& key, osg::Referenced* o);
		
		/** creates an object and adds it to the list of objects to observe */
		template <class T> T* createAndObserve(const std::string& key) {
			T* t = new T();
			
			if (_enabled)
				observe(key, t);
			return t;
		}
		
		template <class T> T* createAndObserve() {
			T* t = new T();
			
			if (_enabled)
				observe(t->className(), t);
				
			return t;
		}
		
		/** returns a StatisticsGroup, which you can adjust and add to the scenegraph. This will disable the output to the console */
		StatisticsGroup* getStatistics() {
			if (!_group.valid())
				createStatistics();
			return _group.get(); 
		}
		/** sets the sampling time-delta (in seconds)*/
		void setSamplingTimeDelta(float t) { _samplingTime = t; }
		
		void setNumSamples(unsigned int s) { _numSamples = s; }
		
		/** enable / disable the AllocationObserver */
		void setEnabledFlag(bool f) { _enabled = f; if (_enabled) { loadPrefs(); addAsAnimation(); } }
		
		/** sets the prefs key to load prefs from, may override the enable-flag */
		void setPrefsKey(const std::string& key) {_prefsKey = key; loadPrefs(); }
		
		/** enable stats for a specific key, AllocationObserver must be enabled to function properly */
		void enableStatisticsForKey(const std::string& key) { _enabledKeys.insert(std::make_pair(key, true)); }
		
		/** allows to observe a whole or parts of a scenegraph, the scenegraph is traversed at regular intervalls and all nodes are added to the observer*/
		void observeSceneGraph(osg::Node* node) { _observedNode = node; }
		
		void dump(std::ostream& os);
	
	protected:
		/** ctor */
		AllocationObserver();

		/** do the work */
		virtual void animate(float elapsed);
		
		virtual ~AllocationObserver() {}
		
	private:
		/** list of observed objects */
		typedef std::list< osg::observer_ptr<osg::Referenced> > ObservedList;
		
		/** check a list of observed objects */
		void checkList(const std::string& key, ObservedList& alist, bool addValue);
		
		/** create the statistics */
		void createStatistics();
		
		/** add a statistics for a specific key */
		void addStatistics(std::string key);
		
		/** is an object already in the list */
		bool isUnique(std::string key, osg::Referenced* o);
		
		void loadPrefs();
		
		void addAsAnimation();
		
		struct ltstr
		{
			bool operator()(const std::string& s1, const std::string& s2) const
			{
				return stricmp(s1.c_str(), s2.c_str()) < 0;
			}
		};
		
		std::map<std::string, ObservedList>					_observed;
		float												_last, _lastValue;
		OpenThreads::Mutex									_mutex;
		osg::ref_ptr<StatisticsGroup>						_group;
		float												_samplingTime;
		bool												_enabled;
		std::string											_prefsKey;
		std::map<std::string, bool,ltstr>					_enabledKeys;
		unsigned int										_numSamples;
		bool												_addedAsAnimation;
		osg::observer_ptr<osg::Node>						_observedNode;
};

}

#endif