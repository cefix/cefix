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

#ifndef CEFIX_BOX_WIDGET_HEADER
#define CEFIX_BOX_WIDGET_HEADER

#include <cefix/TransformWidget.h>
#include <cefix/AbstractWidget.h>
#include <cefix/TextWidget.h>


namespace cefix {
class PropertyList;
class Quad2DGeometry;

class BoxWidget : public RectControlWidgetT<TransformWidget> {
public:

	/** the base class for the box-implementation, it should create a geometry and handle the positioning of the box */
	class Implementation : public AbstractWidget::Implementation<BoxWidget> {
	
		public:
			/** ctor*/
			Implementation(BoxWidget* widget) : AbstractWidget::Implementation<BoxWidget>(widget) {}
			
	};

	BoxWidget(const std::string& identifier, const osg::Vec3& position, const osg::Vec2& size, const std::string& caption);
	
	BoxWidget(cefix::PropertyList* pl);
	
	Implementation* getImplementation() const { return _impl.get(); }
	
	void setCaption(const std::string caption) { _caption->setCaption(caption); update(); }
	
	const std::string& getCaption() const { return _caption->getCaption(); }
	
	virtual void update() 
	{
		_caption->setPosition(_captionDelta + osg::Vec3(0,getHeight(), 0));
		RectControlWidgetT<TransformWidget>::update();
	}
	
	TextWidget* getCaptionWidget() { return _caption.get(); }
	void setCaptionDelta(const osg::Vec3& d) { _captionDelta = d; update(); }
protected:
	void setImplementation(Implementation* impl) { _impl = impl; }
	
private:
	void createCaption( const std::string& caption, cefix::PropertyList* pl = NULL);
	
	osg::ref_ptr<Implementation>	_impl;
	osg::ref_ptr<TextWidget>		_caption;
	osg::Vec3						_captionDelta;
friend class Implemetation;

};


class SimpleBoxDrawImplementation : public BoxWidget::Implementation {

public:
	SimpleBoxDrawImplementation(BoxWidget* widget) : BoxWidget::Implementation(widget) {}
	
	virtual void applyPropertyList(cefix::PropertyList* pl);
	virtual void update();
	
	void createGeometry();
	osg::Node* getNode() { return _node.get(); }
	
private:
	osg::ref_ptr<osg::Geode> _node;
	cefix::Quad2DGeometry*	 _geo;
};

template <class Impl>
class BoxWidgetT : public BoxWidget {

public:
	BoxWidgetT(const std::string identifier, const osg::Vec3& position, const osg::Vec2& size, const std::string& caption) 
	:	BoxWidget(identifier, position, size, caption)
	{
		Impl* impl = new Impl(this);
		setImplementation(impl);
		impl->createGeometry();
		getGroupNode()->addChild(impl->getNode());
	}
	
	BoxWidgetT(cefix::PropertyList* pl) 
	:	BoxWidget(pl)
	{
		Impl* impl = new Impl(this);
		setImplementation(impl);
		impl->createGeometry();
		impl->applyPropertyList(pl);
		
		getGroupNode()->addChild(impl->getNode());
	}
	
	virtual void update() { BoxWidget::update(); getImplementation()->update(); }

};


typedef BoxWidgetT<SimpleBoxDrawImplementation> SimpleBoxWidget;

}

#endif