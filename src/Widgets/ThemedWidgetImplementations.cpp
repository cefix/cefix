/*
 *  ThemedProgressBarWidgetImplementation.cpp
 *  cefixcvTemplate
 *
 *  Created by Stephan Huber on 23.10.08.
 *  Copyright 2008 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */

#include "ThemedWidgetImplementations.h"

#include "WidgetThemeManager.h"
#include <cefix/WidgetFactory.h>
#include <cefix/ColorUtils.h>
#include <osgDB/WriteFile>
#include <cefix/BoxWidget.h>


namespace cefix {
namespace themedWidgets {

	
#pragma mark ProgressBarWidget

	
	class InfiniteProgressbarCallback : public osg::NodeCallback {

	public:
	InfiniteProgressbarCallback(ThemedProgressBarWidget* w, unsigned int max) : osg::NodeCallback(), _w(w), _i(0), _step(0), _max(max) {}

	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) 
	{
		if ((_i % 6) == 0) {
			++_step;
			if (_step >= _max) _step = 0;
			ProgressBarWidgetImplementation* impl = dynamic_cast<ProgressBarWidgetImplementation*>(_w->getImplementation());
			impl->setCurrentInfiniteAnimationStep(_step);
			_w->update();
			_i = 0;
			
		}
		++_i;
	}

	private:
		ThemedProgressBarWidget* _w;
		unsigned int _i, _step,_max;

	};

	
	ProgressBarWidgetImplementation::ProgressBarWidgetImplementation(cefix::ProgressBarWidget* widget)
	:	cefix::ProgressBarWidget::Implementation(widget),
		_geode(new osg::Geode()),
		_inifiniteAnimationStep(0)
	{
		WidgetTheme* theme = WidgetThemeManager::instance()->getCurrentTheme();
		
		_back		= theme->createStateGeometry("ProgressBarBackground");
		_inner		= theme->createStateGeometry("progressbarInner");
		_infinite	= theme->createStateDecorationGeometry("ProgressBarInnerInfinite");
		
		_geode->addDrawable(_back.get());
		_geode->addDrawable(_inner.get());
		_geode->addDrawable(_infinite.get());
		
		_infinite->setColor(osg::Vec4(1,1,1,0));
	}
	
	void ProgressBarWidgetImplementation::update() 
	{
		static unsigned int step = 0;
		
		osg::Vec3 p = getWidget()->getPosition();
		osg::Vec2 s = getWidget()->getSize();

		_back->setRect(p[0], p[1], s[0], s[1]);
		_back->setLocZ(p[2]);
		
		float perc = std::min(getWidget()->getPercentDone(), 100.0f);
		
		if (perc < 0) {
			_infinite->setColor(osg::Vec4(1,1,1,1));
			_inner->setColor(osg::Vec4(1,1,1,0));
			_infinite->setRect(p[0], p[1], s[0], s[1]);
			_infinite->setState(_inifiniteAnimationStep);
			
			_infinite->setLocZ(p[2] + 1);
			if (_geode->getUpdateCallback() == NULL) {
				_geode->setUpdateCallback(new InfiniteProgressbarCallback(dynamic_cast<ThemedProgressBarWidget*>(getWidget()), _infinite->getNumStates()) );
			}
		} else
			_geode->setUpdateCallback(NULL);
		
		if (perc >= 0) {
			_inner->setRect(p[0], p[1], (s[0]) * perc / 100.0,  s[1]);
			_inner->setLocZ(p[2] + 1);
			_infinite->setColor(osg::Vec4(1,1,1,0));
			_inner->setColor(osg::Vec4(1,1,1,1));
		}
		unsigned int state = getWidget()->isEnabled() ? ThemedWidgetStateGeometry::NORMAL : ThemedWidgetStateGeometry::NORMAL_DISABLED;
		_back->setState(state);
		_inner->setState(state);
		_infinite->setState(state);
		++step;
	}
	
	void ProgressBarWidgetImplementation::applyPropertyList(PropertyList* pl) 
	{
		WidgetThemeManager::instance()->applyCustomThemeOrAppearance(_back.get(), pl, "background");
		WidgetThemeManager::instance()->applyCustomThemeOrAppearance(_inner.get(), pl, "inner");
		WidgetThemeManager::instance()->applyCustomThemeOrAppearance(_infinite.get(), pl, "InnerInfinite");
	} 
		
	
#pragma mark SliderDrawImplementation
	
	SliderDrawImplementation::SliderDrawImplementation(const osg::Vec3& pos, const osg::Vec2& size, const osg::Vec2& throbberSize)
	:	osg::Geode(),
		SliderDraggable(),
		_mouseDown(false),
		_inside(false),
		_pos(pos),
		_size(size),
		_throbberSize(throbberSize)
	
