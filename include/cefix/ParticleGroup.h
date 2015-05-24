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

#ifndef PARTICLE_GROUP_HEADER
#define PARTICLE_GROUP_HEADER

#include <list>

#include <cefix/Particle.h>
#include <cefix/ParticlesForRecyclingQueue.h>

namespace cefix {

template <class T>
class StorageList {
public:
	typedef std::list<T*> List;
	typedef std::vector< osg::ref_ptr<T> > Storage;
	typedef std::list<unsigned int> FreeStorageList;
	
	typedef typename List::iterator iterator;
	

	StorageList(): _list(), _storage(), _freeStorageSlots(), _size(0) {}
	
	inline void add(T* t) 
	{ 
		unsigned int ndx(_storage.size());
			
		_list.push_back(t); 
		if (!_freeStorageSlots.empty()) {
			ndx = *_freeStorageSlots.begin();
			_storage[ndx] = t;
			_freeStorageSlots.pop_front();
		} else {
			_storage.push_back(t); 
		}
		
		t->setId(ndx);
        _size++;
	}
	
	iterator erase(const iterator& i) {
		unsigned ndx( (*i)->getId() );
		_freeStorageSlots.push_back( ndx );
		_storage[ndx] = NULL;
		_size--;	
		return _list.erase(i);
	}
	
	inline iterator begin() { return _list.begin(); }
	inline iterator end() { return _list.end(); }
	
	inline const iterator& begin() const { return _list.begin(); }
	inline const iterator& end() const { return _list.end(); }

	
	const List& getList() const { return _list; }
	List& getList() { return _list; }
	
	inline unsigned int size() const { return _size; }
    
    inline void recomputeSize() { _size = _list.size(); }
	
	template<class U> friend std::ostream& operator<<(std::ostream& os, const StorageList<U>& list);
	
private:
	List _list;
	Storage _storage;
	FreeStorageList _freeStorageSlots;
    std::size_t _size;
	

};

/** this class groups particle into a logical unit and allows the addition of new particles and the cleanup of dead particles. Other classes of the particle-system operates with this group */

template <class ParticleClass>
class ParticleGroup : public osg::Referenced {

	public:
		typedef typename StorageList<ParticleClass>::List List;
		typedef typename std::vector< osg::ref_ptr<ParticleClass> > DeletedList;
		typedef typename List::iterator iterator;
		typedef typename List::const_iterator const_iterator;
        
        class DeleteCallback : public osg::Referenced {
        public:
            virtual void operator()(const DeletedList& deleted_particles) = 0;
        };
		
	
		typedef ParticlesForRecyclingQueue<ParticleClass> RecyclingQueue;
		/** ctor */
		ParticleGroup() : osg::Referenced(), _isAlive(true) {}
		
		/** updates all particles and remove dead ones */
		inline void update(const typename ParticleClass::time_t& dt) 
		{
			_deleted.clear();
            unsigned int num_deleted(0);
			for(typename List::iterator i = _list.begin(); i != _list.end(); ) {
				(*i)->update(dt);
				if ((*i)->isAlive()) {
					++i;
				} else {
                    ++num_deleted;
					if (_queue.valid())
						_queue->add((*i));
					
					_deleted.push_back(*i);
					i = _list.erase(i);
				}
			}
            if ((num_deleted > 0) && _deleteCallback.valid())
                (*_deleteCallback)(_deleted);
                
			_isAlive = (_list.size() > 0);		
		}
		
		inline void updateRepresentation() {
			for(typename List::iterator i = _list.begin(); i != _list.end(); ++i) {
				(*i)->updateRepresentation();
			}
			for(typename DeletedList::iterator i = _deleted.begin(); i != _deleted.end(); ++i) 
			{
				(*i).get()->updateRepresentation();
			}
		}
		/** @return the list of all particles */
		inline const List& getParticles() const { return _list.getList(); }
		
		inline List& getParticles() { return _list.getList(); }
		
		/** adds a particle to the list */
		inline  void add(ParticleClass* particle) 
		{ 
			_list.add(particle);
		}
		
		/** @return the number of particles in this group */
		inline unsigned int getNumParticles() { return _list.size(); }
		
		/** return true if this group is alive (has min 1 particle alive) */
		inline bool isAlive() const { return _isAlive; }
		
		/** sets the recycling queue */
		void setRecyclingQueue(RecyclingQueue* queue) { _queue = queue; }
		RecyclingQueue* getRecyclingQueue() { return _queue.get(); }
		
		template<class U> friend std::ostream& operator<<(std::ostream& os, const ParticleGroup<U>& group);
		
		iterator begin() { return _list.begin(); }
		iterator end() { return _list.end(); }
		const_iterator begin() const { return _list.begin(); }
		const_iterator end() const { return _list.end(); }
		
		unsigned int size() const { return _list.size(); }
        
        void setDeletedCallback(DeleteCallback* cb) { _deleteCallback = cb; }
		
	protected:
		virtual ~ParticleGroup() {}
		
	private:
	
		
		StorageList<ParticleClass>			_list;
		bool								_isAlive;
		osg::ref_ptr< RecyclingQueue >		_queue;
		DeletedList							_deleted;
        osg::ref_ptr<DeleteCallback>         _deleteCallback;
};


template<class U> std::ostream& operator<<(std::ostream& os, const StorageList<U>& list)
{
	os << "active: " << list.size() << " storage: " << list._storage.size() << " free slots: " << list._freeStorageSlots.size();
	return os;
}

template<class U> std::ostream& operator<<(std::ostream& os, const ParticleGroup<U>& group)
{
	os << group._list;
	return os;
}
	

}


#endif