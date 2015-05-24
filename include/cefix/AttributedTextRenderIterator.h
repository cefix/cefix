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

#ifndef ATTRIBUTED_TEXT_RENDER_ITERATOR_HEADER_
#define ATTRIBUTED_TEXT_RENDER_ITERATOR_HEADER_

#include <cefix/StandardTextRenderIterator.h>
#include <cefix/FontManager.h>

namespace cefix {

/** AttributedTextRenderIterator can render strings with different fonts. It uses special chars to "switch" between 
 *  the different fonts. The attributes are not culmulative!
 *  An example text could be <code>this text is $bold$, this %here is italic% and this here is #bolditalic#.</code>
 *  In reallife you'll choose unicodepoints you don't use in your normal texts 
 *  Be aware, that this code is not the fastest, you have been warned
 */
class CEFIX_EXPORT AttributedTextRenderIterator : public StandardTextRenderIterator {
	public:
		class FontContainer {
			public:
				FontContainer(cefix::Font* infont, float size, float line_spacing, TextRendererBase::Container* incontainer) : 
					font(infont), 
					fontSize(size),
					lineSpacing(line_spacing), 
					container(incontainer) 
				{
				}
				
				FontContainer() : font(NULL), fontSize(0.0f), lineSpacing(-1.0f), container(NULL) {}
			
				osg::ref_ptr< cefix::Font > font;
				float						fontSize, lineSpacing;
				osg::ref_ptr<TextRendererBase::Container> container;
		};
		
		typedef std::map<unsigned int, FontContainer> FontMap;
		
		/** ctor*/
		AttributedTextRenderIterator() : StandardTextRenderIterator() {}
		AttributedTextRenderIterator(const AttributedTextRenderIterator& itr)
		: StandardTextRenderIterator()
		{
			for (FontMap::const_iterator fi = itr._fontmap.begin(); fi != itr._fontmap.end(); ++fi) {
				_fontmap.insert(
								std::make_pair(
											   fi->first, 
											   FontContainer( fi->second.font.get(), fi->second.fontSize, fi->second.lineSpacing, new TextRendererBase::Container()) 
											   ) 
								);
			}
		}
		
		
		/** register font/container which is used for all the text without attributes */
		void registerStandardFont(Font* font, TextRendererBase::Container* container) {
			registerFont(0, font, container); 
		}
		
		/** registers a font for a special codepoint which will associate the font with,
		    text between this codepoint will be rendered with this font */
		void registerFont(unsigned int codepoint, Font* font, TextRendererBase::Container* container) {
			_fontmap[codepoint] = FontContainer(font,0.0f, -1.0f, container);
			_fontChanged = true;
			//dumpFontMap();
		}
		
		/** registers a font for a special codepoint which will associate the font with,
		    text between this codepoint will be rendered with this font */
		void registerFont(unsigned int codepoint, std::string fontname, TextRendererBase::Container* container) {
			Font* font = FontManager::instance()->loadFont(fontname);
			if(font)
				_fontmap[codepoint] = FontContainer(font, 0.0f, -1.0f, container);
		}
		
		/** set the text color */
		void setTextColor(osg::Vec4 c, int attribute = -1) {
			if (attribute == -1) {
				for(FontMap::iterator i = _fontmap.begin(); i != _fontmap.end(); i++) {
					osg::Vec4Array* colors =(i->second.container->getColors());
					if ((colors) && (colors->size()>0)) {
						(*colors)[0] = c;
						i->second.container->dirtyDisplayList();
					} else {
						colors = new osg::Vec4Array();
						colors->push_back(c);
						i->second.container->setColorArray(colors);
						i->second.container->dirty();
					}
				}
			}
			else {
				TextRendererBase::Container* container = getContainer(attribute);
				if (container) {
					osg::Vec4Array* colors = dynamic_cast<osg::Vec4Array*>(container->getColors());
					if ((colors) && (colors->size()>0)) {
						(*colors)[0] = c;
						container->dirtyDisplayList();
					} else {
						colors = new osg::Vec4Array();
						colors->push_back(c);
						container->setColorArray(colors);
						container->dirty();
					}
				}
			}
		}
		
		/** return the textcolor for a specific attribute */
		osg::Vec4 getTextColor(unsigned int attribute = 0) const {
			const TextRendererBase::Container* container = getContainer(attribute);
			if (container) {
				const osg::Vec4Array* colors = (container->getColors());
				if ((colors) && (colors->size()>0)) {
					return (*colors)[0];
				}
			}
			return osg::Vec4(0,0,0,1);
		}
		
		/** @return the geometry-object for a specific attribute */
		TextRendererBase::Container* getContainer(unsigned int attribute = 0) {
			FontMap::iterator itr = _fontmap.find(attribute);
			if (itr == _fontmap.end())
				return NULL;
			else
				return itr->second.container.get();
		}
	
		const TextRendererBase::Container* getContainer(unsigned int attribute = 0) const {
			FontMap::const_iterator itr = _fontmap.find(attribute);
			if (itr == _fontmap.end())
				return NULL;
			else
				return itr->second.container.get();
		}
		
		/** @return the geometry-object for a specific attribute */
		Font* getFont(unsigned int attribute = 0) const {
			FontMap::const_iterator itr = _fontmap.find(attribute);
			if (itr == _fontmap.end())
				return NULL;
			else
				return itr->second.font.get();
		}
		
		bool hasAttribute(unsigned int attr) {
			FontMap::iterator itr = _fontmap.find(attr);
			return  (itr != _fontmap.end());
		}
		
		void replaceFont(unsigned int attr, Font* font) {
			FontMap::iterator itr = _fontmap.find(attr);
			if (itr == _fontmap.end())
				return;
			itr->second.font = font;
		}
		