	{
		WidgetTheme* theme = WidgetThemeManager::instance()->getCurrentTheme();
		
		std::string id = (size[0] > size[1]) ? "HorizontalSlider" : "VerticalSlider";
		_back		= theme->createStateGeometry(id);
		_front		= theme->createStateGeometry(id+"throbber");
		
		addDrawable(_back.get());
		addDrawable(_front.get());
		_fixedThrobberSize = theme->getPrefs(id+"throbber", "fixedThrobberSize", true);
	}
	
	
	void SliderDrawImplementation::setSlider(SliderWidget* slider) {
		SliderDraggable::setSlider(slider);
		setOrigin(_pos);
		setDragMode(DRAG_ON_SURFACE_PLANE);
	}
	
	
	void SliderDrawImplementation::update(SliderWidget* widget) 
	{
		_widget = widget;
		osg::Vec3 tp(widget->getThrobberPos());
		unsigned int l(_pos[0]);
		unsigned int r(_pos[0]+_size[0]);
		unsigned int tl(_pos[0] + tp[0]);
		unsigned int tt(_pos[1] + tp[1]);
		// std::cout << l << " / " << r << std::endl;
		_back->setRect(osg::Vec4(l, _pos[1], r, _pos[1] + _size[1]));
		_back->setLocZ(_pos[2]);
		
		if (_fixedThrobberSize)
			_front->setRect(osg::Vec4(tl, tt, tl + _throbberSize[0],tt + _throbberSize[1]));
		else 
			_front->setRect(osg::Vec4(_pos[0], _pos[1], tl + _throbberSize[0],tt + _throbberSize[1]));
		_front->setLocZ(_pos[2]+1);
		
		_front->setState( widget->isEnabled() ? _mouseDown ? ThemedWidgetStateGeometry::NORMAL_DOWN : _inside ? ThemedWidgetStateGeometry::NORMAL_OVER : ThemedWidgetStateGeometry::NORMAL : ThemedWidgetStateGeometry::NORMAL_DISABLED);
		_back->setState( widget->isEnabled() ? _inside ? ThemedWidgetStateGeometry::NORMAL_OVER : ThemedWidgetStateGeometry::NORMAL : ThemedWidgetStateGeometry::NORMAL_DISABLED);
	}	
	
	
	void SliderDrawImplementation::applyPropertyList(PropertyList* pl) 
	{
		WidgetThemeManager::instance()->applyCustomThemeOrAppearance(_back.get(), pl, "");
		WidgetThemeManager::instance()->applyCustomThemeOrAppearance(_front.get(), pl, "throbber");
	} 
	
	
#pragma mark ButtonDrawImplementation
	
	ButtonDrawImplementation::ButtonDrawImplementation(const osg::Vec3& pos, const osg::Vec2& size, const std::string& key)
	:	osg::Geode(),
		cefix::Pickable(),
		_pos(pos),
		_size(size)
	{
		stopPropagation();
		WidgetTheme* theme = WidgetThemeManager::instance()->getCurrentTheme();
		_geo = theme->createStateGeometry(key);

		addDrawable(_geo.get());
	}
	
	void ButtonDrawImplementation::update(ButtonWidget* widget)
	{
		_geo->setPositionAndSize(widget->getPosition(),widget->getSize());
		_pos = widget->getPosition();
		_size = widget->getSize();
		
		switch (widget->getState()) {
			case AbstractWidget::DISABLED:
				_geo->setState( (widget->isSelected()) ? ThemedWidgetStateGeometry::SELECTED_DISABLED : ThemedWidgetStateGeometry::NORMAL_DISABLED );
				break;
			
			case AbstractWidget::PRESSED:
				_geo->setState( (widget->isSelected()) ? ThemedWidgetStateGeometry::SELECTED_DOWN : ThemedWidgetStateGeometry::NORMAL_DOWN ) ;
				
				break;
			
			case AbstractWidget::SELECTED:
				_geo->setState( ThemedWidgetStateGeometry::SELECTED);
				break;
			
			case AbstractWidget::UNKNOWN_OVER:
			case AbstractWidget::NORMAL_OVER:
				_geo->setState( ThemedWidgetStateGeometry::NORMAL_OVER);
				break;
			
			case AbstractWidget::SELECTED_OVER:
				_geo->setState( ThemedWidgetStateGeometry::SELECTED_OVER);
				break;
			
			default:
				_geo->setState( ThemedWidgetStateGeometry::NORMAL) ;
				break;
			
		}
	}
	
	void ButtonDrawImplementation::applyPropertyList(cefix::PropertyList* pl) 
	{
		WidgetThemeManager::instance()->applyCustomThemeOrAppearance(_geo.get(), pl, "");
	}
	
	
#pragma mark TextButtonDrawImplementation
	
	TextButtonDrawImplementation::TextButtonDrawImplementation(const osg::Vec3& pos, const osg::Vec2&size, const std::string& themeKey)
	:	osg::Group(),
		_themeKey(themeKey),
		_uppercase(false)
	{
		_btn = new ButtonDrawImplementation(pos, size, themeKey);
		
		WidgetTheme* theme = WidgetThemeManager::instance()->getCurrentTheme();
		
		_textGeode = new Utf8TextGeode(
									   theme->getPrefs("textbutton", "font", "system.xml"),
									   theme->getPrefs("textbutton", "fontsize", 0.0f)
									   );
		_textGeode->setUserData(_btn.get());
		applyPrefs(theme, themeKey);
		addChild(_btn.get());
		addChild(_textGeode.get());
	}
	
