/*
 *  AnimateQuad2DTransparency.cpp
 *  AudiHeritage
 *
 *  Created by Stephan Huber on 22.05.06.
 *  Copyright 2006 digitalmind.de // Stephan Maximilian Huber. All rights reserved.
 *
 */

#include "AnimateQuad2DTransparency.h"

namespace cefix {

AnimateQuad2DTransparency::AnimateQuad2DTransparency(   float starttime, 
                                                        float duration, 
                                                        Quad2DGeometry* geo, 
                                                        float startblend, 
                                                        float endblend,
														bool doWipe) 
    : cefix::AnimationBasedOnDuration(starttime, duration),
    _geo(geo), 
    _startblend(startblend), 
    _endblend(endblend),
	_doWipe(doWipe)
{
	_colors = dynamic_cast<osg::Vec4Array*>(geo->getColorArray());
	
	if (_colors->size() <= 1)
			_doWipe = false;

	animate(0.0f);
}


}