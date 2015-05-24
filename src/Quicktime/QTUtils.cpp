/*
 *  QTUtils.cpp
 *  NativeContext
 *
 *  Created by Stephan Huber on Fri Sep 06 2002.
 *  Copyright (c) 2002 digital mind. All rights reserved.
 *
 */

#include <cefix/Export.h>

#if defined CEFIX_QUICKTIME_AVAILABLE

#include <osg/ref_ptr>
#include <osg/Referenced>
#include <cefix/Log.h>
#include <cefix/QTUtils.h>
#include <osgDB/Registry>
#include <iostream>
#include <osg/Timer>


using namespace std;



namespace cefix {

	class QuicktimeInitializer : public osg::Referenced {
		public:
			QuicktimeInitializer() :osg::Referenced() {
				static bool s_fQuicktimeInited = 0;
				if (!s_fQuicktimeInited) {
                    
					#ifndef __APPLE__
						InitializeQTML(0);
					#endif
					
                    OSErr err = EnterMovies();
					if (err!=0)
					   osg::notify(osg::FATAL) << "Error while initializing quicktime: " << err << endl;
					else
					   osg::notify(osg::DEBUG_INFO) << "Quicktime initialized successfully"  << endl;
					long version;
					OSErr result;

					result = Gestalt(gestaltQuickTime,&version);

				    if ((result == noErr) && (version >= 0x07100000))
					{
						// alles ok
					}
					else {
						std::cout << "Quicktime version not supported! " << std::endl;
						osg::Timer t; 
						osg::Timer_t tick = t.tick();
						while (t.delta_s(tick, t.tick()) < 1)
							;
						exit(1);
					}
					registerQTReader();
					s_fQuicktimeInited = true;
				}
			}
			
			~QuicktimeInitializer() {
				#ifndef __APPLE__
					ExitMovies();
				#endif
				//osg::notify(osg::DEBUG_INFO) << "Quicktime deinitialized successfully"  << endl;
			}

	protected:
		void registerQTReader() {
			osgDB::Registry* r = osgDB::Registry::instance();
			r->addFileExtensionAlias("jpg",  "qt");
			r->addFileExtensionAlias("jpe",  "qt");
			r->addFileExtensionAlias("jpeg", "qt");
			r->addFileExtensionAlias("tif",  "qt");
			r->addFileExtensionAlias("tiff", "qt");
			r->addFileExtensionAlias("gif",  "qt");
			r->addFileExtensionAlias("png",  "qt");
			r->addFileExtensionAlias("psd",  "qt");
			r->addFileExtensionAlias("tga",  "qt");
			r->addFileExtensionAlias("mov",  "qt");
			r->addFileExtensionAlias("avi",  "qt");
			r->addFileExtensionAlias("mpg",  "qt");
			r->addFileExtensionAlias("mpv",  "qt");
			r->addFileExtensionAlias("dv",   "qt");
			r->addFileExtensionAlias("mp4",   "qt");
			r->addFileExtensionAlias("m4v",   "qt");
		}
				
	};
	
	void initQuicktime(bool erase) {

		static osg::ref_ptr<QuicktimeInitializer> s_qt_init = new QuicktimeInitializer();
		if (erase)
			s_qt_init = NULL;
	}

	
	void exitQuicktime() {
		initQuicktime(true);
	}


	

	// ---------------------------------------------------------------------------
	// MakeFSSPecFromPath
	// wandelt einen Posix-Pfad in ein FSSpec um.
	// ---------------------------------------------------------------------------
	OSStatus MakeFSSpecFromPath(const char* path, FSSpec& spec) {
    
		OSStatus    result;
		#if defined( __APPLE__ )
			FSRef    ref;

			result = FSPathMakeRef( (UInt8*)path, &ref, false); // fname is not a directory
			if (result!=0) return result;
			
			result = FSGetCatalogInfo(&ref, kFSCatInfoNone, NULL, NULL, &spec, NULL);
			
			return result;  	
		#else
			// windows implementation to get a fsspec
			
			result = NativePathNameToFSSpec(const_cast<char*>(path), &spec, 0 /* flags */);
			return result;
			
		#endif
	}


	// ---------------------------------------------------------------------------
	// MakeMovieFromPath
	// erzeugt movie-objekt aus Pfad
	// ---------------------------------------------------------------------------
	OSStatus MakeMovieFromPath(const char* path, Movie* movie) {
		OSStatus err;
		FSSpec   spec;
		short    resref;
#ifdef __APPLE__
		MakeFSSpecFromPath(path, spec);
#else
		err = NativePathNameToFSSpec((char*)path, &spec, 0 /* flags */);
#endif
		err = OpenMovieFile(&spec, &resref, fsRdPerm);
		if (err!=0) return err;
		err = NewMovieFromFile(movie, resref, NULL, NULL, 0, NULL);
		if (err==0) err=GetMoviesError();
		return err;
	}



	OSType getNativePixelFormatForQuicktime() {
		#if defined(__APPLE__) && defined(__BIG_ENDIAN__)
			return  k32ARGBPixelFormat;
		#else
			return k32BGRAPixelFormat;
		#endif
	}
	
	GLenum getBestInternalFormatForQuicktime() 
	{	
		return  (GLenum)GL_UNSIGNED_INT_8_8_8_8_REV;
	}

} // namespace


#endif
