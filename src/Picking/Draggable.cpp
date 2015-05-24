/*
 *  Draggable.cpp
 *  cefix
 *
 *  Created by Stephan Huber on 20.04.07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include <cefix/Draggable.h>
#include <cefix/WindowPickable.h>

namespace cefix {

/** animation based template class, which will call the drag-processing on a regular basis */
class AnimateDraggable : public cefix::AnimationBase {

	public:
		/** ctor */
		AnimateDraggable(Draggable* drag) : cefix::AnimationBase(), _drag(drag) {}
		
	protected:	
		virtual void animate(float elapsed) {
			if (!_drag->doDrag())
				setFinishedFlag();
		}
	private:
		Draggable* _drag;

};



DragDebugGeode::DragDebugGeode() : osg::Geode() {
		
	osg::Geometry* geo = new osg::Geometry();
	geo->setSupportsDisplayList(false);
	
	_vertices = new osg::Vec3Array();
	_vertices->resize(12);
	geo->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0,4));
	geo->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 4,4));
	geo->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 8,4));
	
	geo->setVertexArray(_vertices.get());
	
	osg::Vec4Array* colors = new osg::Vec4Array();
	colors->push_back(osg::Vec4(0,1,1,0.5f));
	colors->push_back(osg::Vec4(0,0,1,0.5f));
	colors->push_back(osg::Vec4(1,0,1,0.5f));
	colors->push_back(osg::Vec4(1,1,0.5,0.5f));
	geo->setColorArray(colors);
	geo->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE);
	
	addDrawable(geo);
	geo->getOrCreateStateSet()->setBinNumber(-10);
	//geo->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	geo->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
	//geo->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	geo->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	osg::Depth* depth = new osg::Depth();
	depth->setWriteMask(false);
	geo->getOrCreateStateSet()->setAttribute(depth);
	_geo = geo;
}
/** sets the plane from a point and a normal */
void DragDebugGeode::setPlaneNormal(const osg::Vec3 p, const osg::Vec3& n, float dw) { 
	
	(*_vertices)[0] = p; (*_vertices)[1] = p+n; 

	osg::Matrix m =osg::Matrix::rotate(osg::Vec3(0,-1,0), n) * osg::Matrix::translate(p);
	
	(*_vertices)[4] = osg::Vec3(-dw,0.0,-dw) * m;
	(*_vertices)[5] = osg::Vec3( dw,0.0,-dw) * m;
	(*_vertices)[6] = osg::Vec3( dw,0.0, dw) * m;
	(*_vertices)[7] = osg::Vec3(-dw,0.0, dw) * m;
	_geo->dirtyBound();
};
		
		
/** draws a small quad around the intersecion */
void DragDebugGeode::setPlaneIntersection(const osg::Vec3& p, const osg::Vec3& normal) {
	osg::Matrix m =osg::Matrix::rotate(osg::Vec3(0,-1,0), normal) * osg::Matrix::translate(p);
	const float dw = 1.0f;
	(*_vertices)[8] = osg::Vec3(-dw,0.0,-dw) * m;
	(*_vertices)[9] = osg::Vec3( dw,0.0,-dw) * m;
	(*_vertices)[10] = osg::Vec3( dw,0.0, dw) * m;
	(*_vertices)[11] = osg::Vec3(-dw,0.0, dw) * m;
	_geo->dirtyBound();
}



Draggable::Draggable(DragMode mode) : 
	cefix::Pickable(),
	_dragMode(mode),
	_planeNormal(osg::Vec3(0,0,-1)),
	_dragInProgress(false),
	_debuggeode(NULL),
	_intersectionOnDown(),
	_continuosComputationOfDragPlane(false)
{
}
		
Draggable::Draggable(DragMode mode, osg::Vec3 planeNormal) : 
	cefix::Pickable(),
	_dragMode(mode),
	_planeNormal(planeNormal),
	_dragInProgress(false),
	_debuggeode(NULL),
	_intersectionOnDown(),
	_continuosComputationOfDragPlane(false)
{
}


