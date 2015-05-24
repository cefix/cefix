/*
 *  ImageUtilities.cpp
 *  cefix
 *
 *  Created by Stephan Huber on 29.04.06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */
#include <cefix/Log.h>
#include <osg/GL>
#include <osg/Math>
#include <cefix/ImageUtilities.h>
#include <cefix/Image2PassScale.h>
#include <cefix/Pixel.h>

namespace cefix {
	
	/**
	 * extracts an channel out of an image-object and stores it into the image target
	 */
	bool ImageUtilities::extractChannel(osg::Image* target, Channel channel) {
	
		if ((target->s() != _source->s()) || (target->t() != _source->t()) || (target->r() != _source->r())) {
			log::error("ImageUtilities::extractChannel") << "wrong dimensions of target-image " << std::endl;
			return false;
		}
		
		GLenum pixelFormat = _source->getPixelFormat();
		if ((pixelFormat != GL_RGBA) && (pixelFormat != GL_BGRA) && (pixelFormat != GL_RGB)) {
			log::error("ImageUtilities::extractChannel") << "unsupported pixelformat " << std::endl;
			return false;
		}
		
		if ((pixelFormat == GL_RGB) && (channel == ALPHA)) {
			log::error("ImageUtilities::extractChannel") << "could not extract alpha-channel out of a rgb-image " << std::endl;
			return false;
		}
		
		unsigned char* sourceData = _source->data();
		unsigned char* targetData = target->data();
		int stepSize = (pixelFormat == GL_RGB) ? 3 : 4;
		int ndx = channel;
		if (pixelFormat == GL_BGRA) {
			switch (channel) {
				case ALPHA:
					ndx = 3;
					break;
				default:
					ndx = 2-channel;
					break;
			}
		}
		sourceData += ndx;
		int maxNdx = _source->s() * _source->t() * ((pixelFormat == GL_RGB) ? 3 : 4);
		while (ndx < maxNdx) {
			(*targetData++) = (*sourceData);
			sourceData += stepSize;
			ndx += stepSize;
		}

		return true;
	}
	
	
	osg::Image* ImageUtilities::extractChannel(Channel channel, GLint internalformat, GLenum mode) {
				
		osg::Image* target = new osg::Image();
		unsigned char* buffer = new unsigned char[_source->s() * _source->t()];
			target->setImage(_source->s(),_source->t(), 1,
			internalformat,
			mode,
			GL_UNSIGNED_BYTE,
			buffer,
			osg::Image::USE_NEW_DELETE );
			
		return ( extractChannel(target, channel) ? target : NULL);
	}
	
	
	osg::Image* ImageUtilities::copyPixels(osg::Image* image, unsigned int l, unsigned int t) {
	
		osg::Image* target = dynamic_cast<osg::Image*>(_source->clone(osg::CopyOp::DEEP_COPY_ALL));
		unsigned char r1,g1,b1,a1, r2,g2,b2,a2;
		unsigned char* ptr_1;
		unsigned char* ptr_2;
		unsigned char* ptr_t;
		
		int maxy = _source->t();
		int maxx = _source->s();
		
		for( int y = 0; y < image->t(); ++y) {
			
			if (y+(int)t >= maxy)
				continue;
				
			for( int x = 0; x < image->s(); ++x) {
				if (x+(int)l >= maxx)
					continue;
				ptr_1 = _source->data(x+l,y+t);
				ptr_2 = image->data(x, y);
				ptr_t = target->data(x+l,y+t);
				r1 = (*ptr_1++);
				g1 = (*ptr_1++);
				b1 = (*ptr_1++);
				a1 = (*ptr_1);

				r2 = (*ptr_2++);
				g2 = (*ptr_2++);
				b2 = (*ptr_2++);
				a2 = (*ptr_2);
				
				(*ptr_t) = ((r2 * a2) + r1 * (255-a2)) / 255; ptr_t++;
				(*ptr_t) = ((g2 * a2) + g1 * (255-a2)) / 255; ptr_t++;
				(*ptr_t) = ((b2 * a2) + b1 * (255-a2)) / 255; ptr_t++;
				(*ptr_t) = ((a2 * a2) + a1 * (255-a2)) / 255; 
			}
		}

		return target;		
	}
	
	void ImageUtilities::premultiplyAlpha() {
	
		GLenum pixelFormat = _source->getPixelFormat();
		if ((pixelFormat != GL_RGBA) && (pixelFormat != GL_BGRA)) {
			log::error("ImageUtilities::premultiplyAlpha") << "no alpha channel found" << std::endl;
			return;
		}
	
		Pixel p(_source.get());
		unsigned char r,g,b,a;
		float alpha;
		for ( int y = 0; y < _source->t(); ++y) 
		{
			for( int x = 0; x < _source->s(); ++x) 
			{
				p.getRGBA(x,y, r,g,b,a);
				alpha = a/255.0f;
				r = osg::round(r*alpha);
				g = osg::round(g*alpha);
				b = osg::round(b*alpha);
				
				p.setRGB(x,y, r,g,b);
			}
		}
	
	}
	
