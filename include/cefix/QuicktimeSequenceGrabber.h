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



#ifndef _QUICKTIME_SEQUENCE_GRABBER_HEADER_
#define _QUICKTIME_SEQUENCE_GRABBER_HEADER_

#include <cefix/Export.h>

#if defined CEFIX_QUICKTIME_AVAILABLE

#include <osg/Referenced>
#include <osg/Image>
#include <cefix/Log.h>
#include <cefix/QTUtils.h>
#include <cefix/VideoMedia.h>
#include <cefix/SequenceGrabber.h>

namespace cefix {
	/**
	 * this class encapuslates a Sequence Grabber. It polls a video-device on a regular basis and put the 
	 * resulting image into an osg::Image-object
	 * @author Stephan Maximilian Huber
	 */
	class CEFIX_EXPORT QuicktimeSequenceGrabber : public SequenceGrabber {
	
		public:
			/**
			 * create a new sequence grabber, using default input-device
			 * @param w grabbing-width
			 * @param h grabbing height
			 */
			QuicktimeSequenceGrabber(unsigned int w, unsigned int h, unsigned int rate);
			void setGrabbingDimensions(int w, int h)  {
				_boundsRect.top 	= 0;
				_boundsRect.left 	= 0;
				_boundsRect.bottom 	= h;
				_boundsRect.right 	= w;
			}
            
			/**
			 * create a new sequence grabber, using specified input-device
			 * @param inputName name of the input-device
			 * @param w grabbing-width
			 * @param h grabbing height
			 */
            QuicktimeSequenceGrabber(std::string inputName, unsigned int w, unsigned int h, unsigned int rate);
            
			/**
			 * updates the actual image from the attached device. If an error occured, the sequence grabber works
			 * in faking-mode: it updates the texture with a red rectangle
			 */
			virtual void idle() {
				OSErr err = SGIdle(_sequenceGrabberComponent);
				if ((err != noErr) || (_fakeTracking)){
					_checkError(err,"SGIdle failed");
					fakeTracking();
					newFrameAvailable();
				}
			}
			
			/// stops grabbing
			virtual void stop() {
                setRunning( false);
				SGStop(_sequenceGrabberComponent);
			}
			
			/// start grabbing
			virtual void start() {
				OSErr err = SGStartRecord(_sequenceGrabberComponent);
                setRunning( true );
				_checkError(err,"SGStartRecord failed");
			}
			
			/// @return returns an ImageSequence (don't use in your code)
			inline ImageSequence &getDecompressionSequence() { return _decomSeq;}
			
			/**
			 * installs an carbon-Timer for this grabber. if you are using the Sequence Grabber Manager, then 
			 * you don't need this function, use the IdleCallback-Object for that
			 * This methos works only on MacOSX
			 */
            void installCarbonTimer();

			/// shows the quicktime input-settings dialog
			virtual  void showSettings() { 
                UserData mySGVideoSettings = NULL;
				
                stop();
				
                SGSettingsDialog(_sequenceGrabberComponent, _videoChannel, 0, nil, 0L, 0, nil); 
                SGGetChannelSettings(_sequenceGrabberComponent, _videoChannel, &mySGVideoSettings, 0);
                                
				start();
			}
			
			/// destructor, cleans up all stuff
			virtual ~QuicktimeSequenceGrabber() {
				cleanup();
			}
            
            /**
			 * sets a feature of the input device, see the quicktime documentation for more info
			 * @param wantedFeature the feature to modify
			 * @param value the new value for the feature
			 */
            ComponentResult setFeature(OSType wantedFeature, float value);

			/// @return the value of a specific feature
            float getFeature(OSType wantedFeature);

			/// dumps a selection of features to the console
            void dumpFeatures();
            
			/// sets the gain of the input-device
            virtual void setGain(float val) { setFeature(vdIIDCFeatureGain,  (val < 0) ? val : val / 1000.0);}

			/// sets the focus of the input device -- this will disable the autofocus
            virtual void setFocus(float val) { setFeature(vdIIDCFeatureFocus,  (val < 0) ? val : val / 1000.0);}

			/// sets the exposure-value for the input-device
            virtual void setExposure(float val) {setFeature(vdIIDCFeatureExposure, (val < 0) ? val : val / 1000.0);}
			/// sets the shutter-value for the input-device
            virtual void setShutter(float val) {setFeature(vdIIDCFeatureShutter,  (val < 0) ? val : val / 1000.0);}
			
			virtual void setWhiteBalance(float val) { setFeature(vdIIDCFeatureWhiteBalanceU, (val < 0) ? val : val / 1000.0);}
			
			virtual void setBrightness(float val) { setFeature(vdIIDCFeatureBrightness, (val < 0) ? val : val / 1000.0);}
			
			
            /// @return the current focus-value
            virtual  float getFocus() { return getFeature(vdIIDCFeatureFocus)*1000; }
			
			virtual float getExposure() { return getFeature(vdIIDCFeatureExposure)*1000; }
			
			virtual float getShutter() { return getFeature(vdIIDCFeatureShutter)*1000; } 
			
			virtual float getGain() {  return getFeature(vdIIDCFeatureGain)*1000; } 
			
			virtual float getWhiteBalance() {  return getFeature(vdIIDCFeatureWhiteBalanceU)*1000; } 
			
			virtual float getBrightness() {  return getFeature(vdIIDCFeatureBrightness)*1000; } 
            
           
            static void populateDeviceInfoList(SequenceGrabberDeviceInfoList& deviceList);
			static const char* getGrabberId() { return "quicktime"; }
			
			/// @return true, if this grabber is a working one
			bool valid() {return (_fakeTracking == false); }

			
		private:
			/// setup the decompression
			ComponentResult _setupDecompression();
			/// init the grabber
			OSErr _initSequenceGrabber();

			/// init the grabber
            void _init(unsigned int w, unsigned int h);

			/// cleans up
			void cleanup();

			/// sets the input to a specified device
            void _setChannelInput(std::string inputName);
			
			/// save the settings for this device into the prefs-file
			/// Note: works only Mac OSX
            OSErr saveSettings(std::string inKey, UserData inUserData);

			/// load the settings for this device from the prefs-file
			/// Note: works only Mac OSX
            OSErr loadSettings(std::string inKey, UserData* outUserData);
            
			/// returns a key for the preferences-setting
            std::string _getKeyForPreferences() {
                if (_inputName.empty()) {
                    return std::string("videoin");
                } else {
                    return _inputName;
                }
            }
			
			void startFakeTracking();
			
			/// checks for any errors and put some warnings on the console
			bool _checkError(ComponentResult err, std::string str) {
				if (err !=0) {
					startFakeTracking();
					osg::notify(osg::ALWAYS) << "QuicktimeSequenceGrabber::error #" << err << ": " << str << std::endl;
				}
				return (err!=0);
			}
			/// @return the name of this media
			virtual std::string getName() { return "QuicktimeSequenceGrabber @ " +_inputName; }
			
		ImageSequence		_decomSeq;
		Rect				_boundsRect;
		GWorldPtr			_gWorld;
		SGChannel			_videoChannel;
		SeqGrabComponent    _sequenceGrabberComponent;
		bool				_fakeTracking;
		
		EventLoopTimerRef	_timerRef;
        std::string         _inputName;
		bool				_newFrame;
	
	};
}

#endif
#endif