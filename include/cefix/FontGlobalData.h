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

#ifndef __FONT_GLOBAL_DATA_HEADER__
#define __FONT_GLOBAL_DATA_HEADER__

#include <osg/Referenced>
#include <osg/Vec3>
#include <string>
#include <cefix/tinyxml.h>
#include <cefix/Export.h>


namespace cefix {
/**
 * this class encapsulates all data specific to a font
 * for example the name, the size, etc
 */
class CEFIX_EXPORT FontGlobalData : public osg::Referenced {
	
	protected:
	FontGlobalData();

    public:
		/** 
		 * constructs the global font data from a xml-node
		 * @param node the xml-node
		 */
        FontGlobalData(TiXmlElement* node);
        
		/** @return the name of the font */
		inline const std::string& fontName() {return _fontName;}
		
		/** @return the fontsize fo this font */
        //inline const int &fontSize() { return _nativeFontSize;}
		
		/** @return the blur-amount for this font, not supported anymore */
        inline float blurAmount() {return _blurAmount;}
		
		/** @return the text-color for this font */
        inline const osg::Vec3& textColor() { return _textColor;}
		
		/** @return the standard value for kerning */
        inline int standardKerning() { return _standardKerning;}
		
		/** @return the width of the texture (textures are quadratic) */
        inline int getTextureWidth() { return _textureWidth; }

		/** @return the height of the texture (textures are quadratic) */
        inline int getTextureHeight() { return _textureHeight; }        
        
		
		/** @return true if the texture-coords should be absolute and not normalized, needed für reactangle-textures */
        inline bool useAbsoluteTexCoords() { return _absoluteTexCoordsFlag;}
        
		/** sets the AbsoluteTexCoordsFlag */
        inline void setAbsoluteTexCoordsFlag(bool b) {  _absoluteTexCoordsFlag = b; }
		
		/** sets the starting position 
		 * @param p the new startpos
		 */
        inline void setStartPosition(const osg::Vec3& p) {_startPosition = p;}
        
		/** @return the starting position for rendering */
		inline const osg::Vec3& startPosition() { return _startPosition; }
        
		/** sets the prescale-factor to scale */
        inline void setPreScale(float scale) {_preScale = scale;}
        
		/** @return the prescale-factor */
		inline float preScale() {return _preScale;}
		
		/** sets the fontsize to pointSize */
		inline void setFontSize(float pointSize) { 
            if (pointSize != 0)
                setPreScale( pointSize / _nativeFontSize );
            else
                setPreScale( 1 );
        }
		
		inline float getCurrentFontSize() const  { return _nativeFontSize * _preScale; }
        
        inline float getNativeFontSize() const { return _nativeFontSize; }
		
		inline float getMajuscleHeight() const { return _majuscleHeight; }
		inline float getMinuscleHeight() const { return _minuscleHeight; }
		
		inline void setMinuscleHeight(float f) { _minuscleHeight = f; }
		inline void setMajuscleHeight(float f) { _majuscleHeight = f; }
		inline unsigned int getTextureCount() { return _textureCount; }
		
		inline unsigned int getBleed() const { return _bleed; }
    
    protected:
		virtual ~FontGlobalData() {}
        
        std::string _fontName;
        int _nativeFontSize;
        float _blurAmount;
        osg::Vec3 _textColor;
        int _standardKerning;
        int _textureWidth, _textureHeight;
        bool _absoluteTexCoordsFlag;
        osg::Vec3 _startPosition;
					
		float _minuscleHeight, _majuscleHeight;
        
        float _preScale;
		
		unsigned int _textureCount;
		unsigned int _bleed;
    };

}

#endif