void Draggable::computeInverseAndDragPlane() 
{
	_invMatrix = cefix::NodePathUtils::getWindowToLocalTransform(_intersectionOnDown.nodePath);
	
	switch (_dragMode) {
		case DRAG_ON_SCREEN_PLANE:
			_planeNormal = osg::Matrix::transform3x3(osg::Vec3(0, 0, -1), _invMatrix);
			break;
		case DRAG_ON_MOUSE_PLANE:
			_planeNormal = getWindowPickable()->getLocalRay(_intersectionOnDown.nodePath, _pointerId);
			break;
		case DRAG_ON_SURFACE_PLANE:
			_planeNormal = _intersectionOnDown.localIntersectionNormal;
			break;
		case DRAG_ON_CUSTOM_WORLD_PLANE:
			_planeNormal = osg::Matrix::transform3x3(_planeNormal, _invMatrix);
			break;
		case DRAG_ON_CUSTOM_LOCAL_PLANE:
			break;
	}
	
	_planeNormal.normalize();
	_dragPlane = osg::Plane(_planeNormal, _clickLoc);
	
	if (_debuggeode.valid()) {
		_debuggeode->setPlaneNormal(_clickLoc, _planeNormal, dynamic_cast<osg::Geode*>(_intersectionOnDown.nodePath.back())->getBound().radius() * 2);
		_debuggeode->setNodeMask(0xffff);
	}
}

void Draggable::sequenceBegan()
{
    _pointerId = getLastPointerId();
    Pickable::sequenceBegan();
}


void Draggable::down()
{ 
	_intersectionOnDown = getCurrentHit();
	_clickLoc =  _intersectionOnDown.localIntersectionPoint;
	
	computeInverseAndDragPlane();
	_dragInProgress = dragStarted();
	
	/*
    if (_dragInProgress)
		cefix::AnimationFactory::instance()->addPrioritized(new AnimateDraggable(this));
	*/
    if (_dragInProgress)
        stopPropagation();
    else
        allowPropagation();
}


void Draggable::up(bool inside) {
	bool temp = _dragInProgress;
	_dragInProgress = false;
	if (temp)
		dragStopped();
	if (_debuggeode.valid()) {
		_debuggeode->setNodeMask(0x0);
	}
}

void Draggable::handle()
{
    doDrag();
}

bool Draggable::doDrag() 
{
	if (!_dragInProgress)
		return false;
		
	// FUCK OFF: this should work 
	if (_continuosComputationOfDragPlane)
		computeInverseAndDragPlane();
				
	// maus-koordinaten in local-space umrechnen
	float tx =  getWindowPickable()->getNativeX(_pointerId);
	float ty =  getWindowPickable()->getNativeY(_pointerId);
	osg::Vec3 mouseloc =  osg::Vec3(tx, ty, 0) * _invMatrix;
	osg::Vec3 pointOnDragPlane;
	osg::Vec3 normalizedMouseRay = (osg::Vec3(tx, ty, 1) * _invMatrix) - mouseloc;
			
	// Schnittpunkt mit der drag-ebene finden
	double dist = _dragPlane.distance(mouseloc);
	double numerator = -dist;
	double denominator = _dragPlane.getNormal() * normalizedMouseRay;
	
	if (0.0 == denominator) { //mausloc liegt auf der plane
		return true;
	}
	else 
	{
		pointOnDragPlane = mouseloc + (normalizedMouseRay * numerator/denominator);
	}
	
	_dragInProgress = drag(pointOnDragPlane - _clickLoc);
	
	if (_debuggeode.valid()) 
	{
		_debuggeode->setIntersection(_clickLoc, pointOnDragPlane);
		_debuggeode->setPlaneIntersection(pointOnDragPlane, _planeNormal);
	}
	
	return _dragInProgress;
}

}