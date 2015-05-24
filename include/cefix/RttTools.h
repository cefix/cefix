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

#ifndef CEFIX_RTT_TOOLS_HEADER
#define CEFIX_RTT_TOOLS_HEADER

#include <cefix/DisplayCollection.h>
#include <osg/Texture2D>
#include <osg/TextureRectangle>
#include <osg/Geode>
#include <osg/Geometry>

namespace  cefix {

class Quad2DGeometry;	

/** RttTools encapsulates some useful functions to get RenderToTexture done quickly. It uses a cefix::Display-class to
    transport projection matrix and the different cameras. the display gets registered via the DisplayCollection, so you can
    query for it in a later step and use get3DWorld() and get2DWorld() on it
*/

class RttTools {

public:

	/// creates a simple display-class (60° hfov) with width w, and height h
    static Display* createSimpleDisplay(const std::string& name, unsigned int w, unsigned int h); 
	
    /** creates a Texture2D which gets attached to the camera as rendertarget for the given buffer */
	static osg::Texture2D* createTexture2D(osg::Camera* camera, 
		osg::Camera::BufferComponent buffer, 
		GLenum internal_format, 
		bool generate_mipmaps = false, 
		unsigned int samples = 0);
	
    /** creates a Texture2D which gets attached to the camera of the given display  and buffer */
	static osg::Texture2D* createTexture2D(cefix::Display* display, 
		osg::Camera::BufferComponent buffer, 
		GLenum internal_format, 
		bool generate_mipmaps = false, 
		unsigned int samples = 0);
    
    /** creates a Texture2D which gets attached to the camera of a newly created display with name display_id */
	static inline osg::Texture2D* createTexture2D(const std::string& display_id, unsigned int w, unsigned int h, 
		osg::Camera::BufferComponent buffer, 
		GLenum internal_format, 
		bool generate_mipmaps = false, 
		unsigned int samples = 0)
	{
		return createTexture2D(createSimpleDisplay(display_id, w, h), buffer, internal_format, generate_mipmaps, samples);
	}
    
    /** creates a TextureRectangle which gets attached to the camera as rendertarget for the given buffer */
	static osg::TextureRectangle* createTextureRectangle(osg::Camera* camera, 
		osg::Camera::BufferComponent buffer, 
		GLenum internal_format, 
		bool generate_mipmaps = false, 
		unsigned int samples = 0);
    
    /** creates a TextureRectangle which gets attached to the camera of the given display  and buffer */
	static osg::TextureRectangle* createTextureRectangle(cefix::Display* display, 
		osg::Camera::BufferComponent buffer, 
		GLenum internal_format, 
		bool generate_mipmaps = false, 
		unsigned int samples = 0);
	
    /** creates a TextureRectangle which gets attached to the camera of a newly created display with name display_id */
	static inline osg::TextureRectangle* createTextureRectangle(const std::string& display_id, unsigned int w, unsigned int h, 
		osg::Camera::BufferComponent buffer, 
		GLenum internal_format, 
		bool generate_mipmaps = false, 
		unsigned int samples = 0)
	{
		return createTextureRectangle(createSimpleDisplay(display_id, w, h), buffer, internal_format, generate_mipmaps, samples);
	}
	
    /** creates a TextureRectangle and an image (whichis attached to the texture) which gets attached to the camera of the given display and buffer,
        useful if you wnat the buffer back to your app */
	static osg::TextureRectangle* createTextureRectangleWithImage(cefix::Display* display, 
		osg::Camera::BufferComponent buffer, 
		GLenum internal_format,  
		GLenum type,
		unsigned int samples = 0);
	
    /** creates a Texture2D and an image (whichis attached to the texture) which gets attached to the camera of the given display and buffer,
        useful if you wnat the buffer back to your app */
    static osg::Texture2D* createTexture2DWithImage(cefix::Display* display, 
		osg::Camera::BufferComponent buffer, 
		GLenum internal_format, 
		GLenum type,
		unsigned int samples = 0);
	
    /// creates a textured quad, the texture is the rendering-target for display and buffer
	static cefix::Quad2DGeometry* createQuadWithTextureRectangle(Display* display, osg::Camera::BufferComponent buffer, 
		GLenum internal_format, 
		bool generate_mipmaps = false, 
		unsigned int samples = 0);
	    
    /// creates a textured quad, the texture is the rendering-target for display and buffer
	static cefix::Quad2DGeometry* createQuadWithTexture2D(Display* display, osg::Camera::BufferComponent buffer, 
		GLenum internal_format, 
		bool generate_mipmaps = false, 
		unsigned int samples = 0);
    
    /// creates a textured quad, the texture is the rendering-target for display and buffer
	static inline cefix::Quad2DGeometry* createQuadWithTexture2D(const std::string& display_id, unsigned int w, unsigned int h, osg::Camera::BufferComponent buffer, 
		GLenum internal_format, 
		bool generate_mipmaps = false, 
		unsigned int samples = 0)
	{
		return createQuadWithTexture2D(createSimpleDisplay(display_id, w, h), buffer, internal_format, generate_mipmaps, samples);
	}
	
    /// creates a textured quad, the texture is the rendering-target for display and buffer
	static inline cefix::Quad2DGeometry* createQuadWithTextureRectangle(const std::string& display_id, unsigned int w, unsigned int h, osg::Camera::BufferComponent buffer, 
		GLenum internal_format, 
		bool generate_mipmaps = false, 
		unsigned int samples = 0)
	{
		return createQuadWithTextureRectangle(createSimpleDisplay(display_id, w, h), buffer, internal_format, generate_mipmaps, samples);
	}
	
    /// creates a geode, and attaches the given geometry to it
	static inline osg::Geode* createGeode(osg::Geometry* geo) {
		osg::Geode* geode = new osg::Geode();
		geode->addDrawable(geo);
		return geode;
	}
	
    /// creates a Quad2DGeometry of given size and attach texture tex to it
	static cefix::Quad2DGeometry* createQuadFromTexture(unsigned int w, unsigned int h, osg::Texture* tex);
private:
	RttTools() {}
	
};



}



#endif