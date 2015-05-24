/*
 *  EditableSpline.cpp
 *  MaskEditor
 *
 *  Created by Fakultät für Design Administrator on 10.08.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "EditableSpline.h"

#include <cefix/Serializer.h>
#include <osg/KdTree>
#include <osgUtil/Tessellator>
#include <cefix/Draggable.h>
#include <cefix/Notify.h>
#include <cefix/DebugGeometryFactory.h>

namespace cefix {

class SplineControlPointDraggable : public cefix::Draggable, public osg::Referenced {
public:

SplineControlPointDraggable(EditableSpline* spline, unsigned int ndx, cefix::Quad2DGeometry* middle)
:	cefix::Draggable(),
	osg::Referenced(),
	_spline(spline),
	_ndx(ndx),
	_middle(middle),
	_left(NULL),
	_right(NULL),
	_master(NULL),
	_p(),
	_syncEditing(false)
{
}

void setLeft(SplineControlPointDraggable* left) { _left = left; }
void setRight(SplineControlPointDraggable* right) { _right = right; }
void setMaster(SplineControlPointDraggable* master) { _master = master; }

virtual void mouseDown() {
	_syncEditing = false;
	if (getCurrentPointerData().event->getModKeyMask() &  osgGA::GUIEventAdapter::MODKEY_ALT)
		_syncEditing = true;
	cefix::Draggable::down();
}

virtual bool dragStarted() 
{
	if (_left) _left->dragStarted();
	if (_right) _right->dragStarted();
	_rect = _middle->getRect();
	_p = _spline->getControlPoint(_ndx);
	return true;
}

virtual bool drag(const osg::Vec3& delta) {
	osg::Vec4 r(_rect);
	r[0] += delta[0];
	r[1] += delta[1];
	r[2] += delta[0];
	r[3] += delta[1];
	
	_middle->setRect(r);
	_spline->setControlPoint(_ndx, _p + delta);
	
	if ((_master) && (_syncEditing)) {
		osg::Vec3 d(_spline->getControlPoint(_ndx) - _spline->getControlPoint(_master->_ndx));
		osg::Quat q(osg::PI, osg::Z_AXIS);
		d = q * d;
		
		if (this == _master->_left)
			_spline->setControlPoint(_master->_right->_ndx, _spline->getControlPoint(_master->_ndx) + d);
		else 
			_spline->setControlPoint(_master->_left->_ndx, _spline->getControlPoint(_master->_ndx) + d);
			
	} else {
		if (_left) {
			_left->drag(delta);
		}
		
		if (_right) {
			_right->drag(delta);
		}
	}
	
	if (_left || _right) {
		_spline->setSelectedControlPoint(_ndx);
	} else {
		_spline->setSelectedControlPoint(_master->_ndx);
	}
	
	_spline->update();
	return true;

}

virtual void dragStopped() {
	_spline->update();
}

private:
	EditableSpline*					_spline;
	unsigned int					_ndx;
	cefix::Quad2DGeometry*			_middle;
	SplineControlPointDraggable*	_left;
	SplineControlPointDraggable*	_right;
	SplineControlPointDraggable*	_master;
	osg::Vec4						_rect;
	osg::Vec3						_p;
	bool							_syncEditing;

};


class SplineDraggable : public cefix::Draggable, public osg::Referenced {
public:
SplineDraggable(EditableSpline* spline) 
:	cefix::Draggable(),
	osg::Referenced(),
	_spline(spline),
	_isDragging(false)
{
}

virtual ~SplineDraggable() {
	std::cout << "SplineDraggable :: released " << std::endl;
}

virtual void down() {
	
	// TODO _spline->getMask()->resetSelection();
	_spline->setSelected(true);
	
	cefix::Draggable::down();
}

virtual bool dragStarted() 
{
	
	if (getNthPointerData(0).event->getModKeyMask() & 4) {
		_spline->hilightNearestPoint(getCurrentHit().getLocalIntersectPoint(), true);
		return false; // kein drag mehr, bitte
	} 
	
	_dragmat = new osg::MatrixTransform();
	osg::ref_ptr<osg::Node> nodeToDrag = _spline;
	osg::ref_ptr<osg::Group> parent = nodeToDrag->getParent(0);
	parent->removeChild(nodeToDrag.get());
	_dragmat->addChild(nodeToDrag);
	parent->addChild(_dragmat.get());
	_isDragging = true;
	return true;
	
}

virtual void within()
{
	_spline->hilightNearestPoint(getCurrentHit().getLocalIntersectPoint(), false);
	Draggable::within();
}

virtual bool drag(const osg::Vec3& delta) 
{
	_delta = delta;
	_dragmat->setMatrix(osg::Matrix::translate(delta));
	
	return true;
}

virtual void dragStopped()
{
	_isDragging = false;
	osg::ref_ptr<osg::Node> nodeToDrag = _spline;
	osg::ref_ptr<osg::Group> parent = _dragmat->getParent(0);
	parent->removeChild(_dragmat.get());
	parent->addChild(nodeToDrag);
	_spline->move(_delta);
	std::cout << "moved " << _delta <<std::endl;
}

private:
	EditableSpline* _spline;
	osg::ref_ptr<osg::MatrixTransform> _dragmat;
	osg::Vec3 _delta;
	bool _isDragging;

};

EditableSpline::EditableSpline(bool editable)
:	osg::Group(),
	_splineContourGeo(new cefix::SplineGeometry()),
	_splineSolidGeo(new osg::Geometry()),
	_selected(false)
{
	_contourColor = new osg::Vec4Array();
	_contourColor->push_back(osg::Vec4(1,1,1,1));
	
	_solidColor = new osg::Vec4Array();
	_solidColor->push_back(osg::Vec4(1,1,1,1));
	
	_splineContourGeo->setColorArray(_contourColor.get());
	_splineContourGeo->setColorBinding(osg::Geometry::BIND_OVERALL);
	_splineContourGeo->setRenderMode(cefix::SplineGeometry::Approximated);
	
	_splineSolidGeo->setColorArray(_solidColor.get());
	_splineSolidGeo->setColorBinding(osg::Geometry::BIND_OVERALL);
	
	_splineContourGeode = new osg::Geode();
	_splineContourGeode->addDrawable(_splineContourGeo.get());
	
	_splineSolidGeode = new osg::Geode();
	_splineSolidGeode->addDrawable(_splineSolidGeo.get());
	
	//if (_editable) 
		_splineSolidGeode->setUserData(new SplineDraggable(this));
	
	addChild(_splineSolidGeode.get());
	addChild(_splineContourGeode.get());
	
	_controls = new osg::Group();
	addChild(_controls.get() );
	
	_nearestPointIndicatorGeo = new cefix::Quad2DGeometry(0,0,6,6);
	_nearestPointIndicatorGeo->setColor(osg::Vec4(1,1,1,0.3));
	
}

void EditableSpline::removeSelectedControlPoint() 
{
	
	if (_selectedControlPoint < 0)
		return;
	int start_ndx = (_selectedControlPoint>0) ? _selectedControlPoint -1 : _selectedControlPoint;
	int end_ndx = (_selectedControlPoint+1 < static_cast<int>(_controlPointGeos.size())) ? _selectedControlPoint+1 : _selectedControlPoint; 
	
	for( int i= end_ndx; i >= start_ndx; --i) {
		_splineContourGeo->getControlPoints().erase(_splineContourGeo->getControlPoints().begin()+i);
	}
	recreateControls();
}


void EditableSpline::update() 
{
	if (_selected) {
		(*_solidColor)[0] = osg::Vec4(1,1,1,0.5);
		(*_contourColor)[0] = osg::Vec4(0,0.75,1,1);
		_controls->setNodeMask(0xFFFF);
		_splineContourGeode->setNodeMask(0xFFFF);
	}
	else {
		(*_solidColor)[0] = osg::Vec4(1,1,1,1);
		(*_contourColor)[0] = osg::Vec4(1,1,1,1);
		_controls->setNodeMask(0x0);
		_splineContourGeode->setNodeMask(0x0);
	}
	_splineContourGeo->markDirty();
	_splineContourGeo->update();
	
	// tesselate
	osg::ref_ptr<osg::Vec3Array> vertices = osg::clone(dynamic_cast<osg::Vec3Array*>(_splineContourGeo->getVertexArray()));
	
	for(unsigned int i = 0; i < vertices->size(); ++i) {
		(*vertices)[i]+= osg::Vec3(0,0,10);
	}
	
	_splineSolidGeo->setVertexArray(vertices.get());
	_splineSolidGeo->removePrimitiveSet(0, _splineSolidGeo->getNumPrimitiveSets());
	_splineSolidGeo->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POLYGON, 0, vertices->size()));
	
	osg::ref_ptr<osgUtil::Tessellator> tess = new osgUtil::Tessellator();
	tess->setTessellationNormal(osg::Vec3(0,0,-1));
	tess->setTessellationType(osgUtil::Tessellator::TESS_TYPE_GEOMETRY);
	tess->retessellatePolygons(*_splineSolidGeo.get());
	
	_splineSolidGeo->dirtyBound();
	_splineSolidGeo->computeCorrectBindingsAndArraySizes();
	
	
	// Control-Elemente bauen
	if (1) 
	{
		cefix::Spline3D::ControlPoints& controlpoints = _splineContourGeo->getControlPoints();
		
		for(int i = 0; i < static_cast<int>(_controlPointGeos.size()); ++i) 
		{
			
			osg::Vec3 p(controlpoints[i][0], controlpoints[i][1], 0);
			
			cefix::Quad2DGeometry* quad = _controlPointGeos[i];
			if (i%3 == 0) {
				quad->setRect(p[0] - 4, p[1] - 4, 8, 8, 0);
				if (i == _selectedControlPoint)
					quad->setColor(osg::Vec4(1, 0, 1, 0.7));
				else
					quad->setColor(osg::Vec4(0.7, 0.7, 0.7, 0.7));
			}
			
			else {
				quad->setRect(p[0] - 3, p[1] - 3, 6, 6, 0);
				quad->setColor(osg::Vec4(0,0.75,1,0.75));
				
			}
			quad->setLocZ(100);
		}		
	}
}

void EditableSpline::recreateControls()
{
	update();
	
	
	cefix::notify("resetPicking");
	_controls->removeChildren(0, _controls->getNumChildren());
	_controlPointGeos.clear();
		
	{
		osg::Geode* geode = new osg::Geode();
		geode->addDrawable(_nearestPointIndicatorGeo.get());
		_controls->addChild(geode);
	}
	
	cefix::Spline3D::ControlPoints& controlpoints = _splineContourGeo->getControlPoints();
	std::vector<SplineControlPointDraggable*> draggables(controlpoints.size());
	
	for(unsigned int i = 0; i < controlpoints.size(); ++i) 
	{
		
		osg::Vec3 p(controlpoints[i][0], controlpoints[i][1], 0);
		
		osg::Geode* geode = new osg::Geode();
		cefix::Quad2DGeometry* quad(NULL);
		if (i%3 == 0) {
			quad = new cefix::Quad2DGeometry(p[0] - 4, p[1] - 4, 8, 8, 0);
			quad->setColor(osg::Vec4(0.7, 0.7, 0.7, 0.7));
			
			if (i>0) {
				cefix::DebugGeometryFactory::get("spline_handles")->addLine(controlpoints[i], controlpoints[i-1], osg::Vec4(0,0.75, 1, 0.5));
			}
			if (i < controlpoints.size()-1) {
				cefix::DebugGeometryFactory::get("spline_handles")->addLine(controlpoints[i], controlpoints[i+1], osg::Vec4(0,0.75, 1, 0.5));
			}
		}
		else {
			quad = new cefix::Quad2DGeometry(p[0] - 3, p[1] - 3, 6, 6, 0);
			quad->setColor(osg::Vec4(0,0.75,1,0.75));
			
		}
		quad->setLocZ(100);
		_controlPointGeos.push_back(quad);
		
		geode->addDrawable(quad);
		draggables[i] = new SplineControlPointDraggable(this, i, quad);
		geode->setUserData(draggables[i]);
		_controls->addChild(geode);
	}
	int num_controlpoints = static_cast<int>(controlpoints.size());
	// im 2.ten schritt die nachbarn der draggables setzen
	for(int i = 0; i < static_cast<int>(controlpoints.size()); ++i) 
	{
		if (i%3==0) {
			int left = (i-1);
			if (left < 0) left += num_controlpoints;
			
			int right = (i+1);
			if (right >= num_controlpoints) right -= num_controlpoints;
			
			draggables[i]->setLeft(draggables[left]);
			draggables[i]->setRight(draggables[right]);
			
			draggables[left]->setMaster(draggables[i]);
			draggables[right]->setMaster(draggables[i]);
			
		}
	}
}
	
	
void EditableSpline::hilightNearestPoint(const osg::Vec3& p, bool createNewControlPoint) 
{
	cefix::Spline3D& spline = _splineContourGeo->getSpline();
	
	int startDelta = spline.getStartDelta();
	int endDelta   = spline.getEndDelta();
	
	unsigned int max= spline.controlPoints.size() - endDelta;		
	unsigned int size= spline.controlPoints.size();
	
	int found_control_point(-1);
	double min_distance(1000000);
	osg::Vec3 found_p;
	double found_t;
	
	osg::Vec3 p_on_spline, last_p_on_spline;
	for (unsigned int i= startDelta; i < max; i+= spline.getStep()) {
					
		double stepsize = 1/50.0;
		
		for (double t = 0; t < 1; t+=stepsize)
		{
			p_on_spline = spline.getPointOnSpline(
				spline.controlPoints[i%size], 
				spline.controlPoints[(i+1)%size], 
				spline.controlPoints[(i+2)%size], 
				spline.controlPoints[(i+3)%size], t);
			
			double len = (p_on_spline - p).length2();
			if(len < min_distance) {
				found_control_point = i;
				found_p = p_on_spline;
				found_t = t;
				min_distance = len;
			}			
		}
	}
	
	if (found_control_point < 0)
		return;
		
	
	_nearestPointIndicatorGeo->setRect(found_p[0]-3, found_p[1]-3, 6,6);
	_nearestPointIndicatorGeo->setLocZ(200);
	
	/*
	USE_DEBUG_GEO(
		{
			cefix::DebugGeometryFactory::get("spline")->addText(spline.controlPoints[found_control_point]+osg::Vec3(10,10,10), cefix::intToString(found_control_point)+" / "+ cefix::floatToString(found_t));
			cefix::DebugGeometryFactory::get("spline")->addPoint(spline.controlPoints[found_control_point], osg::Vec4(0,0,1,1));
		}
	)*/
	
	if(createNewControlPoint) 
	{
		osg::Vec3 p0, p1, p2, p3;
		cefix::Spline3D::ControlPoints::iterator itr;
		p0 = spline.controlPoints[found_control_point];
		p1 = spline.controlPoints[(found_control_point+1)%size];
		p2 = spline.controlPoints[(found_control_point+2)%size];
		p3 = spline.controlPoints[(found_control_point+3)%size];
		
		osg::Vec3 pnew0, pnew1, pnew2;
		
		pnew1 = found_p;
		pnew0 = found_p + (found_p - p0 ) * 0.4;
		pnew2 = found_p + (found_p - p2 ) * 0.4;
		
		std::cout << "inserting new points at: " << found_control_point+2 << " size: " << spline.controlPoints.size() << std::endl;
		
		itr = spline.controlPoints.begin() + found_control_point+2;
		if (itr == spline.controlPoints.end()) {
			spline.controlPoints.push_back(pnew0);
			spline.controlPoints.push_back(pnew1);
			spline.controlPoints.push_back(pnew2);
		} else {
			spline.controlPoints.insert(spline.controlPoints.begin() + found_control_point+2, pnew0);
			spline.controlPoints.insert(spline.controlPoints.begin() + found_control_point+2, pnew1);
			spline.controlPoints.insert(spline.controlPoints.begin() + found_control_point+2, pnew2);
			
		}
		recreateControls();
		
	}
	
}