		void replaceFont(unsigned int attr, std::string fontname) {
		
			replaceFont(attr, FontManager::instance()->loadFont(fontname));
		}
		
		/** checks if a given char is an attribute-codepoint. if switchContainer is true,
		 *  the containers are prepared for the rendering 
		 */
		bool isAttributedChar(unsigned int ch, bool switchContainer = false) {
			
			FontMap::iterator itr = _fontmap.find(ch);
			if (itr == _fontmap.end())
				return false;
			
			
			if (switchContainer) {
				if (ch == _lastAttributedChar) {
					_lastAttributedChar = 0;
					itr = _fontmap.find(0);
					setCurrentFont(itr->second.font.get(), itr->second.fontSize, itr->second.lineSpacing);
					setContainer(itr->second.container.get());

				} else {
					_lastAttributedChar = ch;
					setCurrentFont(itr->second.font.get(), itr->second.fontSize, itr->second.lineSpacing);
					setContainer(itr->second.container.get());
				}
			}
			return true;
		}
		
		inline void setCurrentFont(Font* fnt, float size, float lineSpacing) { 
			//std::cout << this << " switching to " << fnt << " size: " << size << " ls: " << lineSpacing << std::endl;
			_font = fnt; 
			_font->setFontSize(size); 
			if (lineSpacing >= 0)
                _lineSpacing = lineSpacing;
		}
		
		void setFontSize(float size, unsigned int attribute = 0) ;
		
		void setLineSpacing(float size, unsigned int attribute = 0);
		
		float getFontSize(unsigned int attribute = 0) const {
			FontMap::const_iterator i = _fontmap.find(attribute);
			return (i != _fontmap.end()) ? i->second.fontSize : 0;
		}
		
		float getLineSpacing(unsigned int attribute = 0) const {
			FontMap::const_iterator i = _fontmap.find(attribute);
			return (i != _fontmap.end()) ? i->second.lineSpacing : 0;
		}
		
		/** begin, sets thge fontsizes for all used fonts */
		
		virtual void begin() {
			if (_fontmap[0].lineSpacing < 0) _fontmap[0].lineSpacing = _fontmap[0].fontSize;
			
			for(FontMap::iterator i = _fontmap.begin(); i != _fontmap.end(); i++) {
                if(i->second.font) {
                    i->second.font->setSubPixelAlignment(getSubPixelAlignment());
                    i->second.font->setFontSize(getFontSize(i->first));
                }
			}
			_lineSpacing = _fontmap[0].lineSpacing;
			
			if (getSubPixelAlignment() == false)
				_startPos.set( (int)_startPos[0], (int)_startPos[1], (int)_startPos[2]);
		}
		
		/** advance */
		inline virtual void advance(const unsigned int &left, const unsigned int &right)
		{
			if (isAttributedChar(left, false) == false) {
				StandardTextRenderIterator::advance(left, right);
			}
			
		}
		/** preflight */
		inline virtual void preflight(const unsigned int &left, const unsigned int &right) {
			if (isAttributedChar(left,false) == false)
				StandardTextRenderIterator::preflight(left, right);
		}
    
        inline virtual void prerender(const unsigned int& ch) {
            if (_textPosition == 0) isAttributedChar(0,true); // set the default font
            
			if (isAttributedChar(ch,true) == false) {
				StandardTextRenderIterator::prerender(ch);
			}
        }
    
        inline virtual void render(const unsigned int& ch) {
            if (isAttributedChar(ch,false) == false) {
				StandardTextRenderIterator::render(ch);
			}
        }
		
		/** finishes the rendering */
		virtual void finish() {
			bool fontchanged = true;
			for (FontMap::iterator itr = _fontmap.begin(); itr != _fontmap.end(); itr++) {
			
				TextRendererBase::Container* container = itr->second.container.get();
				setContainer(container);
				setCurrentFont(itr->second.font.get(), itr->second.fontSize, itr->second.lineSpacing);
				_fontChanged = fontchanged;
				StandardTextRenderIterator::finish(); // für jeden container das finish aufrufen 
			
			}
			_fontChanged = false;
		}
		
		virtual void reset() { 
			_position = osg::Vec2(0,0); 
			
			_startPos = osg::Vec3(0,0,0); 
			setMargins(0.0f,0.0f); 
			setSpatio(0.0f); 
			for (FontMap::iterator itr = _fontmap.begin(); itr != _fontmap.end(); itr++) {
			
				TextRendererBase::Container* container = itr->second.container.get();
				setContainer(container);
				resetContainer(); // für jeden container das finish aufrufen 
			
			}
			
			_lineWidths.clear(); 
			_lineWidths.push_back(_leftMargin);
			_currentLine = 0;
			//dumpFontMap();
		}
		
		void dumpFontMap() const {
			for (FontMap::const_iterator itr = _fontmap.begin(); itr != _fontmap.end(); itr++) 
			{
				std::cout << itr->first << ": " << itr->second.font->getName() << " (" << itr->second.fontSize << ")" << std::endl;
			}
		}
		
        const FontMap& getFontMap() const { return _fontmap; }
        
        FontMap& getFontMap() { return _fontmap; }
    
		virtual void clearStateSets() {
            for(typename FontMap::iterator i = _fontmap.begin(); i != _fontmap.end(); ++i) {
                i->second.container->clearStateSets();
            }
        }
        virtual void setStateSet(osg::StateSet* ss)
        {
            for(typename FontMap::iterator i = _fontmap.begin(); i != _fontmap.end(); ++i) {
                i->second.container->setStateSet(ss);
            }
        }
    
	protected:
		virtual ~AttributedTextRenderIterator() {}
		
	private:
		FontMap			_fontmap;
		unsigned int	_lastAttributedChar;
		
};
}

#endif