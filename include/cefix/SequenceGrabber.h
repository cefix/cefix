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

#ifndef SEQUENCE_GRABBER_BASE_HEADER
#define SEQUENCE_GRABBER_BASE_HEADER

#include <osg/Image>
#include <osg/Array>
#include <string>
#include <cefix/VideoMedia.h>

namespace cefix {

class Serializer;

class SequenceGrabber : public osg::Referenced, public VideoMediaAdapter<SequenceGrabber> {

	public:
        struct DeviceInfo {
        
            DeviceInfo(const std::string& grabber_id, const std::string&  device_id, const std::string& display_name = "")
            :   _grabber(grabber_id),
                _id(device_id),
                _displayName(display_name)
            {
            }
            
            std::string getId(const std::string add_info = "") const { return (add_info.empty()) ? _id : _id + ":" + add_info; }
            
            const std::string& getGrabberId() const { return _grabber; }
            
            std::string getKey(const std::string add_info="") const 
            { 
                return getGrabberId() + "/" + getId(add_info); 
            }
            
            const std::string& getDisplayName() const { return (_displayName.empty()) ? _id : _displayName; }
            
            void setDisplayName(const std::string& name) { _displayName = name; }
            
        private:
            std::string _grabber, _id, _displayName;
        };
        
		struct Selection {
			osg::Vec2 topLeft, bottomLeft, topRight, bottomRight; 
			
			void set(float l, float t, float w, float h) {
				topLeft.set(l, t);
				topRight.set(l+w,t);
				bottomLeft.set(l, t+h);
				bottomRight.set(l+w, t+h);
			}
			Selection() {}
			Selection(osg::Vec2Array* v) {
				topLeft		= (*v)[0];
				topRight	= (*v)[1];
				bottomRight = (*v)[2];
				bottomLeft	= (*v)[3];
			}
			
			osg::Vec2Array* asVec2Array() 
			{
				osg::ref_ptr<osg::Vec2Array> v = new osg::Vec2Array();
				v->push_back(topLeft);
				v->push_back(topRight);
				v->push_back(bottomRight);
				v->push_back(bottomLeft);
				
				return v.release();
			}
				
			
			void writeTo(Serializer& serializer);
			void readFrom(Serializer& serializer);
		};
		
		SequenceGrabber(const std::string& name, unsigned int w, unsigned int h, unsigned int rate) 
		:	osg::Referenced(),
			VideoMediaAdapter<SequenceGrabber>("sequenceGrabber "+name),
			_image(NULL), 
			_width(w), 
			_height(h),
			_rate(rate),
			_name(name), 
			_newFrame(false), 
			_inited(false),
			_selection() 
		{
			_selection.set(0,0,w,h);
		}
		
		virtual void idle() = 0;
		virtual void start() = 0;
		virtual void stop() = 0;

		virtual void showSettings() = 0;
		
		inline osg::Image* getImage() { return _image.get(); }
		inline const std::string& getName() const { return _name; }
		inline unsigned int getWidth() const { return _width; }
		inline unsigned int getHeight() const { return _height; }
		inline unsigned int getRate() const { return _rate; }
		
		void setRate(unsigned int r) { _rate = r; }

		inline const std::string& getIdentifier() const { return _identifier; }
		inline void setIdentifier(const std::string& identifier) { _identifier = identifier; }
		
		inline bool isNewFrameAvailable() const { return _newFrame; }
		
		/// mark the image as dirty, when a new frame ist availabla
		inline void newFrameAvailable() 
        { 
			traverseCallbacks(getImage());
			_newFrame = true;
			_image->dirty(); 
		}
		
		inline bool isCapturing() const { return isRunning(); }
		
		bool valid() const { return (_image.valid() && _inited); }
		
		/// gets the gain (0 - 1000, -1 = not supported)
		virtual float getGain() { return -1; }
		/// gets the exposusre (0 - 1000, -1 = not supported)
		virtual float getExposure() { return -1; }
		/// gets the shutter (0 - 1000, -1 = not supported)
		virtual float getShutter() { return -1; }
		/// gets the focus (0 - 1000, -1 = not supported)
		virtual float getFocus() { return -1; }

		virtual float getWhiteBalance() { return -1; }
		virtual float getContrast() { return -1; }
		virtual float getBrightness() { return -1; }
		
		virtual void setGain(float f) {}
		virtual void setExposure(float f) {}
		virtual void setShutter(float f) {}
		virtual void setFocus(float f) {}
		virtual void setWhiteBalance(float f) {}
		virtual void setContrast(float f) {}
		virtual void setBrightness(float f) {}
		
		void writeTo(Serializer& serializer);
		void readFrom(Serializer& serializer);
		
		void saveSettings(const std::string& key);
		void loadSettings(const std::string& key);
		
		const Selection& getSelection() const { return _selection; }
		Selection& getSelection() { return _selection; }
		void setSelection(const Selection& s) { _selection = s; }
		
	protected:
		inline void setWidth(unsigned int w) { _width = w; }
		inline void setHeight(unsigned int h) { _height = h; }
		inline void setInited(bool f) { _inited = f; }
		inline void dirty() { _image->dirty(); _newFrame = true; }
		inline void setImage(osg::Image* img) { _image = img; _newFrame = true; }
		
		void fakeTracking();
		
		
		
	private:
		osg::ref_ptr<osg::Image>	_image;
		unsigned int				_width, _height, _rate;
		std::string					_name;
		bool						_newFrame;
	protected:
		bool						_inited;
		std::string					_identifier;
		Selection					_selection;
};

typedef std::vector<SequenceGrabber::DeviceInfo> SequenceGrabberDeviceInfoList;

}

#endif