	void TextButtonDrawImplementation::applyPrefs(WidgetTheme* theme, const std::string& key) 
	{
		_uppercase = theme->getPrefs(key, "uppercase", false);
		_textGeode->setFont(theme->getPrefs(key, "font", "system.xml"));
		_textGeode->setFontSize(theme->getPrefs(key, "fontsize", 0.0f));
		_textGeode->setSubPixelAlignment(false);
		
		_textOffset = theme->getPrefs(key, "textoffset", osg::Vec2(0,0));
        
        _textColorMap.clear();
		setTextColorForState(AbstractWidget::DISABLED, theme->getPrefs(key, "disabledTextColor", rgb(0xaaaaaa)));
		setTextColorForState(AbstractWidget::NORMAL, theme->getPrefs(key, "normalTextColor", rgb(0x000000)));
		setTextColorForState(AbstractWidget::NORMAL_OVER, theme->getPrefs(key, "normalOverTextColor", rgb(0x000000)));
		
		setTextColorForState(AbstractWidget::PRESSED, theme->getPrefs(key, "downTextColor", rgb(0xffffff)));
		setTextColorForState(AbstractWidget::SELECTED, theme->getPrefs(key, "selectedTextColor", rgb(0x000000)));
		setTextColorForState(AbstractWidget::SELECTED_OVER, theme->getPrefs(key, "selectedOverTextColor", rgb(0x000000)));
		
		
		std::string alignment =  theme->getPrefs(key, "textaligment", "center");
		if (alignment=="center")
			setTextAlignment(cefix::Utf8TextGeode::RenderIterator::ALIGN_CENTER, _textOffset);
		else if (alignment=="right")
			setTextAlignment(cefix::Utf8TextGeode::RenderIterator::ALIGN_RIGHT, _textOffset);
		else
			setTextAlignment(cefix::Utf8TextGeode::RenderIterator::ALIGN_LEFT, _textOffset);
	}	
		
	
	void TextButtonDrawImplementation::update(TextButtonWidget* widget) 
	{
		_btn->update(widget);
		if (_uppercase) 
			_textGeode->setText(cefix::strToUpper(widget->getCaption()));
		else
			_textGeode->setText(widget->getCaption());
		
		
		Utf8TextGeode::TextAlignment tAlignment = _textGeode->getTextAlignment();
		
        // Fontsize sicherheitshalber mal setzen
        _textGeode->setFontSize(_textGeode->getFontSize());
        
		// das ist etwas genauer als _textGeode->getHeight(), getHeight liefert die Boundingbox zurück, getMajuscleHeight die Höhe von Grossbuchstaben
		
		float h = _textGeode->getFont()->getMajuscleHeight() + (_textGeode->getRenderedLineCount() - 1) * _textGeode->getLineSpacing();
		
        //std::cout << "h: " << h << " majuscle: " << _textGeode->getFont()->getMajuscleHeight() << " prescale: " << _textGeode->getFont()->getPreScale() << "text-offset: " << _textOffset << std::endl;
		
        osg::Vec2 size = (_btn->getSize());
		osg::Vec3 tTextPosition;
		// std::cout <<"size" << size[1] << " height: " << h << std::endl ;
		
		switch(tAlignment){
			case Utf8TextGeode::RenderIterator::ALIGN_LEFT:
				tTextPosition = _btn->getPosition() + osg::Vec3(_textOffset[0] + 0, _textOffset[1] + osg::round((size[1] - h ) / 2.0), 2);
				break;
			case Utf8TextGeode::RenderIterator::ALIGN_CENTER:
				tTextPosition = _btn->getPosition() + osg::Vec3(_textOffset[0] + osg::round(size[0] / 2.0), _textOffset[1] + ((size[1] - h ) / 2.0), 2);
				break;
			case Utf8TextGeode::RenderIterator::ALIGN_RIGHT:
				tTextPosition = _btn->getPosition() + osg::Vec3(_textOffset[0] + size[0], _textOffset[1] + osg::round((size[1] - h ) / 2.0), 2);
				break;
		}
		
		_textGeode->setPosition(tTextPosition);
		_textGeode->refresh();
		if (_textColorMap.find(widget->getState()) != _textColorMap.end())
			_textGeode->setTextColor(_textColorMap[widget->getState()]);
            
	}
	
	void TextButtonDrawImplementation::applyPropertyList(PropertyList* pl) 
	{
		_btn->applyPropertyList(pl);
		
		WidgetTheme* theme = WidgetThemeManager::instance()->getThemeFromPropertyList(pl);
		if (pl->hasKey("customAppearance")) {
			std::string ca(pl->get("customAppearance")->asString());
			if (theme->hasDescription(ca)) 
				applyPrefs(theme, ca);
		}
		
	} 
	
#pragma mark CheckboxDrawImplementation
	
	
	CheckboxDrawImplementation::CheckboxDrawImplementation(const osg::Vec3& pos, const osg::Vec2&size)
	:	TextButtonDrawImplementation(pos, size, "checkbox")
	{
		setTextAlignment(Utf8TextGeode::RenderIterator::ALIGN_LEFT, _textOffset);
	}

	
#pragma mark RadioButtonDrawImplementation

	
	RadioButtonDrawImplementation::RadioButtonDrawImplementation(const osg::Vec3& pos, const osg::Vec2&size)
	:	TextButtonDrawImplementation(pos, size, "radiobutton")
	{
		setTextAlignment(Utf8TextGeode::RenderIterator::ALIGN_LEFT, _textOffset);
	}
	
	
	class ImageWidgetDrawImplementation : public ImageWidget::Implementation {
	public:
		ImageWidgetDrawImplementation(ImageWidget* widget);
		
