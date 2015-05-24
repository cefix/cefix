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

#ifndef CEFIX_SETTINGS_HEADER_
#define CEFIX_SETTINGS_HEADER_
#include <osg/Referenced>
#include <osg/Vec4>
#include <string>
#include <cefix/Export.h>




namespace cefix {

	class PropertyList;
	
    /** this class holds all settings specific for cefix. */
    class CEFIX_EXPORT Settings : public osg::Referenced {
 
        public:
    
            /** get the singleton */
            static Settings* instance();
            
            bool getVideoUsePixelBufferObjects() const { return _quicktimeUsePixelBufferObjects; }
            void setVideoUsePixelBufferObjects(bool b)  { _quicktimeUsePixelBufferObjects = b; }
            bool getFontsUseTextureRectangle() const { return _fontsUseTextureRectangle; }
			bool getImageFactoryUseTextureRectangle() const { return _imageFactoryUseTextureRectangle; }
            const osg::Vec4& getPostscriptWriterBackcolor() const { return _postscriptWriterBackcolor; }
            const osg::Vec4& getPostscriptWriterFrontcolor() const { return _postscriptWriterFrontcolor; }
            const std::string& getPostscriptWriterOutputPath() const { return _postscriptWriterOutputPath; }
			
			bool emulateScrollWheel() const { return _emulateScrollWheel; }
			
			bool useCoreVideo() const { return _useCoreVideo; }
			void setUseCoreVideo(bool f) {  _useCoreVideo = f; }

			
            /** sets the settings from a property-list */
            void set(PropertyList* pl);
						
			bool getReportMissingCodePointsFlag() const { return _reportMissingCodePointsFlag; }
			void setReportMissingCodePointsFlag(bool f)  { _reportMissingCodePointsFlag = f; }
			
			void setEmulateScrollWheel(bool f) { _emulateScrollWheel = f; }
			
			bool useMipMappedFonts() { return _useMipMappedFonts; }
			void setUseMipMappedFonts(bool f) { _useMipMappedFonts = f; }
            
            bool useThreadedMediaIdling() { return _threadedMediaIdling; }
            void setUseThreadedMediaIdling(bool f) { _threadedMediaIdling = f; }
			
			bool useNPOTTextures() const { return _useNPOTTextures; }
			void setUseNPOTTextures(bool f) { _useNPOTTextures = f; }
        protected:
            Settings() ;
            
            virtual ~Settings() {}
        
        private:
            bool _quicktimeUsePixelBufferObjects;
            bool _fontsUseTextureRectangle;
			bool _imageFactoryUseTextureRectangle;
            osg::Vec4 _postscriptWriterBackcolor;
            osg::Vec4 _postscriptWriterFrontcolor;
            std::string _postscriptWriterOutputPath;
			bool	_useCoreVideo;
			float	_allocationMaxSamplingTime;
			bool	_reportMissingCodePointsFlag;
			bool	_emulateScrollWheel;
			bool	_useMipMappedFonts;
            bool    _threadedMediaIdling;
			bool	_useNPOTTextures;
			
    
    };
}


#endif
