/*
 *  WidgetTheme.cpp
 *  cefixcvTemplate
 *
 *  Created by Stephan Huber on 23.10.08.
 *  Copyright 2008 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */
#include <osgDB/FileUtils>
#include <osgDB/ReadFile>
#include "WidgetTheme.h"
#include "WidgetDefaults.h"

namespace cefix {


WidgetTheme::WidgetTheme()
:	osg::Referenced(),
	_texture(NULL),
	_name(),
	_data(NULL)
{
}


bool WidgetTheme::load(const std::string& filename) 
{
	std::string fullname = osgDB::findDataFile(filename);
	if (fullname.empty()) 
	{
		log::error("WidgetTheme::load") << "could not find theme: " << filename << std::endl;
		return false; 
	}
	
	_data = new cefix::PropertyList(filename);
	_name = _data->get("name")->asString();
	
	std::string imagefile(_data->get("image")->asString());
	std::string imagefullname = osgDB::findDataFile(imagefile);
	if (imagefullname.empty())
		imagefullname = osgDB::findDataFile( osgDB::getFilePath(fullname) + "/" + imagefile );
	
	if (imagefullname.empty()) {
		log::error("WidgetTheme::load") << "could not find image: " << imagefile << std::endl;
		return false; 
	}
		
	
	osg::ref_ptr<osg::Image>image = osgDB::readImageFile(imagefullname);
	if (!image.valid()) {
		log::error("WidgetTheme::load") << "could not load image: " << imagefullname << std::endl;
		return false; 
	}
	_w = image->s();
	_h = image->t();
	
	_texture = new osg::Texture2D();
	_texture->setImage(image.get());
	_texture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
	_texture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
	_texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
	_texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
	
		
	
	cefix::PropertyList* defaults = _data->get("defaults")->asPropertyList();
	if (defaults) 
	{
		if (defaults->hasKey("sliderthrobbersize")) WidgetDefaults::setSliderThrobberSize(defaults->get("sliderthrobbersize")->asVec2());
		if (defaults->hasKey("checkboxsize")) WidgetDefaults::setCheckboxSize(defaults->get("checkboxsize")->asVec2());
		if (defaults->hasKey("radiobuttonsize")) WidgetDefaults::setRadioButtonSize(defaults->get("radiobuttonsize")->asVec2());
		if (defaults->hasKey("buttonHeight")) WidgetDefaults::setButtonHeight(defaults->get("buttonHeight")->asFloat());
		if (defaults->hasKey("sliderHeight")) WidgetDefaults::setSliderHeight(defaults->get("sliderHeight")->asFloat());
		if (defaults->hasKey("progressbarHeight")) WidgetDefaults::setProgressBarHeight(defaults->get("progressbarHeight")->asFloat());
		if (defaults->hasKey("hsliderCaptionDelta")) WidgetDefaults::setHSliderCaptionDelta(defaults->get("hsliderCaptionDelta")->asVec2());
		if (defaults->hasKey("hsliderValueDelta")) WidgetDefaults::setHSliderValueDelta(defaults->get("hsliderValueDelta")->asVec2());
		if (defaults->hasKey("textInputHeight")) WidgetDefaults::setTextInputHeight(defaults->get("textInputHeight")->asFloat());
	}
	
	_data = (_data.valid()) ? _data->get("items")->asPropertyList() : NULL;

	
	return true;
}



unsigned int WidgetTheme::computeState(const std::string& str, unsigned int ndx) 
{
	static std::map<std::string, unsigned int> s_state_map;
	if (s_state_map.size() == 0) {
		s_state_map["normal_disabled"]		= ThemedWidgetStateGeometry::NORMAL_DISABLED;
		s_state_map["selected_disabled"]	= ThemedWidgetStateGeometry::SELECTED_DISABLED;
		s_state_map["normal"]				= ThemedWidgetStateGeometry::NORMAL;
		s_state_map["selected"]				= ThemedWidgetStateGeometry::SELECTED;
		s_state_map["normal_down"]			= ThemedWidgetStateGeometry::NORMAL_DOWN;
		s_state_map["selected_down"]		= ThemedWidgetStateGeometry::SELECTED_DOWN;
		s_state_map["normal_over"]			= ThemedWidgetStateGeometry::NORMAL_OVER;
		s_state_map["selected_over"]		= ThemedWidgetStateGeometry::SELECTED_OVER;
	}
	
	std::map<std::string, unsigned int>::iterator itr = s_state_map.find(cefix::strToLower(str));
	return (itr == s_state_map.end()) ? ndx : itr->second;
}


	
ThemedWidgetStateGeometry* WidgetTheme::createStateGeometry(const std::string& name) 
{
	osg::ref_ptr<ThemedWidgetStateGeometry> geo = new ThemedWidgetStateGeometry();
	updateStateGeometry(geo.get(), name);
	return geo.release();
}


ThemedWidgetStateDecorationGeometry* WidgetTheme::createStateDecorationGeometry(const std::string& name) 
{
	osg::ref_ptr<ThemedWidgetStateDecorationGeometry> geo = new ThemedWidgetStateDecorationGeometry();
	updateStateGeometry(geo.get(), name);
	osg::Vec2 insets = getPrefs(name, "inset", osg::Vec2(0,0));
	geo->setInsets(insets[0], insets[1]);
	return geo.release();
}

void WidgetTheme::updateStateGeometry(ThemedWidgetStateGeometry* geo, const std::string& name)
{
	unsigned int s = geo->getState();
	geo->setState(ThemedWidgetStateGeometry::UNKNOWN);
	
	cefix::PropertyList* pl  = (_data.valid()) ? _data->get(name)->asPropertyList() : NULL;
	if (!pl) 
	{
		log::error("WidgetTheme::updateStateGeometry") << "invalid/missing xml entries for " << name << std::endl;
	} 
	else
	{
		float h = _h;
		float w = _w;
		osg::Vec4 r = pl->get("rect")->asVec4();
		float bw = pl->get("borderWidth")->asFloat();
		float bh = pl->get("borderHeight")->asFloat();
		osg::Vec2 delta = pl->get("delta")->asVec2();
		
		std::vector<std::string> state_str;
		cefix::strTokenize(pl->get("states")->asString(), state_str, " ");
		unsigned int ndx(0);
		
		geo->clearStates();
		
		for(std::vector<std::string>::iterator i = state_str.begin(); i != state_str.end(); ++i, ++ndx)
		{
			unsigned int state = computeState(*i, ndx);
			osg::Vec4 rr(osg::Vec4(r[0] / w,  (h - r[3]) / h, r[2] / w, (h- r[1]) / h));
			geo->addState(state, rr, bw, bh);
			//std::cout << name << " (" << *i << ") r: " << rr << " bw: " << bw << " bh: " << bh << std::endl;
			r[0] += delta[0];
			r[2] += delta[0];
			r[1] += delta[1];
			r[3] += delta[1];
		}
		
		geo->addMissingStates();
	}
	geo->setName("ThemedWidgetStateGeometry:  "+ name);
	geo->setTexture(_texture.get());
	geo->setState(s);
	
}

}