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


#ifndef TEXT_GEODE_HEADER_
#define TEXT_GEODE_HEADER_


#include <osg/observer_ptr>
#include <osg/Geode>
#include <osg/Geometry>
#include <cefix/Font.h>
#include <cefix/FontManager.h>
#include <cefix/TextRenderer.h>
#include <cefix/StandardTextRenderIterator.h>
#include <cefix/Export.h>
#include <cefix/Utf8TextRenderer.h>
#include <cefix/StringUtils.h>


namespace cefix {
    
    
	/** base class for all TextGeodes */
	class CEFIX_EXPORT TextGeodeBase : public osg::Geode {
		public:
			TextGeodeBase() : osg::Geode() {}
		protected:
			virtual ~TextGeodeBase() {}
	};
     
	/** 
     * this template class makes standard text-rendering easy
     * you can set the text, font, fontsize and position and it gets updated automagically
     * there's one caveat: the updatecallback is used by the class internally, so you can't use it for your own purposes
     */
    template<class T, class RendererClass = TextRendererT<StandardTextRenderIterator> >class TextGeode : public TextGeodeBase {
        protected:

            /** updatecallback for this geode, it updates the text if necessary */
            class TextGeodeUpdateCallback : public osg::NodeCallback {
                public:
                    TextGeodeUpdateCallback(TextGeode* textgeode) : _textGeode(textgeode) {}
					
                    virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) {
                        if ((nv) && (_textGeode.valid()) && (node == _textGeode.get())) {
                            _textGeode->refresh();
                        }
						//traverse(node,nv);
                    }
                protected:
					virtual  ~TextGeodeUpdateCallback() {}
                    osg::observer_ptr<TextGeode> _textGeode;
            
            };
    
        public:
            typedef RendererClass TextRenderer;
			typedef typename RendererClass::iterator RenderIterator;
			typedef typename RendererClass::TextAlignment TextAlignment;
            typedef typename RendererClass::Container Container;
			
            typedef T string_type;
            
			TextGeode(const TextGeode<T, RendererClass>& tg, const osg::CopyOp& copyop)
			:	_font(tg._font),
				_fontSize(tg._fontSize),
				_text(tg._text),
				_dirty(true),
				_position(tg._position),
				_textColor(tg._textColor)
			{
				_init();
				setTextColor(_textColor);
				setLineSpacing(tg.getLineSpacing());
				//setSubPixelAlignment(tg.getSubPixelAlignment());
				setTextAlignment(tg.getTextAlignment());
				setRenderPlane(tg.getRenderPlane());
				refresh();
			}

					
			/** constructor 
             * @param font font to use
             * @param fontSize fontSize to use
             */
            TextGeode(Font* font, float fontSize) : 
                TextGeodeBase(), 
                _font(font), 
                _fontSize(fontSize), 
                _position(osg::Vec3(0,0,0))
            {
               _init();
            }
			
			/** constructor  
             * @param fontname name of the font to use (will be loaded if necessary)
             * @param fontSize fontSize to use
             */
            TextGeode(std::string fontname, float fontSize) : 
                TextGeodeBase(), 
                _fontSize(fontSize), 
                _position(osg::Vec3(0,0,0)) 
            {
                _font = FontManager::instance()->loadFont(fontname);
                _init();
            }
			
			virtual osg::Object* cloneType() const { return new TextGeode<T, RendererClass> (); } \
			virtual osg::Object* clone(const osg::CopyOp& copyop) const { return new TextGeode<T, RendererClass>(*this,copyop); } \
			virtual bool isSameKindAs(const osg::Object* obj) const { return dynamic_cast<const TextGeode<T, RendererClass> *>(obj)!=NULL; } \
			virtual const char* libraryName() const { return "osg"; }\
			virtual const char* className() const { return "Geode"; }
        
            /** constructor 
             * @param font font to use
             * @param fontSize fontSize to use
             * @param text text to render
             */
            TextGeode(Font* font, float fontSize, const T& text) : 
                TextGeodeBase(), 
                _font(font), 
                _fontSize(fontSize), 
                _position(osg::Vec3(0,0,0))
            {
               _init();
               setText(text);
                
            }
            
