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

#ifndef ELLIPSE_2D_GEOMETRY
#define ELLIPSE_2D_GEOMETRY

#include <osg/Geometry>
#include <osg/TextureRectangle>
#include <osg/Texture2D>
#include <osg/StateSet>

namespace cefix {

/** a simple class drawing an ellipse */
class Ellipse2DGeometry : public osg::Geometry{

	public:
		/// ctor, width, and height of ellipse, if resolution = 0 the resoultion is computed automagically
		Ellipse2DGeometry(float width, float height, int resolution = 0, bool fill = true);
		Ellipse2DGeometry(const osg::Vec4& rect, int resolution = 0, bool fill = true);
		
        void setColor(osg::Vec4 c);
        osg::Vec4 getColor() { return _color; }
		
		void setAlpha(float f) { _color[3] = f; setColor(_color); }
		
		void setCenter(float x, float y);
		void setCenter(const osg::Vec2& v) { setCenter(v.x(), v.y()); }
		void setCenter(const osg::Vec3& v) { setCenter(v.x(), v.y()); }
        osg::Vec2 getCenter() { return osg::Vec2(_x, _y); }
		void setWidth(float width);
		void setHeight(float height);
		void setWidthAndHeight(float w, float h) { _width = w; _height = h; updateEllipse(); }
		void setRect(const osg::Vec4& r) 
		{
			_width = (r[2] - r[0]);
			_height = (r[3] - r[1]);
			_x = r[0] + _width / 2.0;
			_y = r[1] + _height / 2.0;
			updateEllipse();
		}
		
		void setResolution(int resolution) { _resolution = resolution; updateEllipse(); }
		
		void setLocZ(float locZ);
		float getLocZ(){return _locZ;}
		
		void setTextureFromImage(osg::Image* img, bool useTextureRectangle = true);
		
		inline void setTexture(osg::Texture* tex) {
			_tex = tex;
			osg::StateSet* st = getOrCreateStateSet();
			st->removeTextureAttribute(0, tex->getType());
			st->setTextureAttributeAndModes(0, tex, osg::StateAttribute::ON);
        }
		
		osg::Vec2 getCenter() const { return osg::Vec2(_x,_y); }
		float getWidth() const { return _width; }
		float getHeight() const { return _height; }
		void setTextureRect(const osg::Vec4& rect) { _textureRect = rect; updateTexture(); }
		osg::Texture* getTexture() { return _tex.get(); }
		
	protected:
		void init();
		void updateEllipse();
		void updateTexture();
		
		inline int computeResolution() 
		{
			return (_resolution > 0) ? _resolution : std::max<int>(8, osg::PI * sqrt(2 * (_width* _width + _height* _height)) / 5.0);
		}
		
		osg::ref_ptr<osg::Texture> _tex;
		osg::Vec4   _color;
		float       _locZ;
		float _x, _y;
		osg::Vec4 _textureRect;
	private:
		float	_width;
		float	_height;
		int		_resolution;
		bool	_fill;
		osg::ref_ptr<osg::DrawArrays> _da;
};

}

#endif // ELLIPSE_2D_GEOMETRY