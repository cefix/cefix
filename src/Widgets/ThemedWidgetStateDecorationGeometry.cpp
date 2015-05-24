/*
 *  ThemedWidgetStateDecoratonGeometry.cpp
 *  cefixcvTemplate
 *
 *  Created by Stephan Huber on 28.10.08.
 *  Copyright 2008 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */

#include "ThemedWidgetStateDecorationGeometry.h"

namespace cefix {

ThemedWidgetStateDecorationGeometry::ThemedWidgetStateDecorationGeometry()
:	ThemedWidgetStateGeometry(),
	_insetWidth(0.0f),
	_insetHeight(0.0f)
{
}

void ThemedWidgetStateDecorationGeometry::setRect(const osg::Vec4& r, bool resetTextureRect) {
	ThemedWidgetStateGeometry::setRect(osg::Vec4(r[0] + _insetWidth, r[1] + _insetHeight, r[2] - _insetWidth, r[3] - _insetHeight), resetTextureRect);
}

void ThemedWidgetStateDecorationGeometry::setTextureRect(const osg::Vec4& r) 
{
	float w = static_cast<float>(getTexture()->getImage(0)->s());
	ThemedWidgetStateGeometry::setTextureRect(osg::Vec4(0, r[1], (_rect[2] - _rect[0] - (2*_insetWidth)) / w, r[3]));
	
}

}