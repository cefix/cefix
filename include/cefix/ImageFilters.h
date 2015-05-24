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

#ifndef IMAGE_FILTERS_HEADER
#define IMAGE_FILTERS_HEADER

#include <osg/Referenced>
#include <vector>
#include <algorithm>
#include <osg/Math>
namespace cefix {

/** base class for all convolution filters */
class ConvolutionFilterBase : public osg::Referenced {
	public:
		typedef std::vector<int>	Matrix;
		/**Ctor*/
		ConvolutionFilterBase(int width) : _matrix(NULL), _width(width), _offset(0), _useDecomposedMatrix(false) {}
		
		/** get the width of the convolution matrix */
		inline int getWidth() const { return _width; }
		
		/** get the sum of the convolution matrix. Note: the FilterApplier compute its own sums to provide correct results */
		virtual inline int getSum(int sum) const { return sum; }
		
		/** get the matrix value at x/y */
		inline int get(int x, int y) const { return _matrix[y*_width + x]; }
		
		/** @return the offset to apply to the computed value */
		inline int getOffset() const { return _offset; }
		
		void setOffset(int o) { _offset = o; }
		
		/** if useDecomposedMatrix returns true, the FilterApplier assumes, the the matrix is stored decomposed, it's size is not (width x width) it's (width x 1) 
		 *  not all convolution matrices can be used decomposed (first apply the 1D matrix on x, store the result in temp, then apply the 1D matrix on y and return the result)
		 *  but the computing is faster than using a 2D convolution matrix
		 */
		inline bool useDecomposedMatrix() const { return _useDecomposedMatrix; }
		
		/** set the convolution matrix */
		void setMatrix(const Matrix& m) { 
			if (_matrix) delete[] _matrix;
			_size = m.size();
			_matrix = new int[m.size()];
			int j = 0;
			for(Matrix::const_iterator i = m.begin(); i != m.end(); ++i) {
				_matrix[j] = (*i);
				++j;
			}
			/*
			for(j = 0; j < _size; ++j) {
				std::cout << _matrix[j] << "  ";
			}
			std::cout << std::endl;
			*/
			calculateSum(); 
		}
	protected:
		virtual ~ConvolutionFilterBase() {
			delete[] _matrix;
		}
		
		/** sets the setUseDecomposedMatrix-flag */
		void setUseDecomposedMatrix(bool f) { _useDecomposedMatrix = f; }
		
	private:
		void calculateSum() {
			_sum = 0;
			for (int i=0; i < _size; ++i) 
				_sum+= _matrix[i];
				
			if (_sum == 0) _sum = 1;
		}
		
		int*				_matrix;
		int					_size;
		int					_width;
		int					_sum;
		int					_offset;
		bool				_useDecomposedMatrix;

};

/** simple emboss filter, uses a 3x3 matrix */
class EmbossFilter : public ConvolutionFilterBase {

	public:
	EmbossFilter() : 
		ConvolutionFilterBase(3) 
	{ 
		Matrix m(9);
		
		m[0] =  0; m[1] = 0; m[2] = 0;
		m[3] =  0; m[4] = 1; m[5] = 0;
		m[6] =  0; m[7] = 0; m[8] = -1;
		
		setMatrix(m);
		setOffset(128);
	}
};

/** simple EdgeDetect filter, uses a 3x3 matrix */
class EdgeDetectFilter : public ConvolutionFilterBase {

	public:
	EdgeDetectFilter() : 
		ConvolutionFilterBase(3) 
	{ 
		Matrix m(9);
		
		m[0] = -1; m[1] = -1; m[2] = -1;
		m[3] =  0; m[4] =  0; m[5] =  0;
		m[6] =  1; m[7] =  1; m[8] =  1;
		
		setMatrix(m);
		//setOffset(128);
	}
};

/** simple sharpen filter, uses a 3x3 matrix */
class SharpenFilter : public ConvolutionFilterBase {

