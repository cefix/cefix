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

#ifndef QUAD_2D_GEOMETRY_HEADER
#define QUAD_2D_GEOMETRY_HEADER

#include <osg/Geometry>
#include <osg/TextureRectangle>
#include <osg/Texture2D>
#include <osg/StateSet>
#include <cefix/Export.h>

namespace cefix {

/** 
 * Quad2DGeometry creates a quad suitable for 2D-rendering. You can attach an image/texture, etc
 * Be careful, the geometry is laid out on the X/Y-plane, suitbale for HUDs, origin left/bbottom
 */
class CEFIX_EXPORT Quad2DGeometry : public osg::Geometry {

    public:
        /**
         * creates a new Quad2DGeometry
         * @param left left of the quad
         * @param top top of the quad
         * @param width width of the quad
         * @param height height of the quad
         */
        Quad2DGeometry(float left, float top, float width, float height) {
            init();
            setRect(left, top, width, height, true);
            
        }
        
        
        /**
         * creates a new Quad2DGeometry
         * @param rect the rect of the quad
         */
        Quad2DGeometry(const osg::Vec4& rect) {
            init();
            setRect(rect, true);
            
        }
        
        /**
         * creates a new Quad2DGeometry
         * @param left left of the quad
         * @param top top of the quad
         * @param width width of the quad
         * @param height height of the quad
         * @param z z-value of the quad (for depth-sorting)
         */
        Quad2DGeometry(float left, float top, float width, float height, float z) {
            init();
            setRect(left, top, width, height, true);
            setLocZ(z);
        }
        
        /** sets the rect
         *  @param rect the new rect for this quad 
         *  @param resetTExtureRect set to true, if you want to reset the texture-rect
         */
        virtual void setRect(const osg::Vec4& rect, bool resetTextureRect = false);
        
        /**
         * sets the rect of this quad
         * @param left left of the quad
         * @param top top of the quad
         * @param width width of the quad
         * @param height height of the quad
         * @param resetTExtureRect set to true, if you want to reset the texture-rect
         */
        void setRect(float left, float top, float width, float height, bool resetTextureRect = false) {
            setRect(osg::Vec4(left, top, width+left, height+top), resetTextureRect);
        }
        
        /** sets the topleft corner
         *  @param left new left of the quad
         *  @param top new top of the quad */
        inline void setTopLeft(float left, float top) {
            setRect(osg::Vec4(left, top, getWidth(), getHeight()));
        }
        
        /** sets the center of this quad */
        inline void setCenter(float x, float y) {
            float w = getWidth();
            float h = getHeight();
            setRect(x- w/2.0f, y - h/2.0f, w,h);
        }
        
        inline void setCenter(const osg::Vec2& center) {
            setCenter(center[0], center[1]);
        }
        
        /// get the center
        inline osg::Vec2 getCenter() {
            return osg::Vec2(getLeft() + getWidth() / 2.0, getTop() + getHeight()/2.0);
        }
        
        /** @return the left of this quad */
        inline float getLeft() const { return _rect[0]; }
        
        /** @return the top of this quad */
        inline float getTop() const { return _rect[1]; }
        
        /** @return the width of this quad */
        inline float getWidth() const { return _rect[2] - _rect[0]; }
        
        /** @return the height of this quad */
        inline float getHeight() const { return _rect[3] - _rect[1]; }
        
        /** @return the rect of this quad */
        inline const osg::Vec4& getRect() const { return _rect; }
        
        /** sets the texture-rect */
        virtual void setTextureRect(const osg::Vec4& rect);        
        inline void setTextureRect(float left, float top, float width, float height) {
            setTextureRect(osg::Vec4(left, top, width+left, height+top));
        }
		
		inline osg::Vec4 getTextureRect() const { return _texRect; }
		inline osg::Vec4& getTextureRect() { return _texRect; }
        
        /** sets the locZ */
        inline void setLocZ(float z) { 
            _locZ = z; 
            setRect(_rect); 
        }
        
        /** @return the locZ of this quad */
        inline float getLocZ() const { 
            return _locZ;
        }
        
        /** sets the fillcolor to c */
        virtual void setColor(const osg::Vec4& c) {
            _color = c;
            osg::Vec4Array* ca = dynamic_cast<osg::Vec4Array*>(getColorArray());
			for (unsigned int i = 0; i < ca->size(); i++)
				(*ca)[i] = c;
				
            dirtyDisplayList();
        }
		
		virtual void setAlpha(float f) {
            _color[3] = f;
            osg::Vec4Array* ca = dynamic_cast<osg::Vec4Array*>(getColorArray());
			for (unsigned int i = 0; i < ca->size(); i++)
				(*ca)[i][3] = f;
				
            dirtyDisplayList();
        }
		
        /** @return the fill-color of this quad */
        inline const osg::Vec4& getColor() const { return _color; }
        
        /** sets the texture for this quad */
        inline void setTexture(osg::Texture* tex) {
            _tex = tex;
            osg::StateSet* st = getOrCreateStateSet();
			st->removeTextureAttribute(0, tex->getType());
            st->setTextureAttributeAndModes(0, tex, osg::StateAttribute::ON);
        }
        /** creates a new texture with the given image and apply it to the quad*/
        void setTextureFromImage(osg::Image* img, bool useTextureRectangle = true);
        /** @return the texture object */
        inline osg::Texture* getTexture() const { return _tex.get(); }
         /** @return the TextureRectangle object */
        inline osg::TextureRectangle* getTextureRectangle() const { return (_tex.valid()) ? dynamic_cast<osg::TextureRectangle*>(_tex.get()) : NULL; }
		/** @return the TExture2D object (if any) */
        inline osg::Texture2D* getTexture2D() const { return (_tex.valid()) ? dynamic_cast<osg::Texture2D*>(_tex.get()) : NULL; }

		
		static cefix::Quad2DGeometry* createFromImage(osg::Image* img, const osg::Vec3& pos = osg::Vec3(0,0,0), bool flip_tex = false);
        
    protected:
		Quad2DGeometry() {};
    
        void init(); 
		
		virtual ~Quad2DGeometry() {}
        
        osg::Vec4   _rect;
        osg::Vec4   _texRect;
        osg::Vec4   _color;
        float       _locZ;
        osg::ref_ptr<osg::Texture> _tex;
};

}
#endif

        