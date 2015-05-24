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

#ifndef QUICKTIME_MOVIE_HEADER_
#define QUICKTIME_MOVIE_HEADER_

#include <cefix/Export.h>

#if defined CEFIX_QUICKTIME_AVAILABLE

#include <osg/ImageStream>
#include <cefix/VideoNewFrameCallback.h>
#include <cefix/VideoMedia.h>
#include <cefix/Video.h>

namespace cefix {

    class MovieData;
    class CoreVideoAdapter;
    
    /** 
     * this class is an alternative implementation for streaming quicktime movies into image-objects
     * you should use this class to play movies, instead of QuicktimeImageStream, which doesn't work reliably
     */
    class CEFIX_EXPORT QuicktimeMovie : public Video, public VideoMediaAdapter<QuicktimeMovie> {
    
        public: 
            /// Constructor, opens the movie from fileName
            /// @param movie to open
            QuicktimeMovie(std::string fileName = "");
            
            /// Destructor
            virtual ~QuicktimeMovie();
            
            virtual Object* clone(const osg::CopyOp &copyop) const { return new QuicktimeMovie(); }
            virtual bool isSameKindAs(const Object* obj) const {
                return dynamic_cast<const QuicktimeMovie*>(obj) != NULL;
            }
            
            virtual const char* className() const { return "QuicktimeMovie"; }

            /// Start or continue stream.
            virtual void play();
			
			/** @return true, if a movie is playing */
			
			virtual bool isPlaying() const { return (_currentRate != 0); }
            
            /// sets the movierate
            virtual void setMovieRate(double rate);
			
			/// gets the movierate
			virtual double getMovieRate() const;
            
            /// Pause stream at current position.
            virtual void pause();

            /// Rewind stream to beginning.
            virtual void rewind();
            
            /// forward stream to the end
            virtual void forward();
            
            /// stop playing 
            virtual void quit(bool=true);
        
            /// Get total length in seconds.
            virtual double getLength() const { return _len; }
            
            /// jumps to a specific position 
            virtual void jumpTo(double pos);        
            
            /// @return the current moviedata-object
            MovieData* getMovieData() { return _data; }
            
            /// loads a movie from fileName
            virtual void load(const std::string& fileName);
			
			/** @return the current volume as float */
			virtual float getVolume() const;
			
			/** sets the volume of this quicktime to v*/
			virtual void setVolume(float v);
			
			/** @return the current balance-setting (0 = neutral, -1 = left, 1 = right */
			virtual float getAudioBalance();	
			/** sets the current balance-setting (0 = neutral, -1 = left, 1 = right */
			virtual void setAudioBalance(float b);
			
			inline void dirty() {
				if (!getCoreVideoAdapter()) osg::ImageStream::dirty();
			}
			
			virtual float getVideoWidth();
			virtual float getVideoHeight();
			
			/** close and reopen the mvie to release previously used ram */
			void flushUsedRam();
			
			virtual void deactivate();
			virtual void activate();
			virtual bool isActive() const;
			
			

			static void disableQuicktimePlaying() { s_quicktime_playing_disabled = true; }
			
			void idle();
			
			virtual bool valid() const;
			
			void preload(float startTime = 0.0f, float duration = -1.0f);
        
            void setCoreVideoAdapter(CoreVideoAdapter* adapter);
			            
        protected:
            float _lastUpdate;
            float _len;
            
            float _currentRate;
            MovieData*                  _data;

			osg::ImageStream::LoopingMode _cachedLoopingMode;
			
			
            void checkLoop();
			void checkIfMovieIsDone();
			
			
            inline void checkForUpdate();

			static bool s_quicktime_playing_disabled;
                

        friend class QuicktimeMovieMedia;
		
    };


}

#endif

#endif
