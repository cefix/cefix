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

#ifndef CEFIX_PARTICLE_CONSTRAINT_HEADER
#define CEFIX_PARTICLE_CONSTRAINT_HEADER

#define USE_SQRT_APPROC 1

#include <osg/Vec3>
#include <osg/Vec4>
#include <osg/Referenced>
#include <osg/ref_ptr>
#include <algorithm>

namespace cefix {

template <class ParticleClass>
class ParticleConstraint : public osg::Referenced 
{
public:
	ParticleConstraint(ParticleClass* a, ParticleClass* b) : _a(a), _b(b), _isAlive(true), _enabled(true) {}
	
	inline void setId(unsigned int id) { _id = id; }
	inline unsigned int getId() { return _id; }
	
	virtual void solve() = 0;
	
	inline bool isAlive() const { return (_isAlive && (_a->isAlive() && _b->isAlive())); }
	
	void mustDie() { _isAlive = false; }
	
	inline bool isEnabled() const { return _enabled; }
	inline void setEnabled(bool f) { _enabled = f; }
	
	inline ParticleClass* getLeft() const { return _a.get(); }
	inline ParticleClass* getRight() const { return _b.get(); }
	
	void setLeft(ParticleClass* a) { _a = a; }
	void setRight(ParticleClass* b) { _b = b; }

	inline void doSolve() { if (_enabled) solve(); }
	
	void updatePositions(typename ParticleClass::time_t dt) {
		_a->update(dt);
		_b->update(dt);
	}
	
	virtual osg::Vec4 getDebugColor() = 0;
		
private:
	unsigned int _id;
	osg::ref_ptr<ParticleClass> _a, _b;
	bool _isAlive, _enabled;



};


template <class ParticleClass>
class ParticleFixedConstraint : public ParticleConstraint<ParticleClass> {

public:
	ParticleFixedConstraint(ParticleClass* a, ParticleClass* b,  float wishDistance) : ParticleConstraint<ParticleClass>(a,b), _wishDistance(wishDistance) {}
	
	using ParticleConstraint<ParticleClass>::getLeft;
	using ParticleConstraint<ParticleClass>::getRight;
	
	virtual void solve()
	{
		// see http://www.gamasutra.com/resource_guide/20030121/jacobson_03.shtml
		
		ParticleClass* a(getLeft());
		ParticleClass* b(getRight());
		
		osg::Vec3 delta(b->getPosition() - a->getPosition());
		
		float deltalength = std::max<float>(delta.length(), 0.0001);
		float diff=(deltalength-_wishDistance)/deltalength * 0.5;
		
		// delta*=_wishDistance*_wishDistance/(delta*delta+_wishDistance*_wishDistance)-0.5;
		
		//if (!a->isFixed()) a->setPosition( a->getPosition() + delta*diff );
		//if (!b->isFixed()) b->setPosition( b->getPosition() - delta*diff );
		
		if (!a->isFixed()) a->addVelocity( delta*diff );
		if (!b->isFixed()) b->addVelocity( -delta*diff );
	}
		
	inline void setWishDistance(float f) {_wishDistance = f; }
	inline float getWishDistance() { return _wishDistance; }
	
	virtual osg::Vec4 getDebugColor() { return osg::Vec4(1,1,1,0.1); }
	
private:
	float _wishDistance;

};


template <class ParticleClass>
class ParticleSpringConstraint : public ParticleConstraint<ParticleClass> {

public:
	ParticleSpringConstraint(ParticleClass* a, ParticleClass* b, float strength, float wishDistance) : ParticleConstraint<ParticleClass>(a,b), _strength(strength), _wishDistance(wishDistance) {}
	
	using ParticleConstraint<ParticleClass>::getLeft;
	using ParticleConstraint<ParticleClass>::getRight;
	
