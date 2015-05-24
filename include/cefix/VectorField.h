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

#ifndef VECTOR_FIELD_HEADER
#define VECTOR_FIELD_HEADER

#include <osg/Math>
#include <osg/Array>
#include <osg/ref_ptr>

#include <vector>
#include <cefix/MathUtils.h>

namespace cefix {
	
	template <class T> struct VectorFieldArrayTraits {};
	
	
	template <>struct VectorFieldArrayTraits<float> {
		typedef osg::FloatArray ArrayType;
	};
	
	template <>struct VectorFieldArrayTraits<double> {
		typedef osg::DoubleArray ArrayType;
	};
	
	template <>struct VectorFieldArrayTraits<osg::Vec3> {
		typedef osg::Vec3Array ArrayType;
	};
	
	template <>struct VectorFieldArrayTraits<osg::Vec2> {
		typedef osg::Vec2Array ArrayType;
	};
	
	template <>struct VectorFieldArrayTraits<osg::Vec4> {
		typedef osg::Vec4Array ArrayType;
	};

/** a vector-field is a threedimensional array of vectors. You can fill it with vectors and get the resulting vector at a given position */
template <class T>
class VectorFieldT : public osg::Referenced {
	
	public:
	typedef typename VectorFieldArrayTraits<T>::ArrayType ArrayType;
		/** ctor */
		VectorFieldT(unsigned int x, unsigned int y, unsigned int z) : osg::Referenced(), _dimx(x), _dimy(y), _dimz(z) 
		{
			_field = new ArrayType();
			resize(x,y,z);
		}
		/** resizes the vectorfield to the given dimensions */
		void resize(unsigned int x, unsigned int y, unsigned int z) {
			_dimx = x; _dimy = y; _dimz = z;
			unsigned int newsize = getDimX() * getDimY() * getDimZ();
			_field->resize(newsize);
		}
		
		//* @return true if p inside the vectorfield */
		bool includes(const osg::Vec3& p) const {
			return ((p.x() >= 0) && (p.x() <= getDimX()) && (p.y() >= 0) && (p.y() <= getDimY()) && (p.z() >= 0) && (p.z() <= getDimZ()));
		}
		
		
		inline unsigned int getDimX() const { return _dimx; }
		inline unsigned int getDimY() const { return _dimy; }
		inline unsigned int getDimZ() const { return _dimz; }
		
		/** returns the cubic interpolated value in the vector field at position */
		inline T get(float ax, float ay, float az) const 
		{
			unsigned int x = osg::clampTo((unsigned int)ax, (unsigned int)0, getDimX());
			unsigned int y = osg::clampTo((unsigned int)ay, (unsigned int)0, getDimY());
			unsigned int z = osg::clampTo((unsigned int)az, (unsigned int)0, getDimZ());
			
			float frag_x = ax - x;
			float frag_y = ay - y;
			float frag_z = az - z;
			
			// wenn wir nix interpolieren müssen, dann gleich rausspringen
			if ((fabs(frag_x) <= 0.001) && (fabs(frag_y) <= 0.001) && (fabs(frag_y) <= 0.001))
				return (*_field)[getIndex(x, y, z)];
				
			
			int dx = ((x > 0) && (x < getDimX()-1)) ? ( (frag_x < 0) ? -1 : 1 ) : 0;
			int dy = ((y > 0) && (y < getDimY()-1)) ? ( (frag_y < 0) ? -1 : 1 ) : 0;
			int dz = ((z > 0) && (z < getDimZ()-1)) ? ( (frag_z < 0) ? -1 : 1 ) : 0;
			
			// nachbarpunkte ermitteln
			T			b000((*_field)[getIndex(x   ,y   ,z   )]), 
						b100((*_field)[getIndex(x+dx,y   ,z   )]), 
						b110((*_field)[getIndex(x+dx,y+dy,z   )]), 
						b010((*_field)[getIndex(x   ,y+dy,z   )]),
						
						b001((*_field)[getIndex(x   ,y   ,z+dz)]),
						b101((*_field)[getIndex(x+dx,y   ,z+dz)]),
						b111((*_field)[getIndex(x+dx,y+dy,z+dz)]),
						b011((*_field)[getIndex(x   ,y+dy,z+dz)]);
			
			T v  = cefix::interpolate(cefix::interpolate(b000, b100, frag_x), cefix::interpolate(b010, b110, frag_x), frag_y);
			T v2 = cefix::interpolate(cefix::interpolate(b001, b101, frag_x), cefix::interpolate(b011, b111, frag_x), frag_y);
			
			return cefix::interpolate(v, v2, frag_z); 
			
		}
		
		inline T get(const osg::Vec3& p)  { return get(p.x(), p.y(), p.z()); }
		
		inline void put(unsigned int x, unsigned int y, unsigned z, const T& v) { (*_field)[getIndex(x,y,z)] = v; }
		inline void put(const osg::Vec3& p, const T& v) { put(p[0], p[1], p[2], v); }
		
		inline void add(unsigned int x, unsigned int y, unsigned z, const T& v, float weight = 1.0) {
			(*_field)[getIndex(x,y,z)] += (v*weight);
		}
		
		inline void interpolate(unsigned int x, unsigned int y, unsigned z, const T& v, float weight) {
			unsigned int ndx = getIndex(x,y,z);
			(*_field)[ndx] = (*_field)[ndx] * (1-weight) + (v*weight);
		}
		
		inline ArrayType* getData() const { return _field.get(); }
		
		void setData(ArrayType* data) { _field = data; }
	
		ArrayType* swapData(ArrayType* data) { 
			osg::ref_ptr<ArrayType> temp = _field.get();
			_field = data;
			return temp.get();
		}
		
	private:
		inline unsigned int getIndex(unsigned int x, unsigned int y, unsigned z) const {
			unsigned int ndx = (z * getDimY() * getDimX()) + (y * getDimX()) + x;
			// std::cout << x << "/"<< y << "/" << z << ": " << ndx << std::endl;
			return ndx; 
		}
		
		unsigned int _dimx, _dimy, _dimz;
		osg::ref_ptr<ArrayType>	_field;

};
template <class T>
inline cefix::VectorFieldT<T>& operator*=(cefix::VectorFieldT<T>& f, float mul) {
	typename cefix::VectorFieldT<T>::ArrayType* v = f.getData();
	for(unsigned int i = 0; i < v->size(); ++i)
		(*v)[i] *= mul;
	return f;
}

}

#endif