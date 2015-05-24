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
 
#ifndef HISTOGRAM_HEADER
#define HISTOGRAM_HEADER
#include <cefix/Pixel.h>
#include <cefix/ColorUtils.h>
#include <iostream>

namespace cefix {

namespace Histogram {
/** template comparator-class to compare the grey-values of two colors */
template <int maxValues = 255>
struct CompareGrey {
	
	enum  { count = maxValues } value_count;
	
	unsigned int operator()(const osg::Vec4& inc) {
		return maxValues * cefix::getGreyValue(inc);
	}
		
};


template <int maxValues = 255>
struct CompareHue {
	
	enum  { count = maxValues } value_count;

	unsigned int operator()(const osg::Vec4& inc) {
		float h1,s1,v1;
		cefix::RGBtoHSV(inc, h1, s1, v1);
		return maxValues * (h1 / 360.0);
	}
};

template <int maxValues = 255>
struct CompareSaturation {
	
	enum  { count = maxValues } value_count;

	unsigned int operator()(const osg::Vec4& inc) {
		float h1,s1,v1;
		cefix::RGBtoHSV(inc, h1, s1, v1);
		return maxValues * s1;
	}
	

};

template <int maxValues = 255>
struct CompareBrightness {
	
	enum  { count = maxValues } value_count;

	unsigned int operator()(const osg::Vec4& inc) {
		float h1,s1,v1;
		cefix::RGBtoHSV(inc, h1, s1, v1);
		return maxValues * v1;
	}
	

};


}

template <class CompareFunction = Histogram::CompareGrey<255> >
class HistogramT : public osg::Referenced {
	protected:

	public:
		struct CountColorPair {
			unsigned int count;
			osg::Vec4 color;
			
			CountColorPair(unsigned int cnt, const osg::Vec4& c) : count(cnt), color(c) {}
			CountColorPair(const CountColorPair& ccp) : count(ccp.count), color(ccp.color) {}
			CountColorPair() : count(0), color(osg::Vec4(1,1,1,1)) {}
		};
		
		typedef std::map<unsigned int, CountColorPair> ColorMap;
		typedef CompareFunction compare_function;
		
		
		HistogramT(osg::Image* image) : 
			osg::Referenced(),
			_compare()
		{ 
			operator()(image); 
		}
		
		HistogramT() : 
			osg::Referenced(), 
			_compare() 
		{
		}
		
		HistogramT(const CompareFunction& c) : 
			osg::Referenced(), 
			_compare(c) 
		{
		}
		
		inline void setCompareFunction(const CompareFunction& c) { _compare = c; }
		
		inline osg::Vec4 getNearestColor(const osg::Vec4& c) 
		{
			unsigned int ndx = _compare(c);
			typename ColorMap::iterator itr = _colorMap.find(ndx);
			return (itr == _colorMap.end()) ? c : itr->second.color;
		}
		
		inline unsigned int getHistogramIndex(const osg::Vec4& c) 
		{
			return _compare(c);
		}
		
		void operator()(osg::Image* image, unsigned int dx = 1, unsigned int dy = 1) 
		{
			_colorMap.clear();
			cefix::Pixel pixel(image);
			_pixelCount = 0;
			osg::Vec4 c;
			unsigned int ndx;
			for(int y = 0; y < image->t(); y+= dy) 
			{
				for(int x = 0; x < image->s(); x += dx) 
				{
					c = pixel.getColor(x,y);
					ndx = _compare(c);
					if (_colorMap.find(ndx) == _colorMap.end())
						_colorMap[ndx] = CountColorPair(0,c);
					if (0) {
						_colorMap[ndx].count +=1;
					} else {
						typename ColorMap::iterator itr = _colorMap.find(ndx);
						unsigned int m = itr->second.count + 1;
						float fm = static_cast<float>(m);
						if (m != 1) {
							c = itr->second.color * (itr->second.count / fm) + (c / fm);
							_colorMap.erase(itr);
						}
						_colorMap[ndx] = CountColorPair(m, c);
					}
					++_pixelCount;
				}
			}
			unsigned int pc = _pixelCount;
			_pixelCount = 0;
			for(typename ColorMap::iterator i = _colorMap.begin(); i != _colorMap.end(); ++i) {
				_pixelCount += i->second.count;
			}
			if (pc != _pixelCount) {
				std::cout << "fuck " << pc << " != " << _pixelCount << std::endl;
			}
		}	
		
		ColorMap& getColorMap() { return _colorMap; }	
		unsigned int getPixelCount() { return _pixelCount; }
	
	private:
		ColorMap		_colorMap;
		unsigned int	_pixelCount;
		CompareFunction	_compare;
		

};


}
#endif