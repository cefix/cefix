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

#ifndef IMAGE_CACHE_HEADER
#define IMAGE_CACHE_HEADER

#include <osg/Referenced>
#include <cefix/Timer.h>
#include <osg/Timer>
#include <osg/ref_ptr>
#include <map>
#include <vector>
#include <iostream>

/** 
 * template class implementing a cache, which purges old items after a certain amount of inactivity (no access last x msecs). 
 * The cache uses a cefix::Timer to check the cache
 */

namespace cefix {
 
template <class KEY, class T>
class Cache : public osg::Referenced {

	public:
		/// a cache-item, encapsulating the data-object, the key and the last access timestamp
		class Item {
			public:
				/// ctor
				Item() : _t(NULL) { touch(); }
				
				/// ctor
				Item(T* t) : _t(t) { touch(); }
				
				// get the data
				inline T* get() { touch(); return _t.get(); }
                inline T* getReference() const { return _t.get(); }
				
				/// update timestamp of loast usage
				inline void touch() { _lastTouched = osg::Timer::instance()->tick(); }
				
				/// get age of this item
				inline float getAge() const { return osg::Timer::instance()->delta_s(_lastTouched, osg::Timer::instance()->tick()); }
			
			private:
				osg::Timer_t _lastTouched;
				osg::ref_ptr<T>	_t;
		};
		
		typedef std::map<KEY, Item > CacheMap;
		typedef typename std::map<KEY, Item >::const_iterator const_iterator;
		/**
		 * ctor 
		 * @param maxLifeTime max lifetime of cache-items (in secs)
		 * @param maxPurgeTimeMs max time purge is allowed to purge old items (in msec)
		 * @param name name of cache (useful only for debugging)
		 */
		 
		Cache(float maxLifeTime = 60, unsigned int maxPurgeTimeMs =  30, const std::string& name = "Cache" ) 
		:	osg::Referenced(),
			_maxLifeTime(maxLifeTime),
			_t(NULL),
			_maxPurgeTimeMs( maxPurgeTimeMs ),
			_missedHitCount(0),
			_hitCount(0),
			_debug(false),
			_name(name)
		{
		}
		
		/// get the name of this cache 
		inline const std::string& getName() const { return _name;}
		
		/// set the name of this cache
		inline void setName(const std::string& name) { _name = name; }
		
		/// set debug flag, if true, some more informations is put to the console 
		inline void setDebugFlag(bool f) { _debug = f; }
		
        /// returns true,if cache has element key, does not touch the cached-item nor updates its timestamp
        inline bool has(const KEY& key) 
        {
            OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
			typename CacheMap::iterator itr = _cache.find(key); 
            return (itr != _cache.end());
        }
        
		/// get a cached item with @param key
		inline T* get(const KEY& key) {
			OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
			typename CacheMap::iterator itr = _cache.find(key); 
			++_hitCount;
			if (itr == _cache.end()) ++_missedHitCount;
			return (itr != _cache.end()) ? itr->second.get() : NULL;
		}
		
		/// reset stats
		inline void resetStats() {
			_missedHitCount = _hitCount = 0;
		}
		
		/// add a new item with specific key to the cache
		inline void add(const KEY& key, T* t) {
			OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
			_cache[key] = t;
			if (!_t.valid()) {
				_t = cefix::FunctorTimer::create(3 * 1000.0f, this, &Cache::prune,0);
				_t->start();
				//_t->setDebug(true);
				_lastAdd = osg::Timer::instance()->tick();
			}
		}
        
        /// remove an item from the cache
        inline bool remove(const KEY& key) 
        {
            typename CacheMap::iterator itr = _cache.find(key);
            if (itr != _cache.end()) {
                _cache.erase(itr);
                return true;
            }
            return false;
        }
		
		// clears the cache, all items get removed
		void clear() { _cache.clear(); }
		
		
		/// remove old items from the cache
		void prune(cefix::Timer* t = NULL) 
		{
			OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);

			if (_debug)
				osg::notify(osg::ALWAYS) << "pruning " << getName() << " after " << osg::Timer::instance()->delta_s(_lastAdd, osg::Timer::instance()->tick()) << "... " << std::endl;
			
			osg::Timer time;
			osg::Timer_t starttick( time.tick() );
			std::vector<typename CacheMap::iterator> expired;
			
			for(typename CacheMap::iterator i = _cache.begin(); i != _cache.end(); ++i) {
				if (i->second.getAge() > _maxLifeTime) {
					expired.push_back(i);
				}
				
				// falls das zulange dauert, raus hier.
				if(time.delta_m(starttick, time.tick()) > _maxPurgeTimeMs)
					break;
			}
			
			
			for(typename std::vector<typename CacheMap::iterator>::iterator i = expired.begin(); i != expired.end(); ++i) {
				_cache.erase(*i);
			}
			
			if ((_cache.size() == 0) && (_t.valid())) {
				_t->stop();
				_t = NULL;
			}
			
			if (_debug)
				osg::notify(osg::ALWAYS) << getName() << ": removed " << expired.size() << " from cache " << _cache.size() << " items remaining in cache, max lifetime: " << _maxLifeTime << std::endl;
			
		
		}
		
		
		/// return the size of the cache
		inline unsigned int size() { return _cache.size(); }
		
		/// return the num of all hits
		inline unsigned int getHitCount() { return _hitCount; }
		
		/// return the num of missed hits
		inline unsigned int getMissedHitCount() { return _missedHitCount; }
		
		/// return the num of succeeded hits (items in cache)
		inline unsigned int getSucceededHitCount() { return (_hitCount - _missedHitCount); }
		
		/// dump the contents of the cache to the console
		inline void dump() {
			for (typename CacheMap::iterator i = _cache.begin(); i != _cache.end(); ++i) {
				std::cout << i->first << ": " << i->second.getAge() << " " << i->second.get() << std::endl;
			}
		}
        
        const_iterator begin() const { return _cache.begin(); }
        const_iterator end() const { return _cache.end(); }
		
	private:
		float		_maxLifeTime;
		CacheMap	_cache;
		osg::ref_ptr<cefix::Timer>  _t;
		OpenThreads::Mutex _mutex;
		unsigned int _maxPurgeTimeMs;
		unsigned int _missedHitCount, _hitCount;
		bool		 _debug;
		std::string  _name;
		osg::Timer_t _lastAdd;
	
	friend std::ostream& operator <<(std::ostream& os, Cache<KEY, T>& cache) {
		os << "size: " << cache.size() << " missed: " << cache.getMissedHitCount() << " sucessfull hits: " << cache.getSucceededHitCount();
		return os;
	}
		
			
};

}

#endif