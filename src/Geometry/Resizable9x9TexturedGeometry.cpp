/*
 *  Resizable9x9TexturedGeometry.cpp
 *  AudiHeritage
 *
 *  Created by Stephan Huber on 30.05.06.
 *  Copyright 2006 digitalmind.de // Stephan Maximilian Huber. All rights reserved.
 *
 */

#include "Resizable9x9TexturedGeometry.h"
#include <osgDB/ReadFile>

namespace cefix {

Resizable9x9TexturedGeometry::Resizable9x9TexturedGeometry(osg::Vec4 rect, float locZ, float frameWidth, float frameHeight) :
    Quad2DGeometry(),
    _frameWidth(frameWidth) ,
	_frameHeight(frameHeight),
	_texFrameWidth(frameWidth),
	_texFrameHeight(frameHeight)
{
    _rect = rect;
	setLocZ(locZ);
       
    init();
    setRect(rect);
}


void Resizable9x9TexturedGeometry::init() {
    
    setName("Resizable9x9TexturedGeometry");
       
    osg::Vec3Array* normals = new osg::Vec3Array();
    normals->push_back(osg::Vec3(0,0,1));
    setNormalArray(normals);
    setNormalBinding(osg::Geometry::BIND_OVERALL);
    
    osg::Vec4Array* colors = new osg::Vec4Array();
    colors->push_back(osg::Vec4(1,1,1,1));
    setColorArray(colors);
    setColorBinding(osg::Geometry::BIND_OVERALL);
    
    _vertices = new osg::Vec3Array();
    _vertices->resize(16);
    setVertexArray(_vertices.get());
    
    _texCoords = new osg::Vec2Array();
    _texCoords->resize(16);
    setTexCoordArray(0,_texCoords.get());
    
    osg::DrawElementsUShort* index = new osg::DrawElementsUShort(osg::PrimitiveSet::TRIANGLES);
	index->reserve(36*2);
    //innen
	addQuad(index, 0, 1, 2, 3);
    // (*index)[ 0] = 0; (*index)[ 1] = 1; (*index)[ 2] = 2; (*index)[ 3] = 3;
    
	//jetzt gegen den uhrzeigersinn von top-left to bottom-left
    
	addQuad(index, 0, 4, 5, 6);
    // (*index)[ 4] =  0; (*index)[ 5] =  4; (*index)[ 6] =  5; (*index)[ 7] =  6;
	addQuad(index, 1, 0, 6, 7);
    // (*index)[ 8] =  1; (*index)[ 9] =  0; (*index)[10] =  6; (*index)[11] =  7;
    
	addQuad(index, 9, 1, 7, 8);
    // (*index)[12] =  9; (*index)[13] =  1; (*index)[14] =  7; (*index)[15] =  8;
    addQuad(index, 10, 2, 1, 9);
    // (*index)[16] = 10; (*index)[17] =  2; (*index)[18] =  1; (*index)[19] =  9;
    addQuad(index, 11, 12, 2, 10);
    // (*index)[20] = 11; (*index)[21] = 12; (*index)[22] =  2; (*index)[23] = 10;
    addQuad(index, 12, 13, 3, 2);
    // (*index)[24] = 12; (*index)[25] = 13; (*index)[26] =  3; (*index)[27] =  2;
    addQuad(index, 13, 14, 15, 3);
    // (*index)[28] = 13; (*index)[29] = 14; (*index)[30] = 15; (*index)[31] =  3;
    addQuad(index, 3, 15, 4, 0);
    // (*index)[32] =  3; (*index)[33] = 15; (*index)[34] =  4; (*index)[35] =  0;
	
    
    addPrimitiveSet(index);
    
    setRect(_rect);
    setTexCoords();
}

void Resizable9x9TexturedGeometry::setRect(const osg::Vec4& r, bool resetTextureRect) 
{
                            
    _rect = r;
    float dx = _frameWidth;
    float dy = _frameHeight;
    
    _rect[2] = std::max(_rect[2], _rect[0] + 2 * dx);
    _rect[3] = std::max(_rect[3], _rect[1] + 2 * dy);
    
    if (_vertices.valid() == false) return;
    
    //inner-rect
    (*_vertices)[0].set(_rect[0] + dx, _rect[1] + dy, _locZ); // 0
    (*_vertices)[1].set(_rect[2] - dx, _rect[1] + dy, _locZ); // 1
    (*_vertices)[2].set(_rect[2] - dx, _rect[3] - dy, _locZ); // 2
    (*_vertices)[3].set(_rect[0] + dx, _rect[3] - dy, _locZ); // 3
    
    //topleft
//  (*_vertices)[0].set(_rect[0] + dx, _rect[1] + dy, _locZ); // 0
    (*_vertices)[4].set(_rect[0] -  0, _rect[1] + dy, _locZ); // 4
    (*_vertices)[5].set(_rect[0] -  0, _rect[1] -  0, _locZ); // 5
    (*_vertices)[6].set(_rect[0] + dx, _rect[1] -  0, _locZ); // 6
    
    //top-right
    (*_vertices)[9].set(_rect[2] +  0, _rect[1] + dy, _locZ); // 9
//  (*_vertices)[1].set(_rect[2] - dx, _rect[1] + dy, _locZ); // 1
    (*_vertices)[7].set(_rect[2] - dx, _rect[1] -  0, _locZ); // 7
    (*_vertices)[8].set(_rect[2] +  0, _rect[1] -  0, _locZ); // 8
    
    //bottom-right
    (*_vertices)[11].set(_rect[2] +  0, _rect[3] +  0, _locZ); // 11
    (*_vertices)[12].set(_rect[2] - dx, _rect[3] + 0, _locZ); // 12
//  (*_vertices)[ 2].set(_rect[2] - dx, _rect[3] - dy, _locZ); // 2
    (*_vertices)[10].set(_rect[2] +  0, _rect[3] - dy, _locZ); // 10
    
    //bottomleft
    (*_vertices)[13].set(_rect[0] + dx, _rect[3] +  0, _locZ); // 13
    (*_vertices)[14].set(_rect[0] -  0, _rect[3] +  0, _locZ); // 14
    (*_vertices)[15].set(_rect[0] -  0, _rect[3] - dy, _locZ); // 15
//  (*_vertices)[ 3].set(_rect[0] + dx, _rect[3] - dy, _locZ); // 3

    dirtyDisplayList();
    dirtyBound();
    
}

}