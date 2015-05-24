/*
 *  FontCharData.cpp
 *  NativeContext
 *
 *  Created by Stephan Huber on Fri Jan 10 2003.
 *  Copyright (c) 2003 digital mind. All rights reserved.
 *
 */

#include <cefix/Log.h>

#include <cefix/FontCharData.h>


#include <cefix/XMLWrapper.h>

namespace cefix {

FontCharData::FontCharData() {
    _texId = 0;
    _fontData = NULL;
    _id = 0xffff;
    _charWidth = 0.0f;
    _baseLeft = _baseTop = _texLeft = _texTop = _texWidth = _texHeight = 0;
    _kernings.clear();
}

FontCharData::~FontCharData() {
	//osg::notify(osg::ALWAYS) << "clearing Fontchar: " << _id << std::endl;
	_kernings.clear();
	_fontData = NULL;
}


FontCharData::FontCharData(TiXmlElement* node, FontGlobalData* fontData) {
	
	_fontData = fontData;

	_id = atoi(node->Attribute("ID"));

	_charWidth = atof(XML::getAttributeValueFromSubNode(node,"charWidth","value").c_str());
	_baseLeft = atoi(XML::getAttributeValueFromSubNode(node,"BaseCoord","left").c_str());
	_baseTop = atoi(XML::getAttributeValueFromSubNode(node,"BaseCoord","top").c_str());

	_texLeft = atoi(XML::getAttributeValueFromSubNode(node,"TexCoords","left").c_str());
	_texTop =  atoi(XML::getAttributeValueFromSubNode(node,"TexCoords","top").c_str());
	_texWidth = atoi(XML::getAttributeValueFromSubNode(node,"TexCoords","width").c_str());
	_texHeight = atoi(XML::getAttributeValueFromSubNode(node,"TexCoords","height").c_str());
	
	_texId = atoi(XML::getAttributeValueFromSubNode(node,"texId","value").c_str());
	
	_kernings.clear();
	

	KerningTable::iterator itr = _kernings.begin();

	
	_readKernings(node->FirstChild("Kernings"), "Kerning");
	_readKernings(node->FirstChild("CustomKernings"), "CustomKerning");
	
}

void FontCharData::_readKernings(TiXmlNode* node,const char* tagName) {
	
	if (NULL == node)
		return;
		
	TiXmlNode* child = node->FirstChild(tagName); 
	if (NULL == child) 
		return;
	
	while (child) {
		TiXmlElement* kerningNode = child->ToElement();
		
		if (kerningNode) {
			unsigned int toID = atoi(kerningNode->Attribute("toID"));
			float value = atof(kerningNode->Attribute("value")) - _charWidth;
			
			_kernings[toID] = value;
		}
		child = node->IterateChildren(tagName, child);
	}  
}


void FontCharData::addVertices(osg::Vec3Array* vertices, float s, float t, RenderPlane renderPlane,const osg::Vec3 &startLoc) {

    if (!_fontData) return;
     
    float s1,s2,t1,t2,r1,r2;
    float preScaleX, preScaleY;
    preScaleX = preScaleY = _fontData->preScale();
    
    if ((renderPlane == NEGATIVE_XZ_PLANE) ||
        (renderPlane == NEGATIVE_XY_PLANE) ||
        (renderPlane == NEGATIVE_YZ_PLANE))
    {
        preScaleY *= -1;
    }
    
    s1 = s2 = t1 = t2 = r1 = r2 = 0.0f;
    
    s1 = s - (_baseLeft - _texLeft) * preScaleX;
    s2 = s1 + _texWidth * preScaleX;
    
    t1 = t  + (_baseTop - _texTop) * preScaleY;
    t2 = t1 - _texHeight * preScaleY;
    
    	
	unsigned int m = vertices->size();
	vertices->resize(m+4);
    
    switch (renderPlane) {
        
        case NEGATIVE_XZ_PLANE:
        case XZ_PLANE:
            r1 = t1;
            r2 = t2;
            t1 = t2 = 0;
            
            // hier kein break, damit das gleich ins nächste weiterläuft
        case NEGATIVE_XY_PLANE:
        case XY_PLANE:
            (*vertices)[m++].set(startLoc + osg::Vec3(s2,t1,r1));
            (*vertices)[m++].set (startLoc + osg::Vec3(s1,t1,r1));
            (*vertices)[m++].set (startLoc + osg::Vec3(s1,t2,r2));
            (*vertices)[m++].set (startLoc + osg::Vec3(s2,t2,r2));
            break;

        case NEGATIVE_YZ_PLANE:
        case YZ_PLANE:
             (*vertices)[m++].set(startLoc + osg::Vec3(r1,s2,t1) );
             (*vertices)[m++].set(startLoc + osg::Vec3(r1,s1,t1) );
             (*vertices)[m++].set(startLoc + osg::Vec3(r2,s1,t2) );
             (*vertices)[m++].set(startLoc + osg::Vec3(r2,s2,t2) );
            break;        
    }
    
}

osg::Vec3Array* FontCharData::getVertices( int s, int t, RenderPlane renderPlane) {
    
    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array();
    addVertices(vertices.get(),s,t,renderPlane);
    
    return vertices.release();
}
    
    


void FontCharData::addTexCoords(osg::Vec2Array* texCoords) {
    
    if (!_fontData) return; // this char seems to be undefined
   
    float x_divisor = _fontData->useAbsoluteTexCoords() ? 1 : _fontData->getTextureWidth();
    float y_divisor = _fontData->useAbsoluteTexCoords() ? 1 : _fontData->getTextureHeight();
    float s1,s2,t1,t2;
    
    
    s1 = _texLeft / x_divisor;
    s2 = (_texLeft + _texWidth) / x_divisor;
    
    int  h = _fontData->getTextureHeight();
    
    t1 = (h - _texTop) / y_divisor;
    t2 = (h - (_texTop + _texHeight)) / y_divisor;
    
	unsigned int m = texCoords->size();
	texCoords->resize(m+4);
    
    (*texCoords)[m++].set(s2,t1);
    (*texCoords)[m++].set(s1,t1);
    (*texCoords)[m++].set(s1,t2);
    (*texCoords)[m++].set(s2,t2);


}

osg::Vec2Array* FontCharData::getTexCoords() 
{
    osg::ref_ptr<osg::Vec2Array> coords = new osg::Vec2Array();    
    addTexCoords(coords.get());
    
    return coords.release();
}


	

void FontCharData::dump() {

    osg::notify(osg::ALWAYS) << "dumping chardata ID = " << _id << std::endl;
	osg::notify(osg::ALWAYS) << "charwidth = " << _charWidth << ", baseLeft = " << _baseLeft << ", baseTop = " << _baseTop << std::endl;
	osg::notify(osg::ALWAYS) << "TexCoords: l =  " << _texLeft << ", t = " << _texTop << ", w = " << _texWidth << ", h = " << _texHeight << std::endl;

	
	int m = _kernings.size();

	for (int i=0; i<m; i++) {
		osg::notify(osg::ALWAYS) << i << ": " << _kernings[i] << ", ";
		if ((i>0) && ((i % 8 ) == 0))
			osg::notify(osg::ALWAYS) << std::endl;
	}
	osg::notify(osg::ALWAYS) << std::endl;

}

void FontCharData::setDummyValues(FontGlobalData* data) {
	_fontData = data;
	_charWidth = _fontData->getNativeFontSize();
	_texLeft = _texTop = 0;
	_baseLeft = -_fontData->getNativeFontSize() * 0.25;
	_baseTop = _fontData->getNativeFontSize() * 0.5;
	_texWidth = _fontData->getNativeFontSize() * 0.5;
	_texHeight = _fontData->getNativeFontSize() * 0.5;
}


}
