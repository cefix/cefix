/*
 *  AttributedTextRenderIterator.cpp
 *  cefix
 *
 *  Created by Stephan Huber on 10.08.06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include <cefix/AttributedTextRenderIterator.h>

namespace cefix {

void AttributedTextRenderIterator::setFontSize(float size, unsigned int attribute) {
	_fontmap[attribute].fontSize = size;
}

void AttributedTextRenderIterator::setLineSpacing(float size, unsigned int attribute) {
	_fontmap[attribute].lineSpacing = size;
}


} 