/*
 *  QuickTimeMovieExporter.cpp
 *  cefix
 *
 *  Created by Stephan Huber on 10.09.06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include <cefix/Export.h>

#if defined CEFIX_QUICKTIME_AVAILABLE

#include <cefix/Log.h>
#include <osgDB/FileNameUtils>
#include <cefix/QuickTimeMovieExporter.h>
#include <cefix/QTUtils.h>
#include <cefix/DataFactory.h>
#ifdef __APPLE__
#include <Quicktime/Quicktime.h>
#else
#include "Movies.h"
#endif
#include <limits>

namespace cefix {

class QuicktimeMovieExporter::PrivateData {

	public:
		PrivateData() :
			_dataHandler(NULL),
			_movie(0),
			_track(0),
			_media(0),
			_nFileRefNum(0),
			_compressedData(0),
			_imageDesc(0),
			_gworld(0),
			_inited(false),
			_codecQuality(codecMaxQuality),
			_codecType(kJPEGCodecType),
			_width(0),
			_height(0),
			_useAlpha(false)
		{
		}
		
		void setUseAlpha(bool f) { _codecType = f ? kPNGCodecType : kJPEGCodecType; _useAlpha = f; }
		
		bool createMovie(std::string filename, float fps, unsigned int w, unsigned h);
		void closeMovie();
		inline void addFrame(osg::Image* img, double duration);
		
		Movie getMovie() { return _movie; }
		
		void setVerbose(bool b) { _verbose = b; }
		
	private:
		bool checkError(OSErr err, std::string errstr) {
			if (err !=noErr)
				osg::notify(_verbose ? osg::ALWAYS : osg::INFO) << "QuicktimeMovieExporter error: " << err << " " << errstr << std::endl;
			return (err != noErr);
		}
		DataHandler		_dataHandler;
		Movie _movie;
		Track _track;
		Media _media;
		short _nFileRefNum;
		Handle		_compressedData;
		ImageDescriptionHandle	_imageDesc;
		GWorldPtr				_gworld;
		bool		_inited;
		CodecQ		_codecQuality;
		CodecType	_codecType;
		float		_fps;
		int _width, _height;
		bool _verbose, _useAlpha;

};


// --------------------------------------------------------------------------------------------------------------------
// createMovie
// --------------------------------------------------------------------------------------------------------------------

bool QuicktimeMovieExporter::PrivateData::createMovie(std::string filename, float fps, unsigned int w, unsigned h)
{
		
	_fps = fps;
	_movie = 0;
	_width = w;
	_height = h;

    OSErr err = noErr;
	Rect trackFrame = {0, 0, h, w};
	std::string tempfile = filename;
	if ((tempfile.find("/") == std::string::npos) && (tempfile.find("\\") == std::string::npos)) {
		tempfile = cefix::DataFactory::instance()->getRootPath() + "/"+tempfile;
	}
	std::string filepath = osgDB::convertFileNameToNativeStyle(tempfile);		
	
	/*
	FILE *fp = fopen(filepath.c_str(), "wb");
	if (!fp) {
		osg::notify(osg::WARN) << "QuicktimeMovieExporter: could not create file! " << filepath << std::endl;
		return false;
	}
	
	fclose(fp);
	*/
	if (_verbose) {
		std::cout << "creating Movie at " << filepath << std::endl;
	}
	
	Handle		dataRef;
	OSType		dataRefType;
	
	//Create movie file
	CFStringRef movieLocation = CFStringCreateWithCString(NULL, filepath.c_str(), kCFStringEncodingUTF8);
	#ifdef __APPLE__
	err = QTNewDataReferenceFromFullPathCFString(movieLocation, kQTPOSIXPathStyle, 0, &dataRef, &dataRefType);
	#else
	err = QTNewDataReferenceFromFullPathCFString(movieLocation, kQTWindowsPathStyle, 0, &dataRef, &dataRefType);
	#endif
	
	if (checkError( err, "QTNewDataReferenceFromFullPathCFString error" ))
		return false;
	
	// Create a movie for this file (data ref)
	err = CreateMovieStorage(dataRef, dataRefType, 'TVOD', smCurrentScript, createMovieFileDeleteCurFile | createMovieFileDontCreateResFile, &_dataHandler, &_movie);
	if (checkError( err, "CreateMovieStorage error" ))
		return false;
	
	
    CFRelease(movieLocation);
	DisposeHandle(dataRef);
	

    // 1. Create the track
    _track = NewMovieTrack (_movie,     /* movie specifier */
                                    FixRatio(trackFrame.right,1),  /* width */
                                    FixRatio(trackFrame.bottom,1), /* height */
                  kNoVolume);  /* trackVolume */
    if (checkError( GetMoviesError(), "NewMovieTrack error" ))
		return false;

    // 2. Create the media for the track
    _media = NewTrackMedia (_track,    /* track identifier */
                                        VideoMediaType,    /* type of media */
                                        6000,   /* time coordinate system */
                                        nil,      /* data reference - use the file that is associated with the movie  */
                                        0);      /* data reference type */
    if (checkError( GetMoviesError(), "NewTrackMedia error" )) 
		return false;

    // 3. Establish a media-editing session
    err = BeginMediaEdits (_media);
    if (checkError( err, "BeginMediaEdits error" ))
		return false;
	
	
	// init gworld-stuff
	err = QTNewGWorld (&_gworld,	/* pointer to created gworld */	
			32,		/* pixel depth */
			&trackFrame, 		/* bounds */
			nil, 			/* color table */
			nil,			/* handle to GDevice */ 
			(GWorldFlags)0);	/* flags */
	if (checkError (err, "NewGWorld error"))
		return false;

	// Lock the pixels
	LockPixels (GetGWorldPixMap(_gworld)/*GetPortPixMap(theGWorld)*/);

	// Determine the maximum size the image will be after compression.
	// Specify the compression characteristics, along with the image.
	long maxCompressedSize;
	err = GetMaxCompressionSize(GetGWorldPixMap(_gworld),		/* Handle to the source image */
						&trackFrame, 				/* bounds */
						32, 				/* let ICM choose depth */
						_codecQuality,				/* desired image quality */ 
						_codecType,			/* compressor type */ 
						(CompressorComponent)anyCodec,  		/* compressor identifier */
						&maxCompressedSize);		    	/* returned size */
	if (checkError (err, "GetMaxCmpressionSize error"))
		return false;
	
	
	
	// Create a new handle of the right size for our compressed image data
	_compressedData = NewHandle(maxCompressedSize);
	if (checkError( MemError(), "NewHandle error" )) 
		return false;
	
	MoveHHi( _compressedData );
	HLock( _compressedData );
	
	// Create a handle for the Image Description Structure
	_imageDesc = (ImageDescriptionHandle)NewHandle(4);
	if (checkError( MemError(), "NewHandle error" ))
		return false;

	_inited = true;
	
	if (_verbose) {
		std::cout << "Movie created " << std::endl;
	}
	
	return true;
    
}

