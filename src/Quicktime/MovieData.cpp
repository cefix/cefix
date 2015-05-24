/*
 *  MovieData.cpp
 *  lala
 *
 *  Created by Stephan Huber on Wed Mar 10 2004.
 *  Copyright (c) 2004 __MyCompanyName__. All rights reserved.
 *
 */
 
#if defined (CEFIX_QUICKTIME_AVAILABLE)

#include <osg/GL>

#include <osgDB/FileNameUtils>

#include <cefix/MovieData.h>
#include <cefix/QTUtils.h>
#include <cefix/Settings.h>

#define NATIVE_YUV_IMPLEMENTATION


static void MyQTNextTaskNeededSoonerCallback(TimeValue duration,
                                             unsigned long flags, void *refcon)
{
}

namespace cefix {
    
	
// ----------------------------------------------------------------------------------------------------------
// ctor
// ----------------------------------------------------------------------------------------------------------

MovieData::MovieData() : 
	_movie(NULL), 
	_buffers(),
    _movieWidth(0),
    _movieHeight(0),
    _textureWidth(0),
    _textureHeight(0),
    _timescale(),
    _fError(false),
    _movieRate(0),
    _yuvPlaybackUsed(false),
    _filename(),
    _activeFlag(),
    _isLooping(),
    _prerolled(false),
    _hasVideoTrack(false),
    _currentVideoBufferNdx(0),
    _doubleBuffering(false)
{
	setUseYUVPlayback(false);
    
    static bool s_timer_inited = false;
    if (!s_timer_inited) {
        s_timer_inited = true;
        OSErr error = QTInstallNextTaskNeededSoonerCallback(
                NewQTNextTaskNeededSoonerCallbackUPP(MyQTNextTaskNeededSoonerCallback),
                1000, // millisecond timescale
                0,    // No flags
                0); 
    }
    
    _doubleBuffering = cefix::Settings::instance()->useThreadedMediaIdling();
}


// ----------------------------------------------------------------------------------------------------------
// dtor
// ----------------------------------------------------------------------------------------------------------

MovieData::~MovieData()
{
    // std::cout << "destructing moviedata" << std::endl;
	for(unsigned int i = 0; i < _buffers.size(); ++i) {
        if (_buffers[i].gw) DisposeGWorld(_buffers[i].gw);
        if (_buffers[i].buffer) free(_buffers[i].buffer);
    }
    
    if (_movie) DisposeMovie(_movie);
}
    
// ----------------------------------------------------------------------------------------------------------
// load
// ----------------------------------------------------------------------------------------------------------

void MovieData::load(osg::Image* image, std::string afilename, float startTime) {
	
	bool isUrl( osgDB::containsServerAddress(afilename) );
	
	std::string filename = afilename;
	if (!isUrl) {
		if (!osgDB::isFileNameNativeStyle(filename)) 
			filename = osgDB::convertFileNameToNativeStyle(filename);
	}
	
	_filename = filename;
	image->setFileName(filename);
	

	QTNewMoviePropertyElement newMovieProperties[2];
	CFStringRef movieLocation = CFStringCreateWithCString(NULL, filename.c_str(), kCFStringEncodingUTF8);
	CFURLRef movieURL(NULL);
	Boolean trueValue = true;

	newMovieProperties[0].propClass = kQTPropertyClass_DataLocation;
	if (!isUrl) 
	{
		#ifdef __APPLE__
			newMovieProperties[0].propID = kQTDataLocationPropertyID_CFStringPosixPath;
		#else
			newMovieProperties[0].propID = kQTDataLocationPropertyID_CFStringWindowsPath;
		#endif
		
		newMovieProperties[0].propValueSize = sizeof(CFStringRef);
		newMovieProperties[0].propValueAddress = &movieLocation;
	} 
	else 
	{
		movieURL = CFURLCreateWithString(kCFAllocatorDefault, movieLocation, NULL);
		
		newMovieProperties[0].propID = kQTDataLocationPropertyID_CFURL;
		newMovieProperties[0].propValueSize = sizeof(movieURL);
		newMovieProperties[0].propValueAddress = (void*)&movieURL;
	}

	// make movie active
    newMovieProperties[1].propClass = kQTPropertyClass_NewMovieProperty;
    newMovieProperties[1].propID = kQTNewMoviePropertyID_Active;
    newMovieProperties[1].propValueSize = sizeof(trueValue);
    newMovieProperties[1].propValueAddress = &trueValue;
    
    // Instantiate the Movie
    OSStatus status = NewMovieFromProperties(2, newMovieProperties, 0, NULL, &_movie);
	CFRelease(movieLocation);
	if (movieURL) CFRelease(movieURL);
	
	if (status !=0) {
        _fError = true;
        osg::notify(osg::FATAL) << " MovieData :: NewMovieFromProperties failed with err " << status<< std::endl;
        return;
    }
	
	
	Rect bounds;
	
    #ifdef __APPLE__ 
		GetRegionBounds(GetMovieBoundsRgn(_movie), &bounds); 
	#else 
 	    bounds = (*GetMovieBoundsRgn(_movie))->rgnBBox; 
 	#endif
	
    _checkMovieError("Can't get movie bounds\n");
    
    OffsetRect(&bounds, -bounds.left, -bounds.top);
    SetMovieBox(_movie, &bounds);
    _checkMovieError("Can't set movie box\n");

    _movieWidth = bounds.right;
    _movieHeight = bounds.bottom;
	
	_hasVideoTrack = ((_movieWidth > 0) && (_movieHeight > 0));
    
    _timescale = (float)GetMovieTimeScale(_movie);
	
	if (_hasVideoTrack) {
		_initImage(image);
		if (!_fError) _initGWorldStuff(image);
	}
        
    if (!_fError) {
    
        if ( startTime == 0.0f)
            GoToBeginningOfMovie(_movie);
        else {
            TimeValue t = (TimeValue) (startTime*_timescale);
            SetMovieTimeValue(_movie,t);
        }
            
        UpdateMovie(_movie);
        SetMovieRate(_movie,0);
		activate();
		UpdateMovie(_movie);
        MoviesTask(_movie,0);
    }
}


// ---------------------------------------------------------------------------
// _intImage
// create image for storing
// ---------------------------------------------------------------------------

void MovieData::_initImage(osg::Image* image)
{
    _textureWidth = ((_movieWidth + 7) >> 3) << 3;
    _textureHeight = _movieHeight;
    
	image->setOrigin(osg::Image::TOP_LEFT);
    
    _buffers.resize(_doubleBuffering + 1);
    for(unsigned int i = 0; i < _buffers.size(); ++i) {
    
    #ifdef GL_YCBR_422_APPLE
        if (getYUVPlaybackUsed()) {

            _buffers[i].buffer = (char*)malloc(2 * _textureWidth * _textureHeight + 16);

            if (_buffers[i].buffer == NULL) {
                osg::notify(osg::FATAL) << "MovieData: " << "Can't allocate texture buffer" << std::endl;
                _fError= true;
            }

            _buffers[i].pointer = (char*)(((unsigned long)(_buffers[i].buffer + 15) >> 5) << 5);
            
            image->setImage(_textureWidth,_textureHeight,1,
                           (GLint) GL_RGBA,
                           (GLenum)GL_YCBCR_422_APPLE,
                           computeInternalFormat(),
                           (unsigned char*) _buffers[i].pointer, osg::Image::NO_DELETE,2);
        }
        else 
    #endif
        {
            
            _buffers[i].buffer = (char*)malloc(4 * _textureWidth * _textureHeight + 32);

            if (_buffers[i].buffer == NULL) {
                osg::notify(osg::FATAL) << "MovieData: " << "Can't allocate texture buffer" << std::endl;
                _fError= true;
            }

            _buffers[i].pointer = (char*)(((unsigned long)(_buffers[i].buffer + 31) >> 5) << 5);
            image->setImage(_textureWidth,_textureHeight,1,
                           (GLint) GL_RGBA8,
                           (GLenum)GL_BGRA_EXT,
                           computeInternalFormat(),
                           (unsigned char*) _buffers[i].pointer, osg::Image::NO_DELETE,4);
            image->setFileName(_filename);
        }
    }

}

// ---------------------------------------------------------------------------
// _initGWorldStuff
// init gworld-stuff, so quicktime can play the movie into the gWorld.
// ---------------------------------------------------------------------------

void MovieData::_initGWorldStuff(osg::Image * image)  {

    Rect textureBounds;
    OSStatus err;

    textureBounds.left = 0;
    textureBounds.top = 0;
    textureBounds.right = image->s();
    textureBounds.bottom = image->t();
    
    for(unsigned int i = 0; i < _buffers.size(); ++i) {

        if (getYUVPlaybackUsed()) 
            err = QTNewGWorldFromPtr(&_buffers[i].gw, k422YpCbCr8CodecType, &textureBounds, NULL, NULL, 0, _buffers[i].pointer, 2 * image->s());
        else
            err = QTNewGWorldFromPtr(&_buffers[i].gw, getNativePixelFormatForQuicktime(), &textureBounds, NULL, NULL, 0, _buffers[i].pointer, 4 * image->s());

        if (err !=0 )
            osg::notify(osg::FATAL) << "MovieData : Could not create gWorld" << std::endl;
            
        //GetGWorld (&origPort, &origDevice);
        //SetGWorld(_gw, NULL);                                         // set current graphics port to offscreen
        SetMovieGWorld(_movie, (CGrafPtr)_buffers[i].gw, NULL);
    }
    
    _checkMovieError("SetMovieGWorld failed");
    _currentVideoBufferNdx = 0;
}

void MovieData::setMovieTime(float atime) {
	float time = (atime > getMovieDuration()) ? getMovieDuration() : atime;
	
	TimeValue t = (TimeValue) (time * _timescale);
	SetMovieTimeValue(_movie,t);
	_checkMovieError("setMovieTime failed");
	UpdateMovie(_movie);
	MoviesTask(_movie,0);
}


// ----------------------------------------------------------------------------------------------------------
// reopen
// ----------------------------------------------------------------------------------------------------------

void MovieData::reopen() {
	Rect bounds;
	
	float time = getMovieTime();
	float rate = getMovieRate();
	DisposeMovie(_movie);
	
	// open the movie again:
	OSStatus err = MakeMovieFromPath(_filename.c_str(),&_movie);
    if (err !=0) {
        _fError = true;
        osg::notify(osg::FATAL) << " MovieData :: MakeMovieFromPath failed with err " << err << std::endl;
        return;
    }

    GetMovieBox(_movie, &bounds);
    _checkMovieError("Can't get movie box\n");
    
    OffsetRect(&bounds, -bounds.left, -bounds.top);
    SetMovieBox(_movie, &bounds);
    _checkMovieError("Can't set movie box\n");
	
	// assign gworld:
	
	SetMovieGWorld(_movie, (CGrafPtr)_buffers[0].gw, NULL);
    _checkMovieError("SetMovieGWorld failed");
	
	UpdateMovie(_movie);
	SetMovieRate(_movie,0);
	activate();
		
	setMovieTime(time);
	setMovieRate(rate);

	UpdateMovie(_movie);
	MoviesTask(_movie,0);
		
}


// ----------------------------------------------------------------------------------------------------------
// setMovieRate
// ----------------------------------------------------------------------------------------------------------

void MovieData::setMovieRate(float rate) { 
	// osg::notify(osg::ALWAYS) << "new movierate: " << rate << " current: " << getMovieRate() << std::endl;
	_movieRate = rate;
    if (rate != 0) {
		if (!isActive()) activate();
        if (!_prerolled) PrerollMovie(_movie, GetMovieTime(_movie,NULL), X2Fix(rate));
		_prerolled = true;
        _checkMovieError("PrerollMovie failed");
    }
    
	SetMovieRate(_movie, X2Fix(rate)); 
	_checkMovieError("setMovieRate failed");
	MoviesTask(_movie, 0);
	_checkMovieError("MoviesTask failed");
	
	UpdateMovie(_movie);
	_checkMovieError("UpdateMovie failed");

}


unsigned char* MovieData::swapBuffers() 
{
    if (!_hasVideoTrack) return NULL;
    
    unsigned int old_ndx = _currentVideoBufferNdx;
    
    unsigned char* pointer = (unsigned char*)(_buffers[_currentVideoBufferNdx].pointer);
    
    _currentVideoBufferNdx++;
    if (_currentVideoBufferNdx >= _buffers.size())
        _currentVideoBufferNdx = 0;
    
    if (old_ndx != _currentVideoBufferNdx)
        SetMovieGWorld(_movie, (CGrafPtr)_buffers[_currentVideoBufferNdx].gw, NULL);
    
    /*
    void* p1 = (void*)(pointer);
    void* p2 = (void*)(_buffers[_currentVideoBufferNdx].pointer);
    std::cout << "tex: " << p1 << " video: " << p2 << std::endl;
    */
    
    return pointer;
}

// ----------------------------------------------------------------------------------------------------------
// setUseYUVPlayback
// ----------------------------------------------------------------------------------------------------------

void MovieData::setUseYUVPlayback(bool flag) {

	#ifdef __APPLE__
		_yuvPlaybackUsed = flag;
	#else
		// yuv-playback is not supported on windows
		_yuvPlaybackUsed = false;
	#endif
	//osg::notify(osg::ALWAYS) << "using yuv playback" << std::endl;
}

// ----------------------------------------------------------------------------------------------------------
// computeInternalFormat
// ----------------------------------------------------------------------------------------------------------

GLenum MovieData::computeInternalFormat() {

	GLenum format;
#ifdef GL_YCBR_422_APPLE
	if (getYUVPlaybackUsed()) {
		#if defined(__APPLE__) && defined(__BIG_ENDIAN__)
			format = (GLenum)GL_UNSIGNED_SHORT_8_8_REV_APPLE;
		#else
			format = (GLenum)GL_UNSIGNED_SHORT_8_8_APPLE;
		#endif
	}
	else 
#endif
	{
		return getBestInternalFormatForQuicktime();
	}
	
	return format;
}



} // namespace

#endif
