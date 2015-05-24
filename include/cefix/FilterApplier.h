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
 
#ifndef FILTER_APPLIER_HEADER
#define FILTER_APPLIER_HEADER
#include <osg/Image>
#include <cefix/Log.h>
#include <cefix/ImageFilters.h>

namespace cefix {

/** this class applies an ConvolutionFilterBase class to arbitrary images. The ImageFilterClass is provided as a template parameter, so the compiler can optimize the code */
class FilterApplier {

	public:
		typedef std::vector< osg::ref_ptr<osg::Image> > ImageVector;
        class Region : public osg::Referenced {
        public:
            int left, right, top, bottom;
            Region(osg::Image* img) : osg::Referenced(), left(0), right(img->s()), top(0), bottom(img->t()) {}
        };
        
		FilterApplier() {}
		
		/** create an new image with FilterClass filtered image */
		template<class FilterClass> osg::Image* apply(osg::Image* image, FilterClass* filter = NULL, Region* r = NULL, bool hor = true, bool vert = true) 
        {
			osg::ref_ptr<osg::Image> result = new osg::Image();
			result->allocateImage(image->s(), image->t(), image->r(), image->getPixelFormat(), image->getDataType(), image->getPacking());
			if (apply<FilterClass>(image, result.get(), filter, r, hor, vert))
				return result.release();
			else 
				return NULL;
		}
		
		/** apply the filter FilterClass onto image source and store the result in image dest */
		template<class FilterClass> bool apply(osg::Image* source, osg::Image* dest, FilterClass* afilter = NULL, Region* r = NULL, bool hor = true, bool vert = true) 
        {
			osg::ref_ptr<FilterClass> filter = (afilter) ? afilter : new FilterClass();
			
            osg::ref_ptr<Region> region = r ? r : new Region(source);

			if (filter->useDecomposedMatrix()) 
				return applyDecomposed<FilterClass>(source, dest, filter.get(), region, hor, vert);
			else
				return applyComposed<FilterClass>(source, dest, filter.get(), region);
		}
		
		/** apply filter FilterClass to a vector of images */
		template<class FilterClass>void apply(ImageVector& images, FilterClass* afilter = NULL, bool hor = true, bool vert = true) 
        {
			osg::ref_ptr<osg::Image> temp = NULL;
			osg::ref_ptr<FilterClass> filter(afilter);
			for(unsigned int i = 0; i < images.size(); ++i) 
            {
				apply(images[i].get(), images[i].get(), filter.get(), NULL, hor, vert);
				images[i]->dirty();
			}
		}
        
       
	private:
		bool pixelFormatSupported(osg::Image* image) {
			if ((image->getPixelFormat() != GL_RGBA) &&  (image->getPixelFormat() != GL_RGB) && (image->getPixelFormat() != GL_BGRA) && (image->getPixelFormat() != GL_ALPHA) && (image->getPixelFormat() != GL_LUMINANCE)) {
				log::error("FilterApplier") << "pixel format not supported! " << image->getPixelFormat() << std::endl;
				return false;
			}
			return true;
		}
		
		bool isAValidDestination(osg::Image* source, osg::Image* dest) {
		
			if ((source->s() != dest->s()) || 
				(source->t() != dest->t()) || 
				(source->s() != dest->s()) || 
				(source->getPixelFormat() != dest->getPixelFormat()) ||
				(source->getDataType() != dest->getDataType()))
				
				return false;
			else
				return true;
		}
		