	public:
	SharpenFilter() : 
		ConvolutionFilterBase(3) 
	{ 
		Matrix m(9);
		
		m[0] =  0; m[1] = -1; m[2] =  0;
		m[3] = -1; m[4] =  5; m[5] = -1;
		m[6] =  0; m[7] = -1; m[8] =  0;
		
		setMatrix(m);
		//setOffset(128);
	}
};

/** simple sharpen more filter, uses a 3x3 matrix */
class SharpenMoreFilter : public ConvolutionFilterBase {

	public:
	SharpenMoreFilter() : 
		ConvolutionFilterBase(3) 
	{ 
		Matrix m(9);
		
		m[0] = -1; m[1] = -1; m[2] = -1;
		m[3] = -1; m[4] =  9; m[5] = -1;
		m[6] = -1; m[7] = -1; m[8] = -1;
		
		setMatrix(m);
		//setOffset(128);
	}
};

/** simple blur filter , uses a 3x3 matrix*/
class BlurFilter : public ConvolutionFilterBase {

	public:
	BlurFilter() : 
		ConvolutionFilterBase(3) 
	{ 
		Matrix m(9);
		
		m[0] =  1; m[1] =  2; m[2] =  1;
		m[3] =  1; m[4] =  4; m[5] =  2;
		m[6] =  1; m[7] =  2; m[8] =  1;
		
		setMatrix(m);
		//setOffset(128);
	}
};

/** simple blur more filter, uses a 5x5 matrix */
class BlurMoreFilter : public ConvolutionFilterBase {

	public:
	BlurMoreFilter() : 
		ConvolutionFilterBase(5) 
	{ 
		Matrix m(25);
		
		m[ 0] =  1; m[ 1] =  1; m[ 2] =  1; m[ 3] =  1; m[ 4] =  1; 
		m[ 5] =  1; m[ 6] =  2; m[ 7] =  8; m[ 8] =  2; m[ 9] =  1; 
		m[10] =  1; m[11] =  8; m[12] = 16; m[13] =  8; m[14] =  1; 
		m[15] =  1; m[16] =  2; m[17] =  8; m[18] =  2; m[19] =  1; 
		m[20] =  1; m[21] =  2; m[22] =  1; m[23] =  1; m[24] =  1; 
		
		setMatrix(m);
		//setOffset(128);
	}
};

/** gaussian blur, provides its convolution matrix decomposed */
class GaussianBlurFilter : public ConvolutionFilterBase {

	public:
		GaussianBlurFilter(int width = 11,int rho = 1) : 
			ConvolutionFilterBase(width) 
		{ 
			Matrix m(width); 
			int radius = (width) / 2;
			
			
			for (int i = -radius; i < -radius+width; ++i) {
				double f = 8.0/width * i;
				double d = exp (-(f*f) / (2.0*rho*rho)) / (rho * sqrt (2*osg::PI));
				m[i+radius] = d*512;
				// std::cout << i << ": " << m[i] << " (" << d << ")  " << std::endl;
			}
			
			setMatrix(m);
			setUseDecomposedMatrix(true);
		}
};

/** box blur filter, provides its convolution matrix decomposed */
class BoxBlurFilter : public ConvolutionFilterBase {

	public:
		BoxBlurFilter(int width = 31) : 
			ConvolutionFilterBase(width) 
		{ 
			Matrix m(width); 			
			
			for (int i = 0; i < width; ++i) {
				m[i] = 1 / static_cast<float>(width) * 512;
			}
			
			setMatrix(m);
			setUseDecomposedMatrix(true);
		}
};

/** box blur filter, provides its convolution matrix decomposed */
class DilateFilter : public ConvolutionFilterBase {

	public:
		DilateFilter(int width = 3) : 
			ConvolutionFilterBase(width) 
		{ 
			Matrix m(width); 			
			
			for (int i = 0; i < width; ++i) {
				m[i] = 1;
			}
			
			setMatrix(m);
			setUseDecomposedMatrix(true);
		}
		
		virtual inline int getSum(int sum) const { return 1; }
};


}

#endif