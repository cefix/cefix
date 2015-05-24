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
#ifndef FONT_MANAGER_HEADER
#define FONT_MANAGER_HEADER 1

#include <vector>
#include <string>

#include <osg/Referenced>
#include <osg/Object>
#include <cefix/Log.h>
#include <cefix/Export.h>

#include <cefix/StringUtils.h>


namespace cefix {

class Font;

/** 
 * this class implements a font-manager. it will handle all request for laoding fonts etc.
 * it caches currently loaded fonts so unnecessary loading of font-files is suppressed
 *
 * @author Stephan Maximilian Huber
 */
class CEFIX_EXPORT FontManager : public osg::Referenced {

public:
    FontManager();

    struct RewriteFontFileNameCallback : public osg::Referenced {
    RewriteFontFileNameCallback(): osg::Referenced() {}
        virtual std::string operator()(const std::string& file_name) = 0;
    };
    
    /**
     * returns an instance to the FontManager, create a new instance if necessary
     * @return an instance of the FontManager-singleton
     */
    static FontManager* instance(bool erase = false);
    /**
     * loads a font (image and kerning-informations)
     * if the font was loaded before, a cached instance is returned
     * the code looks for the font-files at various places
     * @param fontName the fontName of the font, should ba a valid filename
     * @return the loaded font, NULL if the font could not be loaded
     */
    Font* loadFont(const std::string& fontName); 
    
    /** sets the default font which is returned, when the specified font could not be loaded */
    void setDefaultFont(Font* font) { _defaultFont = font; }
    
    /** sets the default font which is returned, when the specified font could not be loaded */
    
    void setDefaultFont(const std::string& font) { setDefaultFont(getFont(font)); }
    
    /** registers a custom font */
    void registerFont(const std::string& fontname, Font* font) ;	
    
    Font* cloneFont(const std::string& sourceFontName, const std::string& targetFontName) {
        return cloneFont(loadFont(sourceFontName), targetFontName);
    }
    
    Font* cloneFont(Font* sourceFont, const std::string& targetFontName);
    
    bool hasFont(const std::string& fontName);
    
    void setRewriteFontFileNameCallback(RewriteFontFileNameCallback* cb) { _rewriteFontFileNameCallback = cb; }

protected:
    virtual ~FontManager() { 
        //osg::notify(osg::ALWAYS) << "clearing FontManager" << std::endl;
        _fonts.clear(); 
    }	
    
private:
    typedef std::map<std::string, osg::ref_ptr<Font> >          FontMap;
                /**
     * looks up a font with specified filename
     * @param fileName the fileName of the requested font
     * @return NULL if not found, otherwise a reference to the loaded font
     */
    Font* getFont(const std::string& fileName);
    /** the list of already loaded fonts */
    FontMap _fonts;
    
    osg::ref_ptr<Font>		_defaultFont;
    OpenThreads::Mutex      _mutex;
    
    osg::ref_ptr<RewriteFontFileNameCallback> _rewriteFontFileNameCallback;

};
		

}

#endif