	virtual void solve()
	{
		ParticleClass* a(getLeft());
		ParticleClass* b(getRight());
		
		osg::Vec3 delta(b->getPosition() - a->getPosition());
		
		#ifdef USE_SQRT_APPROC
			
			float deltaLength = std::max<float>(0.00001, delta.length2());
			float diff = (deltaLength - _wishDistance*_wishDistance) / (deltaLength * (a->getMassInv() + b->getMassInv()) );
			
			float force = (diff * _strength);
			
			if (!a->isFixed()) a->addVelocity( delta * (a->getMassInv() * force) );
			if (!b->isFixed()) b->addVelocity( delta * (b->getMassInv() * -force) );		
		#else
				
			float deltaLength = delta.length();
			float diff = (deltaLength - _wishDistance) / (deltaLength * (a->getMassInv() + b->getMassInv()) );
			
			float force = (diff * _strength);
			
			if (!a->isFixed()) a->addVelocity( delta * (a->getMassInv() * force) );
			if (!b->isFixed()) b->addVelocity( delta * (b->getMassInv() * -force) );
		#endif
	}
	
	inline void setStrength(float s) { _strength = s; }
	inline float getStrength() const { return _strength; }
	
	inline void setWishDistance(float f) {_wishDistance = f; }
	inline float getWishDistance() { return _wishDistance; }
	
	virtual osg::Vec4 getDebugColor() { return osg::Vec4(0,1,1,0.1); }
	
private:
	float _strength, _wishDistance;

};


/*
template <class ParticleClass>
class ParticleCollisionConstraint : public ParticleConstraint<ParticleClass> {

public:
	ParticleCollisionConstraint(ParticleClass* a, ParticleClass* b, float strength) : ParticleConstraint<ParticleClass>(a,b), _strength(strength) {}
	
	using ParticleConstraint<ParticleClass>::getLeft;
	using ParticleConstraint<ParticleClass>::getRight;
	
	virtual void solve()
	{
		ParticleClass* a(getLeft());
		ParticleClass* b(getRight());
		
		osg::Vec3 delta(b->getPosition() - a->getPosition());
		
		float wishDistance = b->getRadius + a->getRadius();		
		float deltaLength = delta.length();
		float diff = (deltaLength - wishDistance) / (deltaLength * (a->getMassInv() + b->getMassInv()) );
		
		float force = (diff * _strength);
		
		if (!a->isFixed()) a->addVelocity( delta * (a->getMassInv() * force) );
		if (!b->isFixed()) b->addVelocity( delta * (b->getMassInv() * -force) );
	}
	
	inline void setStrength(float s) { _strength = s; }
	inline float getStrength() const { return _strength; }
	
		
private:
	float _strength;

};

*/

template <class ParticleClass>
class ParticleAttractionConstraint : public ParticleConstraint<ParticleClass> {

public:
	ParticleAttractionConstraint(ParticleClass* a, ParticleClass* b, float strength, float minDistance = 0.0f) 
	:	ParticleConstraint<ParticleClass>(a,b), 
		_strength(strength), 
		_minDistance(minDistance), 
		_minDistance2(minDistance*minDistance) 
	{
	}
	
	using ParticleConstraint<ParticleClass>::getLeft;
	using ParticleConstraint<ParticleClass>::getRight;
	
	virtual void solve()
	{
		ParticleClass* a(getLeft());
		ParticleClass* b(getRight());
		
		osg::Vec3 delta(b->getPosition() - a->getPosition());
		
		float l2 (delta.length2());
		if (l2 <= _minDistance2)
			return;
		
		l2 = osg::maximum<float>(0.00001, l2);
		
		float force = _strength * (a->getMass() * b->getMass()) / l2;
		
		if (!a->isFixed()) a->addVelocity( delta * (a->getMassInv() *  force) );
		if (!b->isFixed()) b->addVelocity( delta * (b->getMassInv() * -force) );
	}
	
	inline void setStrength(float s) { _strength = s; }
	inline float getStrength() const { return _strength; }
	
	inline float getMinDistance() { return _minDistance; }
	inline void setMinDistance(float f) { _minDistance = f; _minDistance2 = f*f; }
	
	virtual osg::Vec4 getDebugColor() { return (_strength > 0) ? osg::Vec4(0,1,0,0.7) : osg::Vec4(1,0,0,0.1); }
		
private:
	float _strength, _minDistance, _minDistance2;

};


}


#endif