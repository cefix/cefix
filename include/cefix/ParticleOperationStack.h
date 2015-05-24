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

#ifndef PARTICLE_OPERATION_STACK
#define PARTICLE_OPERATION_STACK

#include <vector>

#include <cefix/ParticleOperator.h>
#include <cefix/ParticleGroup.h>

namespace cefix {

class WidgetComposer;

/** base-class for ParticleOperatorAdapter-templates, it ancapsulates a ParticleOperator-class for handling inside a ParticleOperationStack */
template <class ParticleClass>
class ParticleOperatorAdapter : public osg::Referenced {
	public:
		ParticleOperatorAdapter() : osg::Referenced() {}
		virtual void operator()(const typename ParticleClass::time_t& dt, ParticleGroup<ParticleClass>* group) = 0;
        virtual void applyWidgetComposer(WidgetComposer& composer) {}

};

/** template class which encapsulates a ParticleOperator-class and call its operations */
template <class ParticleClass, class ParticleOperatorClass > class ParticleOperatorAdapterT : public ParticleOperatorAdapter<ParticleClass> {

	public:
		typedef ParticleGroup<ParticleClass> Group;
		
		ParticleOperatorAdapterT(ParticleOperatorClass* op) : ParticleOperatorAdapter<ParticleClass>(), _op(op) {}
		
		virtual void operator()(const typename ParticleClass::time_t& dt, Group* group) {
			if (_op->enabled()) 
			{
				_op->setGroup(group);
				_op->begin(dt);
				const typename Group::List& list = group->getParticles();
				for(typename Group::List::const_iterator i = list.begin(); i != list.end(); ++i) 
				{
					_op->operator()((*i),dt);
				}
				_op->finish();
			}				
		}
        
        virtual void applyWidgetComposer(WidgetComposer& composer) { _op->applyWidgetComposer(composer); }
        
		ParticleOperatorClass* get() { return _op.get(); }
	private:
		osg::ref_ptr<ParticleOperatorClass>	_op;
};

/** this class implements a stack of ParticleOperatorAdapters (which encapsulates the operator-classes) which are executed one after another. */
template <class ParticleClass>
class ParticleOperationStack : public osg::Referenced {

	public:
		typedef std::vector< osg::ref_ptr< ParticleOperatorAdapter<ParticleClass> > > OperationVector;
		typedef ParticleGroup<ParticleClass> Group;
		typedef ParticleOperatorAdapter<ParticleClass> OperatorAdapter;
		
		/** ctor */
		ParticleOperationStack() :  osg::Referenced() {}
		
		/** adds an ParticleOperator */
		inline void addOperatorAdapter(OperatorAdapter* op) { _operations.push_back(op); }
		
		/** get the amount of operations stored in this stack */
		inline unsigned int getNumOperations() const { return _operations.size(); }
		
		/** get a specific ParticleOperatorAdapter */
		template<typename ParticleOperatorClass>ParticleOperatorClass* getOperation(unsigned int ndx) const 
		{ 
			if (ndx >= getNumOperations())
				return NULL;
			ParticleOperatorAdapterT<ParticleClass, ParticleOperatorClass>* op = dynamic_cast< ParticleOperatorAdapterT<ParticleClass, ParticleOperatorClass>* >(_operations[ndx].get());
			if (op)
				return op->get();
			else
				return NULL;
				
		}
		/// adds a ParticleOperator to the operationstack, will create a adapter-class
		template<typename ParticleOperatorClass> void addOperation(ParticleOperatorClass* op) 
		{ 
			addOperatorAdapter(new ParticleOperatorAdapterT<ParticleClass, ParticleOperatorClass>(op)); 
		}
		
		/// sets a ParticleOperator to a specific position operationstack, will create a adapter-class
		template<typename ParticleOperatorClass> void setOperation(unsigned int ndx, ParticleOperatorClass* op) 
		{ 
			if (ndx < getNumOperations())
				setOperatorAdapter(ndx, new ParticleOperatorAdapterT<ParticleClass, ParticleOperatorClass>(op)); 
		}
		
		/** sets a specific ParticleOperatorAdapter */
		inline void setOperatorAdapter(unsigned int ndx, OperatorAdapter* op) {if (ndx < getNumOperations()) _operations[ndx] = op; }
		

		/** remove an operator */
		// TODO inline void removeOperation(unsigned int ndx) { if (ndx < getNumOperations()) _operations.erase(_operations[ndx]); }
		
		/** process all particles in the ParticleGroup */
		void process(const typename ParticleClass::time_t& dt, Group* g) {
			for(typename OperationVector::iterator i = _operations.begin(); i!= _operations.end(); ++i) {
				(*i)->operator()(dt, g);
			}
		}
        
        void applyWidgetComposer(WidgetComposer& composer) {
            for(typename OperationVector::iterator i = _operations.begin(); i!= _operations.end(); ++i) {
				(*i)->applyWidgetComposer(composer);
			}
        }
		
	protected:
		virtual ~ParticleOperationStack() {}
		OperationVector _operations;
	
};

}

#endif