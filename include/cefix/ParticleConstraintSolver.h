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

#ifndef CEFIX_PARTICLE_CONSTRAINT_SOLVER_HEADER
#define CEFIX_PARTICLE_CONSTRAINT_SOLVER_HEADER

#include <cefix/ParticleOperator.h>
#include "ParticleConstraint.h"
#include <cefix/DebugGeometryFactory.h>
#include <cefix/ParticleGroup.h>

namespace cefix {
template <class ParticleClass>
class ParticleConstraintSolver : public cefix::ParticleOperator<ParticleClass> {

public:
	typedef ParticleConstraint<ParticleClass> Constraint;
	typedef StorageList<Constraint> ConstraintList;
	typedef ParticleGroup<ParticleClass> Group;
	ParticleConstraintSolver(unsigned int numIterations = 20) : cefix::ParticleOperator<ParticleClass>(), _numIterations(numIterations) {}
	
	void addConstraint(Constraint* c) { _constraints.add(c); }
	
	cefix::ParticleSpringConstraint<ParticleClass>* addSpring(ParticleClass* p1, ParticleClass* p2, float strength, float restLength)
	{
		cefix::ParticleSpringConstraint<ParticleClass>* constraint = new cefix::ParticleSpringConstraint<ParticleClass>(p1, p2, strength, restLength);
		addConstraint(constraint);
		
		return constraint;
	}
	
	cefix::ParticleFixedConstraint<ParticleClass>* addFixed(ParticleClass* p1, ParticleClass* p2, float restLength)
	{
		cefix::ParticleFixedConstraint<ParticleClass>* constraint = new cefix::ParticleFixedConstraint<ParticleClass>(p1, p2, restLength);
		addConstraint(constraint);
		
		return constraint;
	}
	
	
	cefix::ParticleAttractionConstraint<ParticleClass>* addAttraction(ParticleClass* p1, ParticleClass* p2, float strength) 
	{
		cefix::ParticleAttractionConstraint<ParticleClass>* constraint = new cefix::ParticleAttractionConstraint<ParticleClass>(p1, p2, strength);
		addConstraint(constraint);
		
		return constraint;
	}
	
	cefix::ParticleAttractionConstraint<ParticleClass>* addRepulsion(ParticleClass* p1, ParticleClass* p2, float strength) 
	{
		cefix::ParticleAttractionConstraint<ParticleClass>* constraint = new cefix::ParticleAttractionConstraint<ParticleClass>(p1, p2, -strength);
		addConstraint(constraint);
		
		return constraint;
	}
	
		
	void begin(typename ParticleClass::time_t dt) 
	{
		for(unsigned int j = 0; j < _numIterations; ++j) 
		{
			for(typename ConstraintList::iterator i = _constraints.begin(); i != _constraints.end(); ) 
			{
				(*i)->solve();
				
				if ((*i)->isAlive()) {
					++i;
				} else {
					i = _constraints.erase(i);
				}
				
			}
			float ddt(dt / _numIterations);
			for(typename Group::List::iterator i = this->getParticles().begin(); i != this->getParticles().end(); ++i) {
				(*i)->updatePosition( ddt );
			}
		}
		
		if (_debugGeometry.valid()) {
			_debugGeometry->clear();	
			for(typename ConstraintList::iterator i = _constraints.begin(); i != _constraints.end(); ++i) 
			{
				_debugGeometry->addLine( (*i)->getLeft()->getPosition(), (*i)->getRight()->getPosition(),  (*i)->getDebugColor() );
			}
			_debugGeometry->finish();
		}
	}
	
	unsigned int getNumIterations() const { return _numIterations; }
	void setNumIterations(unsigned int n) { _numIterations = n; }
	
	osg::Node* enableDebug() 
	{
		cefix::DebugGeometryFactory::enable("particleConstraintSolver");
		_debugGeometry = cefix::DebugGeometryFactory::get("particleConstraintSolver");
		_debugGeometry->getOrCreateNode()->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
		_debugGeometry->getOrCreateNode()->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
		return _debugGeometry->getOrCreateNode();
	}
	
	unsigned int getNumConstraints() const { return _constraints.size(); }
	
	ConstraintList& getConstraints() { return _constraints; }
    const ConstraintList& getConstraints() const { return _constraints; }
    
private:
	ConstraintList _constraints;
	unsigned _numIterations;  
	osg::ref_ptr<cefix::DebugOutputBase> _debugGeometry;
}; 


}

#endif