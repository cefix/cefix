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

#ifndef RING_BUFFER_HEADER
#define RING_BUFFER_HEADER

#include <vector>
#include <algorithm>
#include <numeric>

namespace cefix {

template <typename T, std::size_t SIZE>
class RingBuffer {
public:
	RingBuffer() : _buffer(SIZE), _writeNdx(0), _readNdx(0) {}
	
	inline bool add(const T& t) {
		_buffer[_writeNdx] = t;
		_writeNdx = next(_writeNdx);
		return true;
	}
	
	inline T getAverage() const { return std::accumulate( _buffer.begin(), _buffer.end(), T() ) / SIZE; }
    
    inline T getVariance(T f) const 
    {
        T result(0);
        for(unsigned int i = 0; i < SIZE; ++i) {
            float dt = _buffer[i] - f;
            result += dt * dt; 
        }
        return result / static_cast<T>(SIZE);
    }
    
    inline T getAverageOfPositives() const 
    {
        T result(0);
        unsigned int num(0);
        for(unsigned int i = 0; i < SIZE; ++i) {
            if (_buffer[i] > 0) {
                result += _buffer[i];
                num++;
            }
            
        }
        return num > 0 ? result / static_cast<T>(num) : 0;
    }
	inline const T& getRelativeToWritePosition(std::size_t delta) const { return _buffer[(_writeNdx + delta) % SIZE]; }
	
	inline const std::vector<T>& getBuffer() const { return _buffer; }
	
	inline const size_t size() const { return SIZE; }
	
	inline T getNext() { T value(_buffer[_readNdx]); _readNdx = next(_readNdx); return value; }
    
    inline void clear() 
    {
        for(unsigned int i = 0; i < SIZE; ++i) {
            add(T());
        }
    }
	
protected:
	inline std::size_t next(std::size_t n) const { return (n+1)%SIZE; }
	
	
	std::vector<T> _buffer;
	volatile std::size_t _writeNdx;
	volatile std::size_t _readNdx;
};

template<class T, std::size_t SIZE>
class ConcurrentRingBuffer : public RingBuffer<T, SIZE> {
public:
	
	ConcurrentRingBuffer() : RingBuffer<T, SIZE>()	{ }
	
	inline bool add(const T& t) 
	{ 
		if (isFull()) return false; 
		this->_buffer[ this->_writeNdx ] = t; 
		this->_writeNdx = next(this->_writeNdx); 
		return true;
	}
	
	bool get(T& value) const 
	{ 
		if (isEmpty()) return false;
		value = this->_buffer[this->_readNdx];
		this->_readNdx = next(this->_readNdx);
		return true;
	}
	
		
	bool isEmpty() const 
        { return (this->_readNdx == this->_writeNdx); }
    bool isFull() const
        { return (Next(this->_writeNdx) == this->_readNdx); }

protected:
	
};

}


#endif