		virtual void update();
		virtual osg::Node* getNode() { return _geode.get(); }
		virtual void applyPropertyList(cefix::PropertyList* pl);
	private:
		osg::ref_ptr<osg::Geode>			_geode;
		osg::ref_ptr<cefix::Quad2DGeometry> _quad;
		osg::Image*							_cachedImage;
	};

	
	
	
#pragma mark DrawerDrawImplementation
	
	DrawerDrawImplementation::DrawerDrawImplementation(DrawerWidget* widget)
	:	DrawerWidget::Implementation(widget)
	{
		WidgetTheme* theme = WidgetThemeManager::instance()->getCurrentTheme();
		
		_font = theme->getPrefs("drawer", "fontname", "system.xml");
		_fontsize = theme->getPrefs("drawer", "fontsize", 0.0f);
		_tabHeight = theme->getPrefs("drawer", "tabHeight", 10.0f);
		_tabOffset = theme->getPrefs("drawer", "tabOffset", osg::Vec2(20.0f, 2.0f));
		_textOffset = theme->getPrefs("drawer", "textOffset", osg::Vec3(5, 2, 0));
		_textColor = theme->getPrefs("drawer", "textcolor", rgb(0x000000) );
		
		_background = theme->createStateGeometry("drawer");
		_tab = theme->createStateGeometry("drawerTab");
			
	}
	
	void DrawerDrawImplementation::updateDrawerPosition(const osg::Vec3& pos) {
		getWidget()->setMatrix(osg::Matrix::translate(pos+osg::Vec3(0,0,1)));
	}
	
	
	void DrawerDrawImplementation::applyPropertyList(PropertyList* pl) 
	{
		
	}
	
	void DrawerDrawImplementation::createGeometry() {
		osg::Vec2 size = getWidget()->getSize();
		
		_textgeode = new cefix::Utf8TextGeode(_font, _fontsize);
		_textgeode->setText(getWidget()->getCaption());
		_textgeode->setTextColor(_textColor);
		_textTransform = new osg::MatrixTransform();
		_textTransform->addChild(_textgeode.get());
		
		float tab_offset = getWidget()->getTabOffset();
		
		switch(getWidget()->getTabPosition()) {
			case DrawerWidget::TOP:
				_textgeode->setPosition(_textOffset+osg::Vec3(_tabOffset[0] + tab_offset , size[1], 1));
				break;
			case DrawerWidget::BOTTOM:
				_textgeode->setPosition(_textOffset+osg::Vec3(_tabOffset[0] + tab_offset, -_tabHeight, 1));
				break;
			case DrawerWidget::LEFT:
				_textgeode->setPosition(_textOffset+osg::Vec3(_tabOffset[0] + tab_offset,0 , 1));
				_textTransform->setMatrix(osg::Matrix::rotate(osg::PI/2, osg::Vec3(0,0,1)));
				break;
			case DrawerWidget::RIGHT:
				_textgeode->setPosition(osg::Vec3(-_textOffset[0], _textOffset[1], _textOffset[2])+osg::Vec3(-_tabOffset[0] + tab_offset, size[0] , 1));
				_textgeode->setTextAlignment(cefix::AsciiTextGeode::RenderIterator::ALIGN_RIGHT);
				_textTransform->setMatrix(osg::Matrix::rotate(-osg::PI/2, osg::Vec3(0,0,1)));
				break;
		}
		
		osg::Geode* geode = new osg::Geode();
		
		_background->setLocZ(-1);
		_tab->setLocZ(0);
		
		geode->addDrawable(_background.get());
		
		_tabTransform = new osg::MatrixTransform();
		osg::Geode* tabgeode = new osg::Geode();
		tabgeode->addDrawable(_tab.get());
		_tabTransform->addChild(tabgeode);
		
		updateTab();
		
		tabgeode->setUserData(this);
		dynamic_cast<osg::Group*>(getWidget()->getNode())->addChild(geode);
		dynamic_cast<osg::Group*>(getWidget()->getNode())->addChild(_textTransform.get());
		dynamic_cast<osg::Group*>(getWidget()->getNode())->addChild(_tabTransform.get());
		
	}
	
	
	void DrawerDrawImplementation::down() {
		stopPropagation();
	}
	
