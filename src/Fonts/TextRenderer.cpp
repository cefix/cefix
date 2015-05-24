/*
 *  TextRenderer.cpp
 *  NativeContext
 *
 *  Created by Stephan Huber on Mon Apr 07 2003.
 *  Copyright (c) 2003 __MyCompanyName__. All rights reserved.
 *
 */


#include <osg/Geometry>
#include <osg/Vec2>
#include <cefix/Log.h>
#include <osg/Array>

#include <cefix/PropertyList.h>
#include <cefix/PropertyList.h>
#include <cefix/FontManager.h>
#include <cefix/TextRenderer.h>

#include <cefix/AllocationObserver.h>

namespace cefix {


TextRendererBase::Container::Container()
 :	osg::Referenced(), 
	_currentId(0xffff),
	_name("FontContainer"),
	_colorBinding(osg::Geometry::BIND_OVERALL),
	_useDisplayLists(true),
	_callback(NULL),
	_normalBinding(osg::Geometry::BIND_OFF)
{
}

TextRendererBase::Container::~Container() {
    if (_geode.valid())
        detachFrom(_geode.get());
	//std::cout << "textrenderer.dtor()" << std::endl;
}


void TextRendererBase::Container::reset() {
	for (ContainerMap::iterator i = _container.begin(); i != _container.end(); ++i) {
		osg::Geometry* geo = i->second.get();
		
		osg::Vec3Array* v = dynamic_cast<osg::Vec3Array*> (geo->getVertexArray());
		v->resize(0);
		
		osg::Vec3Array* n = dynamic_cast<osg::Vec3Array*> (geo->getNormalArray());
		n->resize(0);
		
		osg::Vec2Array* t = dynamic_cast<osg::Vec2Array*> (geo->getTexCoordArray(0));
		t->resize(0);
	}	
}



void TextRendererBase::Container::dirty() {
	
	for (ContainerMap::iterator i = _container.begin(); i != _container.end(); ++i) {
		osg::Geometry* geo = i->second.get();
		osg::DrawArrays* da = dynamic_cast<osg::DrawArrays*>(geo->getPrimitiveSet(0));
		osg::Vec3Array* v = dynamic_cast<osg::Vec3Array*> (geo->getVertexArray());
		osg::Vec3Array* n = dynamic_cast<osg::Vec3Array*> (geo->getNormalArray());
		
		da->setCount(v->size());
		
		geo->setNormalBinding( n->size() ? _normalBinding : osg::Geometry::BIND_OFF);
		
		
		geo->dirtyBound();
		geo->dirtyDisplayList();
	}	
}

    
void TextRendererBase::Container::addDrawablesTo(osg::Geode* geode) {
    for (ContainerMap::iterator i = _container.begin(); i != _container.end(); ++i) {
        osg::Geometry* geo = i->second.get();
        if (!geode->containsDrawable(geo))
            geode->addDrawable( geo );
        geo->setDataVariance(geode->getDataVariance());
        //std::cout << "adddrawablesto: " << geode->getDataVariance() << std::endl;
    }
}
    void TextRendererBase::Container::removeDrawablesFrom(osg::Geode* geode) {
    for (ContainerMap::iterator i = _container.begin(); i != _container.end(); ++i) {
        osg::Geometry* geo = i->second.get();
        if (!geode->containsDrawable(geo))
            geode->removeDrawable( geo );
        //std::cout << "adddrawablesto: " << geode->getDataVariance() << std::endl;
    }
}
    
    
void TextRendererBase::Container::dirtyDisplayList() {
    for (ContainerMap::iterator i = _container.begin(); i != _container.end(); ++i) {
        osg::Geometry* geo = i->second.get();
        geo->dirtyDisplayList();
    }
}

void TextRendererBase::Container::setUpdateCallback(osg::Drawable::UpdateCallback* cb) {
    _callback = cb;
    for (ContainerMap::iterator i = _container.begin(); i != _container.end(); ++i) {
        osg::Geometry* geo = i->second.get();
        geo->setUpdateCallback(cb);
    }
}

void TextRendererBase::Container::updateDataVariance() {
    for (ContainerMap::iterator i = _container.begin(); i != _container.end(); ++i) {
        osg::Geometry* geo = i->second.get();
        if (_geode.valid()) {
            geo->setDataVariance(_geode->getDataVariance());
        }
    }
}

    
    
void TextRendererBase::Container::setOverallNormal(const osg::Vec3& normal) {
	for (ContainerMap::iterator i = _container.begin(); i != _container.end(); ++i) {
		osg::Geometry* geo = i->second.get();
		osg::Vec3Array* n = dynamic_cast<osg::Vec3Array*> (geo->getNormalArray());
		if (n->size() == 0)
			n->push_back(normal);
	}
	setNormalBinding(osg::Geometry::BIND_OVERALL);
}

void TextRendererBase::Container::setStateSetForFont(Font* font) {
    clearStateSets();
	for (ContainerMap::iterator i = _container.begin(); i != _container.end(); ++i) {
		osg::Geometry* geo = i->second.get();
		geo->setStateSet(font->getStandardStateSet(i->first));
	}
	_hasStateSet = true;
}
    
void TextRendererBase::Container::setStateSet(osg::StateSet* stateset) {
    clearStateSets();
    for (ContainerMap::iterator i = _container.begin(); i != _container.end(); ++i) {
        osg::Geometry* geo = i->second.get();
        geo->setStateSet(stateset);
    }
    _hasStateSet = true;
}

    
    
void TextRendererBase::Container::clearStateSets() {
    for (ContainerMap::iterator i = _container.begin(); i != _container.end(); ++i) {
        osg::Geometry* geo = i->second.get();
        geo->setStateSet(NULL);
    }
    _hasStateSet = false;
}


void TextRendererBase::Container::setName(const std::string& name) { 
	_name = name; 
	for (ContainerMap::iterator i = _container.begin(); i != _container.end(); ++i) {
		osg::Geometry* geo = i->second.get();
		geo->setName(_name);
	}
}
void TextRendererBase::Container::setColorArray(osg::Vec4Array* colors) { 
	_colors = colors; 
	for (ContainerMap::iterator i = _container.begin(); i != _container.end(); ++i) {
		osg::Geometry* geo = i->second.get();
		geo->setColorArray(_colors.get());
	}
	_currentColors = colors;
	// _currentColors = _colors;
}

void TextRendererBase::Container::setColorBinding(osg::Geometry::AttributeBinding binding) 
{ 
	_colorBinding = binding; 
	for (ContainerMap::iterator i = _container.begin(); i != _container.end(); ++i) {
		osg::Geometry* geo = i->second.get();
		geo->setColorBinding(_colorBinding);
	}

}


void TextRendererBase::Container::setNormalBinding(osg::Geometry::AttributeBinding binding) 
{ 
	_normalBinding = binding; 
	for (ContainerMap::iterator i = _container.begin(); i != _container.end(); ++i) {
		osg::Geometry* geo = i->second.get();
		geo->setNormalBinding(_normalBinding);
	}

}

void TextRendererBase::Container::setSupportsDisplayList(bool f) { 
	_useDisplayLists = f;
	for (ContainerMap::iterator i = _container.begin(); i != _container.end(); ++i) {
		osg::Geometry* geo = i->second.get();
		geo->setSupportsDisplayList(_useDisplayLists);
	}
}


osg::Geometry* TextRendererBase::Container::createNewContainer() 
{
	osg::Geometry* geo = new osg::Geometry();
	osg::Vec3Array* vertices = new osg::Vec3Array();
	osg::Vec3Array* normals = new osg::Vec3Array();
	osg::Vec2Array* texcoords = new osg::Vec2Array();
	
	geo->setVertexArray(vertices);
	geo->setNormalArray(normals);
	geo->setNormalBinding(_normalBinding);
	geo->setTexCoordArray(0,texcoords);
	
	geo->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,0));
	
	if (_colors.valid()) {
		geo->setColorBinding(_colorBinding);
		geo->setColorArray(_colors.get());
	}
	
	geo->setName(_name);
	geo->setSupportsDisplayList(_useDisplayLists);
	
	if (_callback.valid())
		geo->setUpdateCallback(_callback.get());
		
	if (_geode.valid()) {
		_geode->addDrawable(geo);
		geo->setDataVariance(_geode->getDataVariance());
	}
	
	_hasStateSet = false;
	return geo;
}


void TextRendererBase::Container::update(osg::Geometry* geo) 
{
	_currentGeometry = geo;
	_currentVertices = dynamic_cast<osg::Vec3Array*> (_currentGeometry->getVertexArray());
	_currentTexCoords = dynamic_cast<osg::Vec2Array*>( _currentGeometry->getTexCoordArray(0));
	_currentNormals = dynamic_cast<osg::Vec3Array*>( _currentGeometry->getNormalArray());
	_currentColors = dynamic_cast<osg::Vec4Array*>( _currentGeometry->getColorArray());
	_currentDrawArray = dynamic_cast<osg::DrawArrays*>(_currentGeometry->getPrimitiveSet(0));
	
}

TextRendererBase::IteratorBase::IteratorBase() : 
	osg::Referenced(),
    _font(NULL),
	_container(NULL),
	_renderPlane(FontCharData::XY_PLANE), 
	_fontChanged(false),
	_lineSpacing(0),
	_spatio(0),
	_subPixelAlignment(true),
    _dirty(true)
{
	//std::cout << "resetting renderplane" << std::endl;
	cefix::AllocationObserver::instance()->observe("TextRendererIterator", this);
}

}

        
