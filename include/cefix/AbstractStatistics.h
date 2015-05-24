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

#ifndef ABSTRACT_STATISTICS_HEADER
#define ABSTRACT_STATISTICS_HEADER

#include <string>
#include <vector>
#include <osg/Group>
#include <cefix/MathUtils.h>
#include <cefix/Export.h>
#include <cefix/TextGeode.h>

namespace cefix {

/** a base-class for statistics, it stores all values and requests redraws when necessary. Scaling etc is done here */
class CEFIX_EXPORT AbstractStatistics : public osg::Group {

	public:
	
		class GroupedCaptionLocations : public osg::Referenced {
		
			public:
				GroupedCaptionLocations() : osg::Referenced(), _max(7) {}
				void clear();
				
				void get(const osg::Vec3& loc, cefix::Utf8TextGeode* textgeode);
				
				void adjustLocations();
				
			private:
				typedef std::multimap<osg::Vec3, osg::ref_ptr<cefix::Utf8TextGeode> > LocationMap;
				LocationMap _locations;
				float				_max;
		
		};
	
		/** ctor
		    @param caption: caption for this statistic
			@param numValues: amount of samples to hold 
		*/
		AbstractStatistics(const std::string& caption, unsigned int numValues = 1000) : osg::Group(), _numValues(numValues), _currentNdx(0), _caption(caption), _min(0), _max(0), _yscale(1) {
			_values.resize(numValues);
	
			_tendencies.resize(numValues);
			_x_stepsize=1;
		}
		
		/** adds a value and a tendency (tendency not implemented yet) */
		inline void addValue(float v, float tendency = 0) {
			_currentNdx++;
			if (_currentNdx >= _numValues) _currentNdx = 0;
			_values[_currentNdx] = v;
			_tendencies[_currentNdx] = tendency;
			computeMinMax();
			updateGeometry();
			updateCaption();
		}

		/** merges a value and a tendency (tendency not implemented yet) */
		inline void mergeValue(float v, float tendency = 0) {	
			_values[_currentNdx] = cefix::interpolate(_values[_currentNdx], v, 0.5f);
			_tendencies[_currentNdx] = cefix::interpolate(_tendencies[_currentNdx], tendency, 0.5f);
			computeMinMax();
			updateGeometry();
			updateCaption();
		}


		
		/** @return the current index */
		inline unsigned int getCurrentIndex() { return _currentNdx; }
		inline void setCurrentIndex(unsigned int n) { _currentNdx = n; }
		/** @return the current value */
		inline float getCurrentValue() { return _values[_currentNdx]; }
		
		/** @return the min-value */
		inline float getMin() { return _min; }
		
		/** @return the max-value */
		inline float getMax() { return _max; }
		
		/** @return the amount of stored values */
		inline unsigned int getNumValues() { return _numValues; }
		
		/** @return the caption */
		inline const std::string& getCaption() { return _caption; }
		
		/** sets the color */
		void setColor(osg::Vec4 c) { _color = c; }
		
		/** @return the color */
		inline const osg::Vec4& getColor() { return _color; }
		
		/** sets the x-step */
		void setXStepSize(float x) { _x_stepsize = x; }
		
		/** @return the x-step-size */
		float getXStepSize() { return _x_stepsize; }
		
		/** recompute the whole geometry */
		void recomputeGeometry() {
			unsigned int old = _currentNdx;
			for(unsigned int i = 0; i < getNumValues(); ++i) {
				_currentNdx = i;
				updateGeometry();
			}
			_currentNdx = old;
			updateGeometry();
			updateCaption();
		}
		
		
		
		/** sets the y-scale */
		void setYScale(float y) { _yscale = y; recomputeGeometry(); }
		
		/** @return the y-scale */
		float getYScale() { return _yscale; }
		
		void setGroupedCaptionLocations(GroupedCaptionLocations* captionLocations) { _locations = captionLocations;}

		virtual void updateCaption() = 0;


	protected:
		virtual void updateGeometry() = 0;
				
		virtual ~AbstractStatistics() {}
			
	
	private:
		/** computes the min-max */
		inline void computeMinMax() {
			_max = 0;
			_min = 10000000000000.0f;
			for(std::vector<float>::iterator i = _values.begin(); i!=_values.end(); ++i) {
				float f = (*i);
				_max = osg::maximum(f, _max);
				_min = osg::minimum(f, _min);
			}
		}
		
		unsigned int _numValues, _currentNdx;
		std::string _caption;
		float _min, _max;
		std::vector<float> _values, _tendencies;
		float								_x_stepsize;
		float								_yscale;
		osg::Vec4							_color;
	protected:
		osg::ref_ptr<GroupedCaptionLocations>	_locations;
};

}			
		

#endif