            /** constructor  
             * @param fontname name of the font to use (will be loaded if necessary)
             * @param fontSize fontSize to use
             * @param text text to render
             */
            TextGeode(std::string fontname, float fontSize, const T& text) : 
                TextGeodeBase(), 
                _fontSize(fontSize), 
                _position(osg::Vec3(0,0,0)) 
            {
                _font = FontManager::instance()->loadFont(fontname);
                _init();
                setText(text);
            }
			
			virtual ~TextGeode() {}
            
            
            /** @return returns the current text */
            const T& getText() const { return _text; }
            
            /** sets the text-alignment */
            void setTextAlignment(TextAlignment alignment) {
				if (getRenderIterator()->getTextAlignment() != alignment) {
					getRenderIterator()->setTextAlignment(alignment);
					dirty();
				}
            }
            
            /** @return the text-alignment */
            TextAlignment getTextAlignment() const {
                return getRenderIterator()->getTextAlignment();
            }
			
			/** sets the subpixel-alignment-flag */
			void setSubPixelAlignment(bool flag) {
				getRenderIterator()->setSubPixelAlignment(flag);
			}
			/** @return the subpixel-alignment-flag */
			bool getSubpixelAlignment() const {
				return getRenderIterator()->getSubPixelAlignment();
			}
			
			/** sets the line-spacing */
			void setLineSpacing(float ls) {
				getRenderIterator()->setLineSpacing(ls);
			}
			/** sets the alpha of the textgeode */
			void setAlpha(float alpha) {
				(*_colors)[0][3] = _textColor[3] = alpha;
				_container->dirtyDisplayList();
				
			}
			
			/** @return the line-spacing */
			float getLineSpacing() const {
				return getRenderIterator()->getLineSpacing();
			}
            
            
			/** sets the renderIterator */
			void setRenderIterator(RenderIterator* itr) {  
				if (itr) {
					_renderer->setIterator(itr);
					setFont(_font.get());
					dirty();
				} 
			}
			
			RenderIterator* getRenderIterator() {
				return _renderer->getIterator(); 
			}
			
			const RenderIterator* getRenderIterator() const {
				return _renderer->getIterator(); 
			}
            
            /** sets a new font */
            void setFont(Font* font) { _font = font; getRenderIterator()->setFont(_font.get()); dirty(); }
			
			/** sets a new font from a fontname/file */
			void setFont(std::string fontname) { 
				
				setFont( FontManager::instance()->loadFont(fontname)); 
				
			}
            
            /** @return the current used font */
			Font* getFont() const { return _font.get();  }
            
            /** sets the fontsize*/
			inline void setFontSize(float fontsize) { 
                _fontSize = fontsize;
                if (_font.valid())
                    _font->setFontSize(_fontSize);
                dirty();
            }
			
			inline float getFontSize() const {
				return _fontSize; 
			}
            
            /** @return the start-position of the text */
            osg::Vec3 getPosition() const { return _position; }
            
            /** sets the starting position for the text */
            void setPosition(osg::Vec3 pos) {
                bool is_dirty = (_position != pos);
                _position = pos;
                if (is_dirty) dirty();
            }
        
            /** @return the current text-color */
            osg::Vec4 getTextColor() const { return (*_colors)[0]; }
            
            /** sets the text color */
            void setTextColor(osg::Vec4 c) { 
                if (_colors.valid()) {
					(*_colors)[0] = _textColor = c; 
					_container->dirtyDisplayList();
				}
            }
			
			
            
            /** call this, if you want to refresh the text */
            void dirty() { 
				if (!_dirty) {
					_dirty = true; 
					setUpdateCallback(new TextGeodeUpdateCallback(this)); 
				}
			}
            
            /** set to true, if you want  to use displaylists. If you often update the text, disable display-lists
             * for better performance
             */
            void setSupportsDisplayList(bool flag) { _container->setSupportsDisplayList(flag); }
			
            void setDataVariance(osg::Object::DataVariance dv) { osg::Geode::setDataVariance(dv); _container->updateDataVariance(); }
			
