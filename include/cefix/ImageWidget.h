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

#ifndef IMAGE_WIDGET_HEADER
#define IMAGE_WIDGET_HEADER


#include <cefix/AbstractWidget.h>
#include <cefix/Quad2DGeometry.h>
#include <osg/Geode>

namespace cefix {

class PropertyList;

class CEFIX_EXPORT ImageWidget : public cefix::RectControlWidget {

	public:
		
		typedef cefix::AbstractWidget::Implementation<ImageWidget> Implementation;
		
		enum ZoomMode { MANUAL, AUTO_PROPORTIONAL, AUTO};
		
		ImageWidget(const std::string& identifier, const osg::Vec3& position, const osg::Vec2& size, osg::Image* image, ZoomMode zoommode = AUTO);
		
		ImageWidget(cefix::PropertyList* pl);
		
		osg::Image* getImage() { return _image.get(); }
		
		void setImage(osg::Image* image) { _image = image; update(); }
		
		void setZoomMode(ZoomMode mode) { _zoomMode = mode; update(); }
		ZoomMode getZoomMode() { return _zoomMode; }
		
		virtual void update() {
			if (_impl.valid()) _impl->update();
		}
		
		virtual osg::Node* getNode() { return (_impl.valid()) ? _impl->getNode() : NULL; }
		
		void setScrollDelta(const osg::Vec2& delta) { _scrollDelta = delta; update(); }
		void setScrollX(float dx) { setScrollDelta(osg::Vec2(dx, _scrollDelta[1])); }
		void setScrollY(float dy) { setScrollDelta(osg::Vec2(_scrollDelta[0], dy)); }
		const osg::Vec2& getScrollDelta() { return _scrollDelta; }
		
		void setZoomLevel(float level) {_zoomLevel = level; update(); }
		float getZoomLevel() { return _zoomLevel; }
		
		void setHorizontalFlip(bool f) { _horizontalFlip = f; }
		bool getHorizontalFlip() { return _horizontalFlip; }
		
		
	protected:
		void setImplementation(Implementation* impl) { _impl = impl; }
		
		virtual ~ImageWidget() {
			if (_impl.valid()) _impl->clear();
		}
		
		osg::ref_ptr<Implementation>	_impl;
		osg::ref_ptr<osg::Image>		_image;
		ZoomMode						_zoomMode;
		float							_zoomLevel;
		osg::Vec2						_scrollDelta;
		bool							_horizontalFlip;
		
};

class ImageWidgetZoomResponder : public cefix::AbstractWidget::Responder {
	public:
		ImageWidgetZoomResponder() : cefix::AbstractWidget::Responder(), _imageWidget(NULL) {};
		
		virtual void addedToWidget(AbstractWidget* widget) 
		{
			ImageWidget* imageWidget = dynamic_cast<ImageWidget*>(widget);
			if (imageWidget && !_imageWidget.valid()) _imageWidget = imageWidget;
		}
		
		virtual void respondToAction (const std::string &action, AbstractWidget *widget);
	protected:
		osg::observer_ptr<ImageWidget>	_imageWidget;
};



class SimpleImageWidgetDrawImplementation : public ImageWidget::Implementation {
	public:
		SimpleImageWidgetDrawImplementation(ImageWidget* widget);
		
		virtual void update();
		virtual osg::Node* getNode() { return _geode.get(); }
		virtual void applyPropertyList(cefix::PropertyList* pl);
	private:
		osg::ref_ptr<osg::Geode> _geode;
		osg::ref_ptr<cefix::Quad2DGeometry> _quad;
		osg::Image*							_cachedImage;
};


template <class DrawImplementationClass = SimpleImageWidgetDrawImplementation> class ImageWidgetT : public ImageWidget {
	public:
		ImageWidgetT(const std::string& identifier, const osg::Vec3& position, const osg::Vec2& size, osg::Image* image, ZoomMode zoommode = AUTO) :
			ImageWidget(identifier, position, size, image, zoommode)
		{
			setImplementation(new DrawImplementationClass(this));
		}
		
		ImageWidgetT(cefix::PropertyList*pl) :
			ImageWidget(pl)
		{
			setImplementation(new DrawImplementationClass(this));
			getImplementation()->applyPropertyList(pl);
		}
		
		DrawImplementationClass* getImplementation() { return dynamic_cast<DrawImplementationClass*>(_impl.get()); }
		
};

typedef ImageWidgetT<> SimpleImageWidget;

}


#endif