	void DrawerDrawImplementation::up(bool inside) {
		bool doFlipFlap = true;
		/*
		switch(getWidget()->getTabPosition()) {
			case DrawerWidget::TOP:
				if (hit().localIntersectionPoint[1] > getWidget()->getSize()[1])
					doFlipFlap = true;
				break;
			case DrawerWidget::BOTTOM:
				if (hit().localIntersectionPoint[1] < 0)
					doFlipFlap = true;
				break;
			case DrawerWidget::LEFT:
				if (hit().localIntersectionPoint[0] < 0)
					doFlipFlap = true;
				break;
			case DrawerWidget::RIGHT:
				if (hit().localIntersectionPoint[0] > getWidget()->getSize()[0])
					doFlipFlap = true;
				break;
		}
		*/
		if (doFlipFlap)
			getWidget()->flipFlap();
	}
	
	
	void DrawerDrawImplementation::updateTab()
	{
		_background->setState(getWidget()->isEnabled() ? ThemedWidgetStateGeometry::NORMAL : ThemedWidgetStateGeometry::NORMAL_DISABLED);
		_tab->setState(getWidget()->isEnabled() ? ThemedWidgetStateGeometry::NORMAL : ThemedWidgetStateGeometry::NORMAL_DISABLED);
		
		osg::Vec2 size = getWidget()->getSize();
		float bw = _background->getFrameWidth();
		float bh = _background->getFrameHeight();
		_background->setRect(-bw, -bh,size[0]+2*bw, size[1]+2*bh);
		
		
		float tl, tt, tr, tb;
		_tabTransform->setMatrix(osg::Matrix::identity());
		
		float tab_offset = getWidget()->getTabOffset();
		
		
		switch(getWidget()->getTabPosition()) {
				
			case DrawerWidget::TOP: 
				
				tl = _tabOffset[0] + tab_offset - _tab->getFrameWidth();
				tr = _tabOffset[0] + tab_offset + _textOffset[0] * 2 + _textgeode->getWidth() + _tab->getFrameWidth();
				tt = size[1] + _tabOffset[1] - _tab->getFrameHeight();
				tb = size[1] + _tabOffset[1] + _tab->getFrameHeight() + _tabHeight;
				break;

			case DrawerWidget::BOTTOM: 
				{
					tl = _tabOffset[0] + tab_offset - _tab->getFrameWidth();
					tr = _tabOffset[0] + tab_offset + _textOffset[0] * 2 + _textgeode->getWidth() + _tab->getFrameWidth();
					tt =  - _tab->getFrameHeight() ;
					tb =  + _tab->getFrameHeight() + _tabHeight;
					_tabTransform->setMatrix(osg::Matrix::scale(1,-1,1) * osg::Matrix::translate(0, - _tabOffset[1], 0));
				}
				break;
			
			case DrawerWidget::LEFT: 
				{
					tl = _tabOffset[0] + tab_offset - _tab->getFrameWidth();
					tr = _tabOffset[0] + tab_offset + _textOffset[0] * 2 + _textgeode->getWidth() + _tab->getFrameWidth();
					tt =  - _tab->getFrameHeight() ;
					tb =  + _tab->getFrameHeight() + _tabHeight;
					_tabTransform->setMatrix(osg::Matrix::rotate(osg::PI_2, osg::Z_AXIS) * osg::Matrix::translate( - _tabOffset[1],0,  0));
				}
				break;
			
			case DrawerWidget::RIGHT: 
				{
					tl = _tabOffset[0] + tab_offset - _tab->getFrameWidth();
					tr = _tabOffset[0] + tab_offset + _textOffset[0] * 2 + _textgeode->getWidth() + _tab->getFrameWidth();
					tt =  - _tab->getFrameHeight() ;
					tb =  + _tab->getFrameHeight() + _tabHeight;
					_tabTransform->setMatrix(osg::Matrix::rotate(-osg::PI_2, osg::Z_AXIS) * osg::Matrix::translate( + _tabOffset[1] + size[0], tr	,  0));
				}
				break;

			default:
				std::cout << "not implemented yet" << std::endl;
				
		};
		_tab->setRect(osg::Vec4(tl,tt,tr, tb));
		
		switch(getWidget()->getTabPosition()) {
			case DrawerWidget::TOP:
				_textgeode->setPosition(_textOffset+osg::Vec3(_tabOffset[0] + tab_offset , size[1], 1));
				break;
			case DrawerWidget::BOTTOM:
				_textgeode->setPosition(_textOffset+osg::Vec3(_tabOffset[0] + tab_offset, -_tabHeight, 1));
				break;
			case DrawerWidget::LEFT:
				_textgeode->setPosition(_textOffset+osg::Vec3(_tabOffset[0] + tab_offset,0 , 1));
				_textTransform->setMatrix(osg::Matrix::rotate(osg::PI/2, osg::Vec3(0,0,1)));
				break;
			case DrawerWidget::RIGHT:
				_textgeode->setPosition(osg::Vec3(-_textOffset[0], _textOffset[1], _textOffset[2])+osg::Vec3(-_tabOffset[0] + tab_offset, size[0] , 1));
				_textgeode->setTextAlignment(cefix::AsciiTextGeode::RenderIterator::ALIGN_RIGHT);
				_textTransform->setMatrix(osg::Matrix::rotate(-osg::PI/2, osg::Vec3(0,0,1)));
				break;
		}
		
		
	}
	
	#pragma mark ThemedDragWidgetDrawImplementation
	
	ThemedWindowTitlebarWidgetDrawImplementation::ThemedWindowTitlebarWidgetDrawImplementation()
	:	osg::Geode(),
		DragWidgetDraggable(),
		_inside(false),
		_down(false)
	{
		WidgetTheme* theme = WidgetThemeManager::instance()->getCurrentTheme();
		_back = theme->createStateGeometry("windowtitlebar");	
		_deco = theme->createStateDecorationGeometry("windowtitlebarDecoration");	
		addDrawable(_back.get());
		addDrawable(_deco.get());
	}
	
