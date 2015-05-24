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

#ifndef LOGGER_HEADER_
#define LOGGER_HEADER_
#include <fstream>
#include <osg/Referenced>
#include <cefix/Log.h>
#include <cefix/TextGeode.h>
#include <osg/Group>

#include <cefix/AnimationController.h>
#include <osg/Material>
#include <osg/Notify>


namespace cefix {
/** 
 * this class encapsulates logging functionality. It can log arbitrary strings to the console, to a file and/or to a text-geode
 */
class AnimateLogger;

class CEFIX_EXPORT Logger : public osg::NotifyHandler {

public:
    /** the different logger-modes */
    enum LoggerMode { NONE = 0, CONSOLE = 1, FILE = 2, TEXT_GEODE = 4, REDIRECT_NOTIFY = 8 };
    
    /**default constructor*/
    Logger(unsigned int mode = CONSOLE);
    
    /**default constructor*/
    Logger(std::string filename, unsigned int mode = CONSOLE + FILE);
    
    /** adds a string to the log */
    void add(const std::string& s, osg::NotifySeverity severity = osg::WARN);
    
    /** @return the textgeode (if any) */
    Utf8TextGeode* getTextGeode() { return _textGeode.get(); }
    
    
    /** attaches the logger-text-geode to a group, please use this funct */
    void attachTo(osg::Group* g) {
        if (getTextGeode()) {
            _parent = g;
            add("");
        }
    }
    
    
    /** sets the semicolonasDelimiter-flag (useful für excel-style-logfiles) */
    void setSemicolonsAsDelimiterFlag(bool flag) { _useSemicolons = flag; }
    
    /** sets the nr of displayed lines for the text-geode */
    void setMaxLineCount(int lc) { _maxLineCount = lc; }
    
    /** if true the current date and time is included in the file-output */
    void setIncludeDateTimeFlag(bool flag) { _includeDateTime = flag; }
    
    /** notify the Logger, when the textfading is finished, clear the animationObject */
    void textFadeoutEnded();
    
    
    /** sets the text-color for the output */
    void setTextColor(osg::Vec4 c);
    
    /** sets the text-color for the output */
    void applyTextColor(osg::Vec4 c);
    
    void setTextPosition(const osg::Vec3 & p);
    
    void notify (osg::NotifySeverity severity, const char *message)
    {
        add(message, severity);
    }

    
protected:
    /** Destructor, closes the logfile if necessary */
    virtual ~Logger();
    
    /** do some initing */
    void init();
    
    osg::Material* getShadowMaterial() { return _shadowMaterial.get(); }
    
    unsigned int    _mode;
    std::string     _filename;
    bool			_fileOpened;
    std::ofstream   _logfile;
    osg::ref_ptr< Utf8TextGeode > _textGeode, _shadowTextGeode;
    osg::ref_ptr<osg::Group> _parent;
    unsigned int    _maxLineCount;
    bool            _includeDateTime;
    bool            _useSemicolons;
    osg::Vec4       _textcolor;
    osg::ref_ptr<cefix::AnimationController> _ctrl;
    bool			_isAnimating;
    std::streambuf*		_oldbuf;
    std::streambuf*		_olderrbuf; 
    osg::ref_ptr<osg::Group>	_group;
    osg::ref_ptr<osg::Material>	_shadowMaterial;
    
friend class AnimateLogger;
            
};

}

#endif