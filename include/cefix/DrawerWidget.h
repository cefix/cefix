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

#ifndef DRAWER_WIDGET_HEADER
#define DRAWER_WIDGET_HEADER


#include <cefix/TransformWidget.h>
#include <cefix/AnimationBasedOnDuration.h>
#include <cefix/AnimationFactory.h>
#include <cefix/TextGeode.h>
#include <cefix/Pickable.h>

namespace cefix {

class PropertyList;

/** a drawerwidget holds a group of widgets, has a acaption and can be collapsed and expanded by clicking on its tab. the tab can be positioned on the four sides of the drawer.
    the drawer disables all enclosed widgets when collapsing and enable them after expanding */
class CEFIX_EXPORT DrawerWidget : public cefix::TransformWidget {

	public:
	
		enum TabPosition { TOP, BOTTOM, LEFT, RIGHT } ;
		
		class Actions {
		
		public:
			static const char* drawerExpanded() { return "drawerExpanded"; }
			static const char* drawerCollapsed() { return "drawerCollapsed"; }		
		};
		
		/** the base class for the drawer-implementation, it should create a geometry and handle the positioning of the drawer */
		class Implementation : public AbstractWidget::Implementation<DrawerWidget> {
		
			public:
				/** ctor*/
			Implementation(DrawerWidget* drawer) : AbstractWidget::Implementation<DrawerWidget>(drawer) {}
				
				/** update the position of the drawer, its called by the DrawerWidget when doing a collapse/expand */
				virtual void updateDrawerPosition(const osg::Vec3& pos) = 0;
				
				/** apply the propertylist */
				virtual void applyPropertyList(PropertyList* pl) = 0;
				
				/** update the caption, called by the drawerWidegt when the caption changes */
				virtual void updateCaption(const std::string& caption) = 0;
				
				virtual void update() {  }
				virtual osg::Node* getNode() { return NULL;}
		};
		
		/** small helper class to animate the expanding/collapsing of the draweer-widget */
		class Animation : public cefix::AnimationBasedOnDuration {
		
			public:	
				Animation(DrawerWidget* drawer, float duration) : 
					cefix::AnimationBasedOnDuration(0,duration),
					_drawer(drawer)
				{
				}
				
			protected:
				virtual void animate(float elapsed_time) {
					cefix::AnimationBasedOnDuration::animate(elapsed_time);
					if (_drawer.valid())
						if (!_drawer->doMovement(_scalar))
							setFinishedFlag();
				}
			private:
				osg::observer_ptr<DrawerWidget> _drawer;
		};
		
		/** ctor */
		DrawerWidget(const std::string& identifier, const osg::Vec3& collapsedPosition, const osg::Vec3& expandedPosition, const osg::Vec2&size, const std::string& caption, TabPosition tabPosition, bool expanded = true);
		
		/** ctor */
		DrawerWidget(cefix::PropertyList* pl);
		
		
		/** update the widget */
		virtual void update() {
			cefix::TransformWidget::update();
			if (_impl.valid()) _impl->updateDrawerPosition(_positionRange.easyInOut(_scalar));
		}
		
		/** calculate the position of the drawer and update its states -- called from the animation-object */
		bool doMovement(float timescalar) {
			bool result = true;
			if (_collapsing) {
				_scalar = 1 - timescalar;
				if (_scalar <= 0.0) {
					_collapsing = false;
					_collapsed = true;
					_expanded = false;
					informAttachedResponder(Actions::drawerCollapsed());
					result = false;
				}
			}
			else if (_expanding) {
				_scalar =timescalar;
				if (_scalar >= 1.0) {
					_expanding = false;
					_expanded = true;
					_collapsed = false;
					informAttachedResponder(Actions::drawerExpanded());
					result = false;
				}
			}
			if (_impl.valid()) _impl->updateDrawerPosition(_positionRange.easyInOut(_scalar));
			return result;
			
		}
		
		/** get the size */
		const osg::Vec2& getSize() { return _size; }
		
		/** get the caption */
		const std::string& getCaption() { return _caption; }
		
		/* sets the caption */
		void setCaption(const std::string& caption) { _caption = caption; if (_impl.valid()) _impl->updateCaption(caption);  }
		
		/** get the tab position */		
		TabPosition getTabPosition() { return _tabPosition; }
		
		/** expand the drawer */
		void expand() {
			if (_collapsing || _expanding || _expanded) return;
			_expanding = true;
			doCollapseOtherDrawers();
			startAnimation();
		}
		/** collapse the drawer */
		void collapse() {
			if (_collapsing || _expanding || _collapsed) return;
			_collapsing = true;
			startAnimation();
		}
		