	void ThemedWindowTitlebarWidgetDrawImplementation::setThemeKey(const std::string& theme_key) 
	{
		WidgetTheme* theme = WidgetThemeManager::instance()->getCurrentTheme();
		theme->updateStateGeometry(_back.get(), "theme_key");	
	}
	
	void ThemedWindowTitlebarWidgetDrawImplementation::update(DragWidget* widget) {
		_widget = widget;
		osg::Vec3 p =_pos = widget->getPosition();
		osg::Vec2 s = widget->getSize();
		float fw = _back->getFrameWidth();
		float fh = _back->getFrameHeight();
		s[0] += 2*fw;
		s[1] += 2*fh;
		p[0] -= fw;
		p[1] -= fh;
		
		_back->setPositionAndSize(p, s);
		_back->setState(
			widget->isEnabled() 
				? (_down) 
					? ThemedWidgetStateGeometry::NORMAL_DOWN 
					: (_inside)
						? ThemedWidgetStateGeometry::NORMAL_OVER 
						: ThemedWidgetStateGeometry::NORMAL  
				
				: ThemedWidgetStateGeometry::NORMAL_DISABLED
		);
		_deco->setState(_back->getState());
		_deco->setPositionAndSize(widget->getPosition(), widget->getSize());
	}


#pragma mark ThemedTextInputWidget

class PulsatingCursorCallback : public osg::NodeCallback {

public:
	PulsatingCursorCallback(ThemedTextInputWidgetImplementation* impl) : osg::NodeCallback(), _impl(impl) {}
	
	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) {
		_impl->pulsateCursor();
		traverse(node, nv);
	}
private:
	ThemedTextInputWidgetImplementation* _impl;
};

	
	
	ThemedTextInputWidgetImplementation::ThemedTextInputWidgetImplementation(TextInputWidget* widget)
	:	TextInputWidget::Implementation(widget)
	{
		_group = new osg::Group();
		osg::Geode* geode = new osg::Geode();
		
		WidgetTheme* theme = WidgetThemeManager::instance()->getCurrentTheme();
		_back = theme->createStateGeometry("textinputwidgetback");
		_cursor = theme->createStateGeometry("textinputwidgetcursor");	
		
		geode->addDrawable(_back.get());
		geode->addDrawable(_cursor.get());
		geode->setUpdateCallback(new PulsatingCursorCallback(this));
		osg::Depth* d= new osg::Depth();
		d->setWriteMask(false);
		_cursor->getOrCreateStateSet()->setAttributeAndModes(d);
		
		geode->setUserData(new TextInputWidget::Pickable<ThemedTextInputWidgetImplementation>(this));
		
		_group->addChild(geode);
		
		_textGeode = new cefix::UnicodeTextGeode(theme->getPrefs("textinputWidget", "font","system.xml"), theme->getPrefs("textinputWidget", "fontsize",0));
		_textGeode->setTextColor(theme->getPrefs("textinputwidget", "textcolor", rgb(0xffffff)));
        _textGeode->getOrCreateStateSet()->setAttributeAndModes(d);
		_group->addChild(_textGeode.get());
		
		_inset = theme->getPrefs("textinputWidget", "inset",osg::Vec2(2,2));
		_cursorWidth = theme->getPrefs("textinputWidget", "cursorWidth", 1.0f);
	}
	
	
	void ThemedTextInputWidgetImplementation::update()
	{
		_back->setState( (getWidget()->hasFocus()) ? ThemedWidgetStateGeometry::SELECTED : ThemedWidgetStateGeometry::NORMAL);
		_cursor->setState( (getWidget()->isEnabled()) ? ThemedWidgetStateGeometry::NORMAL : ThemedWidgetStateGeometry::NORMAL_DISABLED);
		
		_textGeode->setText(getWidget()->getValue());
		
		osg::Vec3 pos = getWidget()->getPosition();
		osg::Vec2 size = getWidget()->getSize();
		_back->setPositionAndSize(pos, size);
		
		int cp = getWidget()->getCursorPosition();
		
		float dy = (cp >= 0) ? _textGeode->getFont()->getWidth( getWidget()->getValue().substr(0, cp+1)) : 0;
	
		float w = _cursor->getFrameWidth();
		// float h = _cursor->getFrameHeight();
		
		_cursor->setPositionAndSize(pos+osg::Vec3(dy-w+_inset[0], 0, 2), osg::Vec2(_cursorWidth+2*w, size[1]));
		_textGeode->setPosition(osg::Vec3(pos + osg::Vec3(_inset[0], _inset[1], 10)));
		
	}
	
	
	void ThemedTextInputWidgetImplementation::applyPropertyList(PropertyList* pl) 
	{
		if (pl->hasKey("inset")) _inset = pl->get("inset")->asVec2();
		if (pl->hasKey("fontname")) _textGeode->setFont(pl->get("fontname")->asString());
		if (pl->hasKey("fontsize")) _textGeode->setFontSize(pl->get("fontsize")->asFloat());
		if (pl->hasKey("textcolor")) _textGeode->setTextColor(pl->get("textcolor")->asVec4());
		
		update();
		
	}
	
	void ThemedTextInputWidgetImplementation::pulsateCursor()
	{
		osg::Vec4 c(1,1,1,1);
		if (getWidget()->hasFocus()) {
			float dt = cefix::AnimationTimeBase::instance()->getCurrentTime();
			c[3] = 0.5*sin(dt * 10.0) + 0.5;
			_cursor->setColor(c);
		} else {
			c[3] = 0.2;
			_cursor->setColor(c);
		}
	}
		
}
#pragma mark ThemedTextWidget
	
	ThemedTextWidget::ThemedTextWidget(const std::string& identifier, osg::Vec3 pos, std::string fontname, float fontsize)
	:	TextWidget(identifier, pos, fontname, fontsize)
	{
		WidgetTheme* theme = WidgetThemeManager::instance()->getCurrentTheme();
		getTextGeode()->setFont(theme->getPrefs("textwidget", "font", fontname));
		getTextGeode()->setFontSize(theme->getPrefs("textwidget", "fontsize", fontsize));
		setPrecision(theme->getPrefs("textwidget", "precision", getPrecision()));
		getTextGeode()->setTextColor(theme->getPrefs("textwidget", "color", rgb(0x000000)));
		_uppercase = theme->getPrefs("textwidget", "uppercase", false);
		
		setCaption(getCaption());
	}
	
	ThemedTextWidget::ThemedTextWidget(const std::string& identifier, std::string text, const osg::Vec3& pos, const osg::Vec4& color, std::string fontname, float fontsize)
	:	TextWidget(identifier, text, pos, color, fontname, fontsize)
	{
		WidgetTheme* theme = WidgetThemeManager::instance()->getCurrentTheme();
		getTextGeode()->setFont(theme->getPrefs("textwidget", "font", fontname));
		getTextGeode()->setFontSize(theme->getPrefs("textwidget", "fontsize", fontsize));
		setPrecision(theme->getPrefs("textwidget", "precision", getPrecision()));
		getTextGeode()->setTextColor(theme->getPrefs("textwidget", "color", rgb(0x000000)));
		_uppercase = theme->getPrefs("textwidget", "uppercase", false);
		setCaption(getCaption());
	}
	
	ThemedTextWidget::ThemedTextWidget(cefix::PropertyList* pl)
	:	TextWidget(pl)
	{
		WidgetTheme* theme = WidgetThemeManager::instance()->getCurrentTheme();
		
		
		if (!pl->hasKey("font")) 
			getTextGeode()->setFont(theme->getPrefs("textwidget", "font", "system.xml"));
		
		if (!pl->hasKey("fontsize"))
			getTextGeode()->setFontSize(theme->getPrefs("textwidget", "fontsize", 0.0f));
		
		if (!pl->hasKey("precision"))
			setPrecision(theme->getPrefs("textwidget", "precision", getPrecision()));
		
		if (!pl->hasKey("color"))
			getTextGeode()->setTextColor(theme->getPrefs("textwidget", "color", rgb(0x000000)));

		if (!pl->hasKey("uppercase"))
			_uppercase = theme->getPrefs("textwidget","uppercase", false);
		else
			_uppercase = pl->get("uppercase", false);
		
		setCaption(getCaption());
	}
	
