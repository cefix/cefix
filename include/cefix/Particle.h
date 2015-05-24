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

#ifndef PARTICLE_HEADER
#define PARTICLE_HEADER

#include <cefix/MathUtils.h>
#include <cefix/Export.h>

namespace cefix {


class CEFIX_EXPORT Particle: public osg::Referenced {

	public:
		typedef float time_t;
		
		/**ctor */
		Particle();
		/** sets the position of this particle */
		inline void setPosition(const osg::Vec3& v) { _position = v; }
		/** get the position */
		inline const osg::Vec3& getPosition() const { return _position; }
        
        inline const osg::Vec3& getLastPosition() const { return _lastPosition; }
		
		/** set the lifetime */
		inline void setLifeTime(time_t t) { _lifetime = t; }
		
		/** sets the velocity */
		inline void setVelocity(const osg::Vec3& v) 
		{ 
			_velocity = v;
		}
		
		/**adds a v to the velocity */
		inline void addVelocity(const osg::Vec3& v) 
		{ 
			_velocity += v; 
		}
		
		/** Interpolates the current velocity with a new velocity and a given weight */
		inline void interpolateVelocity(const osg::Vec3& v, float f) 
		{ 
			_velocity = cefix::interpolate(_velocity, v, f); 
		}
		
		/** return the velocity */
		inline const osg::Vec3& getVelocity() const { return _velocity; }
		
		/** return the direction of the movement */
		inline osg::Vec3 getDirection() const { return (_position - _lastPosition); }
		
		/** return the angular velocity */
		inline osg::Vec3 getAngularVelocity() { return osg::Vec3(_angularVelocity[0], _angularVelocity[1], _angularVelocity[2]); }
		
		/** sets the angular velocity */
		inline void setAngularVelocity(const osg::Vec3& v) 
		{ 
			_angularVelocity.set(v[0], v[1], v[2], 0); 
		}
		
		/** adds v to the angular velocity */
		inline void addAngularVelocity(const osg::Vec3& v) 
		{	
			_angularVelocity += osg::Quat(v[0], v[1], v[2], 0);
		}
		
		/** gets the rotation of the particle */
		const osg::Quat& getRotation() const { return _rotation; }
		
		/** sets the rotation of the particle */
		void setRotation(const osg::Quat& q) { _rotation = q; }
		
		/** sets the acceleration */
		inline void setAcceleration(const osg::Vec3& a) { _acceleration = a; }
		
		/** get the acceleration */
		inline const osg::Vec3& getAcceleration()const {return _acceleration; }
		
		/** adds a to the acceleration */
		inline void addAcceleration(const osg::Vec3& a) { _acceleration += a; }
		
		/** apply a force to the particle */
		inline void addForce(const osg::Vec3& f) { addAcceleration(f * getMassInv()); }
		
		/** set particle to die */
		inline void mustDie() { _t = (_lifetime > 0) ? _lifetime : _lifetime = _t; }
		
		/** gets the age of the particle */
		inline time_t getAge() const { return _t; }
		
		/** return the lifetime of the particle */
		inline time_t getLifeTime() const { return _lifetime; }
		/** sets the mass of this particle */
		inline void setMass(float m) { _mass = m; _massInv = 1/m; }
		
		/** return the mass */
		inline float getMass() const { return _mass; }
		
		/** return the inverse mass */
		inline float getMassInv() const { return _massInv; }
		
		
		
		/** updates the position of the particle */
		inline void updatePosition(time_t dt) 
		{
			if (_isAlive && !_isFixed) {
				
				_lastPosition = _position;
				_position +=   _velocity * dt;
				_velocity += _acceleration * dt;
								
				updateRotation(dt);
				
				_acceleration.set(0,0,0);
			}
		}
		
		inline void update(time_t dt) 
		{ 
			_t += dt;
			_isAlive = (_lifetime >= 0) ? (_t <= _lifetime) : true;
			updatePosition(dt); 
		}
		
		void rebirth() {
			_t = 0;
			_velocity = osg::Vec3(0,0,0);
			_position = _lastPosition = osg::Vec3(0,0,0);
			_acceleration = osg::Vec3(0,0,0);
			_rotation = osg::Quat();
			_angularVelocity = osg::Quat(0,0,0,0);
			_isAlive = true;
			
		}
		
		/** @return true if the particle is alive */
		inline bool isAlive() const { return _isAlive; }
				
		/** @return the radius of the particle */
		float getRadius() { return _radius; }
		
		/** sets the radius of the particle */
		void setRadius(float r) { _radius = r; }
		
		/** @return a scalar for the life 0 = born, 1 is dead, only valid if the lifetime is set */
		inline float getLifeScalar() { return (_lifetime < 0) ? 0 : _t/_lifetime; }
		
		/** set this flag, if this particle may rotate takes more computing time */
		void setMayRotate(bool flag) { _mayRotate = flag; }
		
		/** get the mayRotate-flag */
		bool mayRotate() { return _mayRotate; }
		
		inline void setId(unsigned int id) { _id = id; }
		inline unsigned int getId() const { return _id; }
	
		inline bool isFixed() const { return _isFixed; }
		inline void setFixed(bool f) { _isFixed = f; }

	protected:
	
		inline void updateRotation(time_t dt) 
		{
			if (_mayRotate && !isFixed()) {
				//_rotation *= _angularVelocity * dt;
				_rotation = _rotation + _rotation * _angularVelocity * dt * static_cast<time_t>(0.5);
				_rotation /= _rotation.length();
			}
		}
		
		virtual ~Particle();
		
	protected:
		osg::Vec3 _lastPosition, _position, _velocity, _acceleration;
		osg::Quat _rotation, _angularVelocity;
		bool _isAlive, _mayRotate;
		time_t _t, _lifetime;
		float	_mass, _massInv, _radius;
		unsigned int _id;
		bool _isFixed;
		
};


}

#endif