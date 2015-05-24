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

#ifndef COMMON_PARTICLE_OPERATORS_HEADER
#define COMMON_PARTICLE_OPERATORS_HEADER

#include <osg/Plane>

#include <cefix/MathUtils.h>
#include <cefix/ParticleOperator.h>
#include <cefix/VectorField.h>
#include <cefix/WidgetComposer.h>

namespace cefix {

/** applies a vector-field-force to the particle, if inside the vector-field */
	template <class ParticleClass, class T=osg::Vec3>
class ParticleVectorFieldOperator : public cefix::ParticleOperator<ParticleClass> {
	public:
		/** ctor
		 *  @param vf the vectorfield 
		 *  @param scale scales the force 
		 *  @param weight weight of this particle operator, the resulting velocity is interpolated with the prvious veloctiy
		 */
	
	ParticleVectorFieldOperator(cefix::VectorFieldT<T>* vf, float scale = 1.0f, float weight = 1.0f, float divisor = 1.0f) : cefix::ParticleOperator<ParticleClass>(weight), _vf(vf), _scale(scale), _divisor(divisor) {}
		
		inline void operator()(ParticleClass* p, const typename ParticleClass::time_t& dt) {
			osg::Vec3 pp = p->getPosition() / _divisor;
			if (_vf->includes(pp))
				p->interpolateVelocity(_vf->get(pp) *_scale,  this->getWeight());
		}
        
        void applyWidgetComposer(WidgetComposer& composer) {
            composer.addSlider("VECTORFIELD WEIGHT", doubleRange(0,1), &this->_weight); 
        } 
	private:
		osg::ref_ptr<typename cefix::VectorFieldT<T> > _vf;
		float _scale, _divisor;
};


/** attracts particles to a given particle */
template <class ParticleClass>
class ParticleAttractorOperator : public cefix::ParticleOperator<ParticleClass> {
	public:
		/** 
		 * @param attractor particle which is the attractor
		 * @param attractradius radius in which other particles get attracted
		 * @param weight weight of this operation
		 */
		ParticleAttractorOperator(ParticleClass* attractor, float attractradius = 1.0f, float weight = 1.0f) : cefix::ParticleOperator<ParticleClass>(weight), _attractor(attractor), _attractRadius(attractradius*attractradius) {}
		
		inline void operator()(ParticleClass* p, const typename ParticleClass::time_t& dt) {
			if (p != _attractor.get()) {
				osg::Vec3 delta =  _attractor->getPosition() - p->getPosition();
				if (delta.length() > _attractRadius)
					p->addVelocity(delta * (0.2f+cefix::randomf(0.3f)) * this->getWeight());
			}
		}
        
        void applyWidgetComposer(WidgetComposer& composer) {
            composer.addSlider("ATTRACT RADIUS", doubleRange(0,1000), &this->_attractRadius); 
            composer.addSlider("ATTRACT WEIGHT", doubleRange(0,1), &this->_weight); 
        } 
	private:
		osg::ref_ptr<ParticleClass>		_attractor;
		float							_attractRadius;
};

/** attracts all particles to the centroid of the particle-field */
template <class ParticleClass>
class ParticleCentroidAttractorOperator : public cefix::ParticleOperator<ParticleClass> {
	public:
		/** 
		 * @param minDistance minimum distance from the centroid to get attracted 
		 * @param weight weight of this particle-operator
		 */
		ParticleCentroidAttractorOperator(float minDistance, cefix::floatRange scale = cefix::floatRange(0.2f, 0.5f), float weight=1.0f) : 
			cefix::ParticleOperator<ParticleClass>(weight), 
			_minDistance(minDistance), 
			_scale(scale),
			_centroid(osg::Vec3(0,0,0))
		{
		}
		
		/** computes the current centroid */
		inline void begin(cefix::ParticleGroup<ParticleClass> * g, const typename ParticleClass::time_t& dt) {
			osg::Vec3 newCentroid = osg::Vec3(0,0,0);
			for(typename cefix::ParticleGroup<ParticleClass>::List::const_iterator i = g->getParticles().begin(); i!=g->getParticles().end(); ++i) {
			
				newCentroid += (*i)->getPosition();
			}
			newCentroid /= g->getParticles().size();
			_centroid = newCentroid; //cefix::interpolate(_centroid, newCentroid, 0.5f);
		}
		
