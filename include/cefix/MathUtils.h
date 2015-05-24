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

#ifndef MATH_UTILS_HEADER_
#define MATH_UTILS_HEADER_

// #include <stdlib.h>

#include <osg/Math>
#include <osg/Vec2>
#include <osg/Vec3>
#include <osg/Vec4>
#include <osg/Quat>
#include <osg/Matrix>
#include <cefix/Export.h>
#include <osg/Array>
#include <osg/Plane>

// fix for windows:
#undef min
#undef max

namespace cefix {
	
	/// empty base class for tweening
	class Tweening {};

	/// linear tweening-class basically y = x
	template<typename T>
	class LinearTweening : public Tweening {
	public:
        typedef T value_type;
		static inline value_type getValueAt(value_type f) { return f; }
	};

	/// easyInOut tweening, using cos
	template <class T>
	class EasyInOutTweening : public Tweening {
	public:
		typedef T value_type;
		static inline T getValueAt(T f) { return static_cast<T>(0.5) - static_cast<T>(0.5) * cos(f * osg::PI); }
	};


	/**
	 * @return a random float between 0 and max
	 * @param max maximum for the random float
	 */
	inline float CEFIX_EXPORT randomf(float max = 1.0f) { return float(rand())/RAND_MAX*max;}
	
	/** @return a random float between 0 and 1 */
	inline float CEFIX_EXPORT randomf() {return float(rand());}
	/** @ return a random flaot between f1 and f2 */
    inline float CEFIX_EXPORT in_between(float f1, float f2) { return f1 + randomf(f2 - f1); }	

	/**
	 * @return the angle between vector a and vector b
	 */
	template <typename T>inline double  getAngleBetween(const T& a, const T& b) {
        typename T::value_type tdot,tlens;
        tdot=a*b;
        tlens=a.length()*b.length();
        if (tlens==0)
            return 0;
        typename T::value_type result = (acos(tdot/tlens));
        if(T::num_components != 2)
            return result;
        
        typename T::value_type cross = a.x() * b.y() - a.y()* b.x();
        return cross < 0 ? (2 * osg::PI) - result : result;
    }
	

	namespace RayPlaneIntersection {
		enum Result {
			NO_INTERSECTION = 0,
			ONE_INTERSECTION = 1,
			RAY_ON_PLANE
		};
	}

	/**
	 * @return the intersection of a plane and a ray
	 * @param plane 
	 * @param pointOnRay 
	 * @param rayNormal
	 * @param result the result
	 */
	template<typename T>
	inline RayPlaneIntersection::Result getRayPlaneIntersection(const osg::Plane& plane, const T& pointOnRay, const T& rayNormal, T& result) {
		double numerator = -plane.distance(pointOnRay);
		double denominator = plane.getNormal() * rayNormal;
		
		if (0.0 == denominator) 
		{ 
			// rayNormal parallel zur Ebene,ist es teil der ebene
			if (numerator == 0.0) {
				result = pointOnRay;
				return RayPlaneIntersection::RAY_ON_PLANE;
			}
			else
			{
				// raynormal parallel zur Ebene, aber nicht auf der Ebene -> kein Schnittpunkt
				return RayPlaneIntersection::NO_INTERSECTION;
			}
		}
		else
		{
			result = pointOnRay + (rayNormal * numerator / denominator);
			return RayPlaneIntersection::ONE_INTERSECTION;
		}
		return RayPlaneIntersection::NO_INTERSECTION;
	}
	
