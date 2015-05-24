/*
 *  FontManager.cpp
 *  NativeContext
 *
 *  Created by Stephan Huber on Wed Jan 08 2003.
 *  Copyright (c) 2003 digital mind. All rights reserved.
 *
 */

#include <cefix/Font.h>
#include <cefix/FontManager.h>
#include <cefix/Log.h>
#include <osgDB/FileUtils>
#include <cefix/StringUtils.h>

#include "SystemFont.h"

namespace cefix {


FontManager::FontManager()
    : osg::Referenced()
    , _defaultFont(NULL)
{
}

FontManager* FontManager::instance(bool erase)
{
    static osg::ref_ptr<FontManager> s_fontManager = new FontManager();
    if (erase) {
        s_fontManager = NULL;
        return NULL;
    }
    else
        return s_fontManager.get();
}


void FontManager::registerFont(const std::string& fontname, Font* font) 
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
    _fonts.insert(std::make_pair(cefix::strToLower(fontname), font));
}


bool FontManager::hasFont(const std::string& fontName)
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
    return (_fonts.find(cefix::strToLower(fontName)) != _fonts.end());
}


// ---------------------------------------------------------------------------
// getFont
// liefert Font mit Namen fileName sonst NUKK
// ---------------------------------------------------------------------------
Font* FontManager::getFont(const std::string& fileName)
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
    
    std::string id = cefix::strToLower(fileName);
    FontMap::iterator itr = _fonts.find(id);
    if (itr == _fonts.end())
        return NULL;
    else
        return itr->second.get();
}


// ---------------------------------------------------------------------------
// loadFont
// lädt Font fontName bzw liefert bereits geladenen zurück
// ---------------------------------------------------------------------------
Font* FontManager::loadFont(const std::string& in_fontName)
{
    std::string fontName = in_fontName;
    if (_rewriteFontFileNameCallback.valid())
        fontName = (*_rewriteFontFileNameCallback)(fontName);
    
    osg::ref_ptr<Font> result = getFont(fontName);

    if (result) return result;
    
    
    
    std::string fileFound = osgDB::findDataFile(fontName);
    if (fileFound.empty()) {
        
        // zweiter Versuch in fonts/
        fileFound = osgDB::findDataFile("fonts/"+fontName);

        if (fileFound.empty()) {
            log::error("FontManager::loadFont") << "could not locate font file '" << fontName << "'" << std::endl;
            return NULL;
        }
    }
    

    result = getFont(fileFound);

    if (result) return result;
    
    // serialize font-loading
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
    result = new Font();
    if (result->load(fileFound)) 
    {
        _fonts.insert(std::make_pair(cefix::strToLower(fontName), result));
        _fonts.insert(std::make_pair(cefix::strToLower(fileFound), result));
        return result;
    }

    return NULL;
        

}

Font* FontManager::cloneFont(Font* sourceFont, const std::string& targetFontName) {

    if (sourceFont == NULL) {
        return NULL;
    }
    
    osg::ref_ptr<Font> cloned = dynamic_cast<cefix::Font*>(sourceFont->clone(osg::CopyOp::SHALLOW_COPY));
    
    if (cloned.valid()) {
        cloned->setName(targetFontName);
        registerFont(targetFontName, cloned.get());
    }
    
    return cloned.get();
}
	

class FontRegisterProxy  { public: FontRegisterProxy(std::string name, cefix::Font* font) { cefix::FontManager::instance()->registerFont(name, font); } };


FontRegisterProxy frp("system.xml", SystemFont::create());
}
