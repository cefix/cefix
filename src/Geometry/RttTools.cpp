/*
 *  RttTools.cpp
 *  cefixSketch
 *
 *  Created by Stephan Huber on 28.02.11.
 *  Copyright 2011 Digital Mind. All rights reserved.
 *
 */


#include "RttTools.h"
#include <cefix/Quad2DGeometry.h>
#include <osg/Projection>


namespace cefix {

template<class TextureType> TextureType* prepareCreation(cefix::Display* display, GLenum internal_format)
{
	
	osg::Camera* camera = new osg::Camera();
	camera->setReferenceFrame(osg::Camera::ABSOLUTE_RF);
	
	display->applyTo(camera);
	
	TextureType* tex = new TextureType();
	tex->setTextureSize(camera->getViewport()->width(), camera->getViewport()->height());
	
	tex->setInternalFormat(internal_format);
	tex->setFilter(TextureType::MIN_FILTER, TextureType::LINEAR);
	tex->setFilter(TextureType::MAG_FILTER, TextureType::LINEAR);	
	tex->setWrap(TextureType::WRAP_S, TextureType::CLAMP_TO_EDGE);
	tex->setWrap(TextureType::WRAP_T, TextureType::CLAMP_TO_EDGE);	
	tex->setResizeNonPowerOfTwoHint(false);
	
	// da nested cameras für FBOs nicht funktionieren, müssen wir die 2D-Welt als Projection-node einbauen
	
	osg::MatrixTransform* mat = new osg::MatrixTransform();
	mat->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	mat->setMatrix(display->get2DCamera()->getViewMatrix());
	mat->addChild(display->get2DWorld());
	
	osg::Projection* projection = new osg::Projection();	
	projection->setMatrix(display->get2DCamera()->getProjectionMatrix());
	projection->addChild(mat);
	
	display->get3DCamera()->addChild(display->get3DWorld());
	display->get3DCamera()->addChild(projection);
	
	return tex;
}

template<class TextureType> TextureType* createTexture(
		cefix::Display* display, 
		osg::Camera::BufferComponent buffer, 
		GLenum internal_format, 
		bool generate_mipmaps = false, 
		unsigned int samples = 0)
{
	TextureType* tex = prepareCreation<TextureType>(display, internal_format);
	
	display->get3DCamera()->setRenderOrder(osg::Camera::POST_RENDER);
	display->get3DCamera()->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
	display->get3DCamera()->attach(buffer, tex, 0, 0, generate_mipmaps, samples, samples);

	return tex;
 
}

		
template<class TextureType> TextureType* createImage(
	cefix::Display* display, 
	osg::Camera::BufferComponent buffer, 
	GLenum internal_format, 
	GLenum type,
	unsigned int samples)
{
	TextureType* tex = prepareCreation<TextureType>(display, internal_format);
	
	osg::Image* img = new osg::Image();
	osg::Viewport* vp = display->get3DCamera()->getViewport();
	
	img->allocateImage(vp->width(), vp->height(), 1, internal_format, type);
	
	display->get3DCamera()->setRenderOrder(osg::Camera::POST_RENDER);
	display->get3DCamera()->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
	display->get3DCamera()->attach(buffer, img, samples, samples);
	
	tex->setImage(img);
	
	return tex;
} 


template<class TextureType> Quad2DGeometry* createQuadWithTexture(
		cefix::Display* display, 
		osg::Camera::BufferComponent buffer, 
		GLenum internal_format, 
		bool generate_mipmaps = false, 
		unsigned int samples = 0)
{
	TextureType* tex = createTexture<TextureType>(display, buffer, internal_format, generate_mipmaps, samples);
	osg::Viewport* vp = display->get3DCamera()->getViewport();
	return RttTools::createQuadFromTexture(vp->width(), vp->height(), tex); 
}



cefix::Quad2DGeometry* RttTools::createQuadFromTexture(unsigned int w, unsigned int h, osg::Texture* tex) {
	cefix::Quad2DGeometry* geo = new cefix::Quad2DGeometry(0,0, w, h);
	geo->setTexture(tex);
	if(dynamic_cast<osg::TextureRectangle*> (tex)) {
		geo->setTextureRect(0,0, tex->getTextureWidth(), tex->getTextureHeight());
	} else {
		geo->setTextureRect(0,0,1,1);
	}
	
	return geo;
}


Display* RttTools::createSimpleDisplay(const std::string& name, unsigned int w, unsigned int h) 
{
	// std::cout << "new display:" << name << std::endl;
	
	osg::ref_ptr<Display> display = new Display(name);
	display->setViewport(0, 0, w, h);
	display->set2DConfiguration(w, h);
	display->set3DConfiguration(Display::
	Configuration3D(60, -1, 1, 1000));
	DisplayCollection::instance()->addDisplay(display);
	return display.release();
	
}

osg::Texture2D* RttTools::createTexture2D(cefix::Display* display, 
	osg::Camera::BufferComponent buffer, 
	GLenum internal_format, 
	bool generate_mipmaps, 
	unsigned int samples)
{
	return createTexture<osg::Texture2D>(display, buffer, internal_format, generate_mipmaps, samples);
}

osg::TextureRectangle* RttTools::createTextureRectangle(cefix::Display* display, 
	osg::Camera::BufferComponent buffer, 
	GLenum internal_format, 
	bool generate_mipmaps, 
	unsigned int samples)
{
	return createTexture<osg::TextureRectangle>(display, buffer, internal_format, generate_mipmaps, samples);
}

osg::TextureRectangle* RttTools::createTextureRectangleWithImage(cefix::Display* display, 
	osg::Camera::BufferComponent buffer, 
	GLenum internal_format, 
	GLenum type,
	unsigned int samples)
{
	return createImage<osg::TextureRectangle>(display, buffer, internal_format, type, samples);
}
	
osg::Texture2D* RttTools::createTexture2DWithImage(cefix::Display* display, 
	osg::Camera::BufferComponent buffer, 
	GLenum internal_format, 
	GLenum type,
	unsigned int samples)
{
	return createImage<osg::Texture2D>(display, buffer, internal_format, type, samples);
}


cefix::Quad2DGeometry* RttTools::createQuadWithTextureRectangle(Display* display, osg::Camera::BufferComponent buffer, 
	GLenum internal_format, 
	bool generate_mipmaps, 
	unsigned int samples)
{
	return createQuadWithTexture<osg::TextureRectangle>(display, buffer, internal_format, generate_mipmaps, samples);
}

cefix::Quad2DGeometry* RttTools::createQuadWithTexture2D(Display* display, osg::Camera::BufferComponent buffer, 
	GLenum internal_format, 
	bool generate_mipmaps, 
	unsigned int samples)
{
	return createQuadWithTexture<osg::Texture2D>(display, buffer, internal_format, generate_mipmaps, samples);
}



}