	void ImageUtilities::demultiplyAlpha() {
	
		GLenum pixelFormat = _source->getPixelFormat();
		if ((pixelFormat != GL_RGBA) && (pixelFormat != GL_BGRA)) {
			log::error("ImageUtilities::demultiplyAlpha") << "no alpha channel found" << std::endl;
			return;
		}
	
		Pixel p(_source.get());
		unsigned char r,g,b,a;
		float alpha;
		for ( int y = 0; y < _source->t(); ++y) 
		{
			for( int x = 0; x < _source->s(); ++x) 
			{
				p.getRGBA(x,y, r,g,b,a);
				alpha = a/255.0f;
				r = osg::round(r/alpha);
				g = osg::round(g/alpha);
				b = osg::round(b/alpha);
				
				p.setRGB(x,y, r,g,b);
			}
		}
	
	}
	
	
	osg::Image* ImageUtilities::addBorder(unsigned int borderWidth, const osg::Vec4& color) 
	{
		unsigned int new_width = 2*borderWidth + _source->s();
		unsigned int new_height = 2* borderWidth + _source->t();
		
		osg::ref_ptr<osg::Image> img = new osg::Image();
		img->allocateImage(new_width, new_height, 1, _source->getPixelFormat(), _source->getDataType());
		
		unsigned char* src_ptr = NULL;
		unsigned char* dest_ptr = NULL;
		
		unsigned int row_bytes = _source->getRowSizeInBytes();
	
		for(int y = 0; y < _source->t(); ++y) {
			src_ptr = _source->data(0,y);
			dest_ptr = img->data(borderWidth, y + borderWidth);
			memcpy(dest_ptr, src_ptr, row_bytes);
		}
        
		{
			cefix::Pixel p(img);
			for(unsigned int x=0; x < borderWidth; ++x) {
				for(int y = 0; y < img->t(); ++y) {
					p.setColor(x,y,color);
					p.setColor(x+borderWidth + _source->s(),y,color);
				}
			}
			
			for(unsigned int y=0; y < borderWidth; ++y) {
				for(int x = 0; x < img->s(); ++x) {
					p.setColor(x,y,color);
					p.setColor(x,borderWidth + y+_source->t(),color);
				}
			}
			
		}
        
		img->setOrigin(_source->getOrigin());
		img->setFileName(_source->getFileName());
		return img.release();
	}




osg::Image* ImageUtilities::rotateClockWise() 
{
    osg::ref_ptr<osg::Image> dest = new osg::Image();
    dest->allocateImage(_source->t(), _source->s(), 1, _source->getPixelFormat(), _source->getDataType());
    {
        cefix::Pixel src(_source.get());
        cefix::Pixel dst(dest.get());
        
        for(unsigned int y = 0; y < src.t(); ++y) {
            for(unsigned int x = 0; x < src.s(); ++x) {
                unsigned int inv_x = (src.s() - 1) - x;
                unsigned int inv_y = (src.t() - 1) - y;
                
                dst.setRGBA(y, inv_x, src.getRGBA(x,y));
            }
        }
    }
    dest->dirty();
    return dest.release();
}


osg::Image* ImageUtilities::rotateCounterClockWise()
{
    osg::ref_ptr<osg::Image> dest = new osg::Image();
    dest->allocateImage(_source->t(), _source->s(), 1, _source->getPixelFormat(), _source->getDataType());
    
    {
        cefix::Pixel src(_source.get());
        cefix::Pixel dst(dest.get());
        
        for(unsigned int y = 0; y < src.t(); ++y) {
            for(unsigned int x = 0; x < src.s(); ++x) {
                unsigned int inv_x = (src.s() - 1) - x;
                unsigned int inv_y = (src.t() - 1) - y;
                
                dst.setRGBA(inv_y, x, src.getRGBA(x,y));
            }
        }
        dest->dirty();
    }
    return dest.release();

}


osg::Image* ImageUtilities::rotate180Degrees()
{
    osg::ref_ptr<osg::Image> dest = osg::clone(_source.get());
    
    {
        cefix::Pixel src(_source.get());
        cefix::Pixel dst(dest);
        
        for(unsigned int y = 0; y < src.t(); ++y) {
            for(unsigned int x = 0; x < src.s(); ++x) {
                unsigned int inv_x = (src.s() - 1) - x;
                unsigned int inv_y = (src.t() - 1) - y;
                
                dst.setRGBA(inv_x, inv_y, src.getRGBA(x,y));
            }
        }
        dest->dirty();
    }
    
    return dest.release();
}


}