/*
 *  StandardTextRenderIterator.cpp
 *  cefix
 *
 *  Created by Stephan Huber on 08.04.08.
 *  Copyright 2008 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */

#include "StandardTextRenderIterator.h"

namespace cefix {

// ---------------------------------------------------------------------------
// advance
// bewegt den "RenderCursor" eine Position weiter
// ---------------------------------------------------------------------------
void  StandardTextRenderIterator::advance(const unsigned int &left, const unsigned int &right) {
	
	switch (left) {
		case '\n':
            _currentLine++;
            startNewLine();
			if (_lineSpacing == 0) _lineSpacing = _font->getFontSize();
			
			_position[1] -= _lineSpacing;
			_position[0] = _leftMargin;
			
			break;
			
	   default:
			_position[0] += _font->getCharWidth(left,right) + _spatio;
			break;
			
	}
	
}

// ----------------------------------------------------------------------------------------------------------
// preflight
// ----------------------------------------------------------------------------------------------------------
void StandardTextRenderIterator::preflight(const unsigned int &left, const unsigned int &right) {
	
	switch (left) {
		case '\n':
			if (_lineSpacing == 0) _lineSpacing = _font->getFontSize();
			_lineWidths.push_back(_leftMargin);
			break;
			
	   default:
		   _lineWidths[ _lineWidths.size()-1 ] += _font->getCharWidth(left,right) + _spatio;
			break;
			
	}

}

// ---------------------------------------------------------------------------
// render
// rendert ein Zeichen
// ---------------------------------------------------------------------------
void StandardTextRenderIterator::render(const unsigned int &ch) {
	if (ch > 32) {
		float delta = 0;
		switch (_textAlignment) {
			case ALIGN_LEFT: 
				break;
				
			case ALIGN_CENTER:
				delta = _lineWidths[_currentLine] / 2.0;
				if (getSubPixelAlignment() == false)
					delta = (int)delta;
				break;
			case ALIGN_RIGHT:
				delta = _lineWidths[_currentLine];
				break;
		}
				
		FontCharData* charData = _font->getCharData(ch);
		_container->select(charData->getTexID());
		//std::cout << "renderplane: " << _renderPlane << std::endl;
		charData->addVertices(_container->getVertices(), _position[0] - delta, _position[1], _renderPlane, _startPos);  
		charData->addTexCoords(_container->getTexCoords());
	}
    
}

// ---------------------------------------------------------------------------
// finish
// aufraeumarbeiten machen
// ---------------------------------------------------------------------------
void StandardTextRenderIterator::finish() {
					
	//FlaechenNormal berechnen
	
	osg::Vec3 n,left,up;
	
	//normal berechnen wenn noch keins vorhanden
	osg::Vec3Array* vertices = _container->getVertices();
	if (vertices) {
		unsigned int m = vertices->size();
		if ((m != 0) && (_container->getNormals()->size() == 0))
		{
			
			left = (*vertices)[m-1] - (*vertices)[m-3];
			up   = (*vertices)[m-2] - (*vertices)[m-3];
			
			n = up ^ left;
			n.normalize();
			_container->setOverallNormal(n);
		}
	}
	
	//osg::notify(osg::ALWAYS) << "normals-size: " << _normals->size() << std::endl;
	
	_container->dirty();
	
	if ( _fontChanged || (!_container->hasStateSet())) {
		_container->setStateSetForFont(_font.get());
		_fontChanged = false;
	}
	
}
	  

}
