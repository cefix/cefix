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

#ifndef TEXT_WIDGET_HEADER
#define TEXT_WIDGET_HEADER

#include <cefix/TextGeode.h>
#include <cefix/AbstractWidget.h>
#include <cefix/PropertyList.h>

namespace cefix {

/** a Text-widget, it's only purpose is to show text */
class CEFIX_EXPORT TextWidget : public AbstractWidget, public AbstractWidget::Responder {
	public:
		/** ctor*/
		TextWidget(const std::string& identifier, osg::Vec3 pos, std::string fontname="system.xml", float fontsize = 0) : AbstractWidget(identifier), _pos(pos), _precision(6) {
			_textGeode = new cefix::Utf8TextGeode(fontname, fontsize);
			_textGeode->setPosition(_pos);
		}
		
		/** ctor */
		TextWidget(const std::string& identifier, std::string text, const osg::Vec3& pos, const osg::Vec4& color, std::string fontname="system.xml", float fontsize = 0) : 
			AbstractWidget(identifier), _pos(pos), _precision(6) 
		{
			_textGeode = new cefix::Utf8TextGeode(fontname, fontsize);
			_textGeode->setPosition(_pos);
			_textGeode->setTextColor(color);
			_textGeode->setText(text);
		}
		
		/** ctor for the widgetfactory, see https://wiki.cefix.org/doku.php?id=widgetfactory */
		TextWidget(cefix::PropertyList* pl);
		
		/** get the textgeode holding the text */
		Utf8TextGeode* getTextGeode() { return _textGeode.get(); }
		
		/** get the node */
		virtual osg::Node* getNode() { return getTextGeode(); }
		
		/** implementation of the responder-class, you can add a TextWidget as responder to Sliders for example */
		virtual void respondToAction(const std::string& action, AbstractWidget* widget);
		
		/** update the text */
		virtual void update() {}
		
		/** sets the text position */
		void setPosition(const osg::Vec3& p) { _textGeode->setPosition(p); } 
		
		/** set tge precision when used as a responder for sliders */
		void setPrecision(unsigned int p) { _precision = p; }
		
		/** get the precision */
		unsigned int getPrecision() { return _precision; }
		
		virtual void setCaption(const std::string& s) { _textGeode->setText(s); update(); }
		const std::string& getCaption() const { return _textGeode->getText(); }
	protected:
		~TextWidget();
	private:
		osg::Vec3								_pos;
		osg::ref_ptr< cefix::Utf8TextGeode >	_textGeode;
		unsigned int							_precision;
};

}

#endif