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

#ifndef CARDINAL_SPLINE_HEADER
#define CARDINAL_SPLINE_HEADER

#include <cefix/Hermite.h>
#include <cefix/Export.h>

namespace cefix { 
namespace spline {
	/** a Cardinal-spline, see <http://en.wikipedia.org/wiki/Cardinal_spline> for more infos */
	class CEFIX_EXPORT Cardinal : public Hermite {
	
		public:
			/** ctor */
			Cardinal(ControlPoint::List* controlPoints, float tangentStrength);
			
			/** set the tangent-strength */
			void setTangentStrength(float f) {_tangentStrength = f; }
			
		protected:
			virtual void updateImplementation() {
				calculateTangents(_tangentStrength);
			}
			
		private:
			void calculateTangents(float strength);
			float _tangentStrength;
	
	};

}
}

#endif


