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

#ifndef CEFIX_PROGRESS_BAR_WIDGET_HEADER
#define CEFIX_PROGRESS_BAR_WIDGET_HEADER
#include <osg/Geode>
#include <cefix/AbstractWidget.h>
#include <cefix/PropertyList.h>
#include <cefix/Quad2DGeometry.h>
namespace cefix {


class ProgressBarWidget : public RectControlWidget {

public:
	class Actions : public AbstractWidget::Actions
	{
		public:
			static const char* valueChanged() { return "valuechanged"; } 
	};
	
	typedef cefix::AbstractWidget::Implementation<ProgressBarWidget> Implementation;
	
	ProgressBarWidget(const std::string& id, const osg::Vec3& position, const osg::Vec2& size) : RectControlWidget(id, position, size), _max(-1), _current(0), _impl(NULL) {}
	
	inline void setMaximum(float max) { _max = max; update(); }
	inline float getMaximum() { return _max; }
	
	inline void setValue(float c) { if (c == _current) return; _current = c; update(); informAttachedResponder(Actions::valueChanged()); }
	inline float getValue() { return _current; }
    inline float getPercentDone() { return (_max < 0) ? -1 : _current / _max * 100; }
	virtual void update() { _impl->update(); }
	virtual osg::Node* getNode() { return _impl->getNode(); }
	
protected:
	
	void setImplementation(Implementation* impl) { _impl = impl; }
	
	float _max, _current;
	osg::ref_ptr<Implementation> _impl;

};


class SimpleProgressBarWidgetImplementation : public ProgressBarWidget::Implementation {
public:
	SimpleProgressBarWidgetImplementation(ProgressBarWidget* widget);
	virtual osg::Node* getNode() { return _geode.get(); }
	virtual void update();
	virtual void applyPropertyList(cefix::PropertyList* pl);
	
	void setForegroundColor(const osg::Vec4& c) { _front->setColor(c); _forecolor = c; }
	void setBackgroundColor(const osg::Vec4& c) { _back->setColor(c); _backcolor = c; }
	void setDisabledColor(const osg::Vec4& c) { _disabledcolor = c; }

protected:
	osg::ref_ptr<osg::Geode> _geode;
	osg::ref_ptr<cefix::Quad2DGeometry>	_back, _front;
	osg::Vec4 _backcolor, _forecolor, _disabledcolor;
	
friend class InfiniteProgressbarCallback;
};

template <class DrawImplementationClass = SimpleProgressBarWidgetImplementation> class ProgressBarWidgetT : public ProgressBarWidget {
	public:
		ProgressBarWidgetT(const std::string& identifier, const osg::Vec3& position, const osg::Vec2& size) :
			ProgressBarWidget(identifier, position, size)
		{
			setImplementation(new DrawImplementationClass(this));
		}
		
		ProgressBarWidgetT(cefix::PropertyList*pl) :
			ProgressBarWidget(
				pl->get("id")->asString(),
				pl->get("position")->asVec3(),
				pl->get("size")->asVec2())
		{
			setImplementation(new DrawImplementationClass(this));
			getImplementation()->applyPropertyList(pl);
		}
		
		DrawImplementationClass* getImplementation() { return dynamic_cast<DrawImplementationClass*>(_impl.get()); }
	
};

typedef ProgressBarWidgetT<> SimpleProgressBarWidget;




} // eon


#endif