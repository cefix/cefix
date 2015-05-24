/*
 *  MathUtils.c
 *  NativeContext
 *
 *  Created by Stephan Huber on Wed Jul 24 2002.
 *  Copyright (c) 2002 digital mind. All rights reserved.
 *
 */

#include <cefix/MathUtils.h>

namespace cefix {

float gaussian_random() 
{
	static bool has_result(false);
	static float result;
	
	if (has_result) {
		has_result = false;
		return result;
	}
	
	// from http://www.taygeta.com/random/gaussian.html
	// Algorithm by Dr. Everett (Skip) Carter, Jr.

	 float x1, x2, w, y1;

	 do {
			 x1 = 2.0 * randomf(1.0) - 1.0;
			 x2 = 2.0 * randomf(1.0) - 1.0;
			 w = x1 * x1 + x2 * x2;
	 } while ( w >= 1.0 );

	 w = sqrtf( (-2.0 * ::log( w ) ) / w );
	 y1 = x1 * w;
	 result = x2 * w;
	 has_result = true;
	 return y1;
	 
}
	
}