		inline void operator()(ParticleClass* p, const typename ParticleClass::time_t& dt) {
			
			osg::Vec3 delta =  _centroid - p->getPosition();
				if (delta.length() > _minDistance)
					p->addVelocity(delta * this->getWeight() * _scale.random());
			
		}
	private:
		float		_minDistance;
		cefix::floatRange	_scale;
		osg::Vec3	_centroid;
};

/** attract particle to other particles in the neighborhood*/
template <class ParticleClass>
class ParticleAttractedByNeighborsOperator : public cefix::ParticleOperator<ParticleClass> {
	public:
		/** ctor 
		 *  @param radius particles inside this radius get attracted
		 *  @param weight weight of this particle-operator 
		 */
		ParticleAttractedByNeighborsOperator(float radius, float weight = 1.0f) : cefix::ParticleOperator<ParticleClass>(weight), _radius(radius) {}
		
		typedef ParticleGroup<ParticleClass> Group;
		
		inline void operator()(ParticleClass* p, const typename ParticleClass::time_t& dt) 
		{
			osg::Vec3 attracted;
			unsigned int numNeighbours = 0;
			
			const typename Group::List& list = this->getGroup()->getParticles();
			for(typename Group::List::const_iterator i = list.begin(); i != list.end(); ++i) 
			{
				osg::Vec3 delta =  p->getPosition() - (*i)->getPosition();
				if (((*i) != p) && (delta.length2() <= _radius*_radius)) {
					attracted -= delta;
					++numNeighbours;
				}
			}
			if (numNeighbours > 0) 
			{
				attracted /= numNeighbours;
				p->addVelocity(attracted * this->getWeight());
			}
			
		}
	private:
		float _radius;
};

/** distracts all particles from others if inside a given radius, it adds it avoidance-force to the current velocity, so there is some demping */
template <class ParticleClass>
class ParticleDistractorOperator : public cefix::ParticleOperator<ParticleClass> {
	public:
		/** ctor
		 *  @param radius distracts particles inside the given radius 
		 *  @param weight weight of this particle-operator 
		 */
		ParticleDistractorOperator(float radius, float weight = 1.0f) : cefix::ParticleOperator<ParticleClass>(weight), _radius(radius) {}
	
		
		inline void operator()(ParticleClass* p, const typename ParticleClass::time_t& dt) 
		{
			osg::Vec3 avoiddirection;
			unsigned int numNeighbours = 0;
			
			const typename cefix::ParticleGroup<ParticleClass>::List& list = this->getGroup()->getParticles();
			for(typename cefix::ParticleGroup<ParticleClass>::List::const_iterator i = list.begin(); i != list.end(); ++i) 
			{
				osg::Vec3 delta =  p->getPosition() - (*i)->getPosition();
				if (((*i) != p) && (delta.length2() <= _radius*_radius)) 
				{
					avoiddirection += delta;
					++numNeighbours;
				}
			}
			if (numNeighbours > 0) {
				avoiddirection /= numNeighbours;
				p->addVelocity(avoiddirection * this->getWeight());
			}
			
		}
	private:
		float _radius;
};

/** distracts all particles from another if inside a given radius, and updates the position immeadately */
template <class ParticleClass>
class ParticleRigidDistractorOperator : public cefix::ParticleOperator<ParticleClass> {
	public:
		/** ctor
		 *  @param radius distracts particle inside given radius
		 *  @param weight weight of this particle-operator 
		 */
		ParticleRigidDistractorOperator(float radius, float weight = 1.0f) : cefix::ParticleOperator<ParticleClass>(weight), _radius(radius) {}
	
		
		inline void operator()(ParticleClass* p, const typename ParticleClass::time_t& dt) {
			
			osg::Vec3 avoiddirection;
			unsigned int numNeighbours = 0;
			
			const typename cefix::ParticleGroup<ParticleClass>::List& list = this->getGroup()->getParticles();
			for(typename cefix::ParticleGroup<ParticleClass>::List::const_iterator i = list.begin(); i != list.end(); ++i) {
	
				osg::Vec3 delta =  p->getPosition() - (*i)->getPosition();
				if (((*i) != p) && (delta.length2() <= _radius*_radius)) {
					avoiddirection += delta * ((_radius/delta.length()));
					++numNeighbours;
				}
			}
			if (numNeighbours > 0) {
				avoiddirection /= numNeighbours;
				p->interpolateVelocity(avoiddirection,this->getWeight());
				//p->update(dt);

			}
			
		}
	private:
		float _radius;
};


/** constrains the speedto a given minimum/maximum if outside the constraints. */
template <class ParticleClass>
class ParticleSpeedConstraintOperator : public cefix::ParticleOperator<ParticleClass> {
	public:
		/** @param minspeed minimum velocity 
		 *  @param maxspeed maximum velocity */
		ParticleSpeedConstraintOperator(float minspeed, float maxspeed) : cefix::ParticleOperator<ParticleClass>(), _minSpeed(minspeed), _maxSpeed(maxspeed) {}
	
		
		inline void operator()(ParticleClass* p, const typename ParticleClass::time_t& dt) 
		{
			osg::Vec3 velo = p->getVelocity();
			if (velo.length2() > (_maxSpeed*_maxSpeed)) 
			{
				p->setVelocity(velo * (_maxSpeed / velo.length()));
			} 
			else if (velo.length2() < (_minSpeed*_minSpeed)) 
			{
				p->setVelocity(velo * (_minSpeed / velo.length()));
			}
			
		}
	private:
		float _minSpeed;
		float _maxSpeed;
};


/** align the direction of particles inside a given radius */
template <class ParticleClass>
class ParticleAlignedDirectionOperator : public cefix::ParticleOperator<ParticleClass> {
	public:
		/** ctor
		 *  @param actionRadius align to other particles inside given radius
		 *  @param weight weight of this particle-operator
		 */
		ParticleAlignedDirectionOperator(float actionRadius, float weight) : cefix::ParticleOperator<ParticleClass>(weight), _direction(osg::Vec3(0,0,0)), _actionRadius(actionRadius*actionRadius) {}
		
		
		
