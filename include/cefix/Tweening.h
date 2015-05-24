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

#ifndef CEFIX_TWEENING_HEADER
#define CEFIX_TWEENING_HEADER

#include <cefix/MathUtils.h>

namespace cefix {

	/// Tweening class using t*t
	
	template<typename T>
	class QuadricInTweening : public Tweening {
	public:
        typedef T value_type;
		static inline T getValueAt(T t) { return t*t; }
	};
	
	/// Tweening class using t*t
	template<typename T>
	class QuadricOutTweening : public Tweening {
	public:
        typedef T value_type;
		static inline T getValueAt(T t) { return 1-(1-t)*(1-t); }
	};
	
	/// Tweening class using t*t
	template<typename T>
	class QuadricInOutTweening : public Tweening {
	public:
        typedef T value_type;
		static inline T getValueAt(T t) { return (t < 0.5) ? QuadricInTweening<T>::getValueAt(t*2)/2.0 : QuadricOutTweening<T>::getValueAt(t*2-1)/2.0+0.5; }
	};

	/// Tweening class using t*t*t
	template<typename T>
	class CubicInTweening : public Tweening {
	public:
        typedef T value_type;
		static inline T getValueAt(T t) { return t*t*t; }
	};
	
	/// Tweening class using t*t*t
	template<typename T>
	class CubicOutTweening : public Tweening {
	public:
        typedef T value_type;
		static inline T getValueAt(T t) { return 1-(1-t)*(1-t)*(1-t); }
	};
	
	/// Tweening class using t*t*t
	template<typename T>
	class CubicInOutTweening : public Tweening {
	public:
        typedef T value_type;
		static inline T getValueAt(T t) { return (t < 0.5) ? CubicInTweening<T>::getValueAt(t*2)/2.0 : CubicOutTweening<T>::getValueAt(t*2-1)/2.0+0.5; }
	};
	
	/// Tweening class using t*t*t*t
	template<typename T>
	class QuartInTweening : public Tweening {
	public:
        typedef T value_type;
		static inline T getValueAt(T t) { return t*t*t*t; }
	};
	
	/// Tweening class using t*t*t*t
	template<typename T>
	class QuartOutTweening : public Tweening {
	public:
        typedef T value_type;
		static inline T getValueAt(T t) { T t1(1-t); return 1-(t1*t1*t1*t1); }
	};
	
	/// Tweening class using t*t*t*t
	template<typename T>
	class QuartInOutTweening : public Tweening {
	public:
        typedef T value_type;
		static inline T getValueAt(T t) { return (t < 0.5) ? QuartInTweening<T>::getValueAt(t*2)/2.0 : QuartOutTweening<T>::getValueAt(t*2-1)/2.0+0.5; }
	};
	
	/// Tweening class using t*t*t*t*t
	template<typename T>
	class QuintInTweening : public Tweening {
	public:
        typedef T value_type;
		static inline T getValueAt(T t) { return t*t*t*t*t; }
	};
	
	/// Tweening class using t*t*t*t*t
	template<typename T>
	class QuintOutTweening : public Tweening {
	public:
        typedef T value_type;
		static inline T getValueAt(T t) { return 1 - QuintInTweening<T>::getValueAt(1-t); }
	};
	
	/// Tweening class using t*t*t*t*t
	template<typename T>
	class QuintInOutTweening : public Tweening {
	public:
        typedef T value_type;
		static inline T getValueAt(T t) { return (t < 0.5) ? QuintInTweening<T>::getValueAt(t*2)/2.0 : QuintOutTweening<T>::getValueAt(t*2-1)/2.0+0.5; }
	};
	
	/// Tweening class using 1-cos(t*0.5*pi)
	template<typename T>
	class SineInTweening : public Tweening {
	public:
		static inline T getValueAt(T t) { return 1-cos(0.5f*t*osg::PI); }
	};
	
	/// Tweening class using cos
	template<typename T>
	class SineOutTweening : public Tweening {
	public:
        typedef T value_type;
		static inline T getValueAt(T t) { return cos(0.5*(t-1)*osg::PI); }
	};
	
