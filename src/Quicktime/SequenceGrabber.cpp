/*
 *  SequenceGrabberBase.cpp
 *  cefix
 *
 *  Created by Stephan Huber on 17.09.08.
 *  Copyright 2008 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */

#include <cefix/Pixel.h>
#include <cefix/SequenceGrabber.h>
#include <cefix/Serializer.h>
#include <cefix/DataFactory.h>

namespace cefix {

void SequenceGrabber::fakeTracking() 
{
	if (!_image.valid()) return;
	static unsigned int sx = 0;
	++sx;
	Pixel pixel(_image.get()); 
	for(int y = 0; y < _image->t(); ++y) 
	{
		for(int x = 0; x < _image->s(); ++x) 
		{
			pixel.setRGBA(x,y,(sx + x)  % 256, y % 256, y % 256, 255 ); 
		}
	}
	newFrameAvailable();
}


void SequenceGrabber::Selection::writeTo(Serializer& serializer) 
{
	serializer << "topleft" << topLeft;
	serializer << "bottomleft" << bottomLeft;
	serializer << "topright" << topRight;
	serializer << "bottomright" << bottomRight;
}

void SequenceGrabber::Selection::readFrom(Serializer& serializer) 
{
	serializer >> "topleft" >> topLeft;
	serializer >> "bottomleft" >> bottomLeft;
	serializer >> "topright" >> topRight;
	serializer >> "bottomright" >> bottomRight;
}

void SequenceGrabber::writeTo(Serializer& serializer) 
{
	serializer << "gain" << getGain();
	serializer << "exposure" << getExposure();
	serializer << "shutter" << getShutter();
	serializer << "focus" << getFocus();
	serializer << "whitebalance" << getWhiteBalance();
	serializer << "contrast" << getContrast();
	serializer << "brightness" << getBrightness();
	serializer << "width" << getWidth();
	serializer << "height" << getHeight();
	serializer << "name" << getIdentifier();
	serializer << "selection" << _selection;
}


void SequenceGrabber::readFrom(Serializer& serializer) 
{
	float gain, exposure, shutter, focus, whitebalance, contrast, brightness;
	
	serializer >> "gain" >> gain;
	serializer >> "exposure" >> exposure;
	serializer >> "shutter" >> shutter;
	serializer >> "focus" >> focus;
	serializer >> "whitebalance" >> whitebalance;
	serializer >> "contrast" >> contrast;
	serializer >> "brightness" >> brightness;
	serializer >> "selection" >> _selection;
	
	setGain(gain);
	setExposure(exposure);
	setShutter(shutter);
	setFocus(focus);
	setWhiteBalance(whitebalance);
	setContrast(contrast);
	setBrightness(brightness);
}



void SequenceGrabber::saveSettings(const std::string& key)
{
	osg::ref_ptr<cefix::PropertyList> pl = new cefix::PropertyList();
	cefix::Serializer serializer(pl);
	serializer << "grabber" << (*this);
	cefix::DataFactory::instance()->setPreferencesFor(key, pl->get("grabber")->asPropertyList());
}



void SequenceGrabber::loadSettings(const std::string& key)
{
	cefix::DataFactory* df = cefix::DataFactory::instance();
	setGain(df->getPreferencesFor(key+"/gain", -1));
	setExposure(df->getPreferencesFor(key+"/exposure", -1));
	setShutter(df->getPreferencesFor(key+"/shutter", -1));
	setFocus(df->getPreferencesFor(key+"/focus", -1));
	setWhiteBalance(df->getPreferencesFor(key+"/whitebalance", -1));
	setContrast(df->getPreferencesFor(key+"/contrast", -1));
	setBrightness(df->getPreferencesFor(key+"/brightness", -1));
	
	_selection.topLeft = df->getPreferencesFor(key+"/selection/topleft", osg::Vec2(0,0));
	_selection.topRight = df->getPreferencesFor(key+"/selection/topRight", osg::Vec2(_width,0));
	_selection.bottomRight = df->getPreferencesFor(key+"/selection/bottomright", osg::Vec2(_width,_height));
	_selection.bottomLeft = df->getPreferencesFor(key+"/selection/bottomLeft", osg::Vec2(0,_height));
}

}

	