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

#ifndef DRAG_WIDGET_HEADER
#define DRAG_WIDGET_HEADER

#include <cefix/AbstractWidget.h>
#include <cefix/PropertyList.h>
#include <cefix/SimpleDragWidgetDrawImplementation.h>

namespace cefix {
/** a Drag-Widget is something which can be dragged. It's main-usage for now is for the window-titlebar. Note: this class has no implementation of the graphical representation,
see DragWidgetT */
class DragWidget : public RectControlWidget {
	
	
	public:
	
		class Actions : public AbstractWidget::Actions {
			public:
				static const char* widgetDragged() { return "widgetDragged"; }
			private:
				Actions() : AbstractWidget::Actions() {}
		};
	
		typedef  Functor1ParamAbstractT<void, const osg::Vec3&> DragCallback;
	
	protected:
		/** ctor 
		    @param identifier identifier of this widget, 
		    @param pos the position of the widget, 
		    @param size the size of the widget
		*/
		DragWidget(const std::string& identifier, const osg::Vec3& pos, const osg::Vec2 size) : RectControlWidget(identifier, pos, size) {}
	
	public:	
		
		/** sets the drag-callback */
		virtual void setDragCallback(DragCallback* cb) { _cb = cb; }
		
		/** @return the registered drag-callback */
		DragCallback* getDragCallback() const { return _cb.get(); }
		
				
	private:
		osg::ref_ptr<DragCallback > _cb;

};

/** adapter template class which adapts a DrawImplementationClass with a DragWidget. this mechanisme enures maximal flexibility */
template <class DragWidgetClass, class DrawImplementationClass = SimpleDragWidgetDrawImplementation> class DragWidgetT : public DragWidgetClass {
	public:
		/** ctor 
		    @param identifier identifier of this widget, 
		    @param pos the position of the widget, 
		    @param size the size of the widget
		*/
		DragWidgetT(const std::string& identifier, const osg::Vec3& pos, const osg::Vec2& size) :
			DragWidgetClass(identifier, pos, size)
		{	
			_drawImplementation = new DrawImplementationClass();
			_drawImplementation->update(this);
			setDragCallback(new cefix::Functor1ParamT<void, const osg::Vec3&, DragWidgetT>(this, &DragWidgetT::widgetDragged));
		}
		
		DragWidgetT(cefix::PropertyList* pl) :
			DragWidgetClass(pl->get("id")->asString(), pl->get("position")->asVec3(), pl->get("size")->asVec2())
		{	
			_drawImplementation = new DrawImplementationClass();
			_drawImplementation->update(this);
			setDragCallback(new cefix::Functor1ParamT<void, const osg::Vec3&, DragWidgetT>(this, &DragWidgetT::widgetDragged));
			_drawImplementation->applyPropertyList(pl);
		}
		
		/** @return the drawImplementation */
		DrawImplementationClass* getDrawImplementation() { return _drawImplementation.get(); }
		
		/** sets the DragCallback, which should be called, when a drag occures */
		virtual void setDragCallback(typename DragWidgetClass::DragCallback* cb) { 
			DragWidgetClass::setDragCallback(cb); 
			_drawImplementation->setDragCallback(cb); 
		}
		/** @return the graphical representation */
		virtual osg::Node* getNode() { return _drawImplementation.get(); }
		
		/** update the widget's graphical representation */
		virtual void update() {
			_drawImplementation->update(this);
		}
		
		/** should be called, when the widget got dragged */
		void widgetDragged(const osg::Vec3& newpos) {
			DragWidgetClass::setPosition(newpos);
			update();
			this->informAttachedResponder( DragWidget::Actions::widgetDragged() );
		}
		
		protected:
			virtual ~DragWidgetT() {
				setDragCallback(NULL);
			}
		
		private:
			osg::ref_ptr<DrawImplementationClass>		_drawImplementation;
};

/** SimpleDragWidget, which adapts a SimpleDragWidgetDrawImplementation with a DragWidget */
typedef DragWidgetT<DragWidget, SimpleDragWidgetDrawImplementation> SimpleDragWidget;


}

#endif