#pragma mark ThemedWindowShadowWidget

class ThemedWindowShadowWidget: public RectControlWidget {

	public:
		ThemedWindowShadowWidget(const std::string& identifier, const osg::Vec3& pos, const osg::Vec2& size, const osg::Vec2& delta = osg::Vec2(0,0));
		
		void setWindowWidget(WindowWidget* ww) { _ww = ww; }
				
		virtual void update() 
		{ 
			_shadow->setState(_ww->isEnabled() ? ThemedWidgetStateGeometry::NORMAL : ThemedWidgetStateGeometry::NORMAL_DISABLED);  
			osg::Vec3 pos = getPosition();
			osg::Vec2 size = getSize();
			float fw = _shadow->getFrameWidth();
			float fh = _shadow->getFrameHeight();
			
			_shadow->setRect(osg::Vec4(pos[0]+_delta[0]-fw, pos[1]+_delta[1]-fh , pos[0] + size[0]+_delta[0]+fh, pos[1] + size[1]+_delta[1]+fh));
			_shadow->setLocZ(pos[2] - 1);
		}
		
		virtual osg::Node* getNode() { return _geode.get(); }
		
	private:
		osg::ref_ptr<ThemedWidgetStateGeometry>	_shadow;
		osg::ref_ptr<osg::Geode>				_geode;
		osg::Vec2								_delta;
		WindowWidget*							_ww;
};


ThemedWindowShadowWidget::ThemedWindowShadowWidget(const std::string& identifier, const osg::Vec3& pos, const osg::Vec2& size, const osg::Vec2& delta) 
:	RectControlWidget(identifier, pos, size),
	_delta(delta)
{
	_geode = new osg::Geode();
	
	WidgetTheme* theme = WidgetThemeManager::instance()->getCurrentTheme();
	_shadow = theme->createStateGeometry("windowbackground");	
	_geode->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	_geode->addDrawable(_shadow.get());
}

