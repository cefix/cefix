/*
 *  VideoBase.cpp
 *  cefix-for-iphone
 *
 *  Created by Stephan Huber on 05.05.11.
 *  Copyright 2011 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */

#include "Video.h"
#include <osgDB/FileUtils>
#include <cefix/AllocationObserver.h>
#include <cefix/Settings.h>
#include <osg/TextureRectangle>
#include <osg/Texture2D>
#include <cefix/Export.h>

#ifdef CEFIX_CORE_VIDEO_AVAILABLE
#include <cefix/CoreVideoTexture.h>
#endif


namespace cefix { 

Video::Video()
:   osg::ImageStream(),
    _currentTime(0.0),
    _coreVideoAdapter(NULL),
    _coreVideoUsage(CoreVideoUsageUnspecified)
{
    if (Settings::instance()->getVideoUsePixelBufferObjects())
		setPixelBufferObject(new osg::PixelBufferObject(this));
	else
		setPixelBufferObject(0);
}


void Video::handleDimensionChangedCallbacks()
{
    for(DimensionChangedCallbacks::iterator i = _dimensionChangedCallbacks.begin(); i != _dimensionChangedCallbacks.end(); ++i)
    {
        (*i)->operator()(this);
    }
}


osg::Texture* Video::createTexture(osg::Image* img, bool create_texture_rectangle)
{
    cefix::Video* v = dynamic_cast<Video*>(img);
    osg::Texture* tex(NULL);
    
    #if defined(OSG_GLES1_AVAILABLE) || defined(OSG_GLES2_AVAILABLE)
        bool can_create_texture_rectangle = false;
    #else
        bool can_create_texture_rectangle = create_texture_rectangle;
    #endif
    
    if (v) tex = v->createTexture(can_create_texture_rectangle);
    
    if (!tex)
    {
        if (can_create_texture_rectangle)
        {
            osg::TextureRectangle* rtex = new osg::TextureRectangle();
            rtex->setImage(img);
            tex = rtex;
        }
        else
        {
            static bool s_use_npot_textures = Settings::instance()->useNPOTTextures();
            osg::Texture2D* rtex = new osg::Texture2D();
            rtex->setResizeNonPowerOfTwoHint(!s_use_npot_textures);

            #if defined(OSG_GLES1_AVAILABLE)
                if (s_use_npot_textures) {
                    rtex->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
                    rtex->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
                }
            #endif
            
            rtex->setImage(img);
            tex = rtex;
        }
    }
    
    return tex;
 }


osg::Texture* Video::createTexture(bool create_texture_rectangle)
{
    osg::Texture* tex(NULL);

    if ((_coreVideoUsage == DontUseCoreVideo) || ((_coreVideoUsage == CoreVideoUsageUnspecified) && (!cefix::Settings::instance()->useCoreVideo())))
        return NULL;
        
    #ifdef CEFIX_CORE_VIDEO_AVAILABLE
        tex = new CoreVideoTexture(this);
        tex->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
        tex->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
        
        return tex;
    #endif
    
    return NULL;
    
}


}