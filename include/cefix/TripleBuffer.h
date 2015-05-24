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
 
#ifndef CEFIX_TRIPLE_BUFFER_HEADER
#define CEFIX_TRIPLE_BUFFER_HEADER

#include <OpenThreads/Mutex>
#include <osg/ref_ptr>
#include <osg/Object>
#include <cefix/Log.h>
#include <iomanip>
#include <vector>

namespace cefix {


/** 
 * a templated triplebuffer which support concurrent reading from and writing into 
 */
template <class T>
class TripleBuffer {
public:
    typedef  std::vector<T> Storage;
    enum Status { INVALID, READ, WRITE, UPDATED };
    
    /// ctor
    TripleBuffer() : _size(0) 
    {
        _status[0] = INVALID;
        _status[1] = INVALID;
        _status[2] = INVALID;
    }
    
    /// resize
    void resize(std::size_t size) 
    {
        OpenThreads::ScopedLock<OpenThreads::Mutex> storage_lock(_storageMutex);
        for(unsigned int i = 0; i < 3; ++i) {
            _storage[i].resize(size);
        }
        _size = size;
    }
    
    /// get size
    const std::size_t& size() const 
    { 
        return _size; 
    }
    
    
    // signalize begin of reading, returns false, if no reading-buffer is ready
    bool beginReading() 
    {
        OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_slotMutex); 
        dump("beginReading start");
        const int slot(getSlot(UPDATED));
        if (slot < 0) 
            return false;
            
        setSlot(slot, READ);
        _currentReadSlot = slot;
        
        dump("beginReading end");
        return true;
    }
    
    /// read a specific value
    inline void read(std::size_t ndx, T& t) {
        t = _storage[_currentReadSlot][ndx];
    }
    
    /// signalize end reading
    void endReading() 
    {
        OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_slotMutex); 
        dump("endReading start");
        const int slot(getSlot(READ));
        if (slot < 0) {
            cefix::log::info("TripleBuffer") << "no read buffer available" << std::endl;
            return;
        }
        
        // wenn wir keinen slot haben, der Updated ist, d.h. es wurden noch keine neuen Daten geschrieben, unseren Read-Slot wieder
        // als updated markieren, damit wieder von diesem gelesen wird. Andernfalls auf invalid setzen.
        
        if( getSlot( UPDATED ) == -1 )
            setSlot(slot, UPDATED);
        else
            setSlot(slot, INVALID);
        
        _currentReadSlot = -1;
        dump("endReading end");
    }
    
    /// signalize the beginning of writing, returns false, if no available buffer is available
    bool beginWriting() 
    {
        OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_slotMutex); 
        dump("beginWriting start");
        const int slot(getSlot(INVALID));
        if (slot < 0) {
            // kein leerer slot zum schreiben vorhanden
            return false;
        }
        setSlot(slot, WRITE);
        _currentWriteSlot = slot;
        dump("beginWriting end");
        return true;
    }
    
    /// write a value into the buffer
    inline void write(std::size_t ndx, const T& t) 
    {
        _storage[_currentWriteSlot][ndx] = t;
    }
    
    /// signalize end of writing
    void endWriting() 
    {
        OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_slotMutex); 
        dump("endWriteing start");
        const int update_slot(getSlot(UPDATED));
        if (update_slot >= 0) {
            setSlot(update_slot, INVALID);
        }
        
        const int write_slot(getSlot(WRITE));
        if (write_slot >= 0) {
            setSlot(write_slot, UPDATED);
        }
        dump("endWriting end");
        _currentWriteSlot = -1;
    }
    
    /// d'tor
    virtual ~TripleBuffer() {}
protected:
    int getSlot(Status s) 
    {        
        for(unsigned int i=0; i < 3; ++i) {
            if (_status[i] == s) 
            {
                return i;
            }
        }
        
        return -1;
    }
    
    void setSlot(int slot, Status status) 
    {
       _status[slot] = status;
    }
    
    inline void dump(const char* str) {
        //cefix::log::info("TripleBuffer") << std::setw(20) << str << " 1: " << _status[0] << " 2: " << _status[1] << " 3: " << _status[2] << std::endl;
    }
private:
    OpenThreads::Mutex _storageMutex, _slotMutex;
    Storage _storage[3];
    Status  _status[3];
    std::size_t _size;
    int _currentReadSlot, _currentWriteSlot;

};



template<class T>
class TripleBufferRef : public osg::Referenced, public TripleBuffer<T> {
public:
    TripleBufferRef() : osg::Referenced(), TripleBuffer<T>() {}

};


/** 
 * a triple-buffer class holding a refcounted object, it supports concurrent reading and writing
 */
template<class T>
class ReferencedTripleBuffer {
public:
    /// ctor
    ReferencedTripleBuffer()
    :   _buffer()
    {
        _buffer.resize(1);
    }
    
    /*
     * put a new object into the triple buffer, if do_clone is true, the object gets cloned before stored into the buffer
     * disable cloning only if you are not using the object anymore, otherwise you'll get crashes
     * returns false, if no slot is available
     */
    bool put(T* val, bool do_clone = true) {
        if (_buffer.beginWriting()) {
            osg::ref_ptr<T> t = (do_clone) ? osg::clone(val) : val;
            _buffer.write(0, t);
            _buffer.endWriting();
            return true;
        }
        return false;
    }
    
    /// get a refcounted object from the buffer, returns false, if no reading slot is available
    bool get(osg::ref_ptr<T>& out_val) {
        if (_buffer.beginReading()) {
            _buffer.read(0, out_val);
            _buffer.endReading();
            return true;
        }
        return false;
    }
private:
    cefix::TripleBuffer<osg::ref_ptr<T> > _buffer;
};


}


#endif 