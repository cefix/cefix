/*
 *  QuicktimeSequenceGrabber.cpp
 *  MoviePlayer
 *
 *  Created by Stephan Huber on 29.01.06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include <cefix/Export.h>

#if defined CEFIX_QUICKTIME_AVAILABLE

#include <cefix/QuicktimeSequenceGrabber.h>
#include <cefix/QTUtils.h>
#include <osg/gl>
#include <osg/ImageStream>
#include <cefix/StringUtils.h>


extern "C" {
pascal OSErr mySGDataProc(SGChannel c, 
                            Ptr p,
                            long len,
                            long *offset,
                            long chRefCon,
                            TimeValue time,
                            short writeType, 
                            long refCon);
							
static pascal void mgIdleTimer(EventLoopTimerRef inTimer, void *inUserData);
}

namespace cefix {

	QuicktimeSequenceGrabber::QuicktimeSequenceGrabber(unsigned int w, unsigned int h, unsigned int rate)
	:	SequenceGrabber(getGrabberId(), w, h, rate),
		_inputName(""), 
		_newFrame(false) 
	{
		_init(w,h);
	}
	
	QuicktimeSequenceGrabber::QuicktimeSequenceGrabber(std::string inputName, unsigned int w, unsigned int h, unsigned int rate) 
	:	SequenceGrabber("quicktime sequence grabber", w, h, rate),
		_fakeTracking(false), 
		_inputName(inputName), 
		_newFrame(false) 
	{
		_init(w,h);
	}
    
    
    void QuicktimeSequenceGrabber::_init(unsigned int w, unsigned int h) {
		_fakeTracking = false;
		_decomSeq = 0;
        setGrabbingDimensions(w,h);
		setImage(new osg::ImageStream());
		getImage()->setOrigin(osg::Image::TOP_LEFT);
        _timerRef = NULL;
		
		ComponentResult err;
		err = _initSequenceGrabber();
		err = _setupDecompression();
    }
	
	
	ComponentResult QuicktimeSequenceGrabber::_setupDecompression() {
	
		ComponentResult                     err = noErr;
		Rect                                sourceRect = { 0, 0, 0, 0 };
		MatrixRecord			scaleMatrix;	
		ImageDescriptionHandle              imageDesc = (ImageDescriptionHandle)NewHandle(0);
		
		/* Set up getting grabbed data into the GWorld */
		
		// retrieve a channel‚Äôs current sample description, the channel returns a sample description that is
		// appropriate to the type of data being captured
		err = SGGetChannelSampleDescription(_videoChannel,(Handle)imageDesc);
		if (_checkError(err, "Could not retrieve video channel")) return err; // BailErr(err);
		
		// make a scaling matrix for the sequence
		sourceRect.right = (**imageDesc).width;
		sourceRect.bottom = (**imageDesc).height;
		RectMatrix(&scaleMatrix, &sourceRect, &_boundsRect);
				
		// begin the process of decompressing a sequence of frames
		// this is a set-up call and is only called once for the sequence - the ICM will interrogate different codecs
		// and construct a suitable decompression chain, as this is a time consuming process we don't want to do this
		// once per frame (eg. by using DecompressImage)
		// for more information see Ice Floe #8 http://developer.apple.com/quicktime/icefloe/dispatch008.html
		// the destination is specified as the GWorld
		_decomSeq = 0;
		err = DecompressSequenceBegin(&_decomSeq,	// pointer to field to receive unique ID for sequence
										imageDesc,		// handle to image description structure
										_gWorld,		// port for the DESTINATION image
										NULL,			// graphics device handle, if port is set, set to NULL
										NULL,			// source rectangle defining the portion of the image to decompress
										&scaleMatrix,	// transformation matrix
										srcCopy,		// transfer mode specifier
										NULL,			// clipping region in dest. coordinate system to use as a mask
										0,						// flags
										codecNormalQuality,		// accuracy in decompression
										bestSpeedCodec);		// compressor identifier or special identifiers ie. bestSpeedCodec
	
		
		DisposeHandle((Handle)imageDesc);
		imageDesc = NULL;
		
		if (_checkError(err, "DecompressionSequenceBegin failed")) return err;
		
		
		return (err);
	}
	
	
	

	OSErr QuicktimeSequenceGrabber::_initSequenceGrabber() {
		
		OSErr	err = noErr;
		
		/* initialize the movie toolbox */
		initQuicktime();
		
		
		// open the sequence grabber component and initialize it
		_sequenceGrabberComponent = OpenDefaultComponent(SeqGrabComponentType, 0);
		if (_checkError((_sequenceGrabberComponent == NULL), "OpendDefaultComponent failed")) return err;
		
		err = SGInitialize(_sequenceGrabberComponent);
		if (_checkError(err, "SGinitialize failed")) return err;
		
		// specify the destination data reference for a record operation
		// tell it we're not making a movie
		// if the flag seqGrabDontMakeMovie is used, the sequence grabber still calls
		// your data function, but does not write any data to the movie file
		// writeType will always be set to seqGrabWriteAppend
		err = SGSetDataRef(_sequenceGrabberComponent, 0, 0, seqGrabDontMakeMovie);
		if (_checkError(err, "SGSetDataRef failed")) return err;

		// create a new sequence grabber video channel
		err = SGNewChannel(_sequenceGrabberComponent, VideoMediaType, &_videoChannel);
		_checkError(err, "SGNewChannel failed");
		
        if (!_inputName.empty()) {
            _setChannelInput(_inputName);
        }
		
		//err = SGSetChannelBounds(_sequenceGrabberComponent, &_boundsRect);
		//if  (_checkError(err, "SGSetChannelBounds failed")) ;
		void* buffer;
		char* pointer;
		pointer = (char*)malloc(4 * _boundsRect.right * _boundsRect.bottom + 32);

		if (pointer == NULL) {
			osg::notify(osg::FATAL) << "QuicktimeSequenceGrabber: " << "Can't allocate texture buffer" << std::endl;
			return -1;
		}
		    
		buffer = (void*)(((unsigned long)(pointer + 31) >> 5) << 5);

		GLenum internalformat = getBestInternalFormatForQuicktime();
		
		getImage()->setImage(_boundsRect.right,_boundsRect.bottom,1,
                   (GLint) GL_RGBA8,
                   (GLenum)GL_BGRA_EXT,
                   internalformat,
                   (unsigned char*) buffer,osg::Image::NO_DELETE,4);

		err = QTNewGWorldFromPtr(&_gWorld, getNativePixelFormatForQuicktime(), &_boundsRect, NULL, NULL, 0, getImage()->data(), 4 * getImage()->s());
		/*
		// create the GWorld
		err = QTNewGWorld(&_gWorld,	// returned GWorld
						  k32ARGBPixelFormat,		// pixel format
						  &_boundsRect,				// bounding rectangle
						  0,						// color table
						  NULL,						// graphic device handle
						  0);						// flags*/
		if (_checkError(err, "QTNewGWorld failed")) return err;
		
		
	   
		

		err = SGSetGWorld(_sequenceGrabberComponent, _gWorld, GetMainDevice());
		if (_checkError(err, "SGSetGWorld failed")) return err;

		
				   
		/*_image->setImage(_boundsRect.right,_boundsRect.bottom,1, 4, GL_RGBA, GL_UNSIGNED_BYTE, 
			pixBaseAddr,
			osg::Image::NO_DELETE,
			0);
		*/			  
					  
		// set the bounds for the channel
		err = SGSetChannelBounds(_videoChannel, &_boundsRect);
	    _checkError(err,"SGSetChannelBounds failed");
		
		// set the usage for our new video channel to avoid playthrough
		// note: we do not set seqGrabPlayDuringRecord because if you set this flag
		// the data from the channel may be played during the record operation,
		// if the destination buffer is onscreen. However, playing the
		// data may affect the quality of the recorded sequence by causing frames 
		// to be dropped...something we definitely want to avoid
		err = SGSetChannelUsage(_videoChannel, seqGrabRecord | seqGrabLowLatencyCapture);
		_checkError(err,"SGSetChannelUsage failed");
		
		// specify a data function for use by the sequence grabber
		// whenever any channel assigned to the sequence grabber writes data,
		// this data function is called and may then write the data to another destination
		err = SGSetDataProc(_sequenceGrabberComponent,NewSGDataUPP(&::mySGDataProc),(long)this);
		_checkError(err,"SGSetDataProc failed");

        // hier versuchen wir unsere Prefs zu laden und dem VideoChannel zuzuweisen
        UserData mySGVideoSettings = NULL;

        err = loadSettings(_getKeyForPreferences(), &mySGVideoSettings);
        if (mySGVideoSettings != NULL) {
            SGSetChannelSettings(_sequenceGrabberComponent, _videoChannel, mySGVideoSettings, 0);
            DisposeUserData(mySGVideoSettings);
        }
#ifdef __APPLE__
		err = SGSetFrameRate(_videoChannel, IntToFixed(getRate()));
        _checkError(err,"SGSetFrameRate failed");
#endif
		
		/* lights...camera... */
		err = SGPrepare(_sequenceGrabberComponent,false,true);
		_checkError(err,"SGPrepare failed");
		
		// start recording!!
		err = SGStartRecord(_sequenceGrabberComponent);
		_checkError(err,"SGStartRecord failed");	

		return err;
	}
    
    void QuicktimeSequenceGrabber::installCarbonTimer() {
#ifdef __APPLE__
		const EventTime kTimerInterval = kEventDurationSecond /60.0;
        OSErr err = InstallEventLoopTimer(GetMainEventLoop(), kEventDurationNoWait, kTimerInterval, NewEventLoopTimerUPP(mgIdleTimer), (void*) this , &_timerRef);
        _checkError(err, "InstallEventLoopTimer failed");
#else
		osg::notify(osg::ALWAYS) << "QuicktimeSequenceGrabber::installCarbonTimer not supportet" << std::endl;
#endif
    }
    

	
    
    void QuicktimeSequenceGrabber::_setChannelInput(std::string selectedInputName) {
    
        SGDeviceListRecord *record;
        SGDeviceList deviceList = NULL;
        int i;
        bool found = false;
        
        OSErr err = SGGetChannelDeviceList(_videoChannel, sgDeviceListIncludeInputs, &deviceList);
        if (_checkError(err, "SGGetChannelDeviceList failed")) return;
        record = *deviceList;
        
        for(i = 0; i < record->count; i++)
        {
            SGDeviceName deviceName = record->entry[i];
            SGDeviceInputListRecord *inputListRecord;
            SGDeviceInputName inputName;
            std::string theName;
            
            if(deviceName.inputs == NULL)  {
                continue;
            }
            
            inputListRecord = *(deviceName.inputs);
            
            // we have to check all input devices's names whether they correspond to the device name to select
            for(int j = 0; j < inputListRecord->count; j++)  {
            
                inputName = inputListRecord->entry[j];
                theName = cefix::convertFromPascalString((const char*)inputName.name);
                                
                if(theName == selectedInputName) {

                    found = true;
                    ComponentResult err;
                    
                    // we have to use the name of the device, not the input device itself
                    err = SGSetChannelDevice(_videoChannel, deviceName.name);
                    if (_checkError(err, "SGSetChannelDevicefailed!")) goto bail;
                    
                    
                    // now we can set the actual input device by its index
                    err = SGSetChannelDeviceInput(_videoChannel, j);
                    if (_checkError(err, "SGSetChannelDeviceInput failed!")) goto bail; 
                    

                } 
            }
        }   
        
        bail:
        
        if (!found) {
            osg::notify(osg::FATAL) << "QuicktimeSequenceGrabber::setChannelInput : could not find input device named '" << selectedInputName << "'" << std::endl;
        }
		if (err) {
			osg::notify(osg::FATAL) << "QuicktimeSequenceGrabber::setChannelInput : input device '" << selectedInputName << "' found, but error ocurred" << std::endl;
		}
        
        if (deviceList)  SGDisposeDeviceList(_videoChannel, deviceList);
       
    }
    
    
	
	void QuicktimeSequenceGrabber::cleanup() {
		
		ComponentResult result;
		OSErr 			err;
	
#ifdef __APPLE__
		if (_timerRef) RemoveEventLoopTimer(_timerRef);
#endif
		
		// stop recording
		stop();
		// end our decompression sequences
		if (!_fakeTracking)
			err = CDSequenceEnd(_decomSeq);

		// finally, close our sequence grabber component
		result = CloseComponent(_sequenceGrabberComponent);
		
		// get rid of our gworld
		DisposeGWorld(_gWorld);
	}
    
    
	
