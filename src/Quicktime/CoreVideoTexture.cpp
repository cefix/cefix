/*
 *  CoreVideoTexture.cpp
 *  cefix
 *
 *  Created by Stephan Huber on 02.11.06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include <cefix/Export.h>
#if defined (CEFIX_CORE_VIDEO_AVAILABLE)

#include <cefix/CoreVideoTexture.h>
#include <cefix/MovieData.h>
#include <cefix/QTUtils.h>

namespace cefix {



// ----------------------------------------------------------------------------------------------------------
// CoreVideoTexture
// ----------------------------------------------------------------------------------------------------------

CoreVideoTexture::CoreVideoTexture() : 
	osg::Texture(), 
	_textureTarget(GL_TEXTURE_RECTANGLE_EXT),
	_inited(false),
	_adapter(NULL)
{
	//std::cout << "CoreVideoTexture:: standard constructor " << std::endl;
}

// ----------------------------------------------------------------------------------------------------------
// ctor
// ----------------------------------------------------------------------------------------------------------

CoreVideoTexture::CoreVideoTexture(osg::Image* image): 
	osg::Texture(),
	_textureTarget(GL_TEXTURE_RECTANGLE_EXT),
	_inited(false),
	_adapter(NULL)
{
	setImage(image);
	//std::cout << "CoreVideoTexture:: standard constructor with image" << std::endl;
}
	
// ----------------------------------------------------------------------------------------------------------
// copy ctor
// ----------------------------------------------------------------------------------------------------------

CoreVideoTexture::CoreVideoTexture(const CoreVideoTexture& text,const osg::CopyOp& copyop) : 
	osg::Texture(text, copyop),  
	_textureTarget(text._textureTarget),
	_inited(text._inited),
	_adapter(text._adapter),
	_image(text._image)
{
	//std::cout << "CoreVideoTexture:: copy constructor " << std::endl;
}

// ----------------------------------------------------------------------------------------------------------
// dtor
// ----------------------------------------------------------------------------------------------------------

CoreVideoTexture::~CoreVideoTexture() {
	//std::cout << "CoreVideoTexture:: destructor " << std::endl;
}

// ----------------------------------------------------------------------------------------------------------
// compare
// ----------------------------------------------------------------------------------------------------------

int CoreVideoTexture::compare(const osg::StateAttribute& sa) const {
	COMPARE_StateAttribute_Types(CoreVideoTexture,sa)

    if (_image!=rhs._image) // smart pointer comparison.
    {
        if (_image.valid())
        {
            if (rhs._image.valid())
            {
                int result = _image->compare(*rhs._image);
                if (result!=0) return result;
            }
            else
            {
                return 1; // valid lhs._image is greater than null. 
            }
        }
        else if (rhs._image.valid()) 
        {
            return -1; // valid rhs._image is greater than null. 
        }
    }

    if (!_image && !rhs._image)
    {
        // no image attached to either Texture2D
        // but could these textures already be downloaded?
        // check the _textureObjectBuffer to see if they have been
        // downloaded

        int result = compareTextureObjects(rhs);
        if (result!=0) return result;
    }

    int result = compareTexture(rhs);
    if (result!=0) return result;

    // compare each paramter in turn against the rhs.
#if 1    
    if (_textureWidth != 0 && rhs._textureWidth != 0)
    {
        COMPARE_StateAttribute_Parameter(_textureWidth)
    }
    if (_textureHeight != 0 && rhs._textureHeight != 0)
    {
        COMPARE_StateAttribute_Parameter(_textureHeight)
    }
#endif
	return 0; // passed all the above comparison macro's, must be equal.

}


// ----------------------------------------------------------------------------------------------------------
// setImage
// ----------------------------------------------------------------------------------------------------------

void CoreVideoTexture::setImage(osg::Image* image)
{
    if (_image == image) return;

    if (_image.valid() && _image->requiresUpdateCall())
    {
        setUpdateCallback(0);
        setDataVariance(osg::Object::STATIC);
    }

    _image = image;
    _modifiedCount.setAllElementsTo(0);

    if (_image.valid() && _image->requiresUpdateCall())
    {
        setUpdateCallback(new osg::Image::UpdateCallback());
        setDataVariance(osg::Object::DYNAMIC);
    }
	_adapter = NULL;
}




// ----------------------------------------------------------------------------------------------------------
// apply
// ----------------------------------------------------------------------------------------------------------

void CoreVideoTexture::apply(osg::State& state) const {
	if (!_image.valid())
		return;
		
	if (!_adapter.valid()) {
		cefix::Video* m = dynamic_cast<cefix::Video*>(_image.get());
		if ((m) && (m->getCoreVideoAdapter()))
			_adapter = m->getCoreVideoAdapter();
		else 
			_adapter = new CoreVideoAdapter(state, _image.get());
	}
	_adapter->getFrame();
	_textureTarget = _adapter->getTextureTarget();

	glBindTexture(_textureTarget, _adapter->getTextureName());
}



} //end namespace

#endif