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

#ifndef LINE_STRIP_GEOMETRY
#define LINE_STRIP_GEOMETRY

#include <osg/Geometry>

namespace cefix {
/** simple geometry based class implementing an optionally closed linestrip */
class LineStripGeometry : public osg::Geometry {

	public:
		LineStripGeometry(bool isClosed = false, unsigned int maxPointsShown = 200, float minDistance = 1.0f);		
		void setColor(const osg::Vec4& c) {
			(*_c)[0] = c;
			dirtyDisplayList();
		}
		
		void setAlpha(float f) {
			(*_c)[0][3] = f;
			dirtyDisplayList();
		}
		
		
		const osg::Vec4& getColor() const {
			return (*_c)[0];
		}
		
		
		void clear() 
		{
			_lastInsertIndex = 0;
			_da->setCount(0);
			dirtyDisplayList();
			dirtyBound();
		}
		
		void add(const osg::Vec2& v, float loc_z, bool cleanDirty = true) {
			add(osg::Vec3(v[0], v[1], loc_z), cleanDirty);
		}
		
		void add(const osg::Vec3 v, bool cleanDirty = true);
		
		inline void update() {
			dirtyDisplayList();
			dirtyBound();
		}
		
		void resize(unsigned int sz) {
			_v->resize(sz);
			_da->setCount(sz);
		}
		
		inline osg::Vec3Array* getVertices() { return _v.get(); }
        
        void setMaxPointsShown(unsigned int u) { _maxPointsShown = u; }
        void setMinDistance(float f) { _minDistance = f; }
		
	private:
		osg::ref_ptr< osg::Vec3Array > _v;
		osg::ref_ptr< osg::Vec4Array > _c;
		osg::ref_ptr< osg::DrawArrays > _da;
		unsigned int _lastInsertIndex;
		osg::Vec3	_last;
		unsigned int _maxPointsShown;
		float _minDistance;

};

}

#endif