void EndianUtils_Float_NtoB (float *theFloat)
{
#if TARGET_RT_LITTLE_ENDIAN
  unsigned long  *myLongPtr;
  
  myLongPtr = (unsigned long *)theFloat;
  *myLongPtr = EndianU32_NtoB(*myLongPtr);
#endif
}

void EndianUtils_Float_BtoN (float *theFloat)
{
#if TARGET_RT_LITTLE_ENDIAN
  unsigned long  *myLongPtr;
  
  myLongPtr = (unsigned long *)theFloat;
  *myLongPtr = EndianU32_BtoN(*myLongPtr);
#endif
}
    
    ComponentResult QuicktimeSequenceGrabber::setFeature(OSType wantedFeature, float value) {
            QTAtomContainer         atomContainer;
            QTAtom                  featureAtom;
            VDIIDCFeatureSettings   settings;
            VideoDigitizerComponent vd;
            ComponentDescription    desc;
            ComponentResult         result = paramErr;
			
			//std::cout << wantedFeature << ": " << value << std::endl;

            if (NULL == _videoChannel) goto bail;

            // get the digitizer and make sure it's legit
            vd = SGGetVideoDigitizerComponent(_videoChannel);
            if (NULL == vd) goto bail;

            GetComponentInfo((Component)vd, &desc, NULL, NULL, NULL);
            if (vdSubtypeIIDC != desc.componentSubType) goto bail;

            // *** now do the real work ***

            // return the gain feature in an atom container
            result = VDIIDCGetFeaturesForSpecifier(vd, wantedFeature, &atomContainer);
            if (noErr == result) {

                // find the feature atom
                featureAtom = QTFindChildByIndex(atomContainer, kParentAtomIsContainer,
                                                 vdIIDCAtomTypeFeature, 1, NULL);
                if (0 == featureAtom) { result = cannotFindAtomErr; goto bail; }

                // find the gain settings from the feature atom and copy the data
                // into our settings
                result = QTCopyAtomDataToPtr(atomContainer,
                                             QTFindChildByID(atomContainer, featureAtom,
                                             vdIIDCAtomTypeFeatureSettings,
                                             vdIIDCAtomIDFeatureSettings, NULL),
                                             true, sizeof(settings), &settings, NULL);
                if (noErr == result) {
                    /* When indicating capabilities, the flag being set indicates that the
                       feature can be put into the given state.
                       When indicating/setting state, the flag represents the current/desired
                       state. Note that certain combinations of flags are valid for capabilities
                       (i.e. vdIIDCFeatureFlagOn | vdIIDCFeatureFlagOff) but are mutually
                       exclusive for state.
                     */
                    // is the setting supported?
                    if (value < 0)  {
                        //set to auto mode if available
                        settings.state.flags = EndianU32_NtoB(vdIIDCFeatureFlagOn | vdIIDCFeatureFlagAuto | vdIIDCFeatureFlagTune);
                    } else {
						UInt32 mask = EndianU32_NtoB(vdIIDCFeatureFlagOn | vdIIDCFeatureFlagManual | vdIIDCFeatureFlagRawControl);
						
						if (settings.capabilities.flags & mask) {
                            // set state flags
                            settings.state.flags = EndianU32_NtoB(vdIIDCFeatureFlagOn |  vdIIDCFeatureFlagManual | vdIIDCFeatureFlagRawControl);

                            // set value - will either be 500 or the max value supported by
                            // the camera represented in a float between 0 and 1.0
							UInt16 max = settings.capabilities.rawMaximum;
							//EndianU16_BtoN(max);
							float val = value;
							EndianUtils_Float_NtoB(&val);
                            settings.state.value = val;
							// std::cout << max << " " << value <<	"/" << val << " -> " <<  settings.state.value << std::endl;
                        } 
                        else {
                            // can't do it!
                            result = featureUnsupported;
                            goto bail;
                        }
                    }

                        // store the result back in the container
                    result = QTSetAtomData(atomContainer,
                                           QTFindChildByID(atomContainer, featureAtom,
                                           vdIIDCAtomTypeFeatureSettings,
                                           vdIIDCAtomIDFeatureSettings, NULL),
                                           sizeof(settings), &settings);
                    if (noErr == result) {
                        // set it on the device
                        result = VDIIDCSetFeatures(vd, atomContainer);
                    }
                }
            }

        bail:
            if (result != 0) {
                log::error("SequenceGrabberManager") << "setFeature failed: " << result << std::endl; 
            }
            
            return result;
        }
        
        float QuicktimeSequenceGrabber::getFeature(OSType wantedFeature) {
            QTAtomContainer         atomContainer;
            QTAtom                  featureAtom;
            VDIIDCFeatureSettings   settings;
            VideoDigitizerComponent vd;
            ComponentDescription    desc;
            float                   result = -1.0;

            if (NULL == _videoChannel) goto bail;

            // get the digitizer and make sure it's legit
            vd = SGGetVideoDigitizerComponent(_videoChannel);
            if (NULL == vd) goto bail;

            GetComponentInfo((Component)vd, &desc, NULL, NULL, NULL);
            if (vdSubtypeIIDC != desc.componentSubType) goto bail;

            // *** now do the real work ***

            // return the gain feature in an atom container
            result = VDIIDCGetFeaturesForSpecifier(vd, wantedFeature, &atomContainer);
            if (noErr == result) {

                // find the feature atom
                featureAtom = QTFindChildByIndex(atomContainer, kParentAtomIsContainer,
                                                 vdIIDCAtomTypeFeature, 1, NULL);
                if (0 == featureAtom) { result = cannotFindAtomErr; goto bail; }

                // find the gain settings from the feature atom and copy the data
                // into our settings
                result = QTCopyAtomDataToPtr(atomContainer,
                                             QTFindChildByID(atomContainer, featureAtom,
                                             vdIIDCAtomTypeFeatureSettings,
                                             vdIIDCAtomIDFeatureSettings, NULL),
                                             true, sizeof(settings), &settings, NULL);
                if (noErr == result) {
                    result =  settings.state.value;
					EndianUtils_Float_BtoN(&result);
                }
            }

        bail:
            return result;
        }
        
        void QuicktimeSequenceGrabber::dumpFeatures() {
        
            osg::notify(osg::ALWAYS) << "Focus:     " << getFeature(vdIIDCFeatureFocus) << std::endl;
            osg::notify(osg::ALWAYS) << "Gain:      " << getFeature(vdIIDCFeatureGain) << std::endl;
            osg::notify(osg::ALWAYS) << "Exposure:  " << getFeature(vdIIDCFeatureExposure) << std::endl;
            osg::notify(osg::ALWAYS) << "Shutter:   " << getFeature(vdIIDCFeatureShutter) << std::endl;
            
        }
        
        
        OSErr QuicktimeSequenceGrabber::saveSettings(std::string inKey, UserData inUserData) {
            CFDataRef theCFSettings;
            Handle hSettings;
            OSErr err;
            CFStringRef cfInkey = CFStringCreateWithCStringNoCopy(NULL, inKey.c_str(), kCFStringEncodingMacRoman, NULL);
            if (NULL == inUserData) return paramErr;

            hSettings = NewHandle(0);
            err = MemError();

            if (noErr == err) {
                err = PutUserDataIntoHandle(inUserData, hSettings); 


                if (noErr == err) {
                    HLock(hSettings);


                    theCFSettings = CFDataCreate(kCFAllocatorDefault,  (UInt8 *)*hSettings, GetHandleSize(hSettings));
#ifdef __APPLE__
                    if (theCFSettings) {
                        CFPreferencesSetAppValue(cfInkey, theCFSettings, kCFPreferencesCurrentApplication);
                        CFPreferencesAppSynchronize(kCFPreferencesCurrentApplication);
                        CFRelease(theCFSettings);
                    }
#else
					osg::notify(osg::ALWAYS) << "QuicktimeSequenceGrabber::savesettings not implemented for windows" << std::endl;
#endif
                }

                DisposeHandle(hSettings);
            }
            return err;
        }

        OSErr QuicktimeSequenceGrabber::loadSettings(std::string inKey, UserData* outUserData) {
        
            
            Handle theHandle = NULL;
            UserData theUserData = NULL;
            OSErr err = paramErr;
            CFStringRef cfInKey = CFStringCreateWithCStringNoCopy(NULL, inKey.c_str(), kCFStringEncodingMacRoman, NULL);
             
            // read the new setttings from our preferences
#ifdef __APPLE__
			CFPropertyListRef theCFSettings;
            theCFSettings = CFPreferencesCopyAppValue(cfInKey, kCFPreferencesCurrentApplication);
            if (theCFSettings) {
                err = PtrToHand(CFDataGetBytePtr((CFDataRef)theCFSettings), &theHandle,
                CFDataGetLength((CFDataRef)theCFSettings));


                CFRelease(theCFSettings);
                if (theHandle) {
                    err = NewUserDataFromHandle(theHandle, &theUserData);
                    if (theUserData) {
                        *outUserData = theUserData;
                    }
                    DisposeHandle(theHandle);
                }
            }

            return err;
#else
			osg::notify(osg::ALWAYS) << "QuicktimeSequenceGrabber::savesettings not implemented for windows" << std::endl;
			return -1;
#endif
        }

