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

#ifndef CEFIX_TEXT_INPUT_WIDGET_HEADER
#define CEFIX_TEXT_INPUT_WIDGET_HEADER

#include <osgViewer/Viewer>
#include <cefix/AbstractWidget.h>
#include <cefix/PropertyList.h>
#include <cefix/Quad2DGeometry.h>
#include <cefix/TextGeode.h>
#include <cefix/AnimationTimeBase.h>
#include <cefix/TextInputBaseWidget.h>
#include <cefix/Pickable.h>

#include <string>

namespace cefix {

class TextInputWidget : public TextInputBaseWidget {

public:
	typedef AbstractWidget::Implementation<TextInputWidget> Implementation;
	typedef std::wstring value_type;
    
	class Actions : public AbstractWidget::Actions {
		
		public:
			static const char* valueChanged() { return "valuechanged"; }
	};
	
	template <class Impl>
	class Pickable : public cefix::Pickable, public osg::Referenced {
		
	public:
		Pickable(Impl* impl) 
		:	cefix::Pickable(),	
		osg::Referenced(), 
		_impl(impl),
		_down(false)
		{
			stopPropagation();
		}
		
		virtual void down() {
			_impl->getWidget()->setFocus(true);
			computeCursorPosition();
			_down = true;
			stopPropagation();
		}
		
		virtual void within() {
			if (_down)
				computeCursorPosition();
		}
		
		virtual void up(bool inside) {
			_down = false;
        }
	private:
		inline void computeCursorPosition() {
			float x = getCurrentHit().getLocalIntersectPoint()[0] - _impl->getWidget()->getPosition()[0];
			std::wstring s(L"");
			std::wstring input(_impl->getWidget()->getValue());
			int found = -1;
			int max = 100000;
			for(unsigned int i = 0; i <= input.size(); ++i) {
				float l = _impl->getFont()->getWidth(input.substr(0,i));
				float dx(abs(l-x));
				if (dx < max) {
					max = dx;
					found = i-1;
				}
			}
			// std::cout << "x: " << x << " cusor: " << found <<std::endl;
			
			_impl->getWidget()->setCursorPos(found);
		}
		
		Impl* _impl;
		bool _down;
	};
	
	
	TextInputWidget(const std::string& id, const osg::Vec3& pos, const osg::Vec2& size, const std::wstring& text = L"");
	
	void setValue(const std::wstring& t) { _input = t; computeCursor(); update(); }
	const std::wstring& getValue() const { return _input; }
	
	inline void setCursorPos( int p) { _cursorPos = osg::clampTo<int>(p, -1, _input.size()-1); update(); }
	inline unsigned int getCursorPosition() const { return _cursorPos; }
	virtual osg::Node* getNode();
	virtual void setParent(AbstractWidget* parent);
	virtual void update();
				
	virtual bool handleKey(int key);
	
	void setImplementation(Implementation* impl) { _impl = impl; }

protected:
	void computeCursor();
	
	std::wstring		_input;
	
	int	_cursorPos;
	osg::ref_ptr<Implementation>	_impl;
	bool			_hasFocus, _registered;
};
	
	


class SimpleTextInputWidgetImplementation : public TextInputWidget::Implementation
{
	public:
		SimpleTextInputWidgetImplementation(TextInputWidget* w);
		virtual void update();
		virtual osg::Node* getNode() { return _group.get(); }
		virtual void applyPropertyList (cefix::PropertyList *pl);
		
		void setTextColor(const osg::Vec4& color) { _textColor = color; update(); }
		void setCursorColor(const osg::Vec4& color) { _cursorColor = color; update(); }
		void setFrameColor(const osg::Vec4& color) { _frameColor = color; update(); }
		inline void pulsateCursor()
		{
			osg::Vec4 c(_cursorColor);
			if (getWidget()->hasFocus()) {
				float dt = cefix::AnimationTimeBase::instance()->getCurrentTime();
				c[3] = 0.5*sin(dt * 10.0) + 0.5;
				_cursor->setColor(c);
				_cursor->dirtyDisplayList();
			} else {
				c[3] = 0.2;
				_cursor->setColor(c);
			}
		}
	
		
	cefix::Font* getFont() { return _text->getFont(); }
	
	private: 
		osg::ref_ptr<osg::Group> _group;
		osg::ref_ptr<cefix::Quad2DGeometry> _frame;
		osg::ref_ptr<cefix::Quad2DGeometry> _cursor;
		osg::ref_ptr<cefix::UnicodeTextGeode>  _text;
		
		osg::Vec2 _inset;
		osg::Vec4 _textColor, _cursorColor, _frameColor;
		std::string _fontName;
		float _fontsize;

};


template <class DrawImplementationClass = SimpleTextInputWidgetImplementation> class TextInputWidgetT : public TextInputWidget {
	public:
		TextInputWidgetT(const std::string& identifier, const osg::Vec3& position, const osg::Vec2& size, const std::wstring& text = L"") :
			TextInputWidget(identifier, position, size, text)
		{
			setImplementation(new DrawImplementationClass(this));
		}
		
		TextInputWidgetT(cefix::PropertyList*pl) :
			TextInputWidget(
				pl->get("id")->asString(),
				pl->get("position")->asVec3(),
				pl->get("size")->asVec2(),
				pl->get("value")->asWstring())
		{
			setImplementation(new DrawImplementationClass(this));
			getImplementation()->applyPropertyList(pl);
		}
		
		DrawImplementationClass* getImplementation() { return dynamic_cast<DrawImplementationClass*>(_impl.get()); }
	
};

typedef TextInputWidgetT<> SimpleTextInputWidget;



}


#endif