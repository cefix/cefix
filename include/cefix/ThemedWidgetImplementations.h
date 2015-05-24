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

#ifndef THEMED_PROGRESSBAR_WIDGET_IMPLEMENTATION_HEADER
#define THEMED_PROGRESSBAR_WIDGET_IMPLEMENTATION_HEADER

#include "ProgressBarWidget.h"
#include <cefix/SliderWidget.h>
#include <cefix/AreaSliderWidget.h>
#include <cefix/HorizontalSliderWidget.h>
#include <cefix/VerticalSliderWidget.h>
#include <cefix/ButtonWidget.h>
#include <cefix/TextButtonWidget.h>
#include <cefix/CheckBoxWidget.h>
#include <cefix/Draggable.h>
#include <cefix/DrawerWidget.h>
#include <cefix/ImageWidget.h>
#include <cefix/TextWidget.h>
#include <cefix/WindowWidget.h>
#include <cefix/DragWidget.h>
#include <cefix/ThemedWidgetStateGeometry.h>
#include <cefix/ThemedWidgetStateDecorationGeometry.h>
#include <cefix/TextInputWidget.h>


namespace cefix {
	class WidgetTheme;
namespace themedWidgets {

class ProgressBarWidgetImplementation : public cefix::ProgressBarWidget::Implementation {
	
public:
	ProgressBarWidgetImplementation(cefix::ProgressBarWidget* widget);
	virtual osg::Node* getNode() { return _geode.get(); }
	virtual void update();
	virtual void applyPropertyList(cefix::PropertyList* pl);
	
	inline void setCurrentInfiniteAnimationStep(unsigned int s) {_inifiniteAnimationStep = s; }

private:
	osg::ref_ptr<osg::Geode> _geode;
	osg::ref_ptr<ThemedWidgetStateGeometry> _back, _inner, _infinite;
	unsigned int _inifiniteAnimationStep;
	
};



	
	
	
class  SliderDrawImplementation : public osg::Geode, public SliderDraggable {
	
public:
	SliderDrawImplementation(const osg::Vec3& pos, const osg::Vec2& size, const osg::Vec2& throbberSize);
	
	void setSlider(SliderWidget* widget);
	void update(SliderWidget* widget); 
	
	void applyPropertyList(cefix::PropertyList* pl);
	
	virtual void down()
	{
		_mouseDown = true;
		SliderDraggable::down();
	}
	
	virtual void up(bool inside)
	{
		_mouseDown = false;
		if (inside && _widget->isEnabled())
			_front->setState(ThemedWidgetStateGeometry::NORMAL_OVER);
			
		SliderDraggable::up(inside);
	}
	
	virtual void enter() {
		_inside = true;
		if (_widget->isEnabled()) {
			if (!_mouseDown)
				_front->setState(ThemedWidgetStateGeometry::NORMAL_OVER);
			_back->setState(ThemedWidgetStateGeometry::NORMAL_OVER);
		}
		SliderDraggable::enter();
	}
	
	virtual void leave() {
		_inside = false;
		if (_widget->isEnabled()) {
			_back->setState(ThemedWidgetStateGeometry::NORMAL);
			if (!_mouseDown)
				_front->setState(ThemedWidgetStateGeometry::NORMAL);
		}
		SliderDraggable::leave();
	}
	
	
	
private:
	bool	  _mouseDown, _inside;
	osg::Vec3 _pos;
	osg::Vec2 _size, _throbberSize;
	
	osg::ref_ptr<ThemedWidgetStateGeometry> _back, _front;
	SliderWidget* _widget;
	bool	_fixedThrobberSize;
	
};


class ButtonDrawImplementation : public osg::Geode, public cefix::Pickable {
		
public:
	ButtonDrawImplementation(const osg::Vec3& pos, const osg::Vec2& size, const std::string& key="button");
	
	void update(ButtonWidget* widget);
	
	virtual void enter() { 
		stopPropagation();
		if (_fn.valid()) _fn->call(AbstractWidget::MOUSE_ENTER);
	}
	virtual void leave() { 
		if (_fn.valid()) _fn->call(AbstractWidget::MOUSE_LEAVE); 
	}
	
	virtual void down()  { 
		stopPropagation(); 
		if (_fn.valid()) _fn->call(AbstractWidget::MOUSE_DOWN); 
	}
	
	virtual void up(bool inside) { 
		if (_fn.valid()) _fn->call( (inside) ? AbstractWidget::MOUSE_UP : AbstractWidget::MOUSE_UP_OUTSIDE ); 
	}
	
	void setCallback(cefix::Functor1ParamAbstractT<void, AbstractWidget::EventTypes>* fn) { _fn = fn; }
	