		template<class FilterClass>bool applyDecomposed(osg::Image* source, osg::Image* dest, FilterClass* filter, Region* region, bool hor = true, bool vert = true) 
		{
			if (!pixelFormatSupported(source) || (!isAValidDestination(source, dest)))
				return false;
			dest->dirty();
			
			int radius = filter->getWidth() / 2;
			int rightradius = -radius + filter->getWidth();
			int rowBytes = dest->getRowSizeInBytes();
			int delta_y = 0;
			unsigned int bytesPerPixel = source->getPixelSizeInBits() / 8;
			int val = 0;
			unsigned char* src_start_ptr = source->data();
			unsigned char* dest_start_ptr = dest->data();
			
			unsigned char* srcptr;
			unsigned char* destptr;
			
			int top, bottom, left, right;
			int sum, ff;
			osg::ref_ptr<osg::Image> temp = new osg::Image();
			temp->allocateImage(region->right - region->left, region->bottom - region->top, 1, source->getPixelFormat(), source->getDataType(), source->getPacking());
			
			if (hor && !vert) {
				dest_start_ptr = dest->data(); // bei horizontalem blur gleich ins dest schreiben
				delta_y = 0;
			} else {
				dest_start_ptr = temp->data();
				delta_y = region->top;
			}
			if (hor) {
				for(int y = region->top; y < region->bottom; ++y)
				{
					top = ((y - radius) < 0) ? - y : -radius;
					bottom = ((y + rightradius) > source->t()) ? source->t() - y: rightradius;
					destptr = dest_start_ptr + (rowBytes * (y-delta_y)) + (bytesPerPixel * region->left);
					for(int x = region->left; x < region->right; ++x)
					{
						for(unsigned int k = 0; k < bytesPerPixel; ++k) {
							
							val = sum = 0;
							for (int v = top; v < bottom; ++v)
							{
								srcptr = src_start_ptr + (rowBytes * (y+v)) + ((x) * bytesPerPixel) + k;
								ff = filter->get(v+radius,0);
								val += ff * (*srcptr);
								sum += ff;
							}
							if (sum != 0) val /= filter->getSum(sum);
							val += filter->getOffset();
							(*destptr++) = static_cast<unsigned char>(osg::clampTo<float>(val,0,255));
						}
						
					}
				}
			}
			
			
			// second pass
			if (vert && !hor) {// bei vertikalem blur start-buffer von org-image holen
				src_start_ptr = source->data();
				delta_y = 0;
			} 
			else {
				src_start_ptr = temp->data();
				delta_y = region->top;
			}	
			dest_start_ptr = dest->data();
			
			if (vert) {
				for(int y = region->top; y < region->bottom; ++y)
				{
					destptr = dest_start_ptr + (rowBytes * y) + (bytesPerPixel * region->left);
					
					for(int x = region->left; x < region->right; ++x)
					{
						
						left = ((x - radius) < 0) ? - x : -radius;
						right = ((x + rightradius) > source->s()) ? source->s() - x  : rightradius;
						
						for(unsigned int k = 0; k < bytesPerPixel; ++k) {
							val = sum = 0;
							for (int u = left; u < right; ++u) {
								srcptr = src_start_ptr + (rowBytes * (y - delta_y)) + ((x+u) * bytesPerPixel) + k;
								
								ff = filter->get(u+radius,0);
								val += ff *  (*srcptr);
								sum += ff;
							}
							if (sum != 0) val /= filter->getSum(sum);
							val += filter->getOffset();
							(*destptr++) = static_cast<unsigned char>(osg::clampTo<float>(val,0,255));
						}
					}
				}
			}
			return true;
			
		}


		
		template<class FilterClass>bool applyComposed(osg::Image* source, osg::Image* dest, FilterClass* filter, Region* region) 
		{
			if (!pixelFormatSupported(source) || (!isAValidDestination(source, dest)))
				return false;
			dest->dirty();
			int radius = filter->getWidth() / 2;
			int rightradius = -radius + filter->getWidth();
			int rowBytes = dest->getRowSizeInBytes();
			unsigned int bytesPerPixel = source->getPixelSizeInBits() / 8;
			int val = 0;
			unsigned char* src_start_ptr = source->data();
			unsigned char* dest_start_ptr = dest->data();
			int ff;
			unsigned char* srcptr;
			unsigned char* destptr;
			
			int top, bottom, left, right;
			
			for(int y = region->top; y < region->bottom; ++y)
			{
				destptr = dest_start_ptr + (rowBytes * y);
				top = ((y - radius) < 0) ? - y : -radius;
				bottom = ((y + rightradius) > source->t()) ? source->t() - y : rightradius;
				
				for(int x = region->left; x < region->right; ++x)
				{
					
					left = ((x - radius) < 0) ? - x : -radius;
					right = ((x + rightradius) > source->s()) ? source->s() - x : rightradius;

					
					for(unsigned int k = 0; k < bytesPerPixel; ++k) {
						val  = 0;
						int sum = 0;
						for (int v = top; v < bottom; ++v)
						{
							for (int u = left; u < right; ++u) {
								srcptr = src_start_ptr + (rowBytes * (y+v)) + ((x+u) * bytesPerPixel) + k;
								ff =  filter->get(u+radius,v+radius);
								val += ff *  (*srcptr);
								sum += ff;
							}
						}
                        if (sum != 0) val /= filter->getSum(sum);
						val += filter->getOffset();
						(*destptr++) = static_cast<unsigned char>(osg::clampTo(val,0,255));
					}
					
				}
			}
			
			return true;
			
		}

};

} 
#endif
