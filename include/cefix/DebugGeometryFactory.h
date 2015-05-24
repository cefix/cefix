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

#ifndef CEFIX_DEBUG_GEOMETRY_HEADER
#define CEFIX_DEBUG_GEOMETRY_HEADER

#include <osg/Geometry>
#include <osg/Geode>
#include <osg/Point>
#include <cefix/Export.h>

#ifndef NO_DEBUG_GEOMETRY
#define USE_DEBUG_GEO(x) x;
#else
#define USE_DEBUG_GEO(x)
#endif

namespace cefix {
/** base class defining the public interface for the inherited classes */
class CEFIX_EXPORT DebugOutputBase : public virtual osg::Referenced{
	public:
		/** ctor */
		DebugOutputBase()
		:	_pointColor(osg::Vec4(1,0,0,0.7)),
			_lineStartColor(osg::Vec4(0,1,0,0.7)),
			_lineEndColor(osg::Vec4(0,1,0,0.7)),
			_normalScale(10.0f),
            _locZ(999),
            _numSegments(-1)
		{
		}
		
		/** adds a point */
		inline void addPoint(const osg::Vec3& point) { addPoint(point, _pointColor); }
		
		/** adds a 2D-point (z = 0) */
		inline void addPoint(const osg::Vec2& point) { addPoint(asVec3(point), _pointColor); }
		
		/** adds a 2D-point (z = 0) with custom color*/
		inline void addPoint(const osg::Vec2& point, const osg::Vec4& c) { addPoint(asVec3(point), c); }
	
		/** adds a line */
		inline void addLine(const osg::Vec3& s_point, const osg::Vec3& e_point) { addLine(s_point, e_point, _lineStartColor, _lineEndColor); }
		
		/** adds a line with custom color */
		inline void addLine(const osg::Vec3& s_point, const osg::Vec3& e_point, const osg::Vec4& color) { addLine(s_point, e_point, color, color); }
		
		/** adds a 2d-Line (z=0) */
		inline void addLine(const osg::Vec2& s_point, const osg::Vec2& e_point) { addLine(asVec3(s_point), asVec3(e_point), _lineStartColor, _lineEndColor); }
		
		/** adds a 2d-Line with custom color (z=0) */
		inline void addLine(const osg::Vec2& s_point, const osg::Vec2& e_point, const osg::Vec4& color) { addLine(asVec3(s_point), asVec3(e_point), color, color); }
		
		/** adds a 2d-Line (z=0) with custom start- and end-color*/
		inline void addLine(const osg::Vec2& s_point, const osg::Vec2& e_point, const osg::Vec4& s_color, const osg::Vec4& e_color)
		{
			addLine(asVec3(s_point), asVec3(e_point), s_color, e_color);
		}
		
		inline void addNormal(const osg::Vec3& p, const osg::Vec3& normal) 
		{
			addLine(p, p+normal, _lineStartColor, _lineEndColor);
		}

		inline void addNormal(const osg::Vec3& p, const osg::Vec3& normal, const osg::Vec4& color) 
		{
			addLine(p, p+normal, color);
		}
		
		inline void addNormal(const osg::Vec3& p, const osg::Vec3& normal, const osg::Vec4& s_color, const osg::Vec4& e_color) 
		{
			addLine(p, p+normal, s_color, e_color);
		}
		
		inline void addNormal(const osg::Vec2& p, const osg::Vec2& normal, const osg::Vec4& s_color, const osg::Vec4& e_color) 
		{
			addLine(asVec3(p), asVec3(p+(normal*_normalScale)), s_color, e_color);
		}
		
		inline void addNormal(const osg::Vec2& p, const osg::Vec2& normal, const osg::Vec4& color) 
		{
			addLine(asVec3(p), asVec3(p+(normal*_normalScale)), color);
		}
		
		inline void addNormal(const osg::Vec2& p, const osg::Vec2& normal) 
		{
			addLine(asVec3(p), asVec3(p+(normal*_normalScale)), _lineStartColor, _lineEndColor);
		}
		
		inline void addRect(const osg::Vec3& topLeft, float width, float height, const osg::Vec4& color) 
		{
			addLine(topLeft,								topLeft + osg::Vec3(width, 0,0), color);
			addLine(topLeft + osg::Vec3(width, 0, 0),			topLeft + osg::Vec3(width, height,0), color);
			addLine(topLeft + osg::Vec3(width, height, 0),		topLeft + osg::Vec3(0, height,0), color);
			addLine(topLeft + osg::Vec3(0, height, 0),			topLeft + osg::Vec3(0, 0,0), color);
		}
		
		inline void addRect(const osg::Vec2& topLeft, float width, float height, const osg::Vec4& color) {
			addRect(asVec3(topLeft), width, height, color);
		}
        
        inline void addRect(const osg::Vec4& r, const osg::Vec4& color) {
			addRect(osg::Vec2(r[0], r[1]), r[2]-r[0], r[3]-r[1], color);
		}
		
		
		
