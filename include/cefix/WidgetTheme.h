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

#ifndef CEFIX_WIDGET_THEME_HEADER
#define CEFIX_WIDGET_THEME_HEADER

#include <cefix/PropertyList.h>
#include <cefix/ThemedWidgetStateGeometry.h>
#include <cefix/ThemedWidgetStateDecorationGeometry.h>
namespace cefix {

class WidgetTheme : public osg::Referenced {

protected:
	WidgetTheme();
	bool load(const std::string& filename);
	
public:	
	osg::Texture* getTexture() { return _texture.get(); }
	void computeTexCoords(osg::Vec4 r, osg::Vec2Array::iterator i);
	
	const std::string& getName() { return _name; }
	
	ThemedWidgetStateGeometry* createStateGeometry(const std::string& name);
	ThemedWidgetStateDecorationGeometry* createStateDecorationGeometry(const std::string& name);
	void updateStateGeometry(ThemedWidgetStateGeometry* geo, const std::string& name); 
	
	bool hasDescription(const std::string& d) { return _data->hasKey(d); }
	
	template <typename T> T getPrefs(const std::string& key, const std::string& subkey, T defaultvalue) {
		cefix::PropertyList* pl = _data->get(key)->asPropertyList();
		if (!pl || !pl->hasKey(subkey)) return defaultvalue;
		AbstractProperty* ap = pl->get(subkey);
		PropertyT<T>* t = dynamic_cast<PropertyT<T>* >(ap);
        if (t) return t->data();
        
        T v;
        return (tryCast<T>(ap, v)) ? v : defaultvalue;
	}
	
	std::string getPrefs(const std::string& key, const std::string& subkey, const char* defaultvalue) {
		return getPrefs<std::string>(key, subkey, defaultvalue);
	}
    
	
private:
	unsigned int computeState(const std::string& str, unsigned int ndx);
	
    template<typename T>
    bool tryCast(cefix::AbstractProperty* p, T& v) { return false; }
	
private:
	osg::ref_ptr<osg::Texture2D>	_texture;
	std::string					_name;
	osg::ref_ptr<cefix::PropertyList> _data;
	int _w, _h;

friend class WidgetThemeManager;

};

template<> inline bool WidgetTheme::tryCast<bool>(AbstractProperty* p, bool& value) { value = p->asBool(); return true; }
template<> inline bool WidgetTheme::tryCast<osg::Vec2f>(AbstractProperty* p, osg::Vec2f& value) { value = p->asVec2f(); return true; }
template<> inline bool WidgetTheme::tryCast<osg::Vec3f>(AbstractProperty* p, osg::Vec3f& value) { value = p->asVec3f(); return true; }
template<> inline bool WidgetTheme::tryCast<osg::Vec4f>(AbstractProperty* p, osg::Vec4f& value) { value = p->asVec4f(); return true; }
template<> inline bool WidgetTheme::tryCast<osg::Vec2d>(AbstractProperty* p, osg::Vec2d& value) { value = p->asVec2d(); return true; }
template<> inline bool WidgetTheme::tryCast<osg::Vec3d>(AbstractProperty* p, osg::Vec3d& value) { value = p->asVec3d(); return true; }
template<> inline bool WidgetTheme::tryCast<osg::Vec4d>(AbstractProperty* p, osg::Vec4d& value) { value = p->asVec4d(); return true; }

}

#endif