	/// Tweening class using cos
	template<typename T>
	class SineInOutTweening : public Tweening {
	public:
        typedef T value_type;
		static inline T getValueAt(T t) { return (t < 0.5) ? SineInTweening<T>::getValueAt(t*2)/2.0 : SineOutTweening<T>::getValueAt(t*2-1)/2.0+0.5; }
	};
	
	/// Tweening class using sqrt(1-t*t)
	template<typename T>
	class CircularInTweening : public Tweening {
	public:
        typedef T value_type;
		static inline T getValueAt(T t) { return 1-sqrt(1-t*t); }
	};
	
	/// Tweening class using sqrt(1-t*t)
	template<typename T>
	class CircularOutTweening : public Tweening {
	public:
        typedef T value_type;
		static inline T getValueAt(T t) { return sqrt(1-(t-1)*(t-1)); }
	};
	
	/// Tweening class using sqrt(1-t*t)
	template<typename T>
	class CircularInOutTweening : public Tweening {
	public:
        typedef T value_type;
		static inline T getValueAt(T t) { return (t < 0.5) ? CircularInTweening<T>::getValueAt(t*2)/2.0 : CircularOutTweening<T>::getValueAt(t*2-1)/2.0+0.5; }
	};
	
	
	template<typename T>
	class ElasticOutTweening : public Tweening {
	public:
        typedef T value_type;
		static inline T getValueAt(T t) { return pow(static_cast<T>(2.0), static_cast<T>(-10.0) * t) * sin((t - 0.3 / 4.0) * (2.0 * osg::PI) / 0.3) + 1.0; }
	};
	
	template<typename T>
	class ElasticInTweening : public Tweening {
	public:
        typedef T value_type;
		static inline T getValueAt(T t) { return 1.0f - ElasticOutTweening<T>::getValueAt(1-t); }
	};
	
	template<typename T>
	class ElasticInOutTweening : public Tweening {
	public:
        typedef T value_type;
		static inline T getValueAt(T t) { return (t < 0.5) ? ElasticInTweening<T>::getValueAt(t*2)/2.0 : ElasticOutTweening<T>::getValueAt(t*2-1)/2.0+0.5; }
	};
	
	template<typename T>
	class OvershootInTweening : public Tweening {
	public:
        typedef T value_type;
		static inline T getValueAt(T t) { return ((1.70158+1) * (t*t*t)) - (1.70158 * (t*t)); }
	};
	
	template<typename T>
	class OvershootOutTweening : public Tweening {
	public:
        typedef T value_type;
		static inline T getValueAt(T t) { return 1-OvershootInTweening<T>::getValueAt(1-t); }
	};
		
	template<typename T>
	class OvershootInOutTweening : public Tweening {
	public:
        typedef T value_type;
		static inline T getValueAt(T t) { return (t < 0.5) ? OvershootInTweening<T>::getValueAt(t*2)/2.0 : OvershootOutTweening<T>::getValueAt(t*2-1)/2.0+0.5; }
	};
	
	
	template<typename T>
	class BounceOutTweening : public Tweening {
	public:
        typedef T value_type;
		static inline T getValueAt(T t) 
		{  
			if ((t) < (1/2.75)) 
            {
                return 7.5625 * t * t;
            } 
            else if (t < (2/2.75)) 
            {
                t = t - (1.5/2.75);
                return 7.5625* t * t + .75;
            }
            else if (t < (2.5/2.75)) 
            {
                t = t - (2.25/2.75);
                return 7.5625 * t * t + .9375;
            }
            else 
            {
                t = t - (2.625/2.75);
                return 7.5625* t * t + .984375;
            }
		}
	};
	
	template<typename T>
	class BounceInTweening : public Tweening {
	public:
        typedef T value_type;
		static inline T getValueAt(T t) { return 1.0f - BounceOutTweening<T>::getValueAt(1-t); }
	};
	
	template<typename T>
	class BounceInOutTweening : public Tweening {
	public:
        typedef T value_type;
        
		static inline T getValueAt(T t) { return (t < 0.5) ? BounceInTweening<T>::getValueAt(t*2)/2.0 : BounceOutTweening<T>::getValueAt(t*2-1)/2.0+0.5; }
	};


}


#endif