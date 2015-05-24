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

#ifndef CEFIX_PIXEL_T_HEADER 
#define CEFIX_PIXEL_T_HEADER

#include <osg/Math>
#include <osg/Vec4>
#include <osg/Image>
#include <cefix/MathUtils.h>
#include <assert.h>

#if defined(DEBUG)
#define CHECK_INPUT(x,y) if ((static_cast<int>(x) >= static_cast<int>(_img->s())) || (static_cast<int>(y) >= static_cast<int>(_img->t()))) assert(0);
#else
#define CHECK_INPUT(x,y);
#endif

namespace cefix {


template<typename T>
struct PixelMaxValue {
    static float get() { return 1.0f; }
};

template<>
struct PixelMaxValue<unsigned char> {
    static float get() { return 255.0f; }
};



/** 
 *  the Pixel-class introduce a simple interface to the inner storage of images. You create a Pixel-object on the stack, with a given image and then you can query 
 *  the Pixel-Objects for colors and raw rgba-codes. You can even set the color or rgbs for a specific pixel. The code should run fast even when querying all pixels of an image.
 *  Note, not all pixelformats are supported, but the most used are.
 */
template<typename T>
class PixelT{
	protected: 
		
		/** helper class to store the needed indices to the specific colors of a pixel */
		class PixelIndex {
		
			public:
				enum Order { RGBA, ABGR, RGB, BGRA, ARGB, LUMINANCE };
				PixelIndex() {}
				
				inline void setPixelOrder(Order order) {
				
					switch(order) {
						case RGBA:
						case RGB:
							r = 0;
							g = 1;
							b = 2;
							a = (order == RGB) ? 0 : 3;
							break;
						case ABGR:
							a = 0;
							b = 1;
							g = 2;
							r = 3;
							break;
						case BGRA:
							b = 0;
							g = 1;
							r = 2;
							a = 3;
							break;
						case ARGB:
							a = 0;
							r = 1;
							g = 2;
							b = 3;
							break;
                        case LUMINANCE:
                            a = r = g = b = 0;
                            break;
					}
				}
				int r,g,b,a;
		};
        
        inline T* getPtr(unsigned int x, unsigned int y ) {
            CHECK_INPUT(x,y);
            return (T*)(_img->data(x,y));
        }
        
        inline const T* getPtr(unsigned int x, unsigned int y ) const {
            CHECK_INPUT(x,y);
            return (const T*)(_img->data(x,y));
        }
        
	public:
		/** ctor
		 *  @param image the image to use for querying */
		explicit PixelT(osg::Image* image) : _img(image) { updateValues(); } 
		
		/** copy ctor */
		template <typename U>PixelT(const PixelT<U>& pixel) : _img(pixel._img) {
			updateValues();
		}
		
		
		/** returns a osg::Vec4 storing the color at @param x / @param y, if the image has no alpha-channel, the returned alpha will undefined */
		inline osg::Vec4 getColor(unsigned int x, unsigned int y) const {
			osg::Vec4 result;
			
            const T* ptr = getPtr(x,y);
			
			result[0] = (*(ptr+_ndx.r)) / _maxValue;
			result[1] = (*(ptr+_ndx.g)) / _maxValue;
			result[2] = (*(ptr+_ndx.b)) / _maxValue;
			result[3] = (*(ptr+_ndx.a)) / _maxValue;
			
			return result;
			
		}
	
		/** clamped version of getColor */
		inline osg::Vec4 getColorClamped(unsigned int x, unsigned int y) const {
			return getColor(osg::clampTo<unsigned int>(x,0, _img->s()), osg::clampTo<unsigned int>(y, 0, _img->t()) );
		}
		
		/** fills params r, g, b and a with the raw values of the color at x/y */
		inline void getRGBA(unsigned int x, unsigned int y, T& r, T& g, T&b, T& a) const
        {
			const T* ptr = getPtr(x,y);						
			r = *(ptr+_ndx.r);
			g = *(ptr+_ndx.g);
			b = *(ptr+_ndx.b);
			a = *(ptr+_ndx.a);
		}
		