	const osg::Vec2& getSize() const { return _size; }
	const osg::Vec3& getPosition() const { return _pos; }

	void applyPropertyList(cefix::PropertyList* pl);
	
	void setColor(const osg::Vec4& c) { _geo->setColor(c); }

	void setAlpha(float f) { _geo->setAlpha(f); }
protected:
	osg::Vec3	_pos;
	osg::Vec2	_size;
private:	
	osg::ref_ptr<cefix::ThemedWidgetStateGeometry> _geo;
	osg::ref_ptr< cefix::Functor1ParamAbstractT<void, AbstractWidget::EventTypes> > _fn;
	
};
	
	class TextButtonDrawImplementation : public osg::Group {
	
	public: 
		typedef std::map <unsigned int, osg::Vec4> ColorMap;
		TextButtonDrawImplementation(const osg::Vec3& pos, const osg::Vec2&size, const std::string& themeKey = "textbutton");
		
		void update(TextButtonWidget* widget);
		
		void setTextColorForState(AbstractWidget::States state, osg::Vec4 c) { _textColorMap[state] = c; }
		
		void setCallback(cefix::Functor1ParamAbstractT<void, AbstractWidget::EventTypes>* fn) { _btn->setCallback(fn); }
		
		cefix::Utf8TextGeode* getTextGeode() { return _textGeode.get(); }
		
		void setTextAlignment (Utf8TextGeode::TextAlignment alignment, osg::Vec2 offset) { 
			_textGeode->setTextAlignment(alignment); 
			_textOffset = offset; 
		}
		
		void setOffset(const osg::Vec2& offset) { _textOffset = offset; }
		
		void applyPropertyList(cefix::PropertyList* pl);
		
		void setAlpha(float f) { _textGeode->setAlpha(f); _btn->setAlpha(f); }
		
		
	protected:
		void applyPrefs(WidgetTheme* theme, const std::string&key);
		
		ColorMap	_textColorMap;
		osg::ref_ptr<cefix::Utf8TextGeode>	_textGeode;
		osg::ref_ptr<ButtonDrawImplementation> _btn;
		osg::Vec2 _textOffset;
		std::string	_themeKey;
		bool		_uppercase;
	};
	
	
	class CheckboxDrawImplementation : public TextButtonDrawImplementation {
		
	public: 
		CheckboxDrawImplementation(const osg::Vec3& pos, const osg::Vec2&size);
		
	protected:

		
	};
	
	
	class RadioButtonDrawImplementation : public TextButtonDrawImplementation {
		
	public: 
		RadioButtonDrawImplementation(const osg::Vec3& pos, const osg::Vec2&size);
		
	protected:
		
		
	};
	
	
	
	class DrawerDrawImplementation : public DrawerWidget::Implementation, public cefix::Pickable {
	public:
		/** ctor */
		DrawerDrawImplementation(DrawerWidget* drawer);
		
		virtual void updateDrawerPosition(const osg::Vec3& pos);
		virtual void applyPropertyList(PropertyList* pl);
		virtual void updateCaption(const std::string& caption) { _textgeode->setText(caption); updateTab(); }
		
		virtual void down();
		virtual void up(bool inside);
		
		void createGeometry();
		
	protected:
		void updateTab();
		
		
	private:
		float _tabHeight;
		osg::Vec3 _captionDelta;
		osg::ref_ptr<cefix::Utf8TextGeode>	_textgeode;
		std::string		_font;
		float			_fontsize;
		osg::Vec3		_textOffset;
		osg::Vec2			_tabOffset;
		osg::ref_ptr<osg::MatrixTransform> _textTransform;
		osg::ref_ptr<osg::MatrixTransform> _tabTransform;
		osg::ref_ptr<cefix::ThemedWidgetStateGeometry> _background;
		osg::ref_ptr<cefix::ThemedWidgetStateGeometry> _tab;
		osg::Vec4		_textColor;
	};
	
	#pragma mark ThemedDragWidgetDrawImplementation
	
	class  ThemedWindowTitlebarWidgetDrawImplementation : public osg::Geode, public DragWidgetDraggable {
	
	public:
		ThemedWindowTitlebarWidgetDrawImplementation();
		
		void update(DragWidget* widget); 
		void setThemeKey(const std::string& theme_key); 
		
		virtual void enter() {
			_inside = true;
			_widget->update();
            DragWidgetDraggable::enter();
		}
		
		virtual void leave() {
			_inside = false;
			_widget->update();
			DragWidgetDraggable::leave();
		}
		
		virtual void down() 
		{
			_down = true;
			DragWidgetDraggable::setOrigin(_pos); 
			DragWidgetDraggable::down();
		}
		