void QuicktimeSequenceGrabber::startFakeTracking() {
	_fakeTracking = true; 
}

void QuicktimeSequenceGrabber::populateDeviceInfoList(SequenceGrabberDeviceInfoList& in_deviceList) {
	
	osg::notify(osg::INFO) << "QuicktimeSequenceGrabber::populateDeviceList" << std::endl;
	

	
	ComponentDescription	theDesc;
	Component				sgCompID;
	ComponentResult 		result;
	ComponentInstance       seqGrabber(0);
	SGChannel               videoChannel;
	SGDeviceList            deviceList(0);
	
	// Find and open a sequence grabber
	theDesc.componentType			= SeqGrabComponentType;
	theDesc.componentSubType 		= 0L;
	theDesc.componentManufacturer 	= 'appl';
	theDesc.componentFlags 			= 0L;
	theDesc.componentFlagsMask 		= 0L;	
	sgCompID = FindNextComponent (NULL, &theDesc);
	if (sgCompID != 0)
	{
		seqGrabber = OpenComponent (sgCompID);
		if (seqGrabber)
		{
			result = SGInitialize (seqGrabber);
			if (result == noErr)
			{				
				// Get a video channel
				result = SGNewChannel (seqGrabber, VideoMediaType, &videoChannel);
				if (result == noErr)
				{
					/* build a device list */
					result = SGGetChannelDeviceList(videoChannel,sgDeviceListIncludeInputs,&deviceList);
				}
				else	/* error!! */
				{
					osg::notify(osg::FATAL) << "QuicktimeSequenceGrabber::no device attached?! #" << result << std::endl;
					return;
				}
			}
			result = CloseComponent(seqGrabber);
		}
	}
	
	
	if (!deviceList) {
		osg::notify(osg::FATAL) << "QuicktimeSequenceGrabber::no device attached?! " << std::endl;
		return;
	}

	for (int i = 0; i< (*deviceList)->count; i++) {
		SGDeviceName nameRec;
		SGDeviceInputList deviceInputList = NULL;
		nameRec = (*deviceList)->entry[i];
		deviceInputList = nameRec.inputs;
		if (deviceInputList) {
			for(int j = 0; j < (*deviceInputList)->count; j++) {
				// inputs durchackern
				std::string digitizer = cefix::convertFromPascalString((const char*)nameRec.name);
				std::string device = cefix::convertFromPascalString((const char*)(*deviceInputList)->entry[j].name);
				osg::notify(osg::INFO) 
					<< "Digitizer: '" << digitizer 
					<< "' Device-Name: '" << device << "'" <<std::endl;
                
				DeviceInfo d(getGrabberId(), device);
                in_deviceList.push_back(d);
			}
		}
		
	}
	
	SGDisposeDeviceList(videoChannel, deviceList);
}

        	
	
} // end of namespace