		inline void operator()(ParticleClass* p, const typename ParticleClass::time_t& dt) {
			osg::Vec3 direction;
			unsigned int numNeighbours = 0;
			
			const typename cefix::ParticleGroup<ParticleClass>::List& list = this->getGroup()->getParticles();
			for(typename cefix::ParticleGroup<ParticleClass>::List::const_iterator i = list.begin(); i != list.end(); ++i) {
	
				osg::Vec3 delta =  p->getPosition() - (*i)->getPosition();
				if (((*i) != p) && (delta.length2() <= _actionRadius)) {
					direction += (*i)->getDirection();
					++numNeighbours;
				}
			}
			if (numNeighbours > 0) {
				direction /= numNeighbours;
				direction = direction -  p->getDirection();
				p->addVelocity(direction * this->getWeight());
			}
			
		}
	private:
		osg::Vec3	_direction;
		float		_actionRadius;
};

/** sets the acceleration of the particle */
template <class ParticleClass>
class ParticleSetAccelerationOperator : public cefix::ParticleOperator<ParticleClass> {
	
public:	
	/** ctor
	 *  @param acceleration acceleration to add */
	ParticleSetAccelerationOperator(osg::Vec3 acceleration = osg::Vec3(0,0,0), float weight = 1.0f) : cefix::ParticleOperator<ParticleClass>(weight), _acceleration(acceleration) {}
	
	/** sets the acceleration to the gracvity on earth */
	inline void setToGravity(float scale = 1.0f) { _acceleration.set(0, 0, -9.80665f * scale); }
	
	inline void operator()(ParticleClass* p, const typename ParticleClass::time_t& dt) {
		p->setAcceleration(_acceleration * this->getWeight());
	}
	
	void setAcceleration(const osg::Vec3& a) { _acceleration = a; }
	const osg::Vec3& getAcceleration() const { return _acceleration; }
private:
	osg::Vec3 _acceleration; 
	
};


/** adds an acceleration to the particle */
template <class ParticleClass>
class ParticleAccelerationOperator : public cefix::ParticleOperator<ParticleClass> {

	public:	
		/** ctor
		 *  @param acceleration acceleration to add */
		ParticleAccelerationOperator(osg::Vec3 acceleration = osg::Vec3(0,0,0), float weight = 1.0f) : cefix::ParticleOperator<ParticleClass>(weight), _acceleration(acceleration) {}
		
		/** sets the acceleration to the gracvity on earth */
		inline void setToGravity(float scale = 1.0f) { _acceleration.set(0, 0, -9.80665f * scale); }
		
		inline void operator()(ParticleClass* p, const typename ParticleClass::time_t& dt) {
			p->addVelocity(_acceleration * this->getWeight());
		}
		
		void setAcceleration(const osg::Vec3& a) { _acceleration = a; }
		const osg::Vec3& getAcceleration() const { return _acceleration; }
	private:
		osg::Vec3 _acceleration; 
		
};

/** reflect particles on a given plane */
template <class ParticleClass>
class ParticleReflectionOnPlaneOperator : public cefix::ParticleOperator<ParticleClass> {
	
