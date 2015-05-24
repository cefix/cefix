/*
 *  WidgetFactory.cpp
 *  cefix
 *
 *  Created by Stephan Huber on 30.09.07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "WidgetFactory.h"
#include <osgDB/FileUtils>
#include <cefix/GroupWidget.h>
#include <cefix/Drawerwidget.h>
#include <cefix/AreaSliderWidget.h>
#include <cefix/GroupWidget.h>
#include <cefix/TextButtonWidget.h>
#include <cefix/CheckBoxWidget.h>
#include <cefix/DragWidget.h>
#include <cefix/HorizontalSliderWidget.h>
#include <cefix/ImageWidget.h>
#include <cefix/RadioButtonGroupWidget.h>
#include <cefix/TextWidget.h>
#include <cefix/VerticalSliderWidget.h>
#include <cefix/ViewWidget.h>
#include <cefix/WindowWidget.h>
#include <cefix/VecXSliderWidget.h>
#include <cefix/HSliderWithCaptionAndValueWidget.h>
#include <cefix/RectangleSelectionWidget.h>
#include <cefix/SequenceGrabberWidget.h>
#include <cefix/ProgressBarWidget.h>
#include <cefix/TextInputWidget.h>
#include <cefix/BoxWidget.h>
#include <cefix/DataFactory.h>


namespace cefix {

	class RegisterKnownWidgets {

	public:
		RegisterKnownWidgets(WidgetFactory* f) : _f(f) {
			r<cefix::SimpleDrawerWidget>("simpledrawerwidget", "id collapsedposition expandedposition tabposition size caption");
			r<cefix::SimpleAreaSliderWidget>("SimpleAreaSliderWidget", "id position size hmin hmax vmin vmax throbbersize value");
			r<cefix::SimpleButtonWidget>("simplebuttonwidget", "id position size");
			r<cefix::SimpleCheckboxWidget>("simplecheckboxwidget", "id position size");
			r<cefix::SimpleCheckboxWidget>("simpleradiobuttonwidget", "id position size");
			r<cefix::SimpleDragWidget>("simpledragwidget", "id position size");
			r<cefix::GroupWidget>("groupwidget", "id");
			r<cefix::SimpleHorizontalSliderWidget>("SimpleHorizontalSliderWidget", "id position size min max throbbersize value");
			r<cefix::SimpleImageWidget>("simpleimagewidget", "id position size");
			r<cefix::RadioButtonGroupWidget>("radiobuttongroupwidget", "id");
			r<cefix::SimpleTextButtonWidget>("simpletextbuttonwidget", "id position size caption");
			r<cefix::TextWidget>("simpletextWidget", "id position caption");
			r<cefix::TransformWidget>("transformwidget", "id");
			r<cefix::SimpleVerticalSliderWidget>("SimpleVerticalSliderWidget", "id position size min max throbbersize value");
			r<cefix::ViewWidget>("viewWidget", "id position size mode rendermode");
			r<cefix::SimpleWindowWidget>("SimpleWindowWidget", "id position size title");
			
			r<cefix::HSliderWithCaptionAndValueWidget>("hsliderwithcaptionandvaluewidget", "id position size throbbersize caption");
			r<cefix::HSliderWithCaptionAndValueWidget>("horizontalsliderwithcaptionandvaluewidget", "id position size throbbersize caption");
			
			r<cefix::Vec2SliderWidget>("Vec2SliderWidget", "id position size deltaY captions");
			r<cefix::Vec3SliderWidget>("Vec3SliderWidget", "id position size deltaY captions");
			r<cefix::Vec4SliderWidget>("Vec4SliderWidget", "id position size deltaY captions");
			
			r<cefix::SimpleRectangleSelectionWidget>("rectangleselectionwidget", "id position constrainingrect bottomleft topleft bottomright topright");
			
			r<cefix::SequenceGrabberWidget>("sequencegrabberwidget", "id position previewsize");
			
			r<cefix::SimpleProgressBarWidget>("simpleprogressbarwidget", "id position size");
			r<SimpleTextInputWidget>("simpletextinputwidget", "id position value");
			
			r< cefix::SimpleBoxWidget >("boxwidget", "id position size caption");

	}
	
	template <class T> void r(const std::string& key, const std::string& requiredKeys) {
		_f->registerWidgetClass(key, new WidgetFactory::WidgetCreatorBaseT<T>(), requiredKeys);
	}
	private:
		WidgetFactory* _f;
	
};


// ----------------------------------------------------------------------------------------------------------
// ctor
// ----------------------------------------------------------------------------------------------------------

WidgetFactory::WidgetFactory() : osg::Referenced() {
	
	_factory = new WidgetFactoryImplementation();
	setWidgetImplementationSet("simple");
	
	RegisterKnownWidgets registrar(this);

}

// ----------------------------------------------------------------------------------------------------------
// instance
// ----------------------------------------------------------------------------------------------------------

WidgetFactory* WidgetFactory::instance() {
	static osg::ref_ptr<WidgetFactory> s_ptr = new WidgetFactory();
	return s_ptr.get();
}


// ----------------------------------------------------------------------------------------------------------
// create
// ----------------------------------------------------------------------------------------------------------

AbstractWidget* WidgetFactory::createWidgets(cefix::AbstractProperty* p, WidgetResponderVector& widgetResponder, WidgetMap& widgetMap) 
{
	if (p->getType() != cefix::AbstractProperty::PROPERTY_LIST) 
	{
		log::error("WidgetFactory::createWidgets") << "could not create Widget, Property is of wrong type" << std::endl;
		return NULL;
	}
	
	std::string obj_type = p->getKey();
	if (obj_type.empty()) {
		log::error("WidgetFactory::createWidgets") << "could not create Widget, no xml-key found in property" << std::endl;
		return NULL;
	}
	
	cefix::PropertyList* pl = p->asPropertyList();
	if (!pl->hasKey("id")) {
		static int sid = 0;
		++sid;
		std::ostringstream ss;
		ss << "widget_" << sid;
		pl->add("id", ss.str());
	}
	
	// erstmal im Implementation-set gucken, ob wir eine implementierung haben, dann erst die generalisierte
	if (_requiredKeysMap.find(strToLower(_currentWidgetImplementationSet + obj_type)) != _requiredKeysMap.end())
		obj_type = _currentWidgetImplementationSet + obj_type;
	
	// propertyListe überprfen
	
	std::vector<std::string> required;
	strTokenize(_requiredKeysMap[strToLower(obj_type)], required, " ");
	for(std::vector<std::string>::iterator i = required.begin(); i != required.end(); ++i) {
		if (pl->hasKey(*i) == false) {
			log::error("WidgetFactory::createWidgets") << "could not create widget of type '" << obj_type << "', '"<< *i << "' is missing ... " << std::endl;
			return NULL;
		}
	}
	
	osg::ref_ptr<cefix::AbstractWidget> widget = _factory->create(strToLower(obj_type), pl);
	if (!widget.valid()) {
		log::error("WidgetFactory::createWidgets") << "could not create widget of type '" << obj_type << "' ... " << std::endl;
		_factory->dumpTypes();
		return NULL;
	}
	
	//pl->dump();
	
	// haben wir kinder?
	if (pl->hasKey("widgets")) 
	{
		cefix::GroupWidget* group = dynamic_cast<cefix::GroupWidget*>(widget.get());
		if (group == NULL) 
		{
			log::error("WidgetFactory::createWidgets") << "ignoring childwidgets for widget'" << widget->getIdentifier() << "', parent is not a group" << std::endl;
		}
		else 
		{
			cefix::PropertyList* sub_pl = pl->get("widgets")->asPropertyList();
			if (sub_pl) 
			{
				for(unsigned int i = 0; i < sub_pl->size(); ++i) 
				{
					osg::ref_ptr<cefix::AbstractWidget> childWidget = createWidgets(sub_pl->get(i), widgetResponder, widgetMap);
					if (childWidget.valid())
						group->add(childWidget.get());
				}
			}
		}
	}
	
	if (pl->hasKey("responder")) 
	{
		cefix::PropertyList* sub_pl = pl->get("responder")->asPropertyList();
		if (sub_pl) 
		{
			for(unsigned int i = 0; i < sub_pl->size(); ++i) 
			{
				if (sub_pl->getKeyAt(i) == "responder") {
					widgetResponder.push_back(std::make_pair(widget, sub_pl->get(i)->asString()));
				}
			}
		}
	}
	
	widgetResponder.push_back(std::make_pair(widget, "_default"));
	
	cefix::AbstractWidget::Responder* responder = dynamic_cast<cefix::AbstractWidget::Responder*>(widget.get());
	if (responder)
		widgetMap[widget->getIdentifier()] = responder;
	
	return widget.release();
}


void WidgetFactory::addResponder(ResponderMap& responderMap, WidgetResponderVector& widgetResponder, WidgetMap& widgetMap) 
{	
	
	#ifdef DEBUG_WIDGET_FACTORY
	
	for(ResponderMap::iterator itr = responderMap.begin(); itr != responderMap.end(); ++itr) {
		std::cout << "respondermap" << itr->first << " -> " << itr->second.get() << std::endl;
	}

	#endif

	for(WidgetResponderVector::iterator i = widgetResponder.begin(); i != widgetResponder.end(); ++i)
	{
		osg::ref_ptr< AbstractWidget > widget = i->first;
		std::string responderName = i->second;
		
		#ifdef DEBUG_WIDGET_FACTORY
			std::cout << "try to add responder " << responderName << " to " << widget->getIdentifier() << std::endl;
		#endif
		
		bool found(false);
		// in vom user übergebener respondermap nach einem responder suchen
		ResponderMap::iterator resp_itr = responderMap.find(responderName);
		while ((resp_itr != responderMap.end()) && (resp_itr->first == responderName)) {
			cefix::AbstractWidget::Responder* responder = resp_itr->second;
			widget->addResponder(responder);
			found = true;
			++resp_itr;
		} 
		
		// in der aus den generierten Widgets nach einem passenden Responder suchen
		
		if (widgetMap.find(responderName) != widgetMap.end()) {
			widget->addResponder(widgetMap[responderName].get());
			found = true;
		} 		
		if ((!found) && (responderName != "_default")) {
			log::error("WidgetFactory::addResponder") << "could not add responder '" << responderName << "' to widget '" << widget->getIdentifier() << "', no responder found/given" << std::endl;
		}
	}
	

	
}


cefix::AbstractWidget* WidgetFactory::create(cefix::AbstractProperty* p, cefix::AbstractWidget::Responder* responder) {
	ResponderMap respondermap;
	if (responder) respondermap.add("_default", responder);
	return create(p, respondermap);
}



cefix::AbstractWidget* WidgetFactory::create(cefix::AbstractProperty* p,  ResponderMap& respondermap) {
	ResponderMap myResponder(respondermap);
	// _factory->dumpTypes();
	WidgetResponderVector	widgetResponder;
	WidgetMap				widgetMap;
	osg::ref_ptr<AbstractWidget> w = createWidgets(p, widgetResponder, widgetMap);
	addResponder(myResponder, widgetResponder, widgetMap);

	widgetMap.clear();
	widgetResponder.clear();

	return w.release();
}

cefix::AbstractWidget* WidgetFactory::createFromPrefsKey(const std::string& prefsKey, ResponderMap& respondermap)
{
	cefix::AbstractProperty* p = cefix::DataFactory::instance()->getPreferencesFor(prefsKey);
	if (p && (p->getType() == cefix::AbstractProperty::PROPERTY_LIST)) {
		return create(p, respondermap);
	} 
	
	log::error("WidgetFactory::createFromPrefsKey") << "prefs-key not found " << prefsKey << std::endl;
	return NULL;
}


cefix::AbstractWidget* WidgetFactory::createFromXMLFile(const std::string& filename, ResponderMap& respondermap)
{
	std::string file = osgDB::findDataFile(filename);
	if (file.empty()) {
		log::error("WidgetFactory::createFromXMLFile") << "could not find file " << filename << std::endl;
		return NULL;
	}
	osg::ref_ptr<cefix::PropertyList> pl = new cefix::PropertyList(filename);
	if (pl.valid()) {
		osg::ref_ptr< cefix::PropertyT<osg::Referenced*> > p;
		p = new cefix::PropertyT<osg::Referenced*>(cefix::AbstractProperty::PROPERTY_LIST, pl);
		return create(p, respondermap);
	} 
	
	return NULL;
}
		
		

}