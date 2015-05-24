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

#ifndef FAST_BLUR_HEADER
#define FAST_BLUR_HEADER

#include <osg/Image>
#include <cefix/Pixel.h>
#include <cefix/Export.h>

namespace cefix {


class CEFIX_EXPORT FastBlur : public osg::Referenced {

	public:
		FastBlur(osg::Image* source, float radius);
		
		osg::Image* blur() {
			osg::ref_ptr<osg::Image> dest = dynamic_cast<osg::Image*>(_source->clone(osg::CopyOp::DEEP_COPY_ALL));
			blur(dest.get());
			return dest.release();
		}
		void blur(osg::Image* dest);
		
		void setSourceAndRadius(osg::Image* img, unsigned int r) { _source = img; _pixel.setImage(_source.get()); init(); }
		

		
	virtual ~FastBlur() {}
	private:
		void init();
		
	private:
		osg::ref_ptr<osg::Image>	_source;
		int	_radius;
		int _w, _h, _wh, _hm, _wm;
		std::vector<int>	_r,_g,_b,_a, _vmin, _vmax, _dv;
		cefix::Pixel	_pixel;
};

}

#endif