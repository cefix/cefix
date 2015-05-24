/*
 *  Logger.cpp
 *  AudiHeritage
 *
 *  Created by Stephan Huber on 18.05.06.
 *  Copyright 2006 digitalmind.de // Stephan Maximilian Huber. All rights reserved.
 *
 */
#include <cefix/StringUtils.h>
#include <cefix/AnimationFactory.h>
#include <cefix/Log.h>
#include <osgDB/FileUtils>
#include <osg/observer_ptr>
#include <osg/Material>
#include <osg/MatrixTransform>

#include <cefix/FilterApplier.h>
#include <cefix/ImageFilters.h>
#include <cefix/TextGeode.h>
#include <cefix/Logger.h>

namespace cefix {


/**
 * small animationclass, which will fade out the logger-text
 */
class AnimateLogger : public cefix::AnimationBase {
    
    public:
        /** default constructor */
        AnimateLogger(Logger* logger, osg::Vec4 textcolor) : 
            cefix::AnimationBase(15.0f), 
            _logger(logger), 
            _textcolor(textcolor)
        {
			 //osg::notify(osg::ALWAYS) << "constructing ..." << std::endl; 
			 _logger->applyTextColor(_textcolor);

        }
        
        /** do the animation, fade the logger after 5 sec */
        virtual void animate(float elapsed_time) {
			//osg::notify(osg::ALWAYS) << "animating ... " << elapsed_time << std::endl; 
            float scalar = osg::minimum( elapsed_time / 2.0f, 1.0f);
            if (scalar >= 1.0f) {
                _finished = true;
                return;
            }
            osg::Vec4 c(_textcolor);
            c[3] = _textcolor[3] * (1-scalar);
			if (_logger.valid())
			{
				_logger->applyTextColor(c);

			}
        
        }
		void cleanUp() {
			 //osg::notify(osg::ALWAYS) << "cleanup ..." << std::endl; 
			 if (_logger.valid())
				_logger->textFadeoutEnded();
		}
        
        
        
