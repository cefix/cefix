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

#ifndef ATTRIBUTED_TEXT_GEODE_HEADER
#define ATTRIBUTED_TEXT_GEODE_HEADER

#include <cefix/TextGeode.h>
#include <cefix/FontManager.h>
#include <cefix/AttributedTextRenderIterator.h>
#include <cefix/Utf8TextRenderer.h>

namespace cefix {

template<class T, class RendererClass = TextRendererT<AttributedTextRenderIterator> > class AttributedTextGeode : public TextGeode<T, RendererClass> {

	public:
		typedef TextGeode<T, RendererClass > BaseClass;
        typedef RendererClass TextRenderer;
    
        typedef typename RendererClass::iterator RenderIterator;
        typedef typename RendererClass::TextAlignment TextAlignment;
        typedef typename RendererClass::Container Container;
        typedef T string_type;
    
    
			/** constructor
             * @param font font to use
             * @param fontSize fontSize to use
             */
            AttributedTextGeode(Font* font, float fontSize) : 
                BaseClass()
            {
               init(font, fontSize);
            }
			
			/** constructor  
             * @param fontname name of the font to use (will be loaded if necessary)
             * @param fontSize fontSize to use
             */
            AttributedTextGeode(std::string fontname, float fontSize) : 
                BaseClass() 
            {
                init(FontManager::instance()->loadFont(fontname), fontSize);
            }
        
            /** constructor 
             * @param font font to use
             * @param fontSize fontSize to use
             * @param text text to render
             */
            AttributedTextGeode(Font* font, float fontSize, const T& text) : 
                BaseClass()
            {
               init(font, fontSize);
               setText(text);
                
            }
            
            /** constructor  
             * @param fontname name of the font to use (will be loaded if necessary)
             * @param fontSize fontSize to use
             * @param text text to render
             */
            AttributedTextGeode(std::string fontname, float fontSize, const T& text) : 
                BaseClass() 
            {
				init(FontManager::instance()->loadFont(fontname), fontSize);
                this->setText(text);
            }
			
						
			/** ctor */
			AttributedTextGeode(AttributedTextRenderIterator* itr) {
				BaseClass::setRenderer( new TextRenderer() );
				BaseClass::setRenderIterator(itr);
			}
			AttributedTextGeode() : BaseClass() 
            {
				init(NULL, 0.0f);
            }
	
			/// scheiss komplizierte copy-ctor
			AttributedTextGeode(const AttributedTextGeode<T>& tg, const osg::CopyOp& copyop)
			{
				BaseClass::setRenderer(new TextRenderer());
				BaseClass::setRenderIterator(new AttributedTextRenderIterator());
				for(AttributedTextRenderIterator::FontMap::const_iterator i = tg.getRenderIterator()->getFontMap().begin(); i != tg.getRenderIterator()->getFontMap().end(); ++i) 
				{
					// std::cout << i->second.font.get() << "/" << i->second.fontSize << "/" << i->second.lineSpacing << std::endl;
					
					setFont(i->second.font.get(), i->first);
					setFontSize(i->second.fontSize, i->first);
					setLineSpacing(i->second.lineSpacing, i->first);
					setTextColor(tg.getTextColor(i->first), i->first);
				}
				
				BaseClass::setFont(getFont(0));
				BaseClass::setFontSize(getFontSize(0));
				BaseClass::setPosition(tg.getPosition());
				BaseClass::setLineSpacing(tg.getLineSpacing(0));
				// BaseClass::setSubPixelAlignment(tg.getSubPixelAlignment());
				BaseClass::setTextAlignment(tg.getTextAlignment());
				BaseClass::setRenderPlane(tg.getRenderPlane());
				
				this->setText(tg.getText());
				this->setPosition(tg.getPosition());
			}
			
			virtual osg::Object* clone(const osg::CopyOp& copyop) const { return new AttributedTextGeode<T>(*this,copyop); } 
			virtual bool isSameKindAs(const osg::Object* obj) const { return dynamic_cast<const AttributedTextGeode<T> *>(obj)!=NULL; } 
			virtual const char* libraryName() const { return "osg"; }
			virtual const char* className() const { return "Geode"; }

			
			/** reimplement TextGeode's setFont */
			void setFont(Font* font) {
				BaseClass::getRenderIterator()->registerFont(0, font, newContainer());
				BaseClass::setFont(font);
			}
			
			inline void setFont(std::string fontname) {
				setFont(FontManager::instance()->loadFont(fontname));
			}
			
			/** sets the font for a specific attribute */
			void setFont(Font* font, unsigned int attribute) {
				BaseClass::getRenderIterator()->registerFont(attribute, font, newContainer());
				BaseClass::dirty();
			}
			
			/** sets the font for a specific attribute */
			void setFont(std::string fontname, unsigned int attribute) {
				if (BaseClass::getRenderIterator()->hasAttribute(attribute))
					BaseClass::getRenderIterator()->replaceFont(attribute, fontname);
				else		
					BaseClass::getRenderIterator()->registerFont(attribute, fontname, newContainer());
				
				BaseClass::dirty();
			}
			
			Font* getFont() const { 
				return getFont(0);
			}

			
			/** @return the font associated with a specific attribute */
			Font* getFont(unsigned attr) const {
				return BaseClass::getRenderIterator()->getFont(attr);
			}
			
			void setTextColor(const osg::Vec4& color) {
				setTextColor(color, -1);
			}
			
			/** sets the textcolor, if attribute = -1 then for all attributes */
			void setTextColor(const osg::Vec4& color, int attribute) {
				 BaseClass::getRenderIterator()->setTextColor(color,attribute);
				 BaseClass::dirty();
			}
			
			/** get the textcolor */
			osg::Vec4 getTextColor(int attribute = 0) const {
				return BaseClass::getRenderIterator()->getTextColor(attribute);
			}
			
			/** set the alpha */
			void setAlpha(float f) 
			{
				for(AttributedTextRenderIterator::FontMap::const_iterator i = this->getRenderIterator()->getFontMap().begin(); i != this->getRenderIterator()->getFontMap().end(); ++i) {
					osg::Vec4 c = this->getTextColor(i->first);
					c[3] = f;
					setTextColor(c, i->first);
				}
				BaseClass::dirty();
			}
			
			/** @return the container for a specific attribute */
			Container* getContainer(int attribute = 0) {
				return BaseClass::getRenderIterator()->getContainer(attribute);
			}
			
			void setFontSize(float fs) {
				setFontSize(fs,0);
			}
			
			void setLineSpacing(float fs) {
				setLineSpacing(fs,0);
			}
			float getLineSpacing() const {
				return getLineSpacing(0);
			}
			
			float getLineSpacing(int attribute) const {
				return BaseClass::getRenderIterator()->getLineSpacing(attribute);
			}
			
			void setLineSpacing(float fs, unsigned int attribute) {
				BaseClass::getRenderIterator()->setLineSpacing(fs, attribute);
				BaseClass::dirty();
			}
			
			void setFontSize(float fs, unsigned int attribute) {
				BaseClass::getRenderIterator()->setFontSize(fs, attribute);
				BaseClass::dirty();
			}
			
			float getFontSize() const {
				return getFontSize(0);
			}
			
			float getFontSize(unsigned int attribute) const {
				return BaseClass::getRenderIterator()->getFontSize(attribute);
			}
			
			void setSupportsDisplayList(bool flag) {
				for(unsigned int i=0; i < osg::Geode::getNumDrawables(); i++) {
					osg::Geode::getDrawable(i)->setSupportsDisplayList(flag);
				}
			 }
    
            /** sets the renderIterator */
            void setRenderIterator(RenderIterator* itr) {
                if(BaseClass::getRenderIterator()) {
                    typename RenderIterator::FontMap& font_map = BaseClass::getRenderIterator()->getFontMap();
                    for(typename RenderIterator::FontMap::iterator i = font_map.begin();  i != font_map.end(); ++i) {
                        if(i->second.container)
                            i->second.container->detachFrom(this);
                    }
                }
                BaseClass::setRenderIterator(itr);
                {
                    typename RenderIterator::FontMap& font_map = BaseClass::getRenderIterator()->getFontMap();
                    for(typename RenderIterator::FontMap::iterator i = font_map.begin();  i != font_map.end(); ++i) {
                        i->second.container->attachTo(this);
                    }
                }
            }

			
		protected:
			virtual ~AttributedTextGeode() {}
			
		private:
			/** private init */
			void init(Font* font, float fontSize) {
				BaseClass::setRenderer(new TextRenderer());
				BaseClass::setRenderIterator(new AttributedTextRenderIterator());
				if (font) {
					setFont(font);
					setFontSize(fontSize);
				}
			}
				
				
			Container* newContainer() {
			
				osg::Vec4Array* colors = new osg::Vec4Array();
				osg::Vec4 c = getTextColor(0);
				colors->push_back(c);
				
				osg::ref_ptr<Container> container = new Container();
				container->setName("AttributedTextGeodeGeometry");
				container->setColorArray(colors);
				container->setColorBinding(osg::Geometry::BIND_OVERALL);
                
                container->attachTo(this);
                return container.release();
			}
    
    };
    
    typedef TextRendererT<AttributedTextRenderIterator>       AttributedRendererClass;
	typedef Utf8TextRendererT<AttributedTextRenderIterator> AttributedUtf8RendererClass;
	typedef cefix::AttributedTextGeode<std::string, AttributedRendererClass >			AttributedAsciiTextGeode;
	typedef cefix::AttributedTextGeode<std::wstring, AttributedRendererClass  >			AttributedUnicodeTextGeode;
	typedef cefix::AttributedTextGeode<std::string, AttributedUtf8RendererClass  >       AttributedUtf8TextGeode;

}

#endif