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

#ifndef RECTANGLE_SELECTION_WIDGET_HEADER
#define RECTANGLE_SELECTION_WIDGET_HEADER

#include <cefix/TransformWidget.h>
#include <cefix/Quad2DGeometry.h>
#include <cefix/Draggable.h>
#include <cefix/LineStripGeometry.h>


namespace cefix {

class PropertyList;
class RectangleSelectionWidget : public TransformWidget {

public:
	RectangleSelectionWidget(const std::string& identifier, const osg::Vec3& position, const osg::Vec4& rect, const osg::Vec4& constrainingRect, bool independentCorners = false);
	RectangleSelectionWidget(const std::string& identifier, const osg::Vec3& position, osg::Vec2Array* points, const osg::Vec4& constrainingRect, bool independentCorners = false);
	
	RectangleSelectionWidget(PropertyList* pl);
	
	const osg::Vec2& getTopLeft() { return (*_points)[0]; }
	const osg::Vec2& getTopRight() { return (*_points)[1]; }
	const osg::Vec2& getBottomRight() { return (*_points)[2]; }
	const osg::Vec2& getBottomLeft() { return (*_points)[3]; }
	
	osg::Vec4 getRect() { 
		osg::Vec4 r((*_points)[0][0], (*_points)[0][1], (*_points)[2][0], (*_points)[2][1] );
		
		if (r[3] < r[1]) std::swap(r[1], r[3]);
		if (r[2] < r[0]) std::swap(r[0], r[2]);
		
		return r;
	}
    
    void setRect(const osg::Vec4& rect) 
    {
        (*_points)[0].set(rect[0], rect[1]);
        (*_points)[1].set(rect[2], rect[1]);
        (*_points)[2].set(rect[2], rect[3]);
        (*_points)[3].set(rect[0], rect[3]);
        update();
    }
    
	osg::Vec4 getNormalizedRect() { 
		osg::Vec4 r((*_points)[0][0] / _referenceSize[0], (*_points)[0][1] / _referenceSize[1], (*_points)[2][0] / _referenceSize[0], (*_points)[2][1] / _referenceSize[1]); 
		
		if (r[3] < r[1]) std::swap(r[1], r[3]);
		if (r[2] < r[0]) std::swap(r[0], r[2]);
		
		return r;		
	}
	
	void setRectIsDraggable(bool f) { _rectIsDraggable = f; }
	bool isRectDraggable() const { return _rectIsDraggable; }
	
	/** Actions encapsulating all actions of this widget */
	class Actions : public AbstractWidget::Actions {
	public:
		static const char* selectionChanged() { return "selectionchanged"; }
	private:
		Actions() : AbstractWidget::Actions() {}
	};
	
	/** simple helper class, calls selectionChanged when notified by the widget */
	class Responder : public virtual AbstractWidget::Responder {
			
	public:
		Responder() : AbstractWidget::Responder() {}
		
		virtual void selectionChanged() {}
		
		virtual void respondToAction(const std::string& action, AbstractWidget* w)
		{
			_widget = dynamic_cast<RectangleSelectionWidget*>(w);
			if (_widget) {
				if (action == Actions::selectionChanged() )
					selectionChanged();
			}
		}
	protected:
		RectangleSelectionWidget* getWidget() { return _widget; }
	private:
		RectangleSelectionWidget* _widget; 
	};
	
	/** base class for the implementation */
	class Implementation : public osg::Referenced {
	public:
		/** helper class for the draggable corners, the implementation is agnostic to the used drawing-implementation */
		class CornerDraggable : public cefix::Draggable, public osg::Referenced {
		public:
			CornerDraggable(RectangleSelectionWidget* widget, unsigned int ndx);	
			virtual void down() {
				stopPropagation();
				Draggable::down();
			}
			virtual bool dragStarted ();
			virtual bool drag(const osg::Vec3 &delta);

		private:
			RectangleSelectionWidget*	_widget;
			unsigned int				_ndx;
			osg::Vec2					_p;
		};
		
	
		class QuadDraggable : public cefix::Draggable, public osg::Referenced {
		public:
			QuadDraggable(RectangleSelectionWidget* widget);	
			virtual void down() {
				stopPropagation();
				Draggable::down();
			}
			virtual bool dragStarted ();
			virtual bool drag(const osg::Vec3 &delta);

		private:
			RectangleSelectionWidget*	_widget;
			osg::Vec2					_p1, _p2, _p3, _p4;
		};


		Implementation(RectangleSelectionWidget* widget) : osg::Referenced(), _widget(widget) {}
		
		virtual void update(osg::Vec2Array* points) = 0;
		RectangleSelectionWidget* getWidget() { return _widget; }
		virtual osg::Node* getNode() = 0;
		virtual void applyPropertyList(PropertyList* pl) = 0;
	private:
		RectangleSelectionWidget* _widget;
	};
	
	void setImplementation(Implementation* impl) 
	{ 
		_impl = impl; 
		getMatrixTransform()->removeChildren(0,getMatrixTransform()->getNumChildren());
		if (_impl.valid())
			getMatrixTransform()->addChild(_impl->getNode());
	}
	
	void setTopLeft(const osg::Vec2& p) {
		osg::Vec2 point = constrain(p);
		(*_points)[0] = point;
		if (!_independentCorners) {
			(*_points)[1][1] = point[1];
			(*_points)[3][0] = point[0];
		}
		dirty();
	}
	
	void setBottomLeft(const osg::Vec2& p) {
		osg::Vec2 point = constrain(p);
		(*_points)[3] = point;
		if (!_independentCorners) {
			(*_points)[0][0] = point[0];
			(*_points)[2][1] = point[1];
		}
		dirty();
	}
	