		const osg::Vec3& getCollapsedPosition() const { return _positionRange.max(); }
		const osg::Vec3& getExpandedPosition() const { return _positionRange.min(); }
		float getWidth() { return _size[0]; }
		float getHeight() { return _size[1]; }
		
		void setCollapsedPosition(const osg::Vec3& p) { _positionRange.set(p, _positionRange.max()); update(); }
		void setExpandedPosition(const osg::Vec3& p) {  _positionRange.set(_positionRange.min(), p); update(); }
		/** expand the drawer if it is collapsed o vice-versa */
		void flipFlap() {
			if (_collapsed) 
				expand();
			else if(_expanded)
				collapse();
		}
		
		/** set to true if you want to collapse other expanded drawers in the same group when expanding this one */
		void setCollapseOtherDrawers(bool f) { _collapseOtherDrawers = f; }
		
		void setTabOffset(float o) { _tabOffset = o; _impl->updateCaption(_caption); }
		float getTabOffset() const { return _tabOffset; }
        
        bool isExpanded() const { return _expanded; }
        bool isCollapsed() const { return _collapsed; }
		
	protected:
		void doCollapseOtherDrawers();
		void setImplementation(Implementation* impl) { _impl = impl; }
		
		void startAnimation() {
			if (_animation.valid()) _animation->cancel();
			_animation = new Animation(this, _animationDuration);
			cefix::AnimationController* ctrl = new cefix::AnimationController();
			ctrl->add(_animation.get());
			ctrl->init();
			cefix::AnimationFactory::instance()->add(ctrl);
		}
		
		virtual ~DrawerWidget() {
			if (_impl.valid()) _impl->clear();
		}
		
		osg::Vec2	_size;
		float		_scalar;
		cefix::vec3Range	_positionRange;
		osg::ref_ptr<Implementation>	_impl;
		osg::ref_ptr<Animation>			_animation;
		std::string						_caption;
		TabPosition						_tabPosition;
		float							_animationDuration;
		bool							_collapsed, _collapsing, _expanded, _expanding;
		bool							_collapseOtherDrawers;
		float							_tabOffset;
		

};

/** simple drawimplementation for dthe drawer-widget */
class SimpleDrawerDrawImplementation : public DrawerWidget::Implementation, public cefix::Pickable {
	public:
		/** ctor */
		SimpleDrawerDrawImplementation(DrawerWidget* drawer);
		
		virtual void updateDrawerPosition(const osg::Vec3& pos);
		virtual void applyPropertyList(PropertyList* pl);
		virtual void updateCaption(const std::string& caption) { _textgeode->setText(caption); updateTab(); }
		void createGeometry();
		virtual void down();
		virtual void up(bool inside);
		
	protected:
		void updateTab();
		
	private:
		float _tabHeight;
		osg::Vec3 _captionDelta;
		osg::ref_ptr<cefix::Utf8TextGeode>	_textgeode;
		std::string		_font;
		float			_fontsize;
		osg::Vec3		_textOffset;
		osg::Vec3Array*	_vec;
		osg::Geometry*	_geo;
		osg::Vec4		_backColor;
		osg::Vec4		_captionColor;
		osg::ref_ptr<osg::MatrixTransform> _textTransform;

};


/** template class for the DrawerWidget to combine drawImplementation and widget */
template<class DrawImplementationClass = SimpleDrawerDrawImplementation> class DrawerWidgetT : public DrawerWidget {

	public:
		DrawerWidgetT(
                      const std::string& identifier, 
                      const osg::Vec3& collapsedPosition, 
                      const osg::Vec3& expandedPosition, 
                      const osg::Vec2&size, 
                      const std::string& caption, 
                      TabPosition tabPosition, 
                      bool expanded = true):
			DrawerWidget(identifier, collapsedPosition, expandedPosition, size, caption, tabPosition, expanded)
		{
			DrawImplementationClass* impl = new DrawImplementationClass(this);
			setImplementation(impl);
			impl->createGeometry();
		}
		
		DrawerWidgetT(cefix::PropertyList* pl) :
			DrawerWidget(pl)
		{
			DrawImplementationClass* impl = new DrawImplementationClass(this);
			setImplementation(impl);
			impl->applyPropertyList(pl);
			impl->createGeometry();
		}
		
		DrawImplementationClass& getImplementation() { return *(dynamic_cast<DrawImplementationClass*>(_impl.get())); }

};

typedef DrawerWidgetT<> SimpleDrawerWidget;
}

#endif