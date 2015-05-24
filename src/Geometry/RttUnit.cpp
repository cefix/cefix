/*
 *  RttUnit.cpp
 *  cefixSketch
 *
 *  Created by Stephan Huber on 02.03.11.
 *  Copyright 2011 Digital Mind. All rights reserved.
 *
 */

#include "RttUnit.h"
#include <cefix/Quad2DGeometry.h>
#include "ShaderTools.h"

namespace cefix {

template<class TextureType>TextureType* createTexture(GLenum internal_format, unsigned int w, unsigned int h)
{
	TextureType* tex = new TextureType();
		

	tex->setTextureSize(w, h);

	tex->setInternalFormat(internal_format);
	tex->setFilter(TextureType::MIN_FILTER, TextureType::LINEAR);
	tex->setFilter(TextureType::MAG_FILTER, TextureType::LINEAR);	
	tex->setWrap(TextureType::WRAP_S, TextureType::CLAMP_TO_EDGE);
	tex->setWrap(TextureType::WRAP_T, TextureType::CLAMP_TO_EDGE);	
	
	return tex;
}



RttUnit::RttUnit(const std::string& identifier, unsigned int w, unsigned int h, bool createAsTextureRectangle)
:	osg::Group(),
	_width(w),
	_height(h),
	_createAsTextureRectangle(createAsTextureRectangle),
	_cameraAdded(false)
{
	_display = RttTools::createSimpleDisplay(identifier, w, h);
	_ss = _display->get3DWorld()->getOrCreateStateSet();
}
	
void RttUnit::addInput(const std::string& name, osg::Texture* tex, bool createUniform)
{
	if(!tex) {
		cefix::log::error("RttUnit "+getIdentifier()) << "empty texture for input " << name << std::endl;
		return;
	}
	
	_inputs[name] = tex;
	unsigned int tex_unit = _units.size();
	_units[tex] = tex_unit;
	std::cout << getIdentifier() << ": new input " << name << " at unit " << tex_unit << std::endl;
	inputAdded(name, tex);
    if (createUniform) {
        attachUniform(name);
    }
}

void RttUnit::addInput(const std::string& name, cefix::Quad2DGeometry* geo, bool createUniform) 
{
	addInput(name, geo->getTexture(), createUniform);
}

void RttUnit::addInput(RttUnit* input, bool attachToScene, bool createUniform) 
{
	TextureMap& m = input->getOutputs();
	for(TextureMap::iterator i = m.begin(); i != m.end(); ++i) {
		addInput(i->first, i->second, createUniform);
	}
	if (attachToScene && (getNumParents() == 0))
		input->addChild(this);
}

void RttUnit::removeInput(const std::string& name) 
{
	TextureMap::iterator i = _inputs.find(name);
	if (i != _inputs.end()) {
		_units.erase(_units.find(i->second));
		_inputs.erase(i);
	}
}


osg::Texture* RttUnit::getInput(const std::string& name)
{
	TextureMap::iterator i = _inputs.find(name);
	return (i == _inputs.end()) ? NULL : i->second;
}

osg::Texture* RttUnit::getOutput(const std::string& name)
{
	TextureMap::iterator i = _outputs.find(name);	
	return ( i == _outputs.end()) ? NULL : i->second;
}

cefix::Quad2DGeometry* RttUnit::getOutputAsQuad(const std::string& name, unsigned width, unsigned int height)
{
	osg::Texture* tex = getOutput(name);
	osg::Viewport* vp = _display->get3DCamera()->getViewport();
	return (tex == NULL) ? NULL : RttTools::createQuadFromTexture((width== 0) ? vp->width() : width, (width==0) ? vp->height() : height, tex);
}

cefix::Quad2DGeometry* RttUnit::getOutputAsQuad(const std::string& name, const osg::Vec4 &rect)
{
	cefix::Quad2DGeometry* geo = getOutputAsQuad(name);
	geo->setRect(rect);
	return geo;
}

bool RttUnit::attachShaders(const std::string& name)
{
	return ShaderTools::attachShaders(name, _ss);
}


osg::Texture* RttUnit::createOutputTexture(const std::string& name, osg::Camera::BufferComponent buffer, GLenum internal_format, bool generate_mipmaps, unsigned int samples)
{
	osg::Texture* tex = NULL;
	if(_cameraAdded) 
	{
		osg::Viewport* vp = getDisplay()->get3DCamera()->getViewport();
		if (createAsTextureRectangle())
			tex = createTexture<osg::TextureRectangle>(internal_format, vp->width(), vp->height());
		else 
			tex = createTexture<osg::Texture2D>(internal_format, vp->width(), vp->height());
			
		getDisplay()->get3DCamera()->attach(buffer, tex, 0, 0, generate_mipmaps, samples, samples);
	}
	else 
	{
		if (_createAsTextureRectangle)
			tex = RttTools::createTextureRectangle(_display, buffer, internal_format, generate_mipmaps, samples);
		else
			tex = RttTools::createTexture2D(_display, buffer, internal_format, generate_mipmaps, samples);
	}
	_outputs[name] = tex;
	
	if (!_cameraAdded) 
	{
		addChild(_display->get3DCamera());
		_cameraAdded = true;
		cameraAdded();
	}
	return tex;
}

osg::Texture* RttUnit::setOutputTexture(const std::string& name, osg::Texture* tex, osg::Camera::BufferComponent buffer, GLenum internal_format, bool generate_mipmaps, unsigned int samples)
{
	osg::ref_ptr<osg::Texture> created_tex = createOutputTexture(name, buffer, internal_format, generate_mipmaps, samples);
	getDisplay()->get3DCamera()->detach(buffer);
	getDisplay()->get3DCamera()->attach(buffer, tex, 0, 0, generate_mipmaps, samples, samples);
	_outputs[name] = tex;
	
	return tex;
	
}
	

void RttUnit::attachUniform(osg::Uniform* uniform) {
	if (uniform)
		_ss->addUniform(uniform);
}

osg::Uniform* RttUnit::getUniformForInput(const std::string& name)
{
	UniformMap::iterator i = _uniforms.find(name);
	return (i == _uniforms.end()) ? NULL : i->second.get();
}


osg::Uniform* RttUnit::getOrCreateUniformForInput(const std::string& name)
{
	osg::Uniform* uniform = getUniformForInput(name);
	if(uniform) return uniform;
	
	osg::Texture* tex = getInput(name);
	if (!tex) {
		cefix::log::error("RttUnit") << "could not create a uniform for an unknown input " << name << std::endl;
		return NULL;
	}
	
	uniform = new osg::Uniform(name.c_str(), static_cast<int>(_units[tex]));
	_uniforms[name] = uniform;
	
	if(uniform) {
		int val; 
		uniform->get(val);
		std::cout << "created uniform for " << name << " is " << val << std::endl;
	}
	return uniform;

}


RenderRttUnit::RenderRttUnit(const std::string& identifier, unsigned int w, unsigned int h, bool createTextureRectangle)
:	RttUnit(identifier, w, h, createTextureRectangle)
{
	_geo = new cefix::Quad2DGeometry(0,0,1,1);
	setStateSet(_geo->getOrCreateStateSet());
}

void RenderRttUnit::cameraAdded() 
{
	osg::Viewport* vp = getDisplay()->get3DCamera()->getViewport();
	_geo->setRect(0,0,vp->width(), vp->height());
	
	_geo->setName(getDisplay()->getIdentifier() + " screenaligned quad");
	getDisplay()->get2DWorld()->addChild(RttTools::createGeode(_geo));
}

void RenderRttUnit::inputAdded(const std::string& name, osg::Texture* tex) 
{
	unsigned int tex_unit = _units[tex];
	
	float tw(createAsTextureRectangle() ? tex->getTextureWidth() : 1);
	float th(createAsTextureRectangle() ? tex->getTextureHeight() : 0);
	
	osg::Vec2Array* v = dynamic_cast<osg::Vec2Array*>(_geo->getTexCoordArray(tex_unit));
	if (!v) {
		v = new osg::Vec2Array();
		v->resize(4);
		_geo->setTexCoordArray(tex_unit, v);
	}
	if (v) {
        (*v)[0].set(0, th);
        (*v)[1].set(tw, th);
        (*v)[2].set(tw, 0);
        (*v)[3].set(0, 0);
        _geo->dirtyDisplayList();
    }
	
	osg::Uniform* uniform = getUniformForInput(name);
	if(uniform) {
        uniform->set(static_cast<int>(tex_unit));
        
		int val; 
		uniform->get(val);
		std::cout << "changed uniform for " << name << " is " << val << std::endl;
	}
	
	_geo->getOrCreateStateSet()->setTextureAttribute(tex_unit, tex, osg::StateAttribute::ON);	
}

}