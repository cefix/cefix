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

#ifndef TEXT_INPUT_BASE_MANAGER_HEADER
#define TEXT_INPUT_BASE_MANAGER_HEADER

#include <cefix/TextInputBaseWidget.h>
#include <osgViewer/View>

namespace cefix {

class TextInputWidgetManager : public osgGA::GUIEventHandler {
		
	public:
		typedef std::map<TextInputBaseWidget*, osg::Node*> WidgetNodeMap;
		typedef std::map<osg::Node*, TextInputBaseWidget*> NodeWidgetMap;
		typedef std::map<TextInputBaseWidget*, osgGA::GUIActionAdapter*> WidgetAAMap;
		typedef std::list<TextInputBaseWidget*> WidgetSet;
		typedef std::map<osgGA::GUIActionAdapter*, WidgetSet> GUIActionAdapterWidgetsMap;
		typedef std::map<osgGA::GUIActionAdapter*, osg::observer_ptr<TextInputBaseWidget> > GUIActionAdapterFocusedWidget;
		
		typedef std::vector< osg::ref_ptr< TextInputBaseWidget > > TextInputWidgetVector;
		
		TextInputWidgetManager() : osgGA::GUIEventHandler(), _firstRun(true) {}
		
		static TextInputWidgetManager* instance();
		
		virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa);
		
		TextInputBaseWidget* getTextWidgetWithFocus(osgGA::GUIActionAdapter* aa) 
		{
			// std::cout << "currently in focus: ";
			/*
			for(GUIActionAdapterFocusedWidget::iterator j = _aaFocusedWidgetMap.begin(); j != _aaFocusedWidgetMap.end(); ++j) {
				std::cout << j->second << " ";
			}
			std::cout << std::endl;
			*/
			GUIActionAdapterFocusedWidget::iterator i = _aaFocusedWidgetMap.find(aa);
			return (i != _aaFocusedWidgetMap.end()) ? (i->second.get()) : NULL;
		}
		
		void setFocus(TextInputBaseWidget* tw);
		
		void clearFocus() 
		{
			for(GUIActionAdapterFocusedWidget::iterator i = _aaFocusedWidgetMap.begin(); i != _aaFocusedWidgetMap.end(); ++i) {
				i->second->setFocus(false);
			}
			_aaFocusedWidgetMap.clear();
		}
		
		void registerTextInputWidget(TextInputBaseWidget* w);
		
		void unregisterTextInputWidget(TextInputBaseWidget* w);
	
	protected:
		void computeMaps(osg::Node* node,osgGA::GUIActionAdapter* aa);
		
		WidgetNodeMap					_widgetNodeMap;
		NodeWidgetMap					_nodeWidgetMap;
		WidgetAAMap						_widgetAAMap;
		GUIActionAdapterWidgetsMap		_aaWidgetMap;
		GUIActionAdapterFocusedWidget	_aaFocusedWidgetMap;
		TextInputBaseWidget*				_lastFocus;
		bool _dirty;
		TextInputWidgetVector			_refs;
		bool							_firstRun;
	friend class FindTextWidgetsNodeVisitor;
};

}



#endif