void EditableSpline::writeTo(cefix::Serializer& serializer) 
{
	serializer << "spline" << _splineContourGeo->getSpline();
}


void EditableSpline::readFrom(cefix::Serializer& serializer)
{
	serializer >> "spline" >> _splineContourGeo->getSpline();
	update();
	recreateControls();
}

void EditableSpline::move(const osg::Vec3& delta) 
{
	for(unsigned int i=0; i < _splineContourGeo->getControlPoints().size(); ++i) {
		_splineContourGeo->getControlPoints()[i] += delta;
	}
	_splineContourGeo->markDirty();
	update();
}


void writeToSerializer(cefix::Serializer& serializer, cefix::Spline3D& spline) 
{
	serializer << "mode" << static_cast<int>(spline.getMode());
	serializer << "isClosed" << spline.getSplineClosed();
	serializer << "maxError" << spline.getMaxError();
	serializer << "controlpoints" << spline.controlPoints;
}


void readFromSerializer(cefix::Serializer& serializer, cefix::Spline3D& spline) 
{
	int mode;
	bool isClosed;
	double maxError;
	
	spline.controlPoints.clear();
	serializer >> "mode" >> mode;
	serializer >> "isClosed" >> isClosed;
	serializer >> "maxError" >> maxError;
	serializer >> "controlpoints" >> spline.controlPoints;
	spline.setMode(static_cast<cefix::SplineMode>(mode));
	spline.setMaxError(maxError);
	spline.setSplineClosed(isClosed);
	
}

}
