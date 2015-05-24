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


#ifndef TABBED_TEXT_GEODE_HEADER_
#define TABBED_TEXT_GEODE_HEADER_

#include <cefix/AttributedTextGeode.h>
#include "TabbedTextRenderIterator.h"

namespace cefix {

template<class T>class TabbedTextGeode : public cefix::AttributedTextGeode<T> {

	public:
        typedef T string_type;
		
        /** constructor
		 * @param font font to use
		 * @param fontSize fontSize to use
		 * @param text text to render
		 */
		TabbedTextGeode(cefix::Font* font, float fontSize, const T& text) : cefix::AttributedTextGeode<T>(new TabbedTextRenderIterator())
		{
			cefix::AttributedTextGeode<T>::setName("TabbedTextGeode");
			cefix::AttributedTextGeode<T>::setFont(font);
			cefix::AttributedTextGeode<T>::setFontSize(fontSize);
			
		}
		
		/** constructor  
		 * @param fontname name of the font to use (will be loaded if necessary)
		 * @param fontSize fontSize to use
		 * @param text text to render
		 */
		TabbedTextGeode(std::string fontname, float fontSize, const T& text) : cefix::AttributedTextGeode<T>(new TabbedTextRenderIterator()) 
		
		{
			cefix::AttributedTextGeode<T>::setName("TabbedTextGeode");
			cefix::AttributedTextGeode<T>::setFont(fontname);
		    cefix::AttributedTextGeode<T>::setFontSize(fontSize);
		}
		
		/** constructor 
		 * @param font font to use
		 * @param fontSize fontSize to use
		 * @param text text to render
		 */
		TabbedTextGeode(cefix::Font* font, float fontSize) : cefix::AttributedTextGeode<T>(new TabbedTextRenderIterator())
		{
		   cefix::AttributedTextGeode<T>::setName("TabbedTextGeode");
		   cefix::AttributedTextGeode<T>::setFont(font);
		   cefix::AttributedTextGeode<T>::setFontSize(fontSize);
		}
		
		/** constructor  
		 * @param fontname name of the font to use (will be loaded if necessary)
		 * @param fontSize fontSize to use
		 * @param text text to render
		 */
		TabbedTextGeode(std::string fontname, float fontSize) : cefix::AttributedTextGeode<T>(new TabbedTextRenderIterator()) 		
		{
			cefix::AttributedTextGeode<T>::setName("TabbedTextGeode");
			cefix::AttributedTextGeode<T>::setFont(fontname);
		    cefix::AttributedTextGeode<T>::setFontSize(fontSize);
		}
		
		// copy c'tor
		TabbedTextGeode(const TabbedTextGeode<T>& tg, const osg::CopyOp& copyop)
		{
			cefix::AttributedTextGeode<T>::setRenderer( new TextRendererT<AttributedTextRenderIterator>() );
			cefix::AttributedTextGeode<T>::setRenderIterator(new TabbedTextRenderIterator(*tg.getTabbedTextIterator()));
			cefix::AttributedTextGeode<T>::setName("TabbedTextGeode");
			
			cefix::AttributedTextGeode<T>::setFont(tg.getFont());
			cefix::AttributedTextGeode<T>::setFontSize(tg.getFontSize());
			
			for(AttributedTextRenderIterator::FontMap::const_iterator i = tg.getRenderIterator()->getFontMap().begin(); i != tg.getRenderIterator()->getFontMap().end(); ++i) {
				cefix::AttributedTextGeode<T>::setFont(i->second.font.get(), i->first);
				cefix::AttributedTextGeode<T>::setFontSize(i->second.fontSize, i->first);
				cefix::AttributedTextGeode<T>::setTextColor(tg.getTextColor(i->first), i->first);
				cefix::AttributedTextGeode<T>::setLineSpacing(i->second.lineSpacing, i->first);
			}
			
			cefix::AttributedTextGeode<T>::setFont(tg.getFont());
			cefix::AttributedTextGeode<T>::setFontSize(tg.getFontSize());
			cefix::AttributedTextGeode<T>::setLineSpacing(tg.getLineSpacing());
			cefix::AttributedTextGeode<T>::setPosition(tg.getPosition());
			// BaseClass::setSubPixelAlignment(tg.getSubPixelAlignment());
			cefix::AttributedTextGeode<T>::setTextAlignment(tg.getTextAlignment());
			cefix::AttributedTextGeode<T>::setRenderPlane(tg.getRenderPlane());
			
			this->setText(tg.getText());
			this->setPosition(tg.getPosition());
			//getTabbedTextIterator()->dumpFontMap();
		}
		
		virtual osg::Object* clone(const osg::CopyOp& copyop) const { return new TabbedTextGeode<T>(*this,copyop); } \
		virtual bool isSameKindAs(const osg::Object* obj) const { return dynamic_cast<const TabbedTextGeode<T>* >(obj)!=NULL; } \
		virtual const char* libraryName() const { return "osg"; }\
		virtual const char* className() const { return "Geode"; }

		
		void setTabAt(float pos, TabbedTextRenderIterator::TabAlignment a) {
			getTabbedTextIterator()->setTabAt(pos, a);
			cefix::AttributedTextGeode<T>::dirty();
		}
		
		void clearTabs() { 
			getTabbedTextIterator()->clearTabs();
		}
        
	protected:
		virtual ~TabbedTextGeode() {}

	public:
		TabbedTextRenderIterator* getTabbedTextIterator() { return dynamic_cast<TabbedTextRenderIterator*>(cefix::AttributedTextGeode<T>::getRenderIterator()); }
		const TabbedTextRenderIterator* getTabbedTextIterator() const { return dynamic_cast<const TabbedTextRenderIterator*>(cefix::AttributedTextGeode<T>::getRenderIterator()); }

};

typedef cefix::TabbedTextGeode<std::string>		AsciiTabbedTextGeode;
typedef cefix::TabbedTextGeode<std::wstring>	UnicodeTabbedTextGeode;

}


#endif