	/**
	 * @checks whether a 2D point lies within a polygon 
	 */
	inline bool CEFIX_EXPORT isPointInPolygon(osg::Vec3Array* vertices, float x, float y)  
	{
		int i, j, c = 0;   
		int tVerticeCount = vertices->size();

		for (i = 0, j = tVerticeCount-1; i < tVerticeCount; j = i++) 
		{
			if ( ((( (*vertices)[i][1] <= y) && (y < (*vertices)[j][1] ) )  || 
				(((*vertices)[j][1] <= y) && (y < (*vertices)[i][1]))) && 
				(x < ((*vertices)[j][0] - (*vertices)[i][0]) * (y - (*vertices)[i][1]) / ((*vertices)[j][1] - (*vertices)[i][1]) + (*vertices)[i][0])) {
				c = !c;   
			}
		}   

		return (c==1);  
	}
	
	    
    template<typename T>
    inline bool getLineLineIntersectionImpl(const T& line1_p1, const T& line1_p2, const T& line2_p1, const T& line2_p2, T& result, bool limit_to_line = true, typename T::value_type fuzzy = typename T::value_type(0.1)) {
		
		T u(line1_p2 - line1_p1);
		T v(line2_p2 - line2_p1);
		T w(line1_p1 - line2_p1);
		
		typename T::value_type a = u * u;;
		typename T::value_type b = u * v;
		typename T::value_type c = v * v;
		typename T::value_type d = u * w;
		typename T::value_type e = v * w;
		
		typename T::value_type dt = a * c - b * b;
		typename T::value_type sc(0.0);
		typename T::value_type tc(0.0);
		
		if (dt == typename T::value_type(0.0)) {
			if (b > c)
				tc = d / b;
			else
				tc = e / c;
		}
		else 
		{
			sc = (b * e - c * d) /dt;
			tc = (a * e - b * d) /dt;
		}
		
		T dv( w + (u * sc) - (v * tc ));
		if ( dv*dv <= (fuzzy*fuzzy) ) {
			result = ((line1_p1 + (u * sc)) + (line2_p1 + (v * tc))) * typename T::value_type(0.5);
			return !limit_to_line || ((sc >=0) && (sc <= 1.0) && (tc >= 0) && (tc <=1.0));
		}
		return false;
		
	}
    
    /** get the intersection-point of two lines
	 * @param line1_p1 point 1 of line a
	 * @param line1_p2 point 2 of lina a
	 * @param line2_p1 point 1 of line b
	 * @param line2_p2 point 2 of line b
	 * @param result hold the intersection point of line a and line b (if any)
	 * @param fuzzy fuzzy-value
	 * @return true if any intersection
	*/
	template<typename T>
    inline bool getLineLineIntersection(const T& line1_p1, const T& line1_p2, const T& line2_p1, const T& line2_p2, T& result, typename T::value_type fuzzy = typename T::value_type(0.1))
    {
        return getLineLineIntersectionImpl(line1_p1, line1_p2, line2_p1, line2_p2, result, true, fuzzy);
    }
    
    template<typename T>
    inline bool getRayRayIntersection(const T& line1_p1, const T& line1_p2, const T& line2_p1, const T& line2_p2, T& result, typename T::value_type fuzzy = typename T::value_type(0.1))
    {
        return getLineLineIntersectionImpl(line1_p1, line1_p2, line2_p1, line2_p2, result, false, fuzzy);
    }
	

	/**
	 * rotate Vector v around axis r with angle theta
	 * @param r the rotation-axis
	 * @param v the vector to rotate
	 * @param theta the angle to rotate v (in radians)
	 * @return the rotated vector v'
	 */
	inline osg::Vec3 CEFIX_EXPORT rotate(const osg::Vec3& r, const osg::Vec3& v, double theta) {
		osg::Vec3 q = osg::Vec3(0,0,0);
		double costheta,sintheta;
		costheta = cos(theta);
		sintheta = sin(theta);
		q[0] += (costheta + (1 - costheta) * r.x() * r.x()) * v.x();
		q[0] += ((1 - costheta) * r.x() * r.y() - r.z() * sintheta) * v.y();
		q[0] += ((1 - costheta) * r.x() * r.z() + r.y() * sintheta) * v.z();

		q[1] += ((1 - costheta) * r.x() * r.y() + r.z() * sintheta) * v.x();
		q[1] += (costheta + (1 - costheta) * r.y() * r.y()) * v.y();
		q[1] += ((1 - costheta) * r.y() * r.z() - r.x() * sintheta) * v.z();

		q[2] += ((1 - costheta) * r.x() * r.z() - r.y() * sintheta) * v.x();
		q[2] += ((1 - costheta) * r.y() * r.z() + r.x() * sintheta) * v.y();
		q[2] += (costheta + (1 - costheta) * r.z() * r.z()) * v.z();

		return q;
	}
	
	/** get the euler-angles from a quat (in radians) */
	inline osg::Vec3 toEuler(const osg::Quat& q) {
		float heading, attitude, bank;
		
		heading = atan2(2*q.y()*q.w()-2*q.x()*q.z() , 1 - 2*q.y()*q.y() - 2*q.z()*q.z());
		attitude = asin(2*q.x()*q.y() + 2*q.z()*q.w()) ;
		bank = atan2(2*q.x()*q.w()-2*q.y()*q.z() , 1 - 2*q.x()*q.x() - 2*q.z()*q.z());

		float tmp(q.x()*q.y() + q.z()*q.w());
		if(tmp == 0.5) 
		{
			heading = 2 * atan2(q.x(),q.w());
			bank = 0;
		}
		
		else if(tmp == -0.5) 
		{
			heading = -2 * atan2(q.x(),q.w());
			bank = 0;
		}
		
		return osg::Vec3(bank, heading, attitude);
	}
	
