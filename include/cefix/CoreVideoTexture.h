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

#ifndef CORE_VIDEO_TEXTURE_HEADER
#define CORE_VIDEO_TEXTURE_HEADER


#include <osg/Texture>
#include <cefix/Export.h>
#include <cefix/Video.h>

#if defined (CEFIX_CORE_VIDEO_AVAILABLE)
#include <cefix/QuicktimeMovie.h>
#include <cefix/CoreVideoAdapter.h>

namespace cefix {
	
	
	class CoreVideoTexture : public osg::Texture {
	
		public:
			
			CoreVideoTexture();
			
			CoreVideoTexture(osg::Image* image);
			
			/** Copy constructor using CopyOp to manage deep vs shallow copy. */
			CoreVideoTexture(const CoreVideoTexture& text,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY);
			
			META_StateAttribute(cefix, CoreVideoTexture, TEXTURE);

			/** Return -1 if *this < *rhs, 0 if *this==*rhs, 1 if *this>*rhs. */
			virtual int compare(const osg::StateAttribute& rhs) const;

			virtual GLenum getTextureTarget() const { return _textureTarget; }

			

			/** Sets the texture image, ignoring face. */
			virtual void setImage(unsigned int, osg::Image* image) { setImage(image); }
			
			void setImage(osg::Image* image);
			
			osg::Image* getImage() { return _image.get(); }
			const osg::Image* getImage() const { return _image.get(); }
			
			/** Gets the texture image, ignoring face. */
			virtual osg::Image* getImage(unsigned int) { return _image.get(); }

			/** Gets the const texture image, ignoring face. */
			virtual const osg::Image* getImage(unsigned int) const { return _image.get(); }

			/** Gets the number of images that can be assigned to the Texture. */
			virtual unsigned int getNumImages() const { return 1; }


			
			/** Gets the texture width. */
			virtual int getTextureWidth() const { return _textureWidth; }
			virtual int getTextureHeight() const { return _textureHeight; }
			virtual int getTextureDepth() const { return 1; }
			

			/** Bind the texture object. If the texture object hasn't already been
			  * compiled, create the texture mipmap levels. */
			virtual void apply(osg::State& state) const;
			
			virtual void allocateMipmap(osg::State& state) const {}

			inline unsigned int& getModifiedCount(unsigned int contextID) const
            {
                // get the modified count for the current contextID.
                return _modifiedCount[contextID];
            }
            
		protected:
			virtual void computeInternalFormat() const  {}
			virtual ~CoreVideoTexture();
			
			mutable GLenum			_textureTarget;
			int				_textureWidth;
			int				_textureHeight;
			bool			_inited;
			mutable osg::ref_ptr<CoreVideoAdapter>	_adapter;
			osg::ref_ptr<osg::Image>	_image;
            
            typedef osg::buffered_value<unsigned int> ImageModifiedCount;
            mutable ImageModifiedCount _modifiedCount;
			
		
	
	};

}

#else
#include <osg/TextureRectangle>
namespace cefix {

	typedef osg::TextureRectangle CoreVideoTexture;
	
}
#endif

#endif