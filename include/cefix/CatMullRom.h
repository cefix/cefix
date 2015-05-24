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
 
#ifndef CATMULLROM_SPLINE_HEADER
#define CATMULLROM_SPLINE_HEADER

#include "Cardinal.h"

namespace cefix { 
namespace spline {

	/** a CatMullRom-spline, see <http://en.wikipedia.org/wiki/Catmull-Rom_spline> for more infos */
	class CatMullRom : public Cardinal {
	
		public:
			CatMullRom(ControlPoint::List* controlPoints) : Cardinal(controlPoints, 0.5f) {}
	
	};

}
}

#endif