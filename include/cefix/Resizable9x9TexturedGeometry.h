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

#ifndef DROP_SHADOW_GEOMETRY_HEADER_
#define DROP_SHADOW_GEOMETRY_HEADER_

#include <cefix/Quad2DGeometry.h>

namespace cefix {
/** 
 * this class implements a resizable geometry 9x9.
 */
class CEFIX_EXPORT Resizable9x9TexturedGeometry : public Quad2DGeometry {

    public:
		/** ctor */
        Resizable9x9TexturedGeometry(osg::Vec4 rect, float locZ, float frameWidth, float frameHeight);
		
		/** sets the rect of the shadow, the given rect defines the rect, which casts the shadow */
        virtual void setRect(const osg::Vec4& r, bool resetTextureRect = false) ;
        
		/** sets the rect */
        inline void setRect(float left, float top, float width, float height) {
			setRect(osg::Vec4(left, top, left+width, top+height));
        }
        
		/** sets the frame-width */
        inline void setFrameWidthAndHeight(float w, float h, bool resetTexFrame = true) {
            _frameWidth = w;
			_frameHeight = h;
            setRect(_rect);
			
			if (resetTexFrame) {
				setTexFrameWidthAndHeight(w,h);
			}
        }
	
	inline float getFrameWidth() const { return _frameWidth; }
	inline float getFrameHeight() const { return _frameHeight; }
	
		inline void setTexFrameWidthAndHeight(float w, float h) 
		{
			_texFrameWidth = w;
			_texFrameHeight = h;
			setTexCoords();
		}
        
		virtual void setTextureRect(const osg::Vec4& rect) {
			Quad2DGeometry::setTextureRect(rect);
			setTexCoords();
		}
		/** compute the tex-coords */
        void setTexCoords() {
            
            if (!_tex.valid())
                return;
            if (_tex->getImage(0) == NULL)
                return;
			
			float dx = _texFrameWidth;
			float dy = _texFrameHeight;
			if (dynamic_cast<osg::Texture2D*> (_tex.get())) {
				 dx /= _tex->getImage(0)->s();
				 dy /= _tex->getImage(0)->t();
            }
			
            //inner-rect
            (*_texCoords)[0].set(_texRect[0] + dx, _texRect[1] + dy); // 0
            (*_texCoords)[1].set(_texRect[2] - dx, _texRect[1] + dy); // 1
            (*_texCoords)[2].set(_texRect[2] - dx, _texRect[3] - dy); // 2
            (*_texCoords)[3].set(_texRect[0] + dx, _texRect[3] - dy); // 3
            
            //topleft
		//  (*_texCoords)[0].set(_texRect[0] + dx, _texRect[1] + dy); // 0
            (*_texCoords)[4].set(_texRect[0] -  0, _texRect[1] + dy); // 4
            (*_texCoords)[5].set(_texRect[0] -  0, _texRect[1] -  0); // 5
            (*_texCoords)[6].set(_texRect[0] + dx, _texRect[1] -  0); // 6
            
            //top-right
            (*_texCoords)[9].set(_texRect[2] +  0, _texRect[1] + dy); // 9
		//  (*_texCoords)[1].set(_texRect[2] - dx, _texRect[1] + dy); // 1
            (*_texCoords)[7].set(_texRect[2] - dx, _texRect[1] -  0); // 7
            (*_texCoords)[8].set(_texRect[2] +  0, _texRect[1] -  0); // 8
            
            //bottom-right
            (*_texCoords)[11].set(_texRect[2] +  0, _texRect[3] +  0); // 11
            (*_texCoords)[12].set(_texRect[2] - dx, _texRect[3] +  0); // 12
		//  (*_texCoords)[ 2].set(_texRect[2] - dx, _texRect[3] - dy); // 2
            (*_texCoords)[10].set(_texRect[2] +  0, _texRect[3] - dy); // 10
            
            //bottomleft
            (*_texCoords)[13].set(_texRect[0] + dx, _texRect[3] +  0); // 13
            (*_texCoords)[14].set(_texRect[0] -  0, _texRect[3] +  0); // 14
            (*_texCoords)[15].set(_texRect[0] -  0, _texRect[3] - dy); // 15
		//  (*_texCoords)[ 3].set(_texRect[0] + dx, _texRect[3] - dy); // 3
			
        }
	private:
		inline void addQuad(osg::DrawElementsUShort* index, unsigned short i1, unsigned short i2, unsigned short i3, unsigned short i4) {
			index->push_back(i1);
			index->push_back(i2);
			index->push_back(i3);
			
			index->push_back(i3);
			index->push_back(i4);
			index->push_back(i1);
        }
		
    protected:
		~Resizable9x9TexturedGeometry() {}
		/** init this class */
        void init();
        
        float _frameWidth, _frameHeight;
		float _texFrameWidth, _texFrameHeight;
        
        osg::ref_ptr<osg::Vec3Array> _vertices;
        osg::ref_ptr<osg::Vec2Array> _texCoords;
};

}

#endif