	void setTopRight(const osg::Vec2& p) {
		osg::Vec2 point = constrain(p);
		(*_points)[1] = point;
		if (!_independentCorners) {
			(*_points)[0][1] = point[1];
			(*_points)[2][0] = point[0];
		}
		dirty();
	}
	
	void setBottomRight(const osg::Vec2& p) {
		osg::Vec2 point = constrain(p);
		(*_points)[2] = point;
		if (!_independentCorners) {
			(*_points)[1][0] = point[0];
			(*_points)[3][1] = point[1];
		}
		dirty();
	}
	
	void setNormalizedRect(const osg::Vec4& rect) 
	{
		setTopLeft(osg::Vec2(rect[0] * _referenceSize[0], rect[1] * _referenceSize[1]));
		setTopRight(osg::Vec2(rect[2] * _referenceSize[0], rect[1] * _referenceSize[1]));
		setBottomLeft(osg::Vec2(rect[0] * _referenceSize[0], rect[3] * _referenceSize[1]));
		setBottomRight(osg::Vec2(rect[2] * _referenceSize[0], rect[3] * _referenceSize[1]));
	}
	
	osg::Vec2Array* getSelection() { return _points.get(); }
	
	osg::Vec2Array* getNormalizedSelection(float scale_x = 1.0f, float scale_y = 1.0f) {
		_normalizedPoints = new osg::Vec2Array(4);
		float tx(0), ty(0), mx(1), my(1);
		for (unsigned int i = 0; i < 4; ++i) {
			(*_normalizedPoints)[i].set(tx + mx * (*_points)[i][0] / _referenceSize[0] * scale_x, ty + my * (*_points)[i][1] / _referenceSize[1] * scale_y);
		}
		
		return _normalizedPoints.get();
	}
	
	void setNormalizedSelection(osg::Vec2Array* v, float scale_x = 1.0f, float scale_y = 1.0f) 
	{
		setTopLeft( scale_input( (*v)[0], scale_x*_referenceSize[0], scale_y*_referenceSize[1]));
		setTopRight( scale_input( (*v)[1], scale_x*_referenceSize[0], scale_y*_referenceSize[1]));
		setBottomRight( scale_input( (*v)[2], scale_x*_referenceSize[0], scale_y*_referenceSize[1]));
		setBottomLeft( scale_input( (*v)[3], scale_x*_referenceSize[0], scale_y*_referenceSize[1]));
		
		if (_impl.valid()) _impl->update(_points.get());

	}
	
	virtual void update()
	{
		if (_dirty) 
		{
			informAttachedResponder(Actions::selectionChanged());
			_dirty = false;
		}
		if (_impl.valid()) _impl->update(_points.get());
	}
	
	const osg::Vec4& getConstrainingRect() { return _constrainingRect; }
    void setConstrainingRect(const osg::Vec4& rect) { _constrainingRect = rect; update(); }
	
	osg::Vec2 constrain(const osg::Vec2& p);
	
	void setReferenceSize(const osg::Vec2& size) { _referenceSize = size; }
	
	void setIndependenCornersFlag(bool f) { 
		_independentCorners = f; 
	}
	
private:
	inline osg::Vec2 scale_input(const osg::Vec2& v, float scale_x, float scale_y) 
	{
		return osg::Vec2(v[0]* scale_x, v[1] * scale_y);
	}
	
protected:
	void dirty() { _dirty = true; }
	
	osg::ref_ptr<osg::Vec2Array> _points, _normalizedPoints;
	osg::Vec4					 _constrainingRect;
	bool						 _independentCorners;
	osg::ref_ptr<Implementation> _impl;
	bool						 _dirty;
	osg::Vec2					 _referenceSize;
	bool						 _rectIsDraggable;
	
};

class SimpleRectangleSelectionWidgetImplementation : public RectangleSelectionWidget::Implementation{

public:
	SimpleRectangleSelectionWidgetImplementation(RectangleSelectionWidget* widget);
	virtual void update(osg::Vec2Array* points);
	virtual osg::Node* getNode() { return _group.get(); }
	virtual void applyPropertyList(PropertyList* pl);
    void setColor(const osg::Vec4& color);
private:
	osg::ref_ptr<osg::Group> _group;
	std::vector< osg::ref_ptr< cefix::Quad2DGeometry > > _corners;
	osg::ref_ptr<cefix::LineStripGeometry>				 _frame;
	osg::Geode*				_geode;

	
};

template <class DrawImplementationClass = SimpleRectangleSelectionWidgetImplementation> class RectangleSelectionWidgetT : public RectangleSelectionWidget {
	public:
		RectangleSelectionWidgetT(const std::string& identifier, const osg::Vec3& position, const osg::Vec4& rect, const osg::Vec4& constrainingRect, bool independentCorners = false)
		:	RectangleSelectionWidget(identifier, position, rect, constrainingRect, independentCorners)
			
		{
			setImplementation(new DrawImplementationClass(this));
		}
		
		RectangleSelectionWidgetT(cefix::PropertyList*pl) 
		:	RectangleSelectionWidget(pl)
		{
			setImplementation(new DrawImplementationClass(this));
			getImplementation()->applyPropertyList(pl);
		}
		
		DrawImplementationClass* getImplementation() { return dynamic_cast<DrawImplementationClass*>(_impl.get()); }
	
};

typedef RectangleSelectionWidgetT<> SimpleRectangleSelectionWidget;

}


#endif