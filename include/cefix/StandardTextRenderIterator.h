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

#ifndef CEFIX_STANDARD_TEXT_RENDER_ITERATOR_HEADER
#define CEFIX_STANDARD_TEXT_RENDER_ITERATOR_HEADER

#include <cefix/Export.h>
#include <cefix/TextRenderer.h>

namespace cefix {

class CEFIX_EXPORT StandardTextRenderIterator : public TextRendererBase::IteratorBase {

						
	public:
		
		enum TextAlignment { ALIGN_LEFT, ALIGN_CENTER, ALIGN_RIGHT };

					
		/** Constructor, resets the iterator */
		StandardTextRenderIterator() 
		:	TextRendererBase::IteratorBase (), 
			_leftMargin(0), 
			_topMargin(0),
			_textAlignment(ALIGN_LEFT) 
		{ 
			reset(); 
		}
		
		/**
		 * advance the internal position to the next position, so char left can be rendered
		 * @param left char which is next to be rendered
		 * @param right char following left (for kerning computations)
		 */
		virtual void advance(const unsigned int &left, const unsigned int &right);
		
		/** reset */
		virtual void reset() { 
			_position = osg::Vec2(0,0); 
			
			_startPos = osg::Vec3(0,0,0); 
			setMargins(0.0f,0.0f); 
			setSpatio(0.0f); 
			resetContainer();
			_lineWidths.clear(); 
			_lineWidths.push_back(_leftMargin);
			_currentLine = 0;
		}
		
		/** do some init stuff */
		virtual void begin() {
			_font->setSubPixelAlignment(getSubPixelAlignment());
			if (getSubPixelAlignment() == false)
				_startPos.set( (int)_startPos[0], (int)_startPos[1], (int)_startPos[2]);
		}
		
		/** preflight is needed for other textalignments than left */
		virtual bool needsPreflight() { return _textAlignment != ALIGN_LEFT; }
		
		virtual void preflight(const unsigned int &left, const unsigned int &right);					
		

        /**
		 * render the char
		 * @param ch char to render
		 */
		virtual void render(const unsigned int &ch);
		/**
		 * finishes the rendering process, generate normals
		 */
		virtual void finish();
		
		/** return the nr of lines rendered */
		virtual int getRenderedLineCount() const { return 1+_currentLine; }

		
		/** 
		 * sets the left and top margin
		 * @param left new left margin
		 * @param top new top margin
		 */
		inline void setMargins(float left,float top) { _leftMargin = left, _topMargin = top; }
		
		/** sets the text-alignment */
		inline void setTextAlignment(TextAlignment align) { _textAlignment = align; }
		
		/** @return the text-alignmen */
		TextAlignment getTextAlignment() const { return _textAlignment; }
							
				
	protected:
		virtual ~StandardTextRenderIterator() {}
		/** current position where a char is placed */
		osg::Vec2 _position;
		
		/** the left + top margin */
		float				_leftMargin, _topMargin;
		
		TextAlignment		_textAlignment;
		std::vector<float>  _lineWidths;
		unsigned int        _currentLine;

};



}

#endif