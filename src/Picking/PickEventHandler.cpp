//
//  PickEventHandler.cpp
//  cefixSketch
//
//  Created by Stephan Huber on 30.08.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "PickEventHandler.h"
#include <osg/Billboard>
#include <osg/PagedLOD>
#include <iostream>

namespace cefix {

class LodIntersectionVisitor : public osgUtil::IntersectionVisitor
{
public:
    LodIntersectionVisitor( const osg::Matrixd& view, float lod_scale =
1.0, osgUtil::Intersector* intersector=0, ReadCallback* readCallback=0 )
      : osgUtil::IntersectionVisitor( intersector, readCallback ),
lod_scale(lod_scale)
    {
        pushViewMatrix(new osg::RefMatrix(view));
        eye = osg::Matrix::inverse(view).getTrans();
    }
 
    osg::Vec3 getEyePoint() const;
    float getDistanceToEyePoint( const osg::Vec3&, bool ) const;
    float getDistanceFromEyePoint( const osg::Vec3&, bool ) const;
 
    void apply(osg::PagedLOD& plod);
            
    // Makes use of eyeposition information to produce a model matrix and
    // transform the intersector's space into the billboard's space.
    void apply(osg::Billboard& billboard);
 
private:
    osg::Vec3 eye;
    float lod_scale;
 
};
 
osg::Vec3 LodIntersectionVisitor::getEyePoint() const
{
    // This code needs verification.
 
    osg::Vec3 eye_point = eye;
    
    if( getModelMatrix() ) eye_point = eye_point * osg::Matrix::inverse(
*getModelMatrix() );
 
    return eye_point;
}
 
float LodIntersectionVisitor::getDistanceToEyePoint(const osg::Vec3&
pos, bool with_scale) const
{
    if (with_scale) 
        return (pos-getEyePoint()).length()*lod_scale;
    else 
        return (pos-getEyePoint()).length();
}
 
float LodIntersectionVisitor::getDistanceFromEyePoint(const osg::Vec3&
pos, bool with_scale) const
{
    // Are getDistanceTo and getDistanceFrom really equivalent?
    return getDistanceToEyePoint(pos, with_scale);
}
 
void LodIntersectionVisitor::apply(osg::PagedLOD& plod)
{
    // Treat PagedLODs as regular LODs, as the base apply() method for
    // PagedLODs only acts upon the highest level of detail, regardless of
    // what is actually visible.
    osg::LOD& lod = plod;
    osgUtil::IntersectionVisitor::apply(lod);
}
 
void LodIntersectionVisitor::apply(osg::Billboard& billboard)
{
    if (!enter(billboard)) return;
 
    osg::Vec3 eye_local = getEyePoint();
 
    osg::Matrix model_view = (getModelMatrix()) ? *getModelMatrix() : osg::Matrix();
 
    // ROT_TO_EYE mode expects a Model view matrix with something like M = M1*M2*...*MN * View, so tack the view on
    // before making the billboard compute its transformation.  This properly puts intersection data in the
    // billboard's reference frame.
    // getViewMatrix() is invariantly defined based on the rules of the constructor.
    model_view.postMult( *getViewMatrix() );
 
    for(unsigned int i=0; i<billboard.getNumDrawables(); ++i)
    {
        osg::Matrix bb_matrix(model_view);
 
        const osg::Vec3& pos = billboard.getPosition(i);
        billboard.computeMatrix( bb_matrix, eye_local, pos );
 
        // Remove the view matrix from the model data.
        bb_matrix.postMult( osg::Matrix::inverse( *getViewMatrix() ));
        
        // osgUtil::Intersector* intersector = getIntersector();
 
        
        // Have to push the model matrix to ensure that the intersector clone is properly placed into the
        // Billboard's coordinate frame.
        pushModelMatrix( new osg::RefMatrix(bb_matrix) );
 
        // now push an new intersector clone transform to the new local coordinates
        push_clone();
        
        intersect( billboard.getDrawable(i) );
 
        // pop the clone.
        pop_clone();
          
        popModelMatrix();
      }

    leave();
}

    
#pragma mark -


PickEventHandler::PickEventHandler(osg::Camera* camera)
:   osgGA::GUIEventHandler(),
    _registeredPickables(),
    _handleMultiTouch(false),
    _handleOverState(true),
    _enabled(true),
    _hierarchicalPickingEnabled(false),
    _traversalMask(0xffff),
    _camera(camera)
{
    #ifdef CEFIX_FOR_IPHONE
        setMultiTouchEnabled(true);
        setHandleOverState(false);
    #endif
    
    _windowPickable = new WindowPickable();
    _windowPickable->setPickEventHandler(this);
}


bool PickEventHandler::handle (const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa, osg::Object *, osg::NodeVisitor *)
{
    if (!isEnabled())
        return false;
        
    bool handled = false;
    
    switch(ea.getEventType())
	{
		case(osgGA::GUIEventAdapter::FRAME):
            {
                std::vector<Pickable::PointerData> pointers;
                for(IdStackMap::iterator i = _stacks.begin(); i != _stacks.end(); ++i) {
                    PickStack& s = i->second;
                    s.pt.phase = (s.down) ? Pickable::PointerData::DRAG : Pickable::PointerData::MOVE;
                    pointers.push_back(s.pt);
                }
                clearPicks();
                for(std::vector<Pickable::PointerData>::iterator i = pointers.begin(); i !=pointers.end(); ++i) {
                    intersect(ea, *i);
                }
                fireEvents();
                
                return false;
            }
            
            break;
            
        case(osgGA::GUIEventAdapter::MOVE):
        case(osgGA::GUIEventAdapter::DRAG):
		case(osgGA::GUIEventAdapter::PUSH): 
		case(osgGA::GUIEventAdapter::DOUBLECLICK): 
		case(osgGA::GUIEventAdapter::RELEASE):
			{
				clearPicks();
                
                if (ea.isMultiTouchEvent() && isMultiTouchEnabled()) {
                    osgGA::GUIEventAdapter::TouchData* touch_data = ea.getTouchData();
                    for(osgGA::GUIEventAdapter::TouchData::iterator i = touch_data->begin(); i != touch_data->end(); ++i ) {
                        Pickable::PointerData pt(ea, *i);
                        intersect(ea, pt);
                    }
                    
                }
                else
                {
                    Pickable::PointerData pt(ea);
                    intersect(ea, pt);
                }
                
                handled = fireEvents();
			}
			break;
			
		default:
			return false;
	}

    return handled;
}




bool PickEventHandler::isCameraChild(const osg::NodePath& path) {
	if (path.size() <= 1)
		return false;
		
	for(osg::NodePath::const_iterator itr = path.begin() + 1; itr != path.end(); ++itr) 
	{
		osg::Group* g = (*itr)->asGroup();
        if (g && dynamic_cast<osg::Camera*>(g))
			return true;
	}
	return false;
}


void PickEventHandler::intersect(const osgGA::GUIEventAdapter &ea, Pickable::PointerData& pt)
{
    _stacks[pt.id].pt = pt;
    if (pt.phase == Pickable::PointerData::DOWN)
        _stacks[pt.id].down = true;
    else if (pt.phase == Pickable::PointerData::UP)
        _stacks[pt.id].down = false;
        
    if (ea.getEventType() == osgGA::GUIEventAdapter::FRAME) {
        _stacks[pt.id].pt.phase = (_stacks[pt.id].down) ? Pickable::PointerData::DRAG : Pickable::PointerData::MOVE;
    }
    
    if (!getHandleOverState() && _stacks[pt.id].pt.phase == Pickable::PointerData::MOVE)
        return;
    
	float tx,ty;
	//std::cout << ea.getX() << "/" << ea.getY() << std::endl;
	tx = pt.x;
	ty = pt.y;

	if (osg::isNaN(tx) || osg::isNaN(ty))
		return;
	
	
	osg::Matrix MVPW =  _camera->getViewMatrix() * _camera->getProjectionMatrix();
	
	MVPW.postMult(_camera->getViewport()->computeWindowMatrix());
	osg::Matrixd inverseMVPW;
	inverseMVPW.invert(MVPW);

	pt.nearPoint = osg::Vec3(tx,ty,-1.0f) * inverseMVPW;
	pt.farPoint = osg::Vec3(tx,ty, 1.0f) * inverseMVPW;
    
    // copy pointerdata again
    _stacks[pt.id].pt = pt;
    
    
    if (!_enabled)
		return;
        
    osgUtil::LineSegmentIntersector::CoordinateFrame cf = _camera->getViewport() ? osgUtil::Intersector::WINDOW : osgUtil::Intersector::PROJECTION;
	osgUtil::LineSegmentIntersector* picker = new osgUtil::LineSegmentIntersector(cf, tx, ty);


	//osgUtil::IntersectionVisitor iv(picker);
	LodIntersectionVisitor iv(_camera->getViewMatrix(), 1.0, picker);
	
	iv.setTraversalMask(_traversalMask);
	const_cast<osg::Camera*>(_camera)->accept(iv);

	if (!picker->containsIntersections())
	{
		return;
	}

	osgUtil::LineSegmentIntersector::Intersection resulthit;
	
	for(osgUtil::LineSegmentIntersector::Intersections::iterator hitr=picker->getIntersections().begin();
		hitr!= picker->getIntersections().end();
		++hitr)
	{
		osg::Node* node = (hitr->nodePath.back());
		
		if ((node)) {
			
			Pickable* picked = getPickable(node);
			if (!picked && isHierarchicalPickingEnabled()) {
				for(osg::NodePath::const_reverse_iterator itr = hitr->nodePath.rbegin(); itr != hitr->nodePath.rend(); ++itr) {
					picked = getPickable((*itr));
					// std::cout << "trying parents: " << (*itr)->className() << " " <<  picked << std::endl;
					if (picked) {
						node = *itr;
						break;
					}
				}
			}
			bool valid = picked;
            if (valid) picked->setPickEventHandler(this);
			osgUtil::LineSegmentIntersector::Intersection hit(*hitr);
			if (valid) valid = picked->accept(hit);

			if (valid) {
				if (isCameraChild(hit.nodePath)) {
					hit.ratio -= 10;
				}
				
                if (valid)
                    addPick(pt, hit, picked);
			}
		}
	}
}

void PickEventHandler::addPick(const Pickable::PointerData& pt, const Pickable::Hit& hit, Pickable* picked)
{
    _stacks[pt.id].stack.insert(std::make_pair(hit.ratio, picked));
    _stacks[pt.id].pt = pt;
    
    _currentlyPicked.insert(picked);
    
    picked->addHit(pt, hit);
    if (_windowPickable)
        _windowPickable->addPickable(picked);
}


void PickEventHandler::clearPicks()
{
    _pickedLastFrame.clear();
    std::copy(_currentlyPicked.begin(), _currentlyPicked.end(), std::inserter( _pickedLastFrame, _pickedLastFrame.begin()));
    
    for(PickableSet::iterator i = _currentlyPicked.begin(); i != _currentlyPicked.end(); ++i) {
        (*i)->clearHits();
    }
    _currentlyPicked.clear();
    
    for(IdStackMap::iterator i = _stacks.begin(); i != _stacks.end(); ++i) {
        i->second.stack.clear();
    }
    
    if (_windowPickable) {
        _windowPickable->clearPickables();
        _windowPickable->clearPointers();
    }
}



bool PickEventHandler::fireEvents()
{
    bool result = false;    
    std::vector<unsigned int> finished;
    
    if (_windowPickable)
         _windowPickable->clearPointers();
    
    for(IdStackMap::iterator i = _stacks.begin(); i != _stacks.end(); ++i)
    {
        PickStack& s = i->second;
        
        if (_windowPickable) _windowPickable->addPointer(s.pt);
        
        //std::cout << "fireEvents stack: " << s.pt.id << " phase: " << s.pt.phase << " num down: " << s.currentlyDown.size() << " num over: " << s.currentlyOver.size() << " currently picked: " << s.stack.size() << std::endl;
        

        bool handled = false;
        
        // give all down-pickables the possibility to set the handled-flag, even if the pointer is not above them
        if (s.pt.phase == Pickable::PointerData::DRAG)
        {
            for(PickableSet::iterator j = s.currentlyDown.begin(); j != s.currentlyDown.end(); ++j) {
                if (!handled && (*j)->shouldPropagationBeStopped()) {
                    handled = true;
                 }
                 (*j)->setLastPointerId(i->first);
                 (*j)->within();
                 _currentlyPicked.insert(*j);
            }
        }
        
        PickableSet handled_pickables;
        
        for(PickStack::Stack::iterator j = s.stack.begin(); (!handled) && (j != s.stack.end()); ++j)
        {
            Pickable* pickable = j->second;
            pickable->setLastPointerId(i->first);
            
            switch(s.pt.phase)
            {
                case Pickable::PointerData::DOWN:
                    s.currentlyDown.insert(pickable);
                    pickable->handleSequenceBegan();
                    pickable->down();
                    if (_windowPickable) _windowPickable->down();
                    handled = pickable->shouldPropagationBeStopped();
                    break;
            
                case Pickable::PointerData::UP:
                    {
                        PickableSet::iterator itr = s.currentlyDown.find(pickable);
                        if (itr != s.currentlyDown.end())
                        {
                            s.currentlyDown.erase(pickable);
                            pickable->up(true);
                            handled = pickable->shouldPropagationBeStopped();
                        }
                        if (_windowPickable) _windowPickable->up();
                        finished.push_back(s.pt.id);
                    }
                    break;
                
                case Pickable::PointerData::DRAG:
                case Pickable::PointerData::MOVE:
                    if (getHandleOverState() || (s.pt.phase == Pickable::PointerData::DRAG))
                    {
                        if (s.currentlyOver.find(pickable) == s.currentlyOver.end())
                        {
                            s.currentlyOver.insert(pickable);
                            pickable->enter();
                            handled = pickable->shouldPropagationBeStopped();
                        }
                        else
                        {
                            pickable->within();
                            handled = pickable->shouldPropagationBeStopped();
                        }
                    }
                    break;
                
                default:
                    ;
            }
            
            handled_pickables.insert(pickable);
        }
        
        if (getHandleOverState() && ((s.pt.phase == Pickable::PointerData::MOVE) || (s.pt.phase == Pickable::PointerData::DRAG))) {
            for(PickableSet::iterator j = s.currentlyOver.begin(); j != s.currentlyOver.end(); ) {
                
                bool found = handled_pickables.find(*j) != handled_pickables.end();
                
                if (!found) {
                    (*j)->setLastPointerId(i->first);
                    (*j)->leave();
                    _currentlyPicked.insert(*j);
                    s.currentlyOver.erase(j++);
                } else
                    ++j;
            }
        }

        
        
        // release all pickables which are in the down-state
        if (s.pt.phase == Pickable::PointerData::UP) {
            for(PickableSet::iterator j = s.currentlyDown.begin(); j != s.currentlyDown.end(); ++j) {
                (*j)->setLastPointerId(i->first);
                (*j)->up(false);
                _currentlyPicked.insert(*j);
            }
                
            s.currentlyDown.clear();
            s.ignoreHandledFlag = false;
        }
        else if (s.pt.phase == Pickable::PointerData::DOWN)
        {
            if (s.stack.size() == 0)
                s.ignoreHandledFlag = true;
        }
        
        // if we hava a down state, but no pickables, reset handled-flag
        if (handled && s.ignoreHandledFlag) {
            // std::cout << "resetting handled-flag" << std::endl;
            handled = false;
        }
        
        if (handled)
            result = true;
    }
    
    
    for(PickableSet::iterator i = _currentlyPicked.begin(); i != _currentlyPicked.end(); ++i) {
        (*i)->handleSequenceBegan();
        (*i)->handle();
    }
    
    unsigned int num_ups(0);
    for(IdStackMap::iterator i = _stacks.begin(); i != _stacks.end(); ++i)
    {
        PickStack& s = i->second;
        if (s.pt.phase == Pickable::PointerData::UP)
            num_ups++;
    }
    if ((num_ups > 0) && (num_ups == _stacks.size())) {
        for(PickableSet::iterator i = _currentlyPicked.begin(); i != _currentlyPicked.end(); ++i) {
            (*i)->handleSequenceEnded();
        }
    }
    
    // clear all finished sequences
    for(std::vector<unsigned int>::iterator i = finished.begin(); i != finished.end(); ++i) {
        // std::cout << "deleting sequence " << *i << std::endl;
        IdStackMap::iterator itr = _stacks.find(*i);
        if (itr != _stacks.end()) {
            _stacks.erase(itr);
        }
    }
        
    return result;
}



void PickEventHandler::registerPickable(Pickable* pickable)
{
    PickableSet::iterator itr = _registeredPickables.find(pickable);
    if (itr == _registeredPickables.end()) {
        _registeredPickables.insert(pickable);
    }
}


void PickEventHandler::deregisterPickable(Pickable* pickable)
{
    // std::cout << "deregistering pickable: " << pickable << std::endl;
    
    _registeredPickables.erase(pickable);
    _currentlyPicked.erase(pickable);
    _pickedLastFrame.erase(pickable);
    
    
    for(IdStackMap::iterator i = _stacks.begin(); i != _stacks.end(); ++i) {
        i->second.currentlyDown.erase(pickable);
        i->second.currentlyOver.erase(pickable);
    }
    
}


PickEventHandler::~PickEventHandler()
{
    for(PickableSet::iterator i = _registeredPickables.begin(); i != _registeredPickables.end(); ++i) {
        (*i)->setPickEventHandler(NULL);
    }
}

}