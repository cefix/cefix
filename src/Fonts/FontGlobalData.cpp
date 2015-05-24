/*
 *  FontGlobalData.cpp
 *  NativeContext
 *
 *  Created by Stephan Huber on Tue Jan 21 2003.
 *  Copyright (c) 2003 __MyCompanyName__. All rights reserved.
 *
 */

#include <cefix/FontGlobalData.h>
#include <cefix/XMLWrapper.h>
// XML-Inludes:

using namespace cefix;

namespace cefix {
	FontGlobalData::FontGlobalData() 
	:	osg::Referenced(),
		_nativeFontSize(0),
		_blurAmount(0),
		_textColor(0,0,0),
		_standardKerning(0),
		_textureWidth(0),
		_textureHeight(0),
		_absoluteTexCoordsFlag(0),
		_startPosition(0,0,0),
		_minuscleHeight(-1),
		_majuscleHeight(-1),
		_preScale(1.0),
		_textureCount(0),
		_bleed(0)
	{
	}
    
	
	FontGlobalData::FontGlobalData(TiXmlElement* node) 
	:	osg::Referenced(),
		_nativeFontSize(0),
		_blurAmount(0),
		_textColor(0,0,0),
		_standardKerning(0),
		_textureWidth(0),
		_textureHeight(0),
		_absoluteTexCoordsFlag(0),
		_startPosition(0,0,0),
		_minuscleHeight(-1),
		_majuscleHeight(-1),
		_preScale(1.0),
		_textureCount(0),
		_bleed(0)
	{
    
        _fontName = XML::getStringValueFromSubNode(node, "FontName", "");
        _nativeFontSize = atoi(XML::getAttributeValueFromSubNode(node,"FontSize","value").c_str());

        _blurAmount = atof(XML::getAttributeValueFromSubNode(node,"BlurAmount","value").c_str());
        _standardKerning = atoi(XML::getAttributeValueFromSubNode(node,"StandardKerning","value").c_str());
        _textureWidth = atoi(XML::getAttributeValueFromSubNode(node,"TextureWidth","value").c_str());
        _textureHeight = atoi(XML::getAttributeValueFromSubNode(node,"TextureHeight","value").c_str());
		
		_minuscleHeight = atof(XML::getAttributeValueFromSubNode(node,"minuscleHeight","value").c_str());
		_majuscleHeight = atof(XML::getAttributeValueFromSubNode(node,"majuscleHeight","value").c_str());
		
		_textureCount = atoi(XML::getAttributeValueFromSubNode(node,"TextureCount","value").c_str());
		_textureCount = (_textureCount == 0) ? 1 : _textureCount;
		
		_bleed = atof(XML::getAttributeValueFromSubNode(node,"bleed","value").c_str());
	
        float r,g,b;
        
        r = atof(XML::getAttributeValueFromSubNode(node,"TextColor","red").c_str())/255.0;
        g = atof(XML::getAttributeValueFromSubNode(node,"TextColor","green").c_str())/255.0;
        b = atof(XML::getAttributeValueFromSubNode(node,"TextColor","blue").c_str())/255.0;
        
        _textColor = osg::Vec3(r,g,b);
        
        _preScale = 1.0f;
        _startPosition = osg::Vec3(0,0,0);
    
    }
}