	public:
		/** @param plane plane on which particvles get reflected
		 *  @parem demping demps the reflection
		 *  @param weight weight of this particle-operator
		 */
		ParticleReflectionOnPlaneOperator(const osg::Plane& plane, float demping = 0.0f, float weight = 1.0f) : 
			ParticleOperator<ParticleClass>(weight),
			_plane(plane),
			_demping(1-demping)
		{
		}
		
		
		inline void operator()(ParticleClass* p, const typename ParticleClass::time_t& dt) {
			osg::Vec3 v(p->getVelocity());
			float dist = _plane.distance(p->getPosition());
			if (dist < 0.1) 
			{	/// TODO: force is not computed correctly, some jittering occurs
				
				// R = 2*(-I dot N)*N + I 
				osg::Vec3 normalizedVelocity(v / v.length());
				osg::Vec3 newVelocity =  (_plane.getNormal() * (2 * ( -normalizedVelocity * _plane.getNormal()))) + normalizedVelocity;
				newVelocity *= v.length() * _demping;
				p->setVelocity(newVelocity);
		
			}
		}
		
		
	private:
		osg::Plane	_plane;
		float		_demping;

};

/** marks a particle as dead, when the velocity of the particle is smaller than a given threshold */
template <class ParticleClass>
class ParticleRemoveWhenNoVelocityOperator : public cefix::ParticleOperator<ParticleClass> {
	
	public:
		/** @param minimalLifetime minimal lifetime befor the velocity is checked
		 *  @param minVelocity minimal velocity a particle must have, so it is not marked as dead 
		 */
		ParticleRemoveWhenNoVelocityOperator(float minimalLifetime = 1.0f, float minVelocity = 0.001) : 
			ParticleOperator<ParticleClass>(),
			_minLifetime(minimalLifetime),
			_minVelocity2(minVelocity*minVelocity)
		{
		}
		
		inline void operator()(ParticleClass* p, const typename ParticleClass::time_t& dt) 
		{
			if ((p->getAge() > _minLifetime) && (p->getVelocity().length2() <= _minVelocity2)) {
				p->mustDie();
			}
		}

	private:
		float _minLifetime, _minVelocity2;
};


/** adds a random velocity to all particles */
template <class ParticleClass>
class RandomVelocityOperator : public cefix::ParticleOperator<ParticleClass> {
	public:

		RandomVelocityOperator(osg::Vec3 maxVelocity, float chance, float weight = 1.0f) : cefix::ParticleOperator<ParticleClass>(weight), _maxVelocity(maxVelocity), _chance(chance) {}
		
		void operator()(ParticleClass* p, const typename ParticleClass::time_t& dt) {
			if (cefix::randomf(1.0f) < _chance) {
				p->addVelocity(osg::Vec3(	cefix::randomf(_maxVelocity.x()) - _maxVelocity.x()/2.0f,
											cefix::randomf(_maxVelocity.y()) - _maxVelocity.y()/2.0f, 
											cefix::randomf(_maxVelocity.z()) - _maxVelocity.z()/2.0f) * this->getWeight());
			}
		}
	private:
		osg::Vec3 _maxVelocity;
		float	_chance;
};

/** constrain the particles to stay inside a sphere */
template <class ParticleClass>
class ParticleConstrainToSphereOperator : public cefix::ParticleOperator<ParticleClass> {
	public:
		/** @param center center of the sphere 
		 *  @param radius radius of the spehre
		 *  @param weight weight of this particle-operator
		 */
		ParticleConstrainToSphereOperator(osg::Vec3 center, float radius, float weight = 1.0f) : 
			cefix::ParticleOperator<ParticleClass>(weight),
			_center(center), 
			_radius2(radius*radius), 
			_weight(-weight) 
		{
		}
		
		inline void operator()(ParticleClass* p, const typename ParticleClass::time_t& dt) {
			osg::Vec3 delta = p->getPosition() - _center;
			if (delta.length2() > _radius2) {
				p->addVelocity(-delta * this->getWeight());
			}
		}
		
	private:
		osg::Vec3 _center;
		float	_radius2, _weight;
};

/** slows down the velocity of all particles */
template <class ParticleClass>
class ParticleSlowDownOperator : public cefix::ParticleOperator<ParticleClass> {
	public:
		/** @param factor factor which gets multiplied with the current velocity, should be smaller than 1 */
		ParticleSlowDownOperator(float factor) : cefix::ParticleOperator<ParticleClass>(), _fac(factor) {}
		