		/** fills params r, g, and b with the raw values of the color at x/y */
		inline void getRGB(unsigned int x, unsigned int y, T& r, T& g, T& b) const
        {
			const T* ptr = getPtr(x,y);			
            			
			r = *(ptr+_ndx.r);
			g = *(ptr+_ndx.g);
			b = *(ptr+_ndx.b);
		}
		
		
		/** return the grey-value at x/y */
		inline unsigned int getGrey(unsigned int x, unsigned int y) const
		{
			const T* ptr = getPtr(x,y);
            
			if (_bytesPerPixel == 1) 
				return *ptr;
			else {
				float r = *(ptr+_ndx.r);
				float g = *(ptr+_ndx.g);
				float b = *(ptr+_ndx.b);
				
				return 0.3f * r + 0.59f * g + 0.11f * b;
			}
		}
		
		/** get the raw red value at x/y */
		inline T  r(unsigned int x, unsigned int y) const 
        {
			const T* ptr = getPtr(x,y);
			return *(ptr+_ndx.r);
		}
		
		/** get the raw green value at x/y */
		inline T g(unsigned int x, unsigned int y) const 
        {
			const T* ptr = getPtr(x,y);
            return *(ptr+_ndx.g);
		}
		
		/** get the raw blue value at x/y */
		inline T b(unsigned int x, unsigned int y) const
        {
			const T* ptr = getPtr(x,y);
            return *(ptr+_ndx.b);
		}
		
		/** get the raw alpha value at x/y */
		inline T a(unsigned int x, unsigned int y) const 
        {
			const T* ptr = getPtr(x,y);
            return *(ptr+_ndx.a);
		}


		/** sets the color at position x/y */
		inline void setColor(unsigned int x, unsigned int y, const osg::Vec4& color) {
			setRGBA(x,y,color[0] * _maxValue,color[1] * _maxValue, color[2] * _maxValue, color[3] * _maxValue);
		}
        
        /** sets the color at position x/y */
		inline void setRGBColor(unsigned int x, unsigned int y, const osg::Vec4& color) {
			setRGB(x,y,color[0] * _maxValue,color[1] * _maxValue, color[2] * _maxValue);
		}
		
		/** sets the raw values (r, g, b, a) for a pixel at x/y */
		inline void setRGBA(unsigned int x, unsigned int y, T r, T g, T b, T a) 
        {
			T* ptr = getPtr(x,y);
            
			*(ptr + _ndx.r) = r;
			*(ptr + _ndx.g) = g;
			*(ptr + _ndx.b) = b;
			*(ptr + _ndx.a) = a;
		}
		/** blends the raw values (r, g, b, a) for a pixel at x/y with given opacity*/
		inline void blendRGBA(unsigned int x, unsigned int y, T r, T g, T b, T a, float opacity) 
        {
            T* ptr = getPtr(x,y);
            
			*(ptr + _ndx.r) = cefix::interpolate<T>( *(ptr + _ndx.r), r, 1-opacity);
			*(ptr + _ndx.g) = cefix::interpolate<T>( *(ptr + _ndx.g), g, 1-opacity);
			*(ptr + _ndx.b) = cefix::interpolate<T>( *(ptr + _ndx.b), b, 1-opacity);
			*(ptr + _ndx.a) = cefix::interpolate<T>( *(ptr + _ndx.a), a, 1-opacity);
		}
        
        /** blends the raw values (r, g, b) for a pixel at x/y with given opacity*/
		inline void blendRGB(unsigned int x, unsigned int y, T r, T g, T b, float opacity) 
        {   
            T* ptr = getPtr(x,y);
            
			*(ptr + _ndx.r) = cefix::interpolate<T>( *(ptr + _ndx.r), r, 1-opacity);
			*(ptr + _ndx.g) = cefix::interpolate<T>( *(ptr + _ndx.g), g, 1-opacity);
			*(ptr + _ndx.b) = cefix::interpolate<T>( *(ptr + _ndx.b), b, 1-opacity);
		}
		
		/** sets the raw values (r, g, b) for a pixel at x/y */
		inline void setRGB(unsigned int x, unsigned int y, T r, T g, T b) 
        {
			
            T* ptr = getPtr(x,y);
			
			*(ptr + _ndx.r) = r;
			*(ptr + _ndx.g) = g;
			*(ptr + _ndx.b) = b;
		}
		
		/** sets the raw value for red for a pixel at x/y. If you need to set more components of a color use setRGB / setRGBA instead */
		inline void setR(unsigned int x, unsigned int y, T v) 
        {
			T* ptr = getPtr(x,y);			
			*(ptr + _ndx.r) = v;
		}
		
