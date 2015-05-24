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

#ifndef _MOVIEDATA_HEADER_
#define _MOVIEDATA_HEADER_

#include <cefix/Export.h>

#if defined CEFIX_QUICKTIME_AVAILABLE

#include <cefix/Log.h>
#include <osg/Image>
#include <string>
#include <cefix/QTUtils.h>
#include <cefix/MovieData.h>



namespace cefix {
    /** 
	 * the class MovieData encapsulates all quicktime-related stuff, so it doesn't polute the namespaces
	 * it handles all calls to quicktime etc... It is mainly used by the QuicktimeImageStream, it is 
	 * rarely needed in other contexts 
	 */
    class CEFIX_EXPORT MovieData {
    
        public:
			
            /** default constructor */
			MovieData();
			
			/** default destructor */
            ~MovieData();
            
			/** 
			 * loads a movie, start it playing at startTime, use Image for the storage
			 * @param image the image, to use as storage
			 * @param fileName the movie to open
			 * @param startTime the starttime to begin with
			 */
            void load(osg::Image* image, std::string fileName, float startTime = 0.0f);
			
			/** @return the duration for this movie in seconds */
            inline float getMovieDuration() { return GetMovieDuration(_movie)/(float)_timescale;}
			
			/** @return the current position for this movie in seconds */
            inline float getMovieTime()  {return GetMovieTime(_movie,NULL)/(float)_timescale; }
            
			/** stes the movietime */
			void setMovieTime(float atime);
			
			inline float getMovieWidth() { return _movieWidth; }
			
			inline float getMovieHeight() { return _movieHeight; }
						
			/** @return the Movie-handle, to use it with other quicktime-calls */
            inline Movie &getMovie() { return _movie; }
			
			/** @return the current movieRate */
			inline float getMovieRate() { return Fix2X(GetMovieRate(_movie)); }
			/** @return returns the cached movierate, may differ to the real movierate */
			inline float getCachedMovieRate() { return _movieRate; }
			
			/** sets the MovieRate for this movie */
			void setMovieRate(float rate);
			
			/** sets the volume for the soundtrack of this movie */
			void setVolume(float volume) { SetMovieVolume(_movie,(short)(volume*255));}
			/** get the current volume of this movie */
			float getVolume() { return GetMovieVolume(_movie) / 255.0f; }
			
			/** @return true, if this movie is looping */
			bool isLooping() const { return _isLooping; }
			
			void setLooping(bool loop) {
				if (_isLooping != loop) {
					_isLooping = loop;
					if (_isLooping) 
                    {
                        SetTimeBaseFlags(GetMovieTimeBase(_movie), loopTimeBase);
                    } else {
                        SetTimeBaseFlags(GetMovieTimeBase(_movie), 0);
					}
				}
			}
			
			/** sets the audio-balance -1 = right, 1 = left */
			void setAudioBalance(float f) {
				Float32 balance = f;
				SetMovieAudioBalance(_movie, balance, 0);
			}
			
			/** @return the current audio-balance-setting */
			float getAudioBalance() {
				Float32 balance;
				float f;
				GetMovieAudioBalance(_movie, &balance, 0);
				f = balance;
				return f;
			}
			
			/** @return true, if yuv-playback is used (much faster on os x) */
			inline bool getYUVPlaybackUsed() { return _yuvPlaybackUsed; }
			
			/** reopen the movie -- this will clear all memory previously allocated */			
			void reopen();
			
			/** deactivates the movie */
			inline void deactivate() {
				SetMovieActive(_movie, false);
				UpdateMovie(_movie);
				_activeFlag = false;
			}
			
			/** activates the movie */
			inline void activate() {
				SetMovieActive(_movie, true);
				UpdateMovie(_movie);
				_activeFlag = true;
			}
			
			/** @return true if the movie is active */	
			inline bool isActive() { return _activeFlag; }
				
			inline bool valid() const { return !_fError; }
			
			void preload(float startTime = 0, float duration=-1) 
			{
				if (duration < 0) duration = getMovieDuration();
				LoadMovieIntoRam(_movie, (TimeValue) (startTime*_timescale), (TimeValue) (duration*_timescale), keepInRam);
			}
            
            unsigned char* swapBuffers();
            
            bool hasVideoTrack() { return _hasVideoTrack; }
            
        protected:
			Movie           _movie;
            struct Buffer {
                char*			buffer;
                char*           pointer;
                GWorldPtr       gw;
                Buffer() : buffer(NULL), pointer(NULL), gw(NULL) {}
                Buffer(unsigned int i) : buffer(NULL), pointer(NULL), gw(NULL) {}
            };
            
            osg::buffered_value<Buffer> _buffers;
            
            unsigned int    _movieWidth, _movieHeight, _textureWidth, _textureHeight;
            float           _timescale;
            bool            _fError;
			float			_movieRate;
			bool			_yuvPlaybackUsed;
			std::string		_filename;
			bool			_activeFlag;
			bool			_isLooping;
			bool			_prerolled;
			bool			_hasVideoTrack;
            unsigned int    _currentVideoBufferNdx;
            bool            _doubleBuffering;
            
			/** inits the image for storage */
            void _initImage(osg::Image* image);
			
			/** inits the gWorld, where the movie gets drawn into */
            void _initGWorldStuff(osg::Image * image);
			
			/** inits the texture */
            void _initTexture();
			
            /** checks for an movie-error */
            inline void _checkMovieError(std::string msg) {
                if (GetMoviesError()) {
                    _fError = true;
                    osg::notify(osg::ALWAYS) << "MovieData: GetMoviesError fails at " << msg << std::endl;
                }
            }
			
			void setUseYUVPlayback(bool flag);
			GLenum computeInternalFormat();
            
    };


} // namespace

#endif
#endif
