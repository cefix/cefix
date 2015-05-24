#include <cefix/Export.h>

#ifdef CEFIX_QUICKTIME_AVAILABLE

#include "osg/Image"
#include <cefix/Log.h>

#include <osg/Geode>

#include "osg/GL"

#include "osgDB/FileNameUtils"
#include "osgDB/Registry"
#include "osgDB/FileUtils"


//#include <cefix/QTUtils.h>

#include <cefix/PropertyList.h>
#include <cefix/QuicktimeMovie.h>
#include <cefix/AllocationObserver.h>
#include "QTImportExport.h"


using namespace osg;



class ReaderWriterQT : public osgDB::ReaderWriter
{
    public:
	
		ReaderWriterQT() : osgDB::ReaderWriter() 
		{
			supportsExtension("mov","Movie format");
			supportsExtension("mpg","Movie format");
			supportsExtension("mpv","Movie format");
			supportsExtension("mp4","Movie format");
			supportsExtension("m4v","Movie format");
			supportsExtension("dv","Movie format");
			supportsExtension("avi","Movie format");
			supportsExtension("flv","Movie format");
			supportsExtension("swf","Movie format");
			supportsExtension("3gp","Mobile movie format");

			supportsExtension("live","Live video streaming");
			
			supportsProtocol("http", "streaming media per http");
			supportsProtocol("rtsp", "streaming media per rtsp");

			
			supportsExtension("jpg","jpg image format");
			supportsExtension("jpeg","jpeg image format");
			supportsExtension("tif","tif image format");
			supportsExtension("tiff","tiff image format");
			supportsExtension("gif","gif image format");
			supportsExtension("png","png image format");
			supportsExtension("pict","pict image format");
			supportsExtension("pct","pct image format");
			supportsExtension("tga","tga image format");
			supportsExtension("psd","psd image format");

		}
		
		
        virtual const char* className() const { return "Default Quicktime Image Reader/Writer"; }
        
        virtual bool acceptsMovieExtension(const std::string& extension) const
        {
            return osgDB::equalCaseInsensitive(extension,"mov") ||
                   osgDB::equalCaseInsensitive(extension,"mpg") ||
                   osgDB::equalCaseInsensitive(extension,"mpv") ||
                   osgDB::equalCaseInsensitive(extension,"mp4") ||
                   osgDB::equalCaseInsensitive(extension,"m4v") ||
                   osgDB::equalCaseInsensitive(extension,"mid") ||
                   osgDB::equalCaseInsensitive(extension,"dv");
        }
        
        virtual bool acceptsExtension(const std::string& extension) const
        {
            // this should be the only image importer required on the Mac
            // dont know what else it supports, but these will do
            return osgDB::equalCaseInsensitive(extension,"rgb") ||
                osgDB::equalCaseInsensitive(extension,"rgba") ||
                osgDB::equalCaseInsensitive(extension,"jpg") || 
                osgDB::equalCaseInsensitive(extension,"jpeg") ||
                osgDB::equalCaseInsensitive(extension,"tif") ||               
                osgDB::equalCaseInsensitive(extension,"tiff") || 
                osgDB::equalCaseInsensitive(extension,"gif") ||
                osgDB::equalCaseInsensitive(extension,"png") ||
                osgDB::equalCaseInsensitive(extension,"pict") ||
                osgDB::equalCaseInsensitive(extension,"pct") ||
                osgDB::equalCaseInsensitive(extension,"tga") ||
                osgDB::equalCaseInsensitive(extension,"psd") ||
				osgDB::equalCaseInsensitive(extension,"bmp") ||
                acceptsMovieExtension(extension);
        }