		/** sets the raw value for green for a pixel at x/y. If you need to set more components of a color use setRGB / setRGBA instead */
		inline void setG(unsigned int x, unsigned int y, T v) 
        {
			T* ptr = getPtr(x,y);
			*(ptr + _ndx.g) = v;
		}
		
		/** sets the raw value for blue for a pixel at x/y. If you need to set more components of a color use setRGB / setRGBA instead */
		inline void setB(unsigned int x, unsigned int y, T v) 
        {
			T* ptr = getPtr(x,y);
            *(ptr + _ndx.b) = v;
		}
		
		/** sets the raw value for alpha for a pixel at x/y. If you need to set more components of a color use setRGB / setRGBA instead */
		inline void setA(unsigned int x, unsigned int y, T v) {
			T* ptr = getPtr(x,y);
			*(ptr + _ndx.a) = v;
		}
		
		inline void setImage(osg::Image* img) {
			_img = img;
			updateValues();
		}
		/** returns the color of the pixel as hex-value: RRGGBBAA */
		inline unsigned int getRGBA(unsigned int x, unsigned int y) const
		{
			const T* ptr = getPtr(x,y);
            return (*(ptr + _ndx.r) << 24) + (*(ptr + _ndx.g) << 16) + (*(ptr + _ndx.b) << 8) + *(ptr + _ndx.a);
		}
		
		/** returns the color of the pixel as hex-value: RRGGBB */
		inline unsigned int getRGB(unsigned int x, unsigned int y) const
		{
			const T* ptr = getPtr(x,y);
            return (*(ptr + _ndx.r) << 16) + (*(ptr + _ndx.g) << 8) + *(ptr + _ndx.b);
		}
		
		/** sets the color of the pixel from an unsigned int: RRGGBBAA */
		inline void setRGBA(unsigned int x, unsigned int y, unsigned int rgba) 
		{
			CHECK_INPUT(x,y);
			setRGBA(x,y, (rgba & 0xFF000000) >> 24, (rgba & 0x00FF0000) >> 16, (rgba & 0x0000FF00) >> 8, (rgba & 0x000000FF) );
		}
		
		/** sets the color of the pixel from an unsigned int: RRGGBB */
		inline void setRGB(unsigned int x, unsigned int y, unsigned int rgb) 
		{
			CHECK_INPUT(x,y);
			setRGB(x,y, (rgb & 0xFF0000) >> 16, (rgb & 0x00FF00) >> 8, (rgb & 0x0000FF));
		}
	
		inline unsigned int s() const { return _img->s(); }
		inline unsigned int t() const { return _img->t(); }
	

	private:
	
		void updateValues() {
			_bytesPerPixel = _img->getPixelSizeInBits() / 8;
			_bytesPerRow = _img->getRowSizeInBytes(); 
			typename PixelIndex::Order pixelorder;
			switch (_img->getPixelFormat()) {
				case GL_RGB:
					pixelorder = PixelIndex::RGB;
					break;
				
				case GL_BGRA:
					if (_img->getDataType() == GL_UNSIGNED_INT_8_8_8_8_REV) {
						#if defined(__APPLE__) && defined(__BIG_ENDIAN__)
							pixelorder = PixelIndex::ARGB;
						#else
							pixelorder = PixelIndex::BGRA;
						#endif
					} else {
						pixelorder = PixelIndex::BGRA;
					}
					break;
				
				case GL_RGBA:
					if (_img->getDataType() == GL_UNSIGNED_INT_8_8_8_8_REV) {
						#if defined(__APPLE__) && defined(__BIG_ENDIAN__)
							pixelorder = PixelIndex::ABGR;
						#else
							pixelorder = PixelIndex::RGBA;
						#endif
					} else {
						pixelorder = PixelIndex::RGBA;
					}
					break;
                case GL_LUMINANCE:
                case GL_ALPHA:
                    pixelorder = PixelIndex::LUMINANCE;
                    break;
                    
				default:
					pixelorder = PixelIndex::RGB;
			}
			
			_ndx.setPixelOrder(pixelorder);
            _maxValue = PixelMaxValue<T>::get();

		}
		
		osg::ref_ptr<osg::Image> _img;
		int			_bytesPerPixel;
		PixelIndex  _ndx;
		int			_bytesPerRow;
        float       _maxValue;
		

};

typedef PixelT<unsigned char> Pixel;

}
#endif // _PIXEL_

