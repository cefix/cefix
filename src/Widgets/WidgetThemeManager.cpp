/*
 *  WidgetThemeManager.cpp
 *  cefixcvTemplate
 *
 *  Created by Stephan Huber on 23.10.08.
 *  Copyright 2008 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */

#include "WidgetThemeManager.h"
#include <cefix/DataFactory.h>
#include <cefix/ThemedWidgetImplementations.h>
#include <cefix/WidgetFactory.h>

namespace cefix {

WidgetThemeManager::WidgetThemeManager()
:	osg::Referenced(),
	_themes()
{

	registerImplementations();

	/*
	std::string defaultThemeFile = cefix::DataFactory::instance()->getPreferencesFor("/theme", "sw_widget_theme.xml");
	
	osg::ref_ptr<WidgetTheme> theme = loadTheme(defaultThemeFile);
	
	if (theme.valid()) {
		registerTheme(theme->getName(), theme.get());
		setCurrentTheme(theme->getName());
	}
	
	 */ 
}



WidgetThemeManager* WidgetThemeManager::instance() 
{
	static osg::ref_ptr<WidgetThemeManager> s_ptr = new WidgetThemeManager();
	return s_ptr.get(); 
}



WidgetTheme* WidgetThemeManager::loadTheme(const std::string& filename) 
{
	osg::ref_ptr<WidgetTheme> theme = new WidgetTheme();
	if (theme->load(filename)) {
		registerTheme(theme->getName(), theme.get());
		return theme.release();
	}
	return NULL;
}



WidgetTheme* WidgetThemeManager::get(const std::string& name) 
{
	std::string n(name);
	if (n.empty()) n = _currentTheme;
	
	ThemesMap::iterator itr = _themes.find( cefix::strToLower(n) );
	if (itr == _themes.end()) 
	{
		log::error("WidgetThemeManager::get") << "no theme with name " << name  << " registered, using current ... " << std::endl;
	}
	return (itr != _themes.end()) ? itr->second.get() : NULL;
}

	
	WidgetTheme* WidgetThemeManager::getThemeFromPropertyList(cefix::PropertyList* pl)
	{
		WidgetTheme* theme = getCurrentTheme();
		if (pl->hasKey("customTheme")) {
			std::string tn(pl->get("customTheme")->asString());
			if ( hasTheme(tn) )
				theme = get(tn);
			else {
				log::info("WidgetThemeManager::getThemeFromPropertyList") << "theme " << tn << " not found, using current " << std::endl;
			}
		}
		
		return theme;
	}
	

void WidgetThemeManager::applyCustomThemeOrAppearance(ThemedWidgetStateGeometry* geo, cefix::PropertyList* pl, const std::string subkey)
{
	WidgetTheme* theme = getThemeFromPropertyList(pl);	
					
	if (pl->hasKey("customAppearance")) {
		std::string ca(pl->get("customAppearance")->asString());
		if (theme->hasDescription(ca)) 
			theme->updateStateGeometry(geo, ca + subkey );
	}
}





void WidgetThemeManager::loadThemeAndMakeCurrent(const std::string& filename) 
{
	osg::ref_ptr<WidgetTheme> theme = loadTheme(filename);
	if (theme.valid()) {
		cefix::WidgetFactory::instance()->setWidgetImplementationSet("themed");
		setCurrentTheme(theme->getName());
	}
}



}