extern "C" {

	static pascal void mgIdleTimer(EventLoopTimerRef inTimer, void *inUserData)
    {   
        #pragma unused(inTimer)
        cefix::QuicktimeSequenceGrabber* grabber = (cefix::QuicktimeSequenceGrabber*)inUserData;
        
        if (NULL != grabber)
			grabber->idle();
        
    }
	
	
	pascal OSErr mySGDataProc(SGChannel c, 
								Ptr p,
								long len,
								long *offset,
								long chRefCon,
								TimeValue time,
								short writeType, 
								long refCon)
	{
	#pragma unused(offset,chRefCon,time,writeType)
		
		CodecFlags 		ignore;
		ComponentResult err = noErr;

		/* grab the time scale for use with our fps calculations - but this 
			needs to be done only once */
		cefix::QuicktimeSequenceGrabber* grabber = (cefix::QuicktimeSequenceGrabber*)refCon;

		if(grabber) 
		{
			// decompress a frame into the GWorld - can queue a frame for async decompression when passed in a completion proc
			// once the image is in the GWorld it can be manipulated at will
			err = DecompressSequenceFrameS(grabber->getDecompressionSequence(),	// sequence ID returned by DecompressSequenceBegin
											p,                                      // pointer to compressed image data
											len,					// size of the buffer
											0,                                      // in flags
											&ignore,				// out flags
											NULL);					// async completion proc
			//std::cout << writeType << " t: " << time << " err: " << err << " len " << len << std::endl;
			// tell the class that some update of the gworld took place
			grabber->newFrameAvailable();
			// ******  IMAGE IS NOW IN THE GWORLD ****** /
		}
		
		
		
		return err;
	}

}

#endif