// --------------------------------------------------------------------------------------------------------------------
// closeMovie
// --------------------------------------------------------------------------------------------------------------------

void QuicktimeMovieExporter::PrivateData::closeMovie() 
{
	if (_movie == 0)
		return;
	if (_verbose) {
		std::cout << "Closing Movie" << std::endl;
	}
	_inited = false;
	OSErr err = noErr;
	err = EndMediaEdits (_media);
	
	err = ExtendMediaDecodeDurationToDisplayEndTime(_media, NULL);
	checkError( err, "ExtendMediaDecodeDurationToDisplayEndTime error" );
	
	err = InsertMediaIntoTrack (_track,0, 0,GetMediaDisplayDuration(_media),fixed1);
    checkError( err, "InsertMediaIntoTrack error" );
	
	err = AddMovieToStorage(_movie, _dataHandler);
	checkError( err, "AddMovieToStorage error" );

	
	if (_dataHandler) CloseMovieStorage(_dataHandler);
	if (_movie) DisposeMovie(_movie);
	
	UnlockPixels (GetGWorldPixMap(_gworld)/*GetPortPixMap(theGWorld)*/);
	// Dealocate our previously alocated handles and GWorld
	if (_imageDesc)
	{
			DisposeHandle ((Handle)_imageDesc);
	}
	
	if (_compressedData)
	{
			DisposeHandle (_compressedData);
	}
	
	if (_gworld)
	{
			DisposeGWorld (_gworld);
	}
	
	if (_verbose) {
		std::cout << "Movie closed" << std::endl;
	}
	
		
}


// --------------------------------------------------------------------------------------------------------------------
// addFrame
// --------------------------------------------------------------------------------------------------------------------

inline void QuicktimeMovieExporter::PrivateData::addFrame(osg::Image* img, double duration) 
{
	if ((_movie == 0) || (_inited == false))
		return;

	if ((img->s() > _width) || (img->t() > _height)) {
		log::error("QuicktimeMovieExporter") << "could not add frame, it is too big ... " << std::endl;
		return;
	}

	CGrafPtr oldPort;
	GDHandle oldGDeviceH;
	GetGWorld(&oldPort, &oldGDeviceH);
	SetGWorld(_gworld,NULL);
	unsigned int w = img->s();
	unsigned int h = img->t();	
	Rect bounds = { 0,0, h, w };
	Ptr compressedDataPtr = *_compressedData;
	
	PixMapHandle pmh = GetGWorldPixMap(_gworld);
	
	unsigned char* sptr;
	unsigned char* dptr;
	int pixmapRowBytes = QTGetPixMapHandleRowBytes(pmh);
	unsigned char* pixBaseAddr = (unsigned char*)GetPixBaseAddr(pmh);
	unsigned int sx,sy;
	for(sy = 0; sy < h; sy++) {
		sptr = img->data(0,h - sy - 1);
		dptr = pixBaseAddr + sy * pixmapRowBytes;
		for(sx=0; sx < w; sx++) {
			dptr[0] = (_useAlpha) ? (*(sptr+4)) : 0;
			dptr[1] = (*sptr++);
			dptr[2] = (*sptr++);
			dptr[3] = (*sptr++);
			
			if(_useAlpha) sptr++;
			
			dptr+=4;
		}
	}

	
	OSErr err = CompressImage(pmh,	/* source image to compress */
                                &bounds, 		/* bounds */
                                _codecQuality,	/* desired image quality */
                                _codecType,	/* compressor identifier */
                                _imageDesc, 		/* handle to Image Description Structure; will be resized by call */
                                compressedDataPtr);	/* pointer to a location to recieve the compressed image data */
	checkError( err, "CompressImage error" );
	TimeValue frameduration = duration * 6000; //duration * 600;
	// Add sample data and a description to a media
	err = AddMediaSample(_media,	/* media specifier */ 
			_compressedData,	/* handle to sample data - dataIn */
			0,		/* specifies offset into data reffered to by dataIn handle */
			(**_imageDesc).dataSize, /* number of bytes of sample data to be added */ 
			frameduration,		 /* frame duration = 1/10 sec */
			(SampleDescriptionHandle)_imageDesc,	/* sample description handle */ 
			1,	/* number of samples */
			0,	/* control flag indicating self-contained samples */
			nil);		/* returns a time value where sample was insterted */
	
	checkError( err, "AddMediaSample error" );

	SetGWorld (oldPort, oldGDeviceH);
	
	if (_verbose) {
		std::cout << "frame "<< img->s() << "x" << img->t() << " added, duration: " << frameduration << std::endl;
	}
}


// --------------------------------------------------------------------------------------------------------------------
// ctor
// --------------------------------------------------------------------------------------------------------------------

QuicktimeMovieExporter::QuicktimeMovieExporter(std::string filename, unsigned int w, unsigned int h, float fps, bool verbose) :
	osg::Referenced(),
	OpenThreads::Thread(),
	_isFinished(false),
	_fps(fps),
	_inited(false),
	_preloadFrameCount(0),
	_verbose(verbose),
    _maxConcurrentImages(std::numeric_limits<unsigned int>::max())
{	
	_data = new PrivateData();
	_data->setVerbose(_verbose);
	_data->setUseAlpha(getUseAlphaFlag());
	if (_data->createMovie(filename, fps, w, h)) {
		_inited = true;
		start();
	}

}


void QuicktimeMovieExporter::setUseAlphaFlag(bool use_alpha) 
{ 
	_useAlpha = use_alpha; 
	_data->setUseAlpha(getUseAlphaFlag());
}

// --------------------------------------------------------------------------------------------------------------------
// run
// --------------------------------------------------------------------------------------------------------------------

void QuicktimeMovieExporter::run() {
	_wantToQuit = false;
	_isFinished = false;
#ifdef __APPLE__
	EnterMoviesOnThread(0);
	AttachMovieToCurrentThread(_data->getMovie());
#endif
	while ((!isFinished()) && (!testCancel())) {
		
		unsigned int remaining = getRemainingItems();
		if (((remaining > 0) && (_wantToQuit)) || (remaining > _preloadFrameCount)) {
			_mutex.lock();
			ImagePair imgpair = (*_imageQueue.begin());
			_imageQueue.pop_front();
			_mutex.unlock();
			
			_data->addFrame(imgpair.first.get(), imgpair.second);
			
		}
		else {
			if (_wantToQuit) {
				if (remaining == 0)
					_isFinished = true;
			}
			else
				microSleep(50*1000);
		}	
			
	
	}
	_data->closeMovie();
#ifdef __APPLE__
	ExitMoviesOnThread();
#endif
	_isFinished = true;
}

void QuicktimeMovieExporter::setVerbose(bool b) 
{ 
	_verbose = true; 
	_data->setVerbose(_verbose);
}

void QuicktimeMovieExporter::stop() 
{
	if (_verbose) {
		std::cout << "stopping export" << std::endl;
	}
	_wantToQuit = true;
	while(isRunning())
	{
		OpenThreads::Thread::YieldCurrentThread();
	}
	if (_verbose) {
		std::cout << "export stopped" << std::endl;
	}
}

	
QuicktimeMovieExporter::~QuicktimeMovieExporter() {

	stop();
	delete _data;
	if (_verbose) {
		std::cout << "~QuicktimeMovieExporter destructed" << std::endl;
	}
}


void QuicktimeMovieExporter::PostDrawCallback::operator()(const osg::Camera& camera) const
{
										
	if ((_enabled) && (_exporter.valid())) {
		double duration = _t.delta_s(_lastCapturedTimeStamp, _t.tick());
		
		if (duration < 1/_fps && !_everyFrame) return;
		_lastCapturedTimeStamp = _t.tick();

		const osg::Viewport* vp = camera.getViewport();

		osg::ref_ptr<osg::Image> image = new osg::Image;
		image->readPixels(vp->x(),vp->y(),vp->width(),vp->height(), _exporter->getUseAlphaFlag() ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE);
		
		_exporter->addImage(image.get(), (_everyFrame) ? 1/_fps : duration);
        
        while ((_exporter->getMaxConcurrentImages() > 0) && (_exporter->getRemainingItems() > _exporter->getMaxConcurrentImages())) {
            OpenThreads::Thread::microSleep(1000*50);
        }
		
	}
}

}

#endif