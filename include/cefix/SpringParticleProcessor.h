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


#ifndef SPRING_PARTICLE_PROCESSOR_HEADER
#define SPRING_PARTICLE_PROCESSOR_HEADER


#include <cefix/ParticleProcessor.h>
#include <cefix/ThreadedParticleProcessor.h>
#include <cefix/AnimationFactory.h>
#include <cefix/CommonParticleOperators.h>
#include <cefix/ParticleConstraint.h>
#include <cefix/ParticleConstraintSolver.h>

namespace cefix {

template <class ParticleClass, class OperationStack = ParticleOperationStack<ParticleClass> >
class SpringParticleProcessor : public ParticleProcessor<ParticleClass, OperationStack> {
public:

	SpringParticleProcessor<ParticleClass, OperationStack>(unsigned int numIterations = 20, const osg::Vec3& gravity = osg::Vec3(0,0,-10), float damping = 0.98f) 
	:	ParticleProcessor<ParticleClass, OperationStack>()
	{
		_solver = new cefix::ParticleConstraintSolver<ParticleClass>(numIterations);
		_gravity = new ParticleSetAccelerationOperator<ParticleClass>(gravity);
		_damping = new cefix::ParticleSlowDownOperator<ParticleClass>(damping);
		
		// Wir fügen den ConstraintSolver in einen ParticleOperationStack und den fügen wir dem Processing als PostOperationStack hinzu
		cefix::ParticleOperationStack<ParticleClass>* pos = new cefix::ParticleOperationStack<ParticleClass>();
		
		pos->addOperation(_solver.get());
		pos->addOperation(_damping.get());
		pos->addOperation(_gravity.get());

		
		this->setPostOperationStack(pos);
	}

	
	inline void setDamping(float f) { _damping->setValue(f); }
	inline float getDamping() { return _damping->getValue(); }

	osg::Vec3 getGravity() 
	{ 
		return _gravity->getAcceleration(); 
	}
	
	void setGravity(const osg::Vec3& v) 
	{ 
		_gravity->setAcceleration(v); 
	}
	
	inline unsigned int getNumIterations() { return _solver->getNumIterations(); }
	inline void setNumIterations(unsigned int n) { _solver->setNumIterations(n); }
	
	cefix::ParticleConstraintSolver<ParticleClass>* getSolver() { return _solver.get(); }
	
	
	cefix::ParticleSpringConstraint<ParticleClass>* addSpring(ParticleClass* p1, ParticleClass* p2, float strength, float restLength)
	{
		return _solver->addSpring(p1, p2, strength, restLength);
	}
	
	cefix::ParticleFixedConstraint<ParticleClass>* addFixed(ParticleClass* p1, ParticleClass* p2, float restLength)
	{
		return _solver->addFixed(p1, p2, restLength);
	}
	
	
	cefix::ParticleAttractionConstraint<ParticleClass>* addAttraction(ParticleClass* p1, ParticleClass* p2, float strength) 
	{		
		return _solver->addAttraction(p1, p2, strength);
	}
	
	cefix::ParticleAttractionConstraint<ParticleClass>* addRepulsion(ParticleClass* p1, ParticleClass* p2, float strength) 
	{		
		return _solver->addRepulsion(p1, p2, strength);
	}

	osg::Node* enableDebug() { return _solver->enableDebug(); }
	
	
	unsigned int getNumConstraints() const { return _solver->getNumConstraints(); }


protected:
	osg::ref_ptr<cefix::ParticleConstraintSolver<ParticleClass> >	_solver;
	osg::ref_ptr<cefix::ParticleSlowDownOperator<ParticleClass> >	_damping;
	osg::ref_ptr<cefix::ParticleSetAccelerationOperator<ParticleClass> >	_gravity;

};


template <class ParticleClass>
class ThreadedSpringParticleProcessor : public ThreadedParticleProcessor<ParticleClass> {
public:

	ThreadedSpringParticleProcessor<ParticleClass>(unsigned int numIterations = 20, const osg::Vec3& gravity = osg::Vec3(0,0,-10), float damping = 0.98f) 
	:	ThreadedParticleProcessor<ParticleClass>()
	{
		_solver = new cefix::ParticleConstraintSolver<ParticleClass>(numIterations);
		_gravity = new ParticleSetAccelerationOperator<ParticleClass>(gravity);
		_damping = new cefix::ParticleSlowDownOperator<ParticleClass>(damping);
		
		// Wir fügen den ConstraintSolver in einen ParticleOperationStack und den fügen wir dem Processing als PostOperationStack hinzu
		cefix::ParticleOperationStack<ParticleClass>* pos = new cefix::ParticleOperationStack<ParticleClass>();
		
		pos->addOperation(_solver.get());
		pos->addOperation(_damping.get());
		pos->addOperation(_gravity.get());

		
		this->setPostOperationStack(pos);
	}

	
	inline void setDamping(float f) { _damping->setValue(f); }
	inline float getDamping() { return _damping->getValue(); }

	osg::Vec3 getGravity() 
	{ 
		return _gravity->getAcceleration(); 
	}
	
	void setGravity(const osg::Vec3& v) 
	{ 
		_gravity->setAcceleration(v); 
	}
	
	inline unsigned int getNumIterations() { return _solver->getNumIterations(); }
	inline void setNumIterations(unsigned int n) { _solver->setNumIterations(n); }
	
	cefix::ParticleConstraintSolver<ParticleClass>* getSolver() { return _solver.get(); }
	
	
	cefix::ParticleSpringConstraint<ParticleClass>* addSpring(ParticleClass* p1, ParticleClass* p2, float strength, float restLength)
	{
		return _solver->addSpring(p1, p2, strength, restLength);
	}
	
	cefix::ParticleFixedConstraint<ParticleClass>* addFixed(ParticleClass* p1, ParticleClass* p2, float restLength)
	{
		return _solver->addFixed(p1, p2, restLength);
	}
	
	
	cefix::ParticleAttractionConstraint<ParticleClass>* addAttraction(ParticleClass* p1, ParticleClass* p2, float strength) 
	{		
		return _solver->addAttraction(p1, p2, strength);
	}
	
	cefix::ParticleAttractionConstraint<ParticleClass>* addRepulsion(ParticleClass* p1, ParticleClass* p2, float strength) 
	{		
		return _solver->addRepulsion(p1, p2, strength);
	}

	osg::Node* enableDebug() { return _solver->enableDebug(); }
	
	
	unsigned int getNumConstraints() const { return _solver->getNumConstraints(); }


protected:
	osg::ref_ptr<cefix::ParticleConstraintSolver<ParticleClass> >	_solver;
	osg::ref_ptr<cefix::ParticleSlowDownOperator<ParticleClass> >	_damping;
	osg::ref_ptr<cefix::ParticleSetAccelerationOperator<ParticleClass> >	_gravity;

};



}


#endif