	/** interpolate between a and b, using scalar as scalar */
	template<typename T> inline T interpolate(const T& a, const T& b, float scalar) { return a  + (b-a) * (scalar); }
	
	template <class U, typename T> inline T tween(const T& a, const T& b, float scalar) { return interpolate<T>(a, b, U::getValueAt(scalar)); }
	template<> inline osg::Matrix interpolate<osg::Matrix>(const osg::Matrix& a, const osg::Matrix& b, float scalar) {
		osg::Matrix result;
		for (int i = 0; i < 4; ++i) 
			for(int j = 0; j < 4; ++j) {
				result(i,j) = interpolate(a(i,j), b(i,j), scalar);
			}
		return result;
	}
		
	
	/* 
	 * returns a random value from the gaussian curve
	 * values between -4.78 and 4.78 (ca.)
	 */
	
	float gaussian_random();

	
	template<typename T>inline T easyInOut(T value) { return EasyInOutTweening<T>::getValueAt(value); }
		
	/** small helper struct to define ranges of values */
	template< typename ValueType > struct range {
		
		ValueType _min;
		ValueType _max;
		
		range() : _min(ValueType()), _max(ValueType()) {}
		range(ValueType t1, ValueType t2) : _min(t1), _max(t2) {}
		
		/// get a value at a given scala (0 = min, 1 = max)
		inline ValueType interpolate(float f) const  { return _min * (1.0f - f) + _max * f; }
		
		/// get a specific value with the help of a twening func
		template<class T, class U> inline ValueType tween(U f) const { return interpolate(T::getValueAt(f)); }
		
		/// get a specific value applying a easyInOut-tweening
		template<class T>inline ValueType easyInOut(T f) const { return tween< EasyInOutTweening<T>,T >(f); }
		
		/// get a random value between min and max
		inline ValueType random() const { return interpolate(randomf(1.0f)); }
		
		/// get the stored min-value
		inline const ValueType& min() const { return _min; }
		
		/// get the stored max value
		inline const ValueType& max() const { return _max; }
		
		/// get the stored min-value
		inline  ValueType& min()  { return _min; }
		
		/// get the stored max value
		inline  ValueType& max()  { return _max; }
		
		/// get the value at 0.5
		inline ValueType mid() const { return interpolate(0.5f); }
		
		/// set min and max
		inline void set(ValueType min, ValueType max) { _min = min; _max = max; }
		
		/// clamp a given value to the range
		inline ValueType clampTo(const ValueType& t) const { return (t >= _min) ? (t <= _max) ? t : _max : _min; }
		
		inline void setMin(ValueType in_min)  { _min = in_min; }
		inline void setMax(ValueType in_max)  { _max = in_max; } 
		/// return a random value betwenn min and max applying the gaussian probability curve
		inline ValueType gaussianRandom() { return interpolate(0.5f + std::min(4.78f, gaussian_random()) / (2 * 4.78f)); }
			
	};
	
	typedef range<float> floatRange;
	typedef range<double> doubleRange;
	
	typedef range<osg::Vec3> vec3Range;
	typedef range<osg::Vec2> vec2Range;
	typedef range<osg::Vec4> vec4Range;
	typedef range<osg::Quat> quatRange;
	
	    
    /** small class helping with rects */
    class Rect {
        public:
            template<class T>static inline typename T::value_type getLeft(const T& r) { return r[0]; }
            template<class T>static inline typename T::value_type getRight(const T& r) { return r[2]; }
            template<class T>static inline typename T::value_type getTop(const T& r) { return r[1]; }
            template<class T>static inline typename T::value_type getBottom(const T& r) { return r[3]; }
            
            template<class T>static inline typename T::value_type getWidth(const T& r) { return osg::absolute(r[2] - r[0]); }
            template<class T>static inline typename T::value_type getHeight(const T& r) { return osg::absolute(r[3] - r[1]); }
            
