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

#ifndef SPLINE_GEODE_HEADER
#define SPLINE_GEODE_HEADER


#include <cefix/SplineGeometry.h>
#include <cefix/Quad2DGeometry.h>
#include <cefix/Functor.h>

#include <osg/Geode>
#include <osg/MatrixTransform>

namespace cefix {

class Serializer;

class DisplayMask;

/** Editable Spline is inherited by osg::Group and provides a spine geometry with handlers, pickables etc, so a 
 *  user can edit the form of the spline
 */
class EditableSpline : public osg::Group {

public:
	typedef cefix::FunctorAbstractT<void> ClearSelectionCallback;
	
	EditableSpline(bool editable = true);
	
	cefix::SplineGeometry* getSplineGeo() { return _splineContourGeo.get(); }
	
	void move(const osg::Vec3& delta);
	void update();
	
	void setSelected(bool f) { 
		if (f != _selected) { 
			if (f && _clearSelectionCallback.valid())
				_clearSelectionCallback->call();
			_selected = f; 
			update(); 
			_selectedControlPoint = -1; 
		} 
	}
	
	void writeTo(cefix::Serializer& serializer);
	void readFrom(cefix::Serializer& serializer);
	
	void setEditable(bool e) {
		if (!e) {
			//_splineSolidGeo->setUserData(NULL);
			setSelected(false);
			_splineContourGeode->setNodeMask(0x0);
		}
		_editable = e;
	}
	
	void setControlPoint(unsigned int ndx, const osg::Vec3& p) 
	{
		_splineContourGeo->getControlPoints()[ndx] = p;
		_splineContourGeo->markDirty();
	}
	
	const osg::Vec3& getControlPoint(unsigned int ndx) {
		return _splineContourGeo->getControlPoints()[ndx];
	}
	
	void recreateControls();
	
	void setSelectedControlPoint(int ndx) { _selectedControlPoint = ndx; }
	
	void removeSelectedControlPoint();
	
	bool isSelected() const { return _selected; }
	
	void hilightNearestPoint(const osg::Vec3& p, bool createNewControlPoint = false);
	
	void setClearSelectionCallback(ClearSelectionCallback* cb) { _clearSelectionCallback = cb; }

protected:
	osg::ref_ptr<cefix::SplineGeometry> _splineContourGeo;
	osg::ref_ptr<osg::Geometry>			_splineSolidGeo;
	osg::ref_ptr<osg::Geode>			_splineContourGeode, _splineSolidGeode;
	osg::ref_ptr<osg::Group>			_controls;
	osg::ref_ptr<osg::Vec4Array>		_contourColor, _solidColor;
	
	std::vector<osg::ref_ptr<cefix::Quad2DGeometry> > _controlPointGeos;
	
	osg::ref_ptr<cefix::Quad2DGeometry>	_nearestPointIndicatorGeo;
	
	bool								_editable;
	bool								_selected;
	int									_selectedControlPoint;
	
	osg::ref_ptr< ClearSelectionCallback > _clearSelectionCallback;

	
    static void writeToSerializer(cefix::Serializer& serializer, cefix::Spline3D& spline);
    static void readFromSerializer(cefix::Serializer& serializer, cefix::Spline3D& spline);
};

}
#endif