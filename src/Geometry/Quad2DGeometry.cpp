/*
 *  Quad2DGeometry.cpp
 *  AudiHeritage
 *
 *  Created by Stephan Huber on 22.05.06.
 *  Copyright 2006 digitalmind.de // Stephan Maximilian Huber. All rights reserved.
 *
 */

#include "Quad2DGeometry.h"
#include <cefix/Settings.h>
#ifndef CEFIX_FOR_IPHONE
#include <cefix/CoreVideoTexture.h>
#endif
#include <cefix/AllocationObserver.h>
#include <cefix/PropertyList.h>
#include <cefix/Settings.h>

namespace cefix {

// --------------------------------------------------------------------------------------------------------------------
// init
// --------------------------------------------------------------------------------------------------------------------

void Quad2DGeometry::init() {

    _tex = NULL;
    _locZ = 0;
    

    osg::Vec3Array* v = new osg::Vec3Array();
    v->resize(4);
    setVertexArray(v);
    
    osg::Vec3Array* n = new osg::Vec3Array();
    n->push_back(osg::Vec3(0,0,1));
    setNormalArray(n);
    setNormalBinding(osg::Geometry::BIND_OVERALL);
    
    osg::Vec4Array* c = new osg::Vec4Array();
    c->push_back(osg::Vec4(1,1,1,1));
	c->push_back(osg::Vec4(1,1,1,1));
	c->push_back(osg::Vec4(1,1,1,1));
	c->push_back(osg::Vec4(1,1,1,1));
	
    setColorArray(c);
    setColorBinding(osg::Geometry::BIND_PER_VERTEX);
    
    osg::Vec2Array* tex = new osg::Vec2Array();
    tex->resize(4);
    setTexCoordArray(0,tex);
    
    #ifdef CEFIX_FOR_IPHONE
    osg::DrawElementsUByte* da = new osg::DrawElementsUByte(osg::PrimitiveSet::TRIANGLES);
    da->push_back(0);
    da->push_back(1);
    da->push_back(2);
    
    da->push_back(2);
    da->push_back(3);
    da->push_back(0);
    
    addPrimitiveSet(da);
    
    #else
    addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,v->size()));
    #endif
    
    _color = osg::Vec4(1,1,1,1);
	setName("Quad2DGeometry");
        
}


// --------------------------------------------------------------------------------------------------------------------
// setRect
// --------------------------------------------------------------------------------------------------------------------

void Quad2DGeometry::setRect(const osg::Vec4& rect, bool resetTextureRect) {
    _rect = rect;
    osg::Vec3Array* v = dynamic_cast<osg::Vec3Array*>(getVertexArray());
    if (v) {
        (*v)[0].set(rect[0], rect[3], _locZ);
        (*v)[1].set(rect[2], rect[3], _locZ);
        (*v)[2].set(rect[2], rect[1], _locZ);
        (*v)[3].set(rect[0], rect[1], _locZ);
        dirtyDisplayList();
		dirtyBound();
    }
    if (resetTextureRect)
        setTextureRect(0,0,getWidth(),getHeight());
}

// --------------------------------------------------------------------------------------------------------------------
// set Texture rect
// --------------------------------------------------------------------------------------------------------------------

void Quad2DGeometry::setTextureRect(const osg::Vec4& rect) { 
   osg::Vec2Array* v = dynamic_cast<osg::Vec2Array*>(getTexCoordArray(0));
   _texRect = rect;
    if (v) {
        (*v)[0].set(rect[0], rect[3]);
        (*v)[1].set(rect[2], rect[3]);
        (*v)[2].set(rect[2], rect[1]);
        (*v)[3].set(rect[0], rect[1]);
        dirtyDisplayList();
    }
}

void Quad2DGeometry::setTextureFromImage(osg::Image* img, bool useTextureRectangle) 
{
    osg::ref_ptr<osg::Texture> tex = Video::createTexture(img, useTextureRectangle);
    
	tex->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
	tex->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
	
	AllocationObserver::instance()->observe(tex.get());
	
	setTexture(tex.get());
}


cefix::Quad2DGeometry* Quad2DGeometry::createFromImage(osg::Image* img, const osg::Vec3& pos, bool flip_tex)
{
	Quad2DGeometry* geo = new Quad2DGeometry(pos[0],pos[1],img->s(), img->t());
#ifndef CEFIX_FOR_IPHONE
    geo->setTextureFromImage(img);
    if (flip_tex)
        geo->setTextureRect(0,img->t(),img->s(), -img->t());
    else
        geo->setTextureRect(0,0,img->s(), img->t());
#else
    geo->setTextureFromImage(img, false);
    if(flip_tex)
        geo->setTextureRect(0,1,1,-1);
    else
        geo->setTextureRect(0,0,1,1);
#endif
	geo->setLocZ(pos[2]);
	return geo;
}

}
