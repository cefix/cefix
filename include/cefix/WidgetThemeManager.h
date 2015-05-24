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

#ifndef CEFIX_WIDGET_THEME_MANAGER_HEADER
#define CEFIX_WIDGET_THEME_MANAGER_HEADER

#include <cefix/WidgetTheme.h>
#include <cefix/StringUtils.h>


namespace cefix {
class ThemedWidgetStateGeometry;
class PropertyList;
class WidgetThemeManager : public osg::Referenced {

public:
	typedef std::map<std::string, osg::ref_ptr<WidgetTheme> > ThemesMap;
	
	WidgetThemeManager();
	
	static WidgetThemeManager* instance();
	
	/** load a theme from file filename */
	WidgetTheme* loadTheme(const std::string& filename);
	
	/// set a previously loaded theme to the current
	void setCurrentTheme(const std::string& name) { _currentTheme = name; }
	
	/// get a theme by name
	WidgetTheme* get(const std::string& name);
	
	/// returns true if a theme with name is already reistered
	bool hasTheme(const std::string& name) { return (_themes.find(cefix::strToLower(name)) != _themes.end()); }
	
	/// returns the current theme
	inline WidgetTheme* getCurrentTheme() { return get(_currentTheme); }
	
	/// register a theme
	void registerTheme(const std::string& name, WidgetTheme* theme) 
	{
		_themes[cefix::strToLower(name)] = theme;
	}
	
	
	WidgetTheme* getThemeFromPropertyList(cefix::PropertyList* pl);
	
	// apply a theme or custom appearance to a ThemedWidgetStateGeometry
	void applyCustomThemeOrAppearance(ThemedWidgetStateGeometry* geo, cefix::PropertyList* pl, const std::string subkey = "");
	
	/// load a theme and make it current
	void loadThemeAndMakeCurrent(const std::string& filename);

private:
	ThemesMap _themes;
	std::string _currentTheme;
};


}


#endif