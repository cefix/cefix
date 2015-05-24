/*
 *  Ellipse2DGeometry.cpp
 *  PartialMorph
 *
 *  Created by Ralph Ammer on 5/10/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "Ellipse2DGeometry.h"
#include <osg/Math>
#include <cefix/Settings.h>
#ifndef CEFIX_FOR_IPHONE
	#include <cefix/QuicktimeMovie.h>
	#include <cefix/CoreVideoTexture.h>
#endif
#include <cefix/AllocationObserver.h>

namespace cefix {

// -------------------------------------------------------------------------------------------------------
//
// -------------------------------------------------------------------------------------------------------

Ellipse2DGeometry::Ellipse2DGeometry(float width, float height, int resolution, bool fill)
:	osg::Geometry(),
	_x(0),
	_y(0),
	_width(width),
	_height(height),

	_resolution(resolution),
	_fill(fill)
{
	this->init();
	this->updateEllipse();
}


// -------------------------------------------------------------------------------------------------------
//
// -------------------------------------------------------------------------------------------------------

Ellipse2DGeometry::Ellipse2DGeometry(const osg::Vec4& rect, int resolution, bool fill)
:	osg::Geometry(),
	_x(rect[0]+_width/2.0),
	_y(rect[3]+_height/2.0),
	_width(rect[2]-rect[0]),
	_height(rect[1]-rect[3]),
	_resolution(resolution),
	_fill(fill)
{
	this->init();
	this->updateEllipse();
}



// -------------------------------------------------------------------------------------------------------
//
// -------------------------------------------------------------------------------------------------------

void Ellipse2DGeometry::init(){
	_tex = NULL;
	_locZ = 0.0;

	int resolution = computeResolution();
    

    osg::Vec3Array* v = new osg::Vec3Array();
    v->resize(resolution);
    setVertexArray(v);
    
    osg::Vec3Array* n = new osg::Vec3Array();
    for(int i=0; i<resolution; i++){
		n->push_back(osg::Vec3(0,0,1));
    }
    setNormalArray(n);
    setNormalBinding(osg::Geometry::BIND_PER_VERTEX);
    
    osg::Vec4Array* c = new osg::Vec4Array();
	for(int i=0; i<resolution; i++){
		c->push_back(osg::Vec4(1,1,1,1));
	}
	
    setColorArray(c);
    setColorBinding(osg::Geometry::BIND_PER_VERTEX);
    
    osg::Vec2Array* tex = new osg::Vec2Array();
    tex->resize(resolution);
    setTexCoordArray(0,tex);
    
	if(_fill==true){
		_da = new osg::DrawArrays(osg::PrimitiveSet::TRIANGLE_FAN,0,v->size());
	}else{
		_da = new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP,0,v->size());
	}
    addPrimitiveSet(_da);
    _color = osg::Vec4(1,1,1,1);
	setName("Ellipse2DGeometry");
}





// -------------------------------------------------------------------------------------------------------
//
// -------------------------------------------------------------------------------------------------------

void Ellipse2DGeometry::updateEllipse()
{
	// Vertices berechnen
	osg::Vec3Array* v = dynamic_cast<osg::Vec3Array*>(getVertexArray());	
	osg::Vec3Array* n = dynamic_cast<osg::Vec3Array*>(getNormalArray());
	osg::Vec4Array* c = dynamic_cast<osg::Vec4Array*>(getColorArray());
	int resolution = computeResolution();
			
	v->resize(_fill+resolution);
	if (n) n->resize(_fill+resolution);
	if (c) c->resize(_fill+resolution);
	
	_da->setCount(_fill+resolution);
	if (_fill)
		(*v)[0].set(_x, _y, _locZ);
		
	for(int i=0; i<resolution; i++){
		float tAngle = i* (2 * osg::PI/static_cast<float>(resolution - 1));
		float tX=(_width/2)*::cos(tAngle);
		float tY=(_height/2)*::sin(tAngle);
		(*v)[_fill+i].set(_x+tX, _y+tY, _locZ);
		if (n) {
			osg::Vec3 normal(tX,tY,0); normal.normalize();
			(*n)[_fill+i] = normal;
		}
		if (c) {
			(*c)[_fill+i] = _color;
		}
		
	}
	
	dirtyDisplayList();
	dirtyBound();
	
	//if (resetTextureRect)
    //    updateTexture();
}


// -------------------------------------------------------------------------------------------------------
//
// -------------------------------------------------------------------------------------------------------

void Ellipse2DGeometry::setColor(osg::Vec4 c) {
	_color = c;
	osg::Vec4Array* ca = dynamic_cast<osg::Vec4Array*>(getColorArray());
	for (unsigned int i = 0; i < ca->size(); i++)
		(*ca)[i] = c;
	dirtyDisplayList();
}


// -------------------------------------------------------------------------------------------------------
//
// -------------------------------------------------------------------------------------------------------

void Ellipse2DGeometry::updateTexture() 
{ 
	osg::Vec2Array* v = dynamic_cast<osg::Vec2Array*>(getTexCoordArray(0));
	int resolution = computeResolution();
	v->resize(_fill+resolution);
	float tstep = osg::DegreesToRadians(360*(1/static_cast<float>(resolution-1)));
	float x,y,w,h;
	w = _textureRect[2] - _textureRect[0];
	h = _textureRect[3] - _textureRect[1];
	x = _textureRect[0] + w / 2.0;
	y = _textureRect[1] + h / 2.0;
	
	if (v) 
	{
		if(_fill) 
			(*v)[0].set(x, y);
		
        for(int i = 0; i < resolution; i++)
		{
			float tX = x + ::cos(i * tstep) * w/2.0;
			float tY = y + ::sin(i * tstep) * h/2.0;
			(*v)[_fill+i].set(tX,tY);
		}
        dirtyDisplayList();
    }
}


// -------------------------------------------------------------------------------------------------------
//
// -------------------------------------------------------------------------------------------------------

void Ellipse2DGeometry::setCenter(float x, float y) {
	float dx(x-_x), dy(y-_y);
	_x=x;
	_y=y;
    
	osg::Vec3Array* v = dynamic_cast<osg::Vec3Array*>(getVertexArray());	
	for(unsigned int i = 0;i < v->size(); ++i) {
		(*v)[i] += osg::Vec3(dx,dy,0);
	}
	dirtyBound();
	dirtyDisplayList();
}


// -------------------------------------------------------------------------------------------------------
//
// -------------------------------------------------------------------------------------------------------

void Ellipse2DGeometry::setWidth(float width){
	_width=width;
	this->updateEllipse();
}


// -------------------------------------------------------------------------------------------------------
//
// -------------------------------------------------------------------------------------------------------

void Ellipse2DGeometry::setHeight(float height){
	_height=height;
	this->updateEllipse();
}
		

void Ellipse2DGeometry::setLocZ(float locZ){
	_locZ=locZ;
	this->updateEllipse();
}

// -------------------------------------------------------------------------------------------------------
//
// -------------------------------------------------------------------------------------------------------

void Ellipse2DGeometry::setTextureFromImage(osg::Image* img, bool useTextureRectangle)
{
	osg::ref_ptr<osg::Texture> tex = Video::createTexture(img, useTextureRectangle);
    
	tex->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
	tex->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
	
	AllocationObserver::instance()->observe(tex.get());
	
	setTexture(tex.get());
}


}