            static inline osg::Vec2f getTopLeft(const osg::Vec4f& r) { return osg::Vec2f(r[0], r[1]); }
            static inline osg::Vec2f getBottomRight(const osg::Vec4f& r) { return osg::Vec2f(r[2], r[3]); }
            static inline osg::Vec2f getBottomLeft(const osg::Vec4f& r) { return osg::Vec2f(r[0], r[3]); }
			static inline osg::Vec2f getTopRight(const osg::Vec4f& r) { return osg::Vec2f(r[2], r[1]); }
            static inline osg::Vec2f getCenter(const osg::Vec4f& r) { return osg::Vec2f( (r[0] + r[2]) / 2.0f, (r[1] + r[3]) / 2.0f); }
            
            static inline osg::Vec2d getTopLeft(const osg::Vec4d& r) { return osg::Vec2d(r[0], r[1]); }
            static inline osg::Vec2d getBottomRight(const osg::Vec4d& r) { return osg::Vec2d(r[2], r[3]); }
            static inline osg::Vec2d getBottomLeft(const osg::Vec4d& r) { return osg::Vec2d(r[0], r[3]); }
			static inline osg::Vec2d getTopRight(const osg::Vec4d& r) { return osg::Vec2d(r[2], r[1]); }
            static inline osg::Vec2d getCenter(const osg::Vec4d& r) { return osg::Vec2d( (r[0] + r[2]) / 2.0, (r[1] + r[3]) / 2.0); }
            
            template<class T>static inline T inset(const T& rect, typename T::value_type x, typename T::value_type y) 
            { 
                osg::Vec4 result(rect);
                result[0] += x;
                result[2] -= x;
                result[1] += y;
                result[3] -= y;
                
                return result;
            }
            
            template<class T, class U>
            static inline T inset(const T& rect, const U& delta) {
                return inset(rect, delta[0], delta[1]);
            }
            
            template <class T>
            static inline T offset(const T& rect, typename T::value_type x, typename T::value_type y) 
            { 
                osg::Vec4 result(rect);
                result[0] += x;
                result[2] += x;
                result[1] += y;
                result[3] += y;
                
                return result;
            }
            
            template<class T, class U>
            static inline T offset(const T& rect, const U& delta) {
                return offset(rect, delta[0], delta[1]);
            }
            
            /// returns true, if r2 is conatained by r1
            template<class T>
            static inline bool contains(const T& r1, const T& r2)
            {
                return (r1[0] <= r2[0]) && (r1[1] <= r2[1]) && (r1[2] >= r2[2]) && (r1[3] >= r2[3]);
            }
            
            template<class T>
			static inline bool isIntersecting(const T& r1, const T& r2) 
			{
				return !((r1[0] > r2[2]) || (r1[2] < r2[0]) || (r1[1] > r2[3]) || (r1[3] < r2[1]));
			}
			
			template<class T>
			static inline bool getIntersection(const T& r1, const T& r2, T& result)
			{
				result[0] = std::max(r1[0], r2[0]);
				result[1] = std::max(r1[1], r2[1]);
				result[2] = std::min(r1[2], r2[2]);
				result[3] = std::min(r1[3], r2[3]);
				
				if((result[0] <= result[2]) && (result[1] <= result[3]))
					return true;
					
				result.set(0,0,0,0);
				return false;
			}
			
            template<class T>
			static inline void getUnion(const T& r1, const T& r2, T& result)
			{
				result[0] = std::min(r1[0], r2[0]);
				result[1] = std::min(r1[1], r2[1]);
				result[2] = std::max(r1[2], r2[2]);
				result[3] = std::max(r1[3], r2[3]);
			}
            