		inline void operator()(ParticleClass* p, const typename ParticleClass::time_t& dt) {
			p->setVelocity(p->getVelocity() * _fac);
		}
		
		float getValue() const { return  _fac; }
		void setValue(float f) { _fac = f; }
        
        void applyWidgetComposer(WidgetComposer& composer) {
            composer.addSlider("SLOW-DOWN", doubleRange(0,1), &_fac); 
        }  
				
	private:
		float _fac;
};

/** computes the friction of the particle inside a given fluid, applies a wind-force if needed */
template <class ParticleClass>
class FluidFrictionParticleOperator : public cefix::ParticleOperator<ParticleClass> {
	
	public:
		/** @param wind force of the wind
		 *  @param weight weight of this particle-operator
		 */
		FluidFrictionParticleOperator(osg::Vec3 wind = osg::Vec3(0,0,0), float weight= 1.0f) : 
			ParticleOperator<ParticleClass>(weight), 
			_viscosityCoefficient(0),
			_densityCoefficeint(0),
			_density(0),
			_viscosity(0),
			_ovr_rad(0),
			_wind(wind)
		{ 
			setFluidToAir(); 
		}
		
		/** @return the density of the fluid */
		inline float getFluidDensity() const
		{
			return _density;
		}

		/** @return the viscosity of the fluid */
		inline float getFluidViscosity() const
		{
			return _viscosity;
		}
		
		/** sets the density of the fluid */
		inline void setFluidDensity(float d)
		{
			_density = d;
			_densityCoefficeint = 0.2f * osg::PI * _density;
		}

		/** sets the viscosity of the fluid */
		inline void setFluidViscosity(float v)
		{
			_viscosity = v;
			_viscosityCoefficient = 6 * osg::PI * _viscosity;
		}
		
		/** sets the fluid to air */
		inline void setFluidToAir()
		{
			setFluidViscosity(1.8e-5f);
			setFluidDensity(1.2929f);
			setToGravity(1.0f);
		}

		/** sets the fluid to water */
		inline void setFluidToWater()
		{
			setFluidViscosity(1.002e-3f);
			setFluidDensity(1.0f);
			setToGravity(1.0f);
		}
		
		
		/// Set the acceleration vector.
        inline void setAcceleration(const osg::Vec3& v) { _acceleration = v; }
        
        /// Get the acceleration vector.
        inline const osg::Vec3& getAcceleration() const { return _acceleration; }
		
		/** Set the acceleration vector to the gravity on earth (0, 0, -9.81).
            The acceleration will be multiplied by the <CODE>scale</CODE> parameter.
        */
        inline void setToGravity(float scale = 1.0f) { _acceleration.set(0, 0, -9.80665f*scale); }

		
		inline void operator()(ParticleClass* particle, const typename ParticleClass::time_t& dt)
		{
			// taken from the osgparticle-system
			const float four_over_three = 4.0f/3.0f;
			
			float radius = particle->getRadius();
            float Area = osg::PI*radius*radius;
            float Volume = Area*radius*four_over_three;
        
            // compute force due to gravity + boyancy of displacing the fluid that the particle is emersed in.
            osg::Vec3 accel_gravity = _acceleration * ((particle->getMass() - _density*Volume) * particle->getMassInv());
            
            // compute force due to friction
            osg::Vec3 velBefore = particle->getVelocity();
            osg::Vec3 relative_wind = particle->getVelocity()-_wind;            
            osg::Vec3 wind_force = - relative_wind * Area * (_viscosityCoefficient + _densityCoefficeint*relative_wind.length());
            osg::Vec3 wind_accel = wind_force * particle->getMassInv();

            double critical_dt2 = relative_wind.length2()/wind_accel.length2();
            double compenstated_dt = dt;
            if (critical_dt2 < dt*dt)
            {
                // osg::notify(osg::NOTICE)<<"** Could be critical: dt="<<dt<<" critical_dt="<<sqrtf(critical_dt2)<<std::endl;
                compenstated_dt = sqrtf(critical_dt2)*0.8f;
            }
			particle->addVelocity(accel_gravity*dt + wind_accel*compenstated_dt);
		}

	
	private:
        float       _viscosityCoefficient;
        float       _densityCoefficeint;
        float       _density;
        float       _viscosity;
        float       _ovr_rad;
        osg::Vec3   _wind, _acceleration;

};


}
#endif