        virtual ReadResult readImage(const std::string& file, const osgDB::ReaderWriter::Options* options) const
        {                    
            
			std::string ext = osgDB::getLowerCaseFileExtension(file);
            if (!acceptsExtension(ext)) return ReadResult::FILE_NOT_HANDLED;

			std::string fileName(file);
			if (!osgDB::containsServerAddress(file)) {
				fileName = osgDB::findDataFile( file, options );
				if (fileName.empty()) return ReadResult::FILE_NOT_FOUND;
			}
            
            // if the file is a movie file then load as an ImageStream.
            if (acceptsMovieExtension(ext))
            {
				cefix::QuicktimeMovie* movie = new cefix::QuicktimeMovie(fileName);
				cefix::AllocationObserver::instance()->observe(movie);
				
				return movie;
            }
			
			if (!osgDB::fileExists(fileName))
				return  ReadResult::FILE_NOT_FOUND; 
					
			QuicktimeImportExport importer;
			
			std::ifstream is;
			is.open (fileName.c_str(), std::ios::binary | std::ios::in );
			is.seekg (0, std::ios::end);
			long length = is.tellg();
			is.seekg (0, std::ios::beg);
			
			osg::Image* image = (length > 0) ? importer.readFromStream(is, "dummy." + osgDB::getFileExtension(fileName), length) : NULL;
			is.close();
			if (!importer.success() || (image == NULL)) {
				cefix::log::error("ReaderWriterQT") << "Error reading file " << file << " : " << importer.getLastErrorString() << std::endl;
				return ReadResult::ERROR_IN_READING_FILE;
			}
			cefix::log::info("ReaderWriterQT") << "image read from " << file << std::endl;
			
			image->setFileName(fileName);
			cefix::AllocationObserver::instance()->observe(image);
            return image;
        }
		
		/**
		 * readImage from streams support two osgDB::Options: SIZE and FILENAME, if no SIZE-option is set, the quicktime plugin will read the stream until EOF, 
		 * this is not suitable for some cirsumstances, the FILENAME is uses as a hint for the format of the image */
		 
		virtual ReadResult readImage (std::istream& is, const osgDB::ReaderWriter::Options* options=NULL) const 
		{
			std::string filename = "";
			// check options for a file-type-hint 
			long sizeHint(0);
			if (options) {
				std::istringstream iss(options->getOptionString());
                std::string opt;
                while (iss >> opt) 
                {
                    int index = opt.find( "=" );
                    if( opt.substr( 0, index ) == "filename" ||
                        opt.substr( 0, index ) == "FILENAME" )
                    {
                        filename = opt.substr( index+1 );
                    }
					else if( opt.substr( 0, index ) == "size" ||
                        opt.substr( 0, index ) == "SIZE" )
                    {
                        std::string sizestr = opt.substr( index+1 );
						sizeHint = atol(sizestr.c_str());
                    }
				}
			}
			
			QuicktimeImportExport importer;
			osg::Image* image = importer.readFromStream(is, filename, sizeHint);
			
			if (!importer.success() || (image == NULL)) {
				cefix::log::error("ReaderWriterQT") << "Error reading from stream "  << importer.getLastErrorString() << std::endl;
				return ReadResult::ERROR_IN_READING_FILE;
			}
			cefix::AllocationObserver::instance()->observe(image);
            return image;
			
		}
        
        virtual WriteResult writeImage(const osg::Image &img,const std::string& fileName, const osgDB::ReaderWriter::Options*) const
        {
            std::string ext = osgDB::getFileExtension(fileName);
            if (!acceptsExtension(ext)) return WriteResult::FILE_NOT_HANDLED;
			
			
			std::ofstream os(fileName.c_str(), std::ios::binary | std::ios::trunc | std::ios::out);
			if(os.good()) {
				QuicktimeImportExport exporter;
				exporter.writeToStream(os, const_cast<osg::Image*>(&img), fileName);
				
				if (exporter.success()) 
					return WriteResult::FILE_SAVED;
			} 
			return WriteResult::ERROR_IN_WRITING_FILE; 
		}
		
		
		virtual WriteResult writeImage (const osg::Image& img, std::ostream& os, const Options* options=NULL) const
		{
			std::string filename = "file.jpg"; // use jpeg if not otherwise specified
			
			if (options) {
				std::istringstream iss(options->getOptionString());
                std::string opt;
                while (iss >> opt) 
                {
                    int index = opt.find( "=" );
                    if( opt.substr( 0, index ) == "filename" ||
                        opt.substr( 0, index ) == "FILENAME" )
                    {
                        filename = opt.substr( index+1 );
                    }
				}
			}
			
			QuicktimeImportExport exporter;
			exporter.writeToStream(os, const_cast<osg::Image*>(&img), filename);
				
			if (exporter.success()) 
				return WriteResult::FILE_SAVED;
			
			return WriteResult::ERROR_IN_WRITING_FILE; 		
		}

        
            

};

// now register with Registry to instantiate the above
// reader/writer.
osgDB::RegisterReaderWriterProxy<ReaderWriterQT> g_readerWriter_QT_Proxy;

#endif