#pragma mark ThemedWindowWidget

void ThemedWindowWidget::init(osg::Vec3 pos, osg::Vec2 size) 
{
		
	ThemedWindowTitlebarWidget* dw = new ThemedWindowTitlebarWidget("Titlebar", pos, size);
	setTitleBarWidget(dw);
	
	ThemedWindowShadowWidget* wsw = new ThemedWindowShadowWidget("Shadow", pos, size);
	wsw->setWindowWidget(this);
	setShadowWidget(wsw);
	TextWidget* tw = new ThemedTextWidget("TitleText", pos);
	setTitleTextWidget(tw);

	WidgetTheme* theme = WidgetThemeManager::instance()->getCurrentTheme();
	
	setTitlebarHeight(theme->getPrefs("windowtitlebar", "titlebarHeight", 11.0f));
	setTitleTextDelta(theme->getPrefs("windowtitlebar", "titleTextDelta", osg::Vec3(10,0,1)));
	setTitleBarDelta(theme->getPrefs("windowtitlebar", "titlebarDelta", osg::Vec3(20,0,0)));
	setBackgroundIncludesTitleBarFlag(theme->getPrefs("windowbackground", "includeTitlebar", 0));

	update();
}
	
		
#pragma mark ThemedBoxDrawImplementation
		
class ThemedBoxDrawImplementation : public BoxWidget::Implementation {

public:
	ThemedBoxDrawImplementation(BoxWidget* widget);
	
	virtual void applyPropertyList(cefix::PropertyList* pl) {}
	virtual void update();
	
	void createGeometry();
	osg::Node* getNode() { return _node.get(); }
	
private:
	osg::ref_ptr<osg::Geode> _node;
	osg::ref_ptr<cefix::ThemedWidgetStateGeometry> _background;
};


ThemedBoxDrawImplementation::ThemedBoxDrawImplementation(BoxWidget* widget) : BoxWidget::Implementation(widget) 
{
}

void ThemedBoxDrawImplementation::createGeometry() 
{
	WidgetTheme* theme = WidgetThemeManager::instance()->getCurrentTheme();
	_background = theme->createStateGeometry("boxwidget");
	_background->setLocZ(1);
	_node = new osg::Geode();
	_node->addDrawable(_background.get());
	
	getWidget()->setCaptionDelta(theme->getPrefs("boxwidget", "captionDelta", osg::Vec3(8,8,0) ));
	
	getWidget()->getCaptionWidget()->getTextGeode()->setFont(theme->getPrefs("boxwidget", "Font", "system.xml"));
	getWidget()->getCaptionWidget()->getTextGeode()->setFontSize(theme->getPrefs("boxwidget", "FontSize", 0.0f));	

}

void ThemedBoxDrawImplementation::update() {
	_background->setState( ThemedWidgetStateGeometry::NORMAL) ;
	_background->setRect(0,0,getWidget()->getWidth(), getWidget()->getHeight());
	//std::cout << _background->getRect() << std::endl;
}
	
	
	
#pragma mark typedefs

void registerImplementations() {

	static WidgetFactory::RegisterProxy<ThemedProgressBarWidget> themed_progress_bar_widget("themedprogressbarwidget", "id position size");

	static WidgetFactory::RegisterProxy<ThemedHorizontalSliderWidget > themed_horizontal_slider_widget("themedHorizontalSliderWidget", "id position size min max value");
	static WidgetFactory::RegisterProxy<ThemedVerticalSliderWidget > themed_vertical_slider_widget("themedVerticalSliderWidget", "id position size min max value");
	static WidgetFactory::RegisterProxy<ThemedAreaSliderWidget > themed_area_slider_widget("themedAreaSliderWidget", "id position size hmin hmax vmin vmax value");

	static WidgetFactory::RegisterProxy< ThemedButtonWidget > themed_button_widget("themedButtonWidget", "id position size");
	static WidgetFactory::RegisterProxy< ThemedTextButtonWidget > themed_text_button_widget("themedTextButtonWidget", "id position size caption");
	static WidgetFactory::RegisterProxy< ThemedRadioButtonWidget > themed_radio_button_widget("themedRadioButtonWidget", "id position size caption");
	static WidgetFactory::RegisterProxy< ThemedCheckboxWidget > themed_check_box_widget("themedCheckBoxWidget", "id position size caption");
	static WidgetFactory::RegisterProxy< ThemedDrawerWidget > themed_drawer_widget("themedDrawerWidget", "id collapsedposition expandedposition tabposition size caption");
	static WidgetFactory::RegisterProxy< ThemedImageWidget > themed_image_widget("themedImageWidget", "id position size");
	static WidgetFactory::RegisterProxy< ThemedTextWidget > themed_text_widget("ThemedTextWidget", "id position caption");
	static WidgetFactory::RegisterProxy< ThemedWindowWidget > themed_window_widget("ThemedWindowWidget", "id position title");
	static WidgetFactory::RegisterProxy< ThemedTextInputWidget > themed_text_input_widget("ThemedTextInputWidget", "id position size value");
	static WidgetFactory::RegisterProxy< BoxWidgetT<ThemedBoxDrawImplementation> > themed_box_widget_proxy("themedboxwidget", "id position size caption");

}

}	