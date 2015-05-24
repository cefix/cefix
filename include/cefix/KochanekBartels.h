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


#ifndef KOCHANEKBARTELS_SPLINE_HEADER
#define KOCHANEKBARTELS_SPLINE_HEADER

#include <cefix/Export.h>
#include <cefix/Hermite.h>

namespace cefix { 
namespace spline {
	
	/** a KochanekBartels-spline, see <http://en.wikipedia.org/wiki/Kochanek-Bartels_spline> */
	class CEFIX_EXPORT KochanekBartels : public Hermite {
	
		public:
			/** ctor
			 * @param controlPoints list of controlpoints
			 * @param tension tension
			 * @param continuity continuity
			 * @param bias bias
			 */
			KochanekBartels(ControlPoint::List* controlPoints, float tension, float continuity, float bias) :
				Hermite(controlPoints),
				_tension(tension),
				_continuity(continuity),
				_bias(bias) 
			{
				calculateTangents(tension, continuity, bias);
				calculateLength(1000.0f);
			}
			
			/** sets the tension */
			void setTension(float f) { _tension = f; }
			
			/** sets the continuity */
			void setContinuity(float f) { _continuity = f; }
			
			/** sets the bias */
			void setBias(float f) { _bias = f; }
            
            using Curve::getWorldPosition;
		
		protected:
			virtual void updateImplementation() {
				calculateTangents(_tension, _continuity, _bias);
			}

		private:
			void calculateTangents(float tension, float continuity, float bias);
			
			float _tension, _continuity, _bias;
	
	};

}
}

#endif