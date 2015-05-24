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

#ifndef STATISTICS_GROUP_HEADER_
#define STATISTICS_GROUP_HEADER_

#include <osg/Group>
#include <osg/MatrixTransform>
#include <cefix/AbstractStatistics.h>
#include <cefix/Export.h>

namespace cefix {

/** this class groups a bunch of AbstractStatistics and provide a convenient interface to update the values of the statistics, this class scales the graphs to fit in a specific range */
class CEFIX_EXPORT StatisticsGroup : public osg::Group {

	public:
		/** different placement-methods, (not really worked out yet) */
		enum PlacementMode
			{
				OVERLAY,
				STACK,
				PILE, 
				TILE
			};
		
		typedef std::map<std::string, osg::ref_ptr<AbstractStatistics> >	StatsMap;
		
		/** ctor
			@param placement the placement-method, currently OVERLAY and TILE do make sense
			@param dx, dy, dz deltas for the various placement-methods */
		StatisticsGroup(PlacementMode placement = OVERLAY, float dx = 0, float dy = 200, float dz = 10) :  
			osg::Group(), 
			_placement(placement), 
			_dx(dx), 
			_dy(dy), 
			_dz(dz), 
			_lastYScale(0),
			_locations(new AbstractStatistics::GroupedCaptionLocations()) 
		{ 
			setMaxDimensions(500,250); 
		}
		
		/** adds a value to the statistic with a specified key */
		void addValue(const std::string& key, float value, float tendency) {
			if (_stats.find(key) != _stats.end()) {
				_stats[key]->addValue(value, tendency);	
			}
		}

		/** adds a value to the statistic with a specified key */
		void mergeValue(const std::string& key, float value, float tendency) {
			if (_stats.find(key) != _stats.end()) {
				_stats[key]->mergeValue(value, tendency);
			}
		}
		
		/** sets the maximum dimensions for this group */
		void setMaxDimensions(float x, float y) { _maxx = x; _maxy = y; computePlacement(); }
		
		/** adds a statistic-object to this group */
		void addStatistic(const std::string& key, AbstractStatistics* stat) {
			if (_stats.size() > 0) {
				AbstractStatistics* ref_stat = _stats.begin()->second.get();
				stat->setCurrentIndex(ref_stat->getCurrentIndex());
			}
			osg::MatrixTransform* mat = new osg::MatrixTransform();
			mat->addChild(stat);
			addChild(mat);
			_stats[key] = stat;
			computeColors();
			computePlacement();
			stat->setGroupedCaptionLocations(_locations.get());

		}
		
		/** @return true if this group has already a statistics-object with the specified key */
		bool hasStatistic(const std::string& key) {
			return (_stats.find(key) != _stats.end());
		}
		
		/** adjusts the y scale, so the graphs fit in the max. area */
		void adjustYScale();
		
		void clear() {
			_locations->clear();
		}
		
		void adjustLocations() {
			adjustYScale();
			_locations->adjustLocations();
		}
		
	protected:
		/** dtor */
		~StatisticsGroup() {
			;
		}
		
	private:
		/** compute the colors for the child-graphs, does this by stepping throught the HSV-colormodel */
		void computeColors();
		/** compute the placement of the graphs */
		void computePlacement();
	
		StatsMap		_stats;
		PlacementMode	_placement;
		float			_maxx, _maxy;
		float			_dx,_dy,_dz;
		float			_lastYScale;
		osg::ref_ptr<AbstractStatistics::GroupedCaptionLocations> _locations;
	

};

}
#endif