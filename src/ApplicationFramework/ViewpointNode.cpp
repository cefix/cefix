/*
 *  CameraRepresentation.cpp
 *  AudiHeritage
 *
 *  Created by Stephan Huber on 16.05.06.
 *  Copyright 2006 digitalmind.de // Stephan Maximilian Huber. All rights reserved.
 *
 */

#include "ViewPointNode.h"
#include <osg/GL>
#include <osg/Geode>
#include <osg/Material>
#include <osg/StateSet>
#include <osg/PolygonOffset>
#include <osg/PolygonMode>

namespace cefix {

void ViewPointNode::createGeometry(float scale) {

    osg::Geometry* geometry = new osg::Geometry();
    geometry->setName("ViewPointNode");
    osg::Vec3Array* vertices = new osg::Vec3Array();
    vertices->resize(16);
    (*vertices)[0] = osg::Vec3(  0, -0.5,  -0.5);
    (*vertices)[1] = osg::Vec3(  1, -0.5,  -0.5);
    (*vertices)[2] = osg::Vec3(  1, -0.5,   0.5);
    (*vertices)[3] = osg::Vec3(  0, -0.5,   0.5);
    (*vertices)[4] = osg::Vec3(  0,  0.5, -0.5);
    (*vertices)[5] = osg::Vec3(  1,  0.5, -0.5);
    (*vertices)[6] = osg::Vec3(  1,  0.5,  0.5);
    (*vertices)[7] = osg::Vec3(  0,  0.5,  0.5);
    
    (*vertices)[8]  = osg::Vec3( -1, -0.4, -0.5);
    (*vertices)[9]  = osg::Vec3(  0, -0.2, -0.3);
    (*vertices)[10] = osg::Vec3(  0, -0.2,  0.3);
    (*vertices)[11] = osg::Vec3( -1, -0.4,  0.5);
    (*vertices)[12] = osg::Vec3( -1,  0.4, -0.5);
    (*vertices)[13] = osg::Vec3(  0,  0.2, -0.3);
    (*vertices)[14] = osg::Vec3(  0,  0.2,  0.3);
    (*vertices)[15] = osg::Vec3( -1,  0.4,  0.5);
    
    geometry->setVertexArray(vertices);
    
    osg::DrawElementsUShort* index = new osg::DrawElementsUShort(osg::PrimitiveSet::QUADS, 40);

    // wuerfel:
    (*index)[ 0] = 0; (*index)[ 1] = 1; (*index)[ 2] = 2; (*index)[ 3] = 3;
    (*index)[ 4] = 1; (*index)[ 5] = 5; (*index)[ 6] = 6; (*index)[ 7] = 2;
    (*index)[ 8] = 5; (*index)[ 9] = 4; (*index)[10] = 7; (*index)[11] = 6;
    (*index)[12] = 4; (*index)[13] = 0; (*index)[14] = 3; (*index)[15] = 7;
    (*index)[16] = 3; (*index)[17] = 2; (*index)[18] = 6; (*index)[19] = 7;
    (*index)[20] = 1; (*index)[21] = 0; (*index)[22] = 4; (*index)[23] = 5;
    
    // frustum
    (*index)[24] =  8; (*index)[25] =  9; (*index)[26] = 10; (*index)[27] = 11;
    (*index)[28] = 11; (*index)[29] = 10; (*index)[30] = 14; (*index)[31] = 15;
    (*index)[32] = 15; (*index)[33] = 14; (*index)[34] = 13; (*index)[35] = 12;
    (*index)[36] =  9; (*index)[37] =  8; (*index)[38] = 12; (*index)[39] = 13;
    
    // leider habe ich die vertices falsch angelegt, deswegen nochmal schnel 90 grad um die X-Achse
    osg::Matrix m = osg::Matrix::rotate(osg::DegreesToRadians(90.0f), 1,0,0);
    for(osg::Vec3Array::iterator itr = vertices->begin(); itr != vertices->end(); itr++) {
        (*itr) = m * (*itr);
    }
    vertices->push_back(osg::Vec3(0,0,0.2));
    vertices->push_back(osg::Vec3(0,0,1));

    
    // normals berechnen
    osg::Vec3Array* normals = new osg::Vec3Array();
    for(int i = 0; i < 10; i++) {
        unsigned short ndx = i*4;
        osg::Vec3 u = (*vertices)[(*index)[ndx+1]] - (*vertices)[(*index)[ndx]];
        osg::Vec3 v = (*vertices)[(*index)[ndx+3]] - (*vertices)[(*index)[ndx]];
        osg::Vec3 n = u ^ v;
        n.normalize();
        normals->push_back(n);
    }
    
    geometry->setNormalArray(normals);
    geometry->setNormalBinding(osg::Geometry::BIND_PER_PRIMITIVE);
    
    // farben
    osg::Vec4Array* colors = new osg::Vec4Array();
    colors->push_back(osg::Vec4(0,1,0,1));
    colors->push_back(osg::Vec4(1,1,0,1));
    
    for(osg::Vec3Array::iterator itr = vertices->begin(); itr != vertices->end(); itr++) {
        (*itr) = (*itr) * scale;
    }
    
    geometry->setColorArray(colors);
    geometry->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE_SET);
    
    geometry->addPrimitiveSet(index);
    geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 16,2));
	
	geometry->dirtyBound();
	normals->push_back(osg::Vec3(0,1,0));
    
    {   // solid rendering
        osg::Geode* geode_1 = new osg::Geode();
        geode_1->addDrawable(geometry);
        
        //material
        osg::StateSet* st = geode_1->getOrCreateStateSet();
        osg::Material* mat = new osg::Material();
        mat->setDiffuse(osg::Material::FRONT, osg::Vec4(0,1,0,0.2f));
        st->setAttributeAndModes(mat, osg::StateAttribute::ON);
        st->setMode(GL_LIGHTING, osg::StateAttribute::ON);
        st->setMode(GL_BLEND, osg::StateAttribute::ON);
        st->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
        addChild(geode_1);
    }
    
    {   //wireframe rendering
        osg::Geode* geode_2 = new osg::Geode();
        geode_2->addDrawable(geometry);
        
        //material
        osg::StateSet* stateset = geode_2->getOrCreateStateSet();
        osg::PolygonOffset* polyoffset = new osg::PolygonOffset;
        polyoffset->setFactor(-1.0f);
        polyoffset->setUnits(-1.0f);
        osg::PolygonMode* polymode = new osg::PolygonMode;
        polymode->setMode(osg::PolygonMode::FRONT_AND_BACK,osg::PolygonMode::LINE);
        stateset->setAttributeAndModes(polyoffset,osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON);
        stateset->setAttributeAndModes(polymode,osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON);

    
        osg::Material* material = new osg::Material;
        material->setColorMode(osg::Material::AMBIENT_AND_DIFFUSE); // switch glColor usage off
        // turn all lighting off 
        material->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0,0.0f,0.0f,1.0f));
        material->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0,0.0f,0.0f,1.0f));
        material->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0,0.0f,0.0f,1.0f));
        // except emission... in which we set the color we desire
        material->setEmission(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0,1.0f,0.0f,0.7f));
        stateset->setAttributeAndModes(material,osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON);
        stateset->setMode(GL_LIGHTING,osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON);
        stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
        stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
        addChild(geode_2);
    }
    
}
    
    
}
    
    