    protected:
		virtual ~AnimateLogger() { /*osg::notify(osg::ALWAYS) << "Destructing ..." << std::endl;*/ }
        osg::observer_ptr<Logger> _logger;
        osg::Vec4 _textcolor;
};

// --------------------------------------------------------------------------------------------------------------------
// Constructor
// --------------------------------------------------------------------------------------------------------------------

Logger::Logger(unsigned int mode) : 
	osg::NotifyHandler(), 
	_mode(mode), 
	_filename(""), 
	_fileOpened(false),
	_textGeode(NULL),
	_shadowTextGeode(NULL),
	_isAnimating(false),
	_oldbuf(NULL),
	_olderrbuf(NULL)
{
    init();
}

// --------------------------------------------------------------------------------------------------------------------
// Constructor
// --------------------------------------------------------------------------------------------------------------------
Logger::Logger(std::string filename, unsigned int mode) : 
    osg::NotifyHandler(), 
	_mode(mode), 
	_filename(filename), 
	_fileOpened(false),
	_textGeode(NULL),
	_isAnimating(false),
	_oldbuf(NULL),
	_olderrbuf(NULL)
{
    init();
}

// --------------------------------------------------------------------------------------------------------------------
// destructor
// --------------------------------------------------------------------------------------------------------------------
Logger::~Logger() {

	if (_oldbuf) {
		std::cout.rdbuf(_oldbuf);
		_oldbuf = NULL;
	}
	
	if (_olderrbuf) {
		std::cerr.rdbuf(_olderrbuf);
		_olderrbuf = NULL;
	}
	
    if (!_filename.empty() && _fileOpened)
        _logfile.close();
}

// --------------------------------------------------------------------------------------------------------------------
// liefert das aktuelle datum als string
// --------------------------------------------------------------------------------------------------------------------
std::string getCurrentDateTime() {

    time_t  datum; 
    struct tm *tmzgr; 
    char datum_str[30]; 

    datum = time(NULL); 
    tmzgr = localtime(&datum); 
    strftime(datum_str,30,"%Y.%m.%d %H:%M:%S",tmzgr); 
    return std::string(datum_str);
    
}

// --------------------------------------------------------------------------------------------------------------------
// init
// --------------------------------------------------------------------------------------------------------------------

void Logger::init() {
    _maxLineCount = 40;
    _useSemicolons = false;
    _includeDateTime = true;
    _parent = NULL;
    
    if (_mode & TEXT_GEODE) {
		
		_group = new osg::Group();
		
		_textGeode = new Utf8TextGeode("system.xml", 0,"starting...");
		_textGeode->setName("LoggedTextGeode");
		_textGeode->setDataVariance(osg::Object::DYNAMIC);
		_group->addChild(_textGeode.get());
				
		if (!cefix::FontManager::instance()->hasFont("system_blurred.xml")) {
			cefix::Font* blurredFont = cefix::FontManager::instance()->cloneFont("system.xml", "system_blurred.xml");
			cefix::FilterApplier().apply(blurredFont->getImages(), new cefix::DilateFilter(3));
			//cefix::FilterApplier().apply(blurredFont->getImages(), new cefix::GaussianBlurFilter(5.0));
		}
				
		if (cefix::FontManager::instance()->loadFont("system_blurred.xml") != 0) {
			_shadowTextGeode = new Utf8TextGeode("system_blurred.xml", 0,"starting...");
			_shadowTextGeode->setDataVariance(osg::Object::DYNAMIC);
			_group->addChild(_shadowTextGeode.get());
		}
		
	}
	
	if (_mode & REDIRECT_NOTIFY) {
		
		osg::setNotifyHandler(this);
	}

    setTextColor(osg::Vec4(0,0.8,0,0.7));
	_ctrl = new cefix::AnimationController();
}

// --------------------------------------------------------------------------------------------------------------------
// neuer text zum loggen
// --------------------------------------------------------------------------------------------------------------------

void Logger::add(const std::string& s, osg::NotifySeverity severity) {

	const char* severity_strs[7] = { 
		"", 
		"FATAL  ",
		"WARN   ",
		"NOTICE ",
		"INFO   ",
		"DEBUG_INFO ",
		"DEBUG_FP   "};
	
    
    if ((_mode & CONSOLE)) 
    {
        std::cout << getCurrentDateTime() << " " << severity_strs[severity] << s;
        if ((s.length() > 0) && ((s[s.length()-1] != 10) && (s[s.length()-1] != 13)))
            std::cout << std::endl;
    }
    if ((_mode & FILE) && (!s.empty())) 
    {
		if (!_fileOpened) {
			osgDB::makeDirectoryForFile(_filename);	
			_logfile.open(_filename.c_str(), ios::out | ios::app);
			_fileOpened = true;
		}
        if (_includeDateTime) {
            _logfile << getCurrentDateTime() << ((_useSemicolons) ? ";" : " ");
        }
            
        _logfile << s << ((_useSemicolons) ? ";" : " ");
    }
        
    if (_mode & TEXT_GEODE) {
        
        if(!s.empty())  {
            std::vector<std::string> s_vec;
            cefix::strTokenize(_textGeode->getText(),s_vec,"\n");
            
            s_vec.push_back(s);
            unsigned int start = osg::maximum((int)(s_vec.size() - _maxLineCount), 0);
            std::string rslt("");
            for(unsigned int i= start; i < s_vec.size(); i++) {
                rslt += s_vec[i] + "\n";
            }
            _textGeode->setText(rslt);
			_shadowTextGeode->setText(rslt);
        }
        
        if (_isAnimating) {
			_ctrl->cancel();
            cefix::AnimationFactory::instance()->remove(_ctrl.get()); // laufende animation cancellen
			//osg::notify(osg::ALWAYS) << "animation removed: " << b <<std::endl;
		}
        if (_parent.valid()) {
			_parent->removeChild(_group.get());
			_parent->addChild(_group.get());
			// osg::notify(osg::ALWAYS) << "num of children after add: " << _parent->getNumChildren() << std::endl;

        }

        _ctrl->add(new AnimateLogger(this, _textcolor));
		_ctrl->init();
        cefix::AnimationFactory::instance()->add(_ctrl.get());
		_isAnimating = true;
    }
}


// ----------------------------------------------------------------------------------------------------------
// textFadeOutEnded
// ----------------------------------------------------------------------------------------------------------

void Logger::textFadeoutEnded() { 
	_isAnimating = false;
	if (_parent.valid()) {
		_parent->removeChild(_group.get()); 
		//osg::notify(osg::ALWAYS) << "num of children after remove: " << _parent->getNumChildren() << " " << b << std::endl;
	}
}

void Logger::setTextPosition(const osg::Vec3 & p) {
    if (getTextGeode()) {
        _textGeode->setPosition(p);
        _shadowTextGeode->setPosition( p-osg::Vec3(0,0,1) );
    }
}

void Logger::applyTextColor(osg::Vec4 c) { 
   if (_textGeode.valid()) {
        _textGeode->setTextColor(c);
        _shadowTextGeode->setTextColor(osg::Vec4(0,0,0,c[3]));
    }
}

void Logger::setTextColor(osg::Vec4 c) { 
    _textcolor = c; 
    if ((_textGeode.valid()) && (_isAnimating == false)) {
        _textGeode->setTextColor(c);
        _shadowTextGeode->setTextColor(osg::Vec4(0,0,0,c[3]));
    }
}

}