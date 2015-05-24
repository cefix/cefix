/*
 *  TextInputBaseManager.cpp
 *  cefixcvTemplate
 *
 *  Created by Stephan Huber on 31.10.08.
 *  Copyright 2008 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */

#include "TextInputWidgetManager.h"
#include <cefix/ApplicationWindow.h>
#include <cefix/PickEventHandler.h>
#include <cefix/WindowPickable.h>

namespace cefix {

class TextInputWindowPickable : public WindowPickable {

public:
	TextInputWindowPickable() : WindowPickable() {  }
	
	virtual void mouseDown()
	{
		if (getNumPickables() == 0)
			TextInputWidgetManager::instance()->clearFocus();
	}

};

	TextInputWidgetManager* TextInputWidgetManager::instance()
	{
		static osg::ref_ptr<TextInputWidgetManager> s_ptr = new TextInputWidgetManager();
		
		return s_ptr.get();
	}
	
	class FindTextWidgetsNodeVisitor: public osg::NodeVisitor {
	
	public:
		FindTextWidgetsNodeVisitor(TextInputWidgetManager* tiwm, osgGA::GUIActionAdapter* aa) : osg::NodeVisitor(TRAVERSE_ACTIVE_CHILDREN), _tiwm(tiwm), _aa(aa) {}
		
		virtual void apply(osg::Node &node) 
		{
			// std::cout << "visiting: " << (&node) << std::endl;
			TextInputWidgetManager::NodeWidgetMap::iterator itr = _tiwm->_nodeWidgetMap.find(&node);
			if (itr != _tiwm->_nodeWidgetMap.end()) {
				_tiwm->_aaWidgetMap[_aa].push_back(itr->second);
				_tiwm->_widgetAAMap[itr->second] = _aa;
			}
			traverse(node);
		}
	private:
		TextInputWidgetManager* _tiwm;
		osgGA::GUIActionAdapter* _aa;
	};
	
	void TextInputWidgetManager::computeMaps(osg::Node* node,osgGA::GUIActionAdapter* aa) 
	{
		
		_aaWidgetMap.clear();
		_widgetAAMap.clear();
		
		FindTextWidgetsNodeVisitor ftwnv(this, aa);
		node->accept(ftwnv);
		
	}
	
	
	void TextInputWidgetManager::setFocus(TextInputBaseWidget* tw) 
		{
			WidgetAAMap::iterator i = _widgetAAMap.find(tw);
			if (i == _widgetAAMap.end()) {
				_lastFocus = tw;
				return;
			}
			
			WidgetSet widgets = _aaWidgetMap[i->second];
			for(WidgetSet::iterator j = widgets.begin(); j!= widgets.end(); ++j) {
				if (*j != tw)
					(*j)->setFocus(false);
			}
			tw->setFocus(true, true);
			_aaFocusedWidgetMap[i->second] = tw;
		}

	
	bool TextInputWidgetManager::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
	{
		//std::cout << "TextInputWidgetManager::handle " << std::endl<< std::endl;
		if (_dirty) {
			osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
			if (view) computeMaps(view->getSceneData(), view);
			_dirty = false;
			
			if (_firstRun) {
				for(GUIActionAdapterWidgetsMap::iterator i = _aaWidgetMap.begin(); i != _aaWidgetMap.end(); ++i) {
					if (i->second.size() > 0) 
						setFocus(*(i->second.begin()));
				}
				
				ApplicationWindow* win = dynamic_cast<ApplicationWindow*>(&aa);
				if (win) {
					PickEventHandler* picker = win->getPickEventHandler();
					if (picker) picker->setWindowPickable(new TextInputWindowPickable());
				}
				_firstRun = false;
			}
			

		}
		
		if(ea.getEventType() == osgGA::GUIEventAdapter::KEYDOWN) {
			if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Tab) 
			{
				GUIActionAdapterWidgetsMap::iterator i = _aaWidgetMap.find(&aa);
				TextInputBaseWidget* w = getTextWidgetWithFocus(&aa);
				
				if (i != _aaWidgetMap.end()) {
					for(WidgetSet::iterator j = i->second.begin(); j != i->second.end(); ++j) {
						if (*j == w) {
							++j;
							if (j != i->second.end())
								setFocus(*j);
							else
								setFocus(*(i->second.begin()));
							break;	
						}
					}
				}
				
				return true;
			}
			else 
			{
				TextInputBaseWidget* w = getTextWidgetWithFocus(&aa);
				return (w) ? w->handleKey(ea.getKey()) : false;
			}
		}
		return false;
	}




void TextInputWidgetManager::registerTextInputWidget(TextInputBaseWidget* w) { 
	_widgetNodeMap[w] = w->getNode(); 
	_nodeWidgetMap[w->getNode()] = w; 
	_refs.push_back(w); 
	cefix::log::info("TextInputWidgetManager") << "registering: " << (w->getNode()) << std::endl;
	
	_dirty = true; 
}



void TextInputWidgetManager::unregisterTextInputWidget(TextInputBaseWidget* w) 
{ 
	_widgetNodeMap.erase(_widgetNodeMap.find(w)); 
	_nodeWidgetMap.erase(_nodeWidgetMap.find(w->getNode())); 
	cefix::log::info("TextInputWidgetManager") << "deregistering: " << (w->getNode()) << std::endl;
	for(TextInputWidgetVector::iterator i = _refs.begin(); i != _refs.end(); ){
		if (*i == w) {
			i = _refs.erase(i);
		} else {
			++i;
		}
	}
	_dirty = true; 
}
	
} // end of namespace 