			/** sets the text to render */
            void setText(const T& text) {
                if ((_font.valid()) && ((_text != text) || _dirty || getRenderIterator()->isDirty())) {
					_text = text;
                    RenderIterator* itr = getRenderIterator();
                    itr->setDirty(false);
                    setFontSize(_fontSize);
                    
                    itr->reset();
                    itr->setStartPosition(_position);
                    _renderer->render(_text, _container.get(), itr);   
                    _dirty = false;
                    setUpdateCallback(0);
                }
            }
			
			/** returns the container holding the geometry */
			Container* getContainer() { return _container.get(); }
            
            /** refreshes the rendered representation if needed */
            inline void refresh() {
                setText(_text);
            }
            /** sets the renderplane */
            inline void setRenderPlane(FontCharData::RenderPlane plane) { getRenderIterator()->setRenderPlane(plane); dirty(); }
            
			/** @return the renderplane */
            inline FontCharData::RenderPlane getRenderPlane() const { return getRenderIterator()->getRenderPlane(); }
			
			inline float getXWidth() {
				return (_text.empty()) ? 0 : fabs(getBoundingBox().xMax() - getBoundingBox().xMin());
			}
			
			inline float getYWidth() {
				return (_text.empty()) ? 0 : fabs(getBoundingBox().yMax() - getBoundingBox().yMin());
			}
			
			inline float getZWidth() {
				return (_text.empty()) ? 0 : fabs(getBoundingBox().zMax() - getBoundingBox().zMin());
			}
			
			inline float getWidth() {
				
				if (_dirty) refresh();
				
				switch (getRenderPlane()) {
				
					case FontCharData::NEGATIVE_YZ_PLANE:
                    case FontCharData::YZ_PLANE:
						return getYWidth();
                    
                    default:
						return getXWidth();

				}
				return 0;
			}
			
			inline float getHeight() {
				if (_dirty) refresh();
				
				switch (getRenderPlane()) 
                {
                	case FontCharData::NEGATIVE_XY_PLANE:
                    case FontCharData::XY_PLANE:
						return getYWidth();
                    default:
                        return getZWidth();
				}
				return 0;
			}
			
			int getRenderedLineCount() { return getRenderIterator()->getRenderedLineCount(); }
		
                
        protected:
			TextGeode() : TextGeodeBase() {}
            
            void _init() {
                _colors = new osg::Vec4Array();
                _colors->push_back(osg::Vec4(0,0,0,1));
                _renderer = new RendererClass();
                _container = new typename RendererClass::Container();
				_container->setName("TextGeodeGeometry");
                _container->setColorArray(_colors.get());
                _container->setColorBinding(osg::Geometry::BIND_OVERALL);
                setRenderIterator(new typename RendererClass::iterator());
                _container->attachTo(this);
                setFont(_font.get());
                setFontSize(_fontSize);
            }

            
            inline void setRenderer(RendererClass* renderer) { _renderer = renderer; }
            
            osg::ref_ptr<Font>              _font;
            float                           _fontSize;
            T                               _text;
            bool                            _dirty;
            osg::Vec3                       _position;
            osg::ref_ptr<osg::Vec4Array>    _colors;
            osg::ref_ptr<typename RendererClass::Container>     _container;
            osg::ref_ptr<RendererClass>      _renderer; 
            osg::ref_ptr<typename RendererClass::iterator>		_itr;
            osg::Vec4                       _textColor;
            
    };
	typedef TextRendererT<StandardTextRenderIterator> TextRenderer;
	typedef Utf8TextRendererT<StandardTextRenderIterator> Utf8TextRenderer;
	typedef cefix::TextGeode<std::string, TextRenderer >			AsciiTextGeode;
	typedef cefix::TextGeode<std::wstring, TextRenderer  >			UnicodeTextGeode;
	typedef cefix::TextGeode<std::string, Utf8TextRenderer  >		Utf8TextGeode;
	
}

template <class TextRenderer> 
std::ostream& operator << (std::ostream& os, cefix::TextGeode<std::string, TextRenderer>& tg) {
	os << tg.getText();
	return os;
}

template <class TextRenderer> 
std::ostream& operator << (std::ostream& os, cefix::TextGeode<std::wstring, TextRenderer>& tg) {
	os << tg.getText();
	return os;
}
#endif