        private:
            Rect() {}
        };




static inline  void gaussian_elimination(osg::Matrix::value_type *input, int n){
	// ported to c from pseudocode in
	// http://en.wikipedia.org/wiki/Gaussian_elimination

	osg::Matrix::value_type * A = input;
	int i = 0;
	int j = 0;
	int m = n-1;
	while (i < m && j < n){
	  // Find pivot in column j, starting in row i:
	  int maxi = i;
	  for(int k = i+1; k<m; k++){
	    if(fabs(A[k*n+j]) > fabs(A[maxi*n+j])){
	      maxi = k;
	    }
	  }
	  if (A[maxi*n+j] != 0){
	    //swap rows i and maxi, but do not change the value of i
		if(i!=maxi)
		for(int k=0;k<n;k++){
			osg::Matrix::value_type aux = A[i*n+k];
			A[i*n+k]=A[maxi*n+k];
			A[maxi*n+k]=aux;
		}
	    //Now A[i,j] will contain the old value of A[maxi,j].
	    //divide each entry in row i by A[i,j]
		osg::Matrix::value_type A_ij=A[i*n+j];
		for(int k=0;k<n;k++){
			A[i*n+k]/=A_ij;
		}
	    //Now A[i,j] will have the value 1.
	    for(int u = i+1; u< m; u++){
    		//subtract A[u,j] * row i from row u
	    	osg::Matrix::value_type A_uj = A[u*n+j];
	    	for(int k=0;k<n;k++){
	    		A[u*n+k]-=A_uj*A[i*n+k];
	    	}
	      //Now A[u,j] will be 0, since A[u,j] - A[i,j] * A[u,j] = A[u,j] - 1 * A[u,j] = 0.
	    }

	    i++;
	  }
	  j++;
	}

	//back substitution
	for(int i=m-2;i>=0;i--){
		for(int j=i+1;j<n-1;j++){
			A[i*n+m]-=A[i*n+j]*A[j*n+m];
			//A[i*n+j]=0;
		}
	}
}

static inline  void findHomography(const std::vector<osg::Vec2>& src, const std::vector<osg::Vec2>& dst, osg::Matrix::value_type homography[16]){

	// create the equation system to be solved
	//
	// from: Multiple View Geometry in Computer Vision 2ed
	//       Hartley R. and Zisserman A.
	//
	// x' = xH
	// where H is the homography: a 3 by 3 matrix
	// that transformed to inhomogeneous coordinates for each point
	// gives the following equations for each point:
	//
	// x' * (h31*x + h32*y + h33) = h11*x + h12*y + h13
	// y' * (h31*x + h32*y + h33) = h21*x + h22*y + h23
	//
	// as the homography is scale independent we can let h33 be 1 (indeed any of the terms)
	// so for 4 points we have 8 equations for 8 terms to solve: h11 - h32
	// after ordering the terms it gives the following matrix
	// that can be solved with gaussian elimination:

	osg::Matrix::value_type P[8][9]={
			{-src[0].x(), -src[0].y(), -1,   0,   0,  0, src[0].x()*dst[0].x(), src[0].y()*dst[0].x(), -dst[0].x() }, // h11
			{  0,   0,  0, -src[0].x(), -src[0].y(), -1, src[0].x()*dst[0].y(), src[0].y()*dst[0].y(), -dst[0].y() }, // h12

			{-src[1].x(), -src[1].y(), -1,   0,   0,  0, src[1].x()*dst[1].x(), src[1].y()*dst[1].x(), -dst[1].x() }, // h13
		    {  0,   0,  0, -src[1].x(), -src[1].y(), -1, src[1].x()*dst[1].y(), src[1].y()*dst[1].y(), -dst[1].y() }, // h21

			{-src[2].x(), -src[2].y(), -1,   0,   0,  0, src[2].x()*dst[2].x(), src[2].y()*dst[2].x(), -dst[2].x() }, // h22
		    {  0,   0,  0, -src[2].x(), -src[2].y(), -1, src[2].x()*dst[2].y(), src[2].y()*dst[2].y(), -dst[2].y() }, // h23

			{-src[3].x(), -src[3].y(), -1,   0,   0,  0, src[3].x()*dst[3].x(), src[3].y()*dst[3].x(), -dst[3].x() }, // h31
		    {  0,   0,  0, -src[3].x(), -src[3].y(), -1, src[3].x()*dst[3].y(), src[3].y()*dst[3].y(), -dst[3].y() }, // h32
	};

	gaussian_elimination(&P[0][0],9);

	// gaussian elimination gives the results of the equation system
	// in the last column of the original matrix.
	// opengl needs the transposed 4x4 matrix:
	osg::Matrix::value_type aux_H[]={ P[0][8],P[3][8],0,P[6][8], // h11  h21 0 h31
					P[1][8],P[4][8],0,P[7][8], // h12  h22 0 h32
					0      ,      0,0,0,       // 0    0   0 0
					P[2][8],P[5][8],0,1};      // h13  h23 0 h33

	for(int i=0;i<16;i++) homography[i] = aux_H[i];
}

static inline osg::Matrix findHomography(const std::vector<osg::Vec2>& src, const std::vector<osg::Vec2>& dst){
	osg::Matrix matrix;

	osg::Matrix::value_type mat[16];
	findHomography(src, dst, mat);
	matrix.set(mat);

	return matrix;
}

} // end of namespace

#endif
