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
 
#ifndef IMAGE_UTILITIES_HEADER_
#define IMAGE_UTILITIES_HEADER_


#include <cefix/Export.h>
#include <osg/ref_ptr>
#include <osg/Image>
#include <cefix/Image2PassScale.h>


namespace cefix {


	class CEFIX_EXPORT ImageUtilities {
		private:
			ImageUtilities() {}
		public:
			
			enum Channel { RED = 0, GREEN = 1, BLUE = 2, ALPHA = 3 };
			/**
			 * constructs an ImageUtilites-object
			 * @param source the source-image to work with
			 */
			ImageUtilities(osg::Image* source) : _source(source) {} 
			
			/** sets the source-image */
			void setSource(osg::Image* s) { _source = s; }
			
			/**
			 * extract the channel and stores it in the target-image
			 * @param target the target-image, should be in the right format
			 * @param channel what channel to extract
			 * @return true if the alpha-channel 
			 */
			bool extractChannel(osg::Image* target, Channel channel);
			
			/**
			 * gets the channel of an image as a new image
			 * @param channel the channel to extract
			 * @param mode the OpenGL-Mode for this image (when used for texturing)
			 * @return the alpha-channel as a newly created image
			 */
			osg::Image* extractChannel(Channel channel, GLint internalformat = 1, GLenum mode = GL_LUMINANCE);	
			
			/** 
			 * scales an image using the template parameter as filter
			 * @param image target-image, should have the right dimensions
			 */
			template<class T> void scale(osg::Image* target) {
						
				Image2PassScale<T> scaler;
				scaler.Scale(_source->data(), _source->getPixelSizeInBits() / 8, 
					_source->s(), _source->getRowSizeInBytes(), _source->t(), 
					target->data(), target->s(), target->getRowSizeInBytes(), target->t());
				
			
			}
			
			template<class T> osg::Image* scale(unsigned int target_width, unsigned int target_height) {
			
				osg::Image* img = new osg::Image();
				img->allocateImage(target_width, target_height, 1, _source->getPixelFormat(), _source->getDataType(), _source->getPacking());
				img->setOrigin(_source->getOrigin());
				scale<T>(img);
				return img;
			
			}
			
			
			osg::Image* copyPixels(osg::Image* image, unsigned int l = 0, unsigned int t = 0);
			
			//osg::Image* flattenImageStack(ImageStack stack);
			
			/** premultiply alpha with image, this is helpful for applying filters */
			void premultiplyAlpha();
			
			/** demultiplyAlpha, converts an image with premultiplied alpha into an unmultiplied image */
			void demultiplyAlpha();
			
			osg::Image* addBorder(unsigned int borderWidth, const osg::Vec4& color);
            
            osg::Image* rotateCounterClockWise();
            osg::Image* rotateClockWise();
            osg::Image* rotate180Degrees();
			
		protected: 
			osg::ref_ptr<osg::Image> _source;

	};



} // end of namespace

#endif