		virtual void up(bool inside){
			_down = false;
			_widget->update();
			DragWidgetDraggable::up(inside);
		}
		
		void setDragCallback(Functor1ParamAbstractT<void, const osg::Vec3&>* functor) {
			if (functor) {
				setFunctor(functor); 
				setOrigin(_pos);
			} else
				setFunctor(NULL);
		}
		
		void applyPropertyList(cefix::PropertyList* pl) {}
		
	private:
		osg::ref_ptr<cefix::ThemedWidgetStateGeometry> _back;
		osg::ref_ptr<cefix::ThemedWidgetStateDecorationGeometry> _deco;
		bool	_inside, _down;
		osg::Vec3 _pos;
		DragWidget*		_widget;

};
	
	class ThemedTextInputWidgetImplementation : public TextInputWidget::Implementation
	{
	public:
		ThemedTextInputWidgetImplementation(TextInputWidget* widget);
		
		virtual void update();
		virtual osg::Node* getNode() {return _group.get(); }
		virtual void applyPropertyList(PropertyList* pl);
		
		cefix::Font* getFont() { return _textGeode->getFont(); }
		
		void pulsateCursor();
	
	private:
		osg::ref_ptr<osg::Group>	_group;
		osg::ref_ptr<cefix::UnicodeTextGeode> _textGeode;
		osg::ref_ptr<cefix::ThemedWidgetStateGeometry> _back, _cursor;
		
		osg::Vec2	_inset;
		float		_cursorWidth;
	};


}
	
	class ThemedTextWidget : public TextWidget {
	
	public:
		ThemedTextWidget(const std::string& identifier, osg::Vec3 pos, std::string fontname="system.xml", float fontsize = 0);		
		ThemedTextWidget(const std::string& identifier, std::string text, const osg::Vec3& pos, const osg::Vec4& color, std::string fontname="system.xml", float fontsize = 0);
		ThemedTextWidget(cefix::PropertyList* pl);
		
		virtual void setCaption(const std::string& s) 
		{ 
			
			
			if (_uppercase) {
				std::string temp(cefix::strToUpper(s));
				getTextGeode()->setText(temp); 
				getTextGeode()->refresh();
			} else 
				TextWidget::setCaption(s);
		}
		
	private:
		bool _uppercase;
	};
	

	
	class ThemedWindowWidget: public WindowWidget {

		public:
			ThemedWindowWidget(const std::string& identifier, const std::string& windowTitle, const osg::Vec3& pos, const osg::Vec2& size, cefix::ViewWidget::Mode mode = ViewWidget::ORTHO, cefix::ViewWidget::RenderMode rendermode = ViewWidget::DIRECT) :
				WindowWidget(identifier, windowTitle, pos, size, mode, rendermode)
			{
				init(pos, size);
			}
			
			ThemedWindowWidget(cefix::PropertyList* pl) :
				WindowWidget(pl)
			{
				init(pl->get("position")->asVec3(), pl->get("size")->asVec2());
			}
			
		private:
			void init(osg::Vec3 pos, osg::Vec2 size);
	};
	
	#pragma mark typedefs

	typedef ButtonWidgetT<ButtonWidget, themedWidgets::ButtonDrawImplementation> ThemedButtonWidget;
	typedef TextButtonWidgetT<TextButtonWidget, themedWidgets::TextButtonDrawImplementation> ThemedTextButtonWidget;
	typedef CheckboxWidgetT<CheckboxWidget, themedWidgets::CheckboxDrawImplementation> ThemedCheckboxWidget;
	typedef CheckboxWidgetT<CheckboxWidget, themedWidgets::RadioButtonDrawImplementation> ThemedRadioButtonWidget;

	typedef ProgressBarWidgetT<themedWidgets::ProgressBarWidgetImplementation> ThemedProgressBarWidget;
	typedef SliderWidgetT<HorizontalSliderWidget, themedWidgets::SliderDrawImplementation> ThemedHorizontalSliderWidget;
	typedef SliderWidgetT<VerticalSliderWidget, themedWidgets::SliderDrawImplementation> ThemedVerticalSliderWidget;
	typedef SliderWidgetT<AreaSliderWidget, themedWidgets::SliderDrawImplementation> ThemedAreaSliderWidget;
	
	typedef DrawerWidgetT<themedWidgets::DrawerDrawImplementation> ThemedDrawerWidget;
	typedef DragWidgetT<DragWidget, themedWidgets::ThemedWindowTitlebarWidgetDrawImplementation> ThemedWindowTitlebarWidget;
	
	typedef TextInputWidgetT<themedWidgets::ThemedTextInputWidgetImplementation> ThemedTextInputWidget;
	typedef SimpleImageWidget ThemedImageWidget;

	void registerImplementations();
}

#endif