		inline void addEllipse(const osg::Vec3& center, float radiusx, float radiusy, const osg::Vec4& color) 
		{
			unsigned int  nsegments = (_numSegments > 0) ? _numSegments : osg::clampTo<unsigned int>(2*osg::PI * (radiusx+radiusy) / 20.0, 5, 72);
			float dAngle =  (2*osg::PI) / (float) nsegments;
			for(unsigned int i= 0; i < nsegments; ++i) {
				addLine(center + osg::Vec3(cos(dAngle*i)*radiusx, sin(dAngle*i)*radiusy, 0), center + osg::Vec3(cos(dAngle*(1+i))*radiusx, sin(dAngle*(i+1))*radiusy, 0), color);
			}				
		}
		
		inline void addCircle(const osg::Vec3& center, float radius, const osg::Vec4& color) { addEllipse(center, radius, radius, color); }
		
		inline void addEllipse(const osg::Vec2& center, float radiusx, float radiusy, const osg::Vec4& color) { addEllipse(asVec3(center), radiusx, radiusy, color); }
		inline void addCircle(const osg::Vec2& center, float radius, const osg::Vec4& color) { addEllipse(asVec3(center), radius, radius,color); }
	
		/** adds a quad (as linestrip) */
		inline void addQuad(osg::Vec3Array* arr, unsigned int start_ndx) { addQuad(arr, start_ndx, _lineStartColor); }
			
		/** adds a quad with custom color */
		void addQuad(osg::Vec3Array* arr, unsigned int start_ndx, const osg::Vec4 color) {
			for(unsigned int i = 0; i < 3; ++i) {
				addLine((*arr)[i+start_ndx], (*arr)[i+start_ndx+1], color);
			}
			addLine((*arr)[3+start_ndx], (*arr)[start_ndx], color);
		}
	
		/** adds a point (to be implemented by a subclass) */
		virtual void addPoint(const osg::Vec3& point, const osg::Vec4& color) = 0;
		
		/** adds a line (to be implemented by a subclass) */
		virtual void addLine(const osg::Vec3& s_point, const osg::Vec3& e_point, const osg::Vec4& s_color, const osg::Vec4& e_color) = 0;
		
		/** clears the output */
		virtual void clear() = 0; 
		
		/** returns a geode containing the debug-geometry */
		virtual osg::Node* getOrCreateNode() = 0;
		
		/** sets the point size */
		virtual void setPointSize(float ps) = 0;
		
		/** sets the default point-color */
		void setPointColor(const osg::Vec4& c) { _pointColor = c; }
		
		/** sets the default line-color */
		void setLineColor(const osg::Vec4& s_c, const osg::Vec4& e_c) { _lineStartColor = s_c; _lineEndColor = e_c; }
		
		/** sets the default line-color */
		void setLineColor(const osg::Vec4& c) { setLineColor(c, c); }
		
		inline void setNormalScale(float f) { _normalScale = f; }
		
		virtual void finish() {}
		
		
		virtual void addText(const osg::Vec3& pos, const std::string& text, const osg::Vec4& color = osg::Vec4(1,1,1,1), const std::string& font="system.xml", float fontsize = 0) = 0;
		inline  void addText(const osg::Vec2& pos, const std::string& text, const osg::Vec4& color = osg::Vec4(1,1,1,1), const std::string& font="system.xml", float fontsize = 0)
		{
			addText(asVec3(pos), text, color, font, fontsize);
		}
        
        inline void setLocZ(float z) {_locZ = z; }
        void setNumSegments(int f) { _numSegments = f; }
        int getNumSegments() const { return _numSegments; }
        
	protected:
		virtual ~DebugOutputBase() {}
	private:
		osg::Vec3 asVec3(const osg::Vec2& v) { return osg::Vec3(v[0], v[1], _locZ); }
		
	protected:
		osg::Vec4 _pointColor, _lineStartColor, _lineEndColor;
		float _normalScale, _locZ;
        int _numSegments;
};

/** no-op implementation of DebugOutputBase, this is what you get for a disabled debuggeometry */
class CEFIX_EXPORT NoDebugOutput : public DebugOutputBase {
	public:
		
		virtual void addPoint(const osg::Vec3& point, const osg::Vec4& color) {};
		virtual void addLine(const osg::Vec3& s_point, const osg::Vec3& e_point, const osg::Vec4& s_color, const osg::Vec4& e_color) {}
		virtual void clear() {}; 
		virtual osg::Node* getOrCreateNode() { return NULL;}		
		virtual void setPointSize(float ps) {};
		virtual void addText(const osg::Vec3& pos, const std::string& text, const osg::Vec4& color = osg::Vec4(1,1,1,1), const std::string& font="system.xml", float fontsize = 0) {}

	public:
		virtual ~NoDebugOutput() {}
		
};

/** DbeugGeometry implements all needed methods og DebugOutputBase and isthe standard object returned for enabled debuggeometry*/
	
class CEFIX_EXPORT DebugGeometry : public DebugOutputBase {

	public:
        #ifdef CEFIX_FOR_IPHONE
            typedef osg::DrawElementsUShort DrawElements;
        #else
            typedef osg::DrawElementsUInt DrawElements;
		#endif
        
        DebugGeometry();
		
		virtual void addPoint(const osg::Vec3& point, const osg::Vec4& color) 
		{
			_vertices->push_back(point);
			_colors->push_back(color);
			_points->push_back(_vertices->size()-1);
			if (_geode.valid()) _geode->setNodeMask(0xFFFFFFFF);
		}
		
		virtual void addLine(const osg::Vec3& s_point, const osg::Vec3& e_point, const osg::Vec4& s_color, const osg::Vec4& e_color) 
		{ 
			_vertices->push_back(e_point);
			_vertices->push_back(s_point);
			
			_colors->push_back(s_color);
			_colors->push_back(e_color);
			
			_lines->push_back(_vertices->size()-2);
			_lines->push_back(_vertices->size()-1);
			
		}
		
		virtual void addText(const osg::Vec3& pos, const std::string& text, const osg::Vec4& color = osg::Vec4(1,1,1,1), const std::string& font="system.xml", float fontsize = 0);
				
		virtual osg::Node* getOrCreateNode();
		
		virtual void clear() {
			_vertices->clear();
			_colors->clear();
			_points->clear();
			_lines->clear();
			if (_geo->getNumPrimitiveSets() != 0) _geo->removePrimitiveSet(0, _geo->getNumPrimitiveSets());
			if (_group->getNumChildren()) {
				osg::ref_ptr<osg::Geode> geode = _geode.get();
				_group->removeChild(0, _group->getNumChildren());
				_group->addChild(_geode.get());
			}
		}
		
		void setPointSize(float ps) {
			osg::Point* p = new osg::Point();
			p->setSize(ps);
			_geo->getOrCreateStateSet()->setAttributeAndModes(p, osg::StateAttribute::ON);
		}
		
		virtual void finish() { 
			if (_lines->size() != 0) _geo->addPrimitiveSet(_lines.get());
			if (_points->size() != 0) _geo->addPrimitiveSet(_points.get());
			_geo->dirtyBound(); 
		}

		void setName(const std::string& str) { 
			_geo->setName(str);
		}
		
	private:
		osg::ref_ptr<osg::Geometry> _geo;
		osg::ref_ptr<DrawElements> _points, _lines;
		osg::Vec3Array* _vertices;
		osg::Vec4Array* _colors;
		osg::observer_ptr<osg::Geode> _geode;
		osg::ref_ptr<osg::Group> _group;
			
};


/** DebugGeometryFactory, returns for a given key an object whith convenience methods for drawing point, lines and quads in different colors. 
    You can enable or disable the geometry for a given key, the best way is to compile your debug-drawing code conditionally */
class CEFIX_EXPORT DebugGeometryFactory : public osg::Referenced {
	public:
		typedef std::map<std::string, std::pair<bool, osg::ref_ptr<DebugOutputBase> > > GeometryMap;
		
		/** returns a previously createde DebugOutputBase or creates a new one, enabled by default */
		static DebugOutputBase* get(const std::string& id) { return getInstance()->getOrCreate(id); }
		
		/** enable a specific debugOutputBase */
		static void enable(const std::string& id) { getInstance()->setEnabledFlag(id, true); }
		
		/** disables a specific debugOutputBase */
		static void disable(const std::string& id) { getInstance()->setEnabledFlag(id, false); }
		
		/** resets all debugOutputBases (clears the geometry) */
		static void reset() { getInstance()->resetAllGeometries(); }

		static void finish() { getInstance()->finishAllGeometries(); }
        
        static inline bool inUse() { return (getInstance()->_geometries.size() > 0); }
		
	protected:
		DebugGeometryFactory() : osg::Referenced(), _nodebug(new NoDebugOutput()) {}
		virtual ~DebugGeometryFactory();
		static DebugGeometryFactory* getInstance();
		
		DebugOutputBase* getOrCreate(const std::string& id) {
			GeometryMap::iterator itr = _geometries.find(id);
			if (itr == _geometries.end())
				_geometries[id] = std::make_pair(true, new DebugGeometry() );
			itr = _geometries.find(id);
			return (itr->second.first) ? (itr->second.second.get()) : _nodebug.get();
		}
	
	
		void setEnabledFlag(const std::string& id, bool f) {
			GeometryMap::iterator itr = _geometries.find(id);
			if (itr != _geometries.end()) {
				_geometries[id] = std::make_pair(f, itr->second.second.get());
			}
		}
	
		void resetAllGeometries() {
			for(GeometryMap::iterator i = _geometries.begin(); i != _geometries.end(); ++i) {
				i->second.second->clear();
			}
		}
		
		void finishAllGeometries() {
		for(GeometryMap::iterator i = _geometries.begin(); i != _geometries.end(); ++i) {
			i->second.second->finish();
		}
	}
				
	private:
		GeometryMap _geometries;
		osg::ref_ptr<NoDebugOutput>	_nodebug;

};
}



#endif