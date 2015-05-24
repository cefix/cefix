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

#ifndef SEQUENCE_GRABBER_WIDGET_HEADER
#define SEQUENCE_GRABBER_WIDGET_HEADER

#include <cefix/TransformWidget.h>
#include <cefix/SequenceGrabber.h>
#include <cefix/Timer.h>
#include <cefix/RectangleSelectionWidget.h>

namespace cefix {
class ImageWidget;
class SequenceGrabberWidget : public cefix::TransformWidget, public cefix::AbstractWidget::Responder 
{

public:
	
	class Actions : public AbstractWidget::Actions {
	public:
		static const char* selectionChanged() { return "selectionchanged"; }
		static const char* sequenceGrabberChanged() { return "sequencegrabberchanged"; }
	};
	
	class Responder : public virtual AbstractWidget::Responder {
			
	public:
		Responder() : AbstractWidget::Responder() {}
		
		virtual void selectionChanged() {}
		
		virtual void respondToAction(const std::string& action, AbstractWidget* w)
		{
			_widget = dynamic_cast<SequenceGrabberWidget*>(w);
			if (_widget) {
				if (action == Actions::selectionChanged() )
					selectionChanged();
			}
		}
	protected:
		SequenceGrabberWidget* getWidget() { return _widget; }
	private:
		SequenceGrabberWidget* _widget; 
	};

	/// ctor
	SequenceGrabberWidget(const std::string& identifer, const osg::Vec3& position, const osg::Vec2& previewsize);
	/// ctor
	SequenceGrabberWidget(cefix::PropertyList* pl);
	
	
	virtual void respondToAction (const std::string &action, AbstractWidget *widget);
	
	/// set the videoseparator
	void setSequenceGrabber(cefix::SequenceGrabber* sg, bool autoUpdate = false) { 
		_sg = sg; updateValues(); 
		_autoDiscoverDevice = false;
		if (autoUpdate) {
			cefix::Timer* t = cefix::FunctorTimer::create(5000, this, &SequenceGrabberWidget::updateValues);
			t->start();
		}
		informAttachedResponder(Actions::sequenceGrabberChanged());
	}
	
	/// update all values from values of the videoseparator
	void updateValues();
	
	RectangleSelectionWidget* getRectangleSelectionWidget() { return _rectangleSelectionWidget.get(); }
	
	osg::Vec2Array* getSelection() { 
		return _rectangleSelectionWidget->getSelection(); 
	}
	
	osg::Vec2Array* getNormalizedSelection() {
		return _rectangleSelectionWidget->getNormalizedSelection();
	}
	
	void startAutoDiscover() 
	{
		cefix::Timer* t = cefix::FunctorTimer::create(500, this, &SequenceGrabberWidget::checkForAvailableSequenceGrabber, 1);
		t->start();
	}
	
	cefix::SequenceGrabber* getSequenceGrabber() const { return _sg.get(); }
	
	void setPreviewImage(osg::Image* img) { _previewImage = img; updateValues(); }
	
protected:
	void checkForAvailableSequenceGrabber();
	void createWidgets(const osg::Vec2& size, cefix::PropertyList* prefs_pl = NULL);

private:
	osg::ref_ptr<cefix::SequenceGrabber>	_sg;
	osg::ref_ptr<RectangleSelectionWidget>  _rectangleSelectionWidget;
	osg::ref_ptr<osg::Image>				_previewImage;
	bool	_ignoreValuesSet, _autoDiscoverDevice;
	osg::Vec2 _size;
};





}

#endif