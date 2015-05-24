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

#ifndef PARTICLE_BOX_PLACER_HEADER
#define PARTICLE_BOX_PLACER_HEADER

#include <cefix/MathUtils.h>
#include <osg/Vec3>
#include <cefix/ParticlePlacer.h>
namespace cefix {
class Particle; 

/** this class places new particles in a box or plane */
template <class ParticleClass>
class ParticleBoxPlacer : public ParticlePlacer {
	
	public:
		/** @param origin origin of the box 
		 *  @param x x-dimension
		 *  @param y y-dimension
		 *  @param z z-dimension
		 */
		ParticleBoxPlacer(const osg::Vec3& origin = osg::Vec3(0,0,0), float x=1.0f, float y=1.0f, float z=1.0f) : ParticlePlacer(), _origin(origin), _x(x), _y(y), _z(z), _delta(osg::Vec3(x/2.0f, y/2.0f, z/2.0f)) {}
	
		inline osg::Vec3 operator()(ParticleClass* p) {
			return (_origin + osg::Vec3(cefix::randomf(_x), cefix::randomf(_y), cefix::randomf(_z)) - _delta);
		}
        
        void setOrigin(const osg::Vec3& v) { _origin = v; }
        const osg::Vec3& getOrigin() const { return _origin; }
        void setDimensions(float x, float y, float z) { _x = x; _y = y; _z = z; _delta.set(x/2.0, y/2.0, z/2.0); }
	
	private:
		osg::Vec3 _origin;
		float _x,_y,_z;
		osg::Vec3 _delta;

};

}


#endif
