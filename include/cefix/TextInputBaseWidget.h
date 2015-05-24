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

#ifndef TEXT_INPUT_BASE_WIDGET_HEADER
#define TEXT_INPUT_BASE_WIDGET_HEADER

#include <cefix/AbstractWidget.h>

namespace cefix {

class TextInputBaseWidget : public RectControlWidget {
public:
	TextInputBaseWidget(const std::string& id, const osg::Vec3& pos, const osg::Vec2& size);
	
	void setFocus(bool f, bool calledFromManager = false);
	inline bool hasFocus() { return _hasFocus; }
	
	virtual bool handleKey(int key) = 0;
	
protected:
	~TextInputBaseWidget();
	void registerInputWidget();
	void unregisterInputWidget();
private:
	bool _hasFocus, _registered;
};

}


#endif