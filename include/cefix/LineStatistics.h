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
#ifndef LINE_STATISTICS_HEADER_
#define LINE_STATISTICS_HEADER_

#include <cefix/AbstractStatistics.h>
#include <cefix/TextGeode.h>
#include <cefix/Export.h>

namespace cefix {

/** LineStatistics provides a line graph showing the capturec values, nothing fancy */
class CEFIX_EXPORT LineStatistics: public AbstractStatistics {

	public:
		/** ctor 
		    @param caption caption
		    @param numValues amount of values to store 
		    @param color color for this graph
		*/
		LineStatistics(const std::string& caption, unsigned int numValues = 1000, const osg::Vec4& color = osg::Vec4(1,1,1,0.7)) : AbstractStatistics(caption, numValues)
		{
			setColor(color);
			createGeometry();
		}
		
	protected:
		/** updates the geometry for a specific value*/
		virtual void updateGeometry();
		
		virtual void updateCaption();
		
		virtual ~LineStatistics() {}
		
	
	private:
	
		/** creates the geometry */
		void createGeometry();
		
		/** compute the colors */
		void computeColors();
	

		osg::ref_ptr< osg::Vec3Array >		_vertices;
		osg::ref_ptr< osg::Vec4Array >		_colors;
		osg::ref_ptr< Utf8TextGeode >		_textgeode;
		osg::ref_ptr<osg::Geometry>			_geometry;
		

};
}


#endif