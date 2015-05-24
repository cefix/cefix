/*
 *  QTImportExport.h
 *  cefix
 *
 *  Created by Stephan Huber on 07.02.08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include <osg/Image>

#ifndef QT_IMPORT_EXPORT_HEADER
#define QT_IMPORT_EXPORT_HEADER


class QuicktimeImportExport {

	public:
		
		QuicktimeImportExport();
		
		 osg::Image* readFromStream(std::istream & inStream, const std::string& fileTypeHint, long sizeHint = 0);
		 void writeToStream(std::ostream& outStream, osg::Image* image, const std::string& fileTypeHint) ;
		 
		 const std::string getLastErrorString() { return _lastError; }
		 bool success() { return (_error == false); }
	
	protected:
		 void flipImage(unsigned char* buffer, int bytesPerPixel, unsigned int width, unsigned height);
		 unsigned char* pepareBufferForOSG(unsigned char * buffer, int bytesPerPixel, unsigned int width, unsigned height);
		 unsigned char* prepareBufferForQuicktime(unsigned char* buffer, GLenum pixelFormat, int bytesPerPixel, unsigned int width, unsigned int height)  ;
		 void setError(const std::string& msg) { _lastError = msg; _error = true; }
		 
		 osg::Image* doImport(unsigned char* buffer, unsigned int dataSize, const std::string& fileTypeHint);
		 
		 
	private:
		bool			_error;
		std::string _lastError;


};



#endif