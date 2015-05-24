/*
 *  Settings.cpp
 *  cefix
 *
 *  Created by Stephan Huber on 17.07.06.
 *  Copyright 2006 digitalmind.de // Stephan Maximilian Huber. All rights reserved.
 *
 */

#include <cefix/Settings.h>
#include <osg/ref_ptr>
#include <cefix/AllocationObserver.h>
#include <cefix/PropertyList.h>

namespace cefix {


	Settings::Settings() 
		: osg::Referenced(),
		_quicktimeUsePixelBufferObjects(true),
		_fontsUseTextureRectangle(false),
		_imageFactoryUseTextureRectangle(true),
		_postscriptWriterBackcolor(osg::Vec4(0.3,0.3,0.3,1)),
		_postscriptWriterFrontcolor(osg::Vec4(0.0,0.0,0.0,1)),
		_postscriptWriterOutputPath(""),
		_useCoreVideo(false),
		_reportMissingCodePointsFlag(false),
		_emulateScrollWheel(false),
		_useMipMappedFonts(true),
        _threadedMediaIdling(false),
		_useNPOTTextures(true)
	{
#ifdef CEFIX_FOR_IPHONE
        _quicktimeUsePixelBufferObjects = false;
#endif
	}

    Settings* Settings::instance() {
        static osg::ref_ptr<Settings> s_settings = new Settings();
        return s_settings.get(); 
        
    }
    
    
    void Settings::set(PropertyList* pl) {
    
        if (pl->hasKey("videoUsePixelBufferObjects"))
            _quicktimeUsePixelBufferObjects = pl->get("videoUsePixelBufferObjects")->asInt();
        
        if (pl->hasKey("fontsUseTextureRectangle"))
            _fontsUseTextureRectangle = pl->get("fontsUseTextureRectangle")->asInt();

        if (pl->hasKey("PostscriptWriterBackcolor"))
            _postscriptWriterBackcolor = pl->get("PostscriptWriterBackcolor")->asVec4();
            
        if (pl->hasKey("PostscriptWriterFrontcolor"))
            _postscriptWriterFrontcolor = pl->get("PostscriptWriterFrontcolor")->asVec4();
            
        if (pl->hasKey("PostscriptWriterOutputPath"))
            _postscriptWriterOutputPath = pl->get("PostscriptWriterOutputPath")->asString();
		
		if (pl->hasKey("useCoreVideo"))
			_useCoreVideo = (pl->get("useCoreVideo")->asInt() != 0);

		if (pl->hasKey("reportMissingCodePoints"))
			_reportMissingCodePointsFlag = (pl->get("reportMissingCodePoints")->asInt() != 0);

		if (pl->hasKey("AllocationObserverPrefsKey"))
			cefix::AllocationObserver::instance()->setPrefsKey(pl->get("AllocationObserverPrefsKey")->asString());
			
		if (pl->hasKey("emulateScrollWheel"))
			_emulateScrollWheel = (pl->get("emulateScrollWheel")->asBool());
		
		if (pl->hasKey("useMipMappedFonts"))
			_useMipMappedFonts = (pl->get("useMipMappedFonts")->asBool() );
            
        if (pl->hasKey("useThreadedMediaIdling"))
			_threadedMediaIdling = (pl->get("useThreadedMediaIdling")->asBool() );
		
		if (pl->hasKey("useNPOTTextures"))
			_useNPOTTextures = (pl->get("useNPOTTextures")->asBool() );

    
    }


}