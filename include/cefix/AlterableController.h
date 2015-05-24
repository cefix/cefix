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

#pragma once

#include <cefix/Alterable.h>

namespace cefix {

/// a small class, encapsulating a number of Alterables
struct AlterableGroup {
    typedef std::list< osg::ref_ptr<Alterable> > List;
    List items;
    osg::ref_ptr<AlterableGroupInfo> groupInfo;
    
    void add(Alterable* a) { a->setGroupInfo(groupInfo); items.push_back(a); }
    
    AlterableGroup(const std::string& name) : items(), groupInfo(new AlterableGroupInfo(name)) {}
    AlterableGroup() : items(), groupInfo(NULL) {}
};

class AlterableController;

/**
 * an implementation for an AlterableController, the implementation-class does all the heavy work
 */
class AlterableControllerImplementation : public osg::Referenced {
public:
    /// ctor
    AlterableControllerImplementation(): osg::Referenced(), _controller(NULL) {}
    
    /// set the AlterableController
    void setController(AlterableController* controller) { _controller = controller; }
    
    /// rebuild the interface
    virtual void rebuild() = 0;

protected:
    /// get the controller
    AlterableController* getController() { return _controller; }

private:
    AlterableController* _controller;
    
};

/**
 * the AlterableController controls all Alterables, it stores all alterables, and creates the interfaces when necessary. 
 * It uses a set of implementation-classes to do the heavy work
 * An AlterableController can have more than one implementation
 */
class AlterableController : public osg::Referenced {
public:
    typedef std::map<std::string, AlterableGroup> AlterableGroups;
    typedef std::list<osg::ref_ptr< AlterableControllerImplementation > > ImplementationList;
    typedef AlterableGroups::iterator iterator;
    
    /// add a value-reference/object-pair as an Alterable, with no constraint
    template <class T> 
    static ConstraintAlterableT<T, Alterable::NoConstraintPolicy >* add(const std::string& group_name, const std::string& name, osg::Referenced* holder, T& t) 
    {
        osg::ref_ptr<ConstraintAlterableT<T, Alterable::NoConstraintPolicy > > a = new ConstraintAlterableT<T, Alterable::NoConstraintPolicy>(name, holder, t);
        instance()->addAlterable(group_name, a);
        return a;
    }
    
    /// add a value-reference/object-pair as an Alterable, with a range-constraint
    template <class T> 
    static ConstraintAlterableT<T, Alterable::ConstraintByRangePolicy >* add(const std::string& group_name, const std::string& name, osg::Referenced* holder, T& t, const doubleRange& constraining_range) 
    {
        osg::ref_ptr<ConstraintAlterableT<T, Alterable::ConstraintByRangePolicy > > a = new ConstraintAlterableT<T, Alterable::ConstraintByRangePolicy>(name, holder, t, Alterable::ConstraintByRangePolicy(constraining_range));
        instance()->addAlterable(group_name, a);
        return a;
    }
    
	/// add a getter/setter + object as an Alterable, with a range-constraint
    template <class T, typename U> 
    static ConstraintGetterSetterAlterableT<T, U, U, Alterable::ConstraintByRangePolicy >* add(
		const std::string& group_name, 
		const std::string& name, 
		T* holder, 
		U(T::*getter_fpt)() const, 
		void(T::*setter_fpt)(U), 
		const doubleRange& constraining_range
	) 
    {
        osg::ref_ptr<ConstraintGetterSetterAlterableT<T, U, U, Alterable::ConstraintByRangePolicy > > a = new ConstraintGetterSetterAlterableT<T, U, U, Alterable::ConstraintByRangePolicy>(name, holder, getter_fpt, setter_fpt, Alterable::ConstraintByRangePolicy(constraining_range));
        instance()->addAlterable(group_name, a);
        return a;
    }
    
    /// add a getter/setter + object as an Alterable, with a range-constraint, const variant
    template <class T, typename U> 
    static ConstraintGetterSetterAlterableT<T, U, const U&, Alterable::ConstraintByRangePolicy >* add(
		const std::string& group_name, 
		const std::string& name, 
		T* holder, 
		const U&(T::*getter_fpt)() const,
		void(T::*setter_fpt)(const U&),
		const doubleRange& constraining_range
	) 
    {
        osg::ref_ptr<ConstraintGetterSetterAlterableT<T, U, const U&, Alterable::ConstraintByRangePolicy > > a = new ConstraintGetterSetterAlterableT<T, U, const U&, Alterable::ConstraintByRangePolicy>(name, holder, getter_fpt, setter_fpt, Alterable::ConstraintByRangePolicy(constraining_range));
        instance()->addAlterable(group_name, a);
        return a;
    }
    
    /// add a value-reference as an Alterable, with no constraint, suitable for static variables
    template <class T> 
    static ConstraintAlterableT<T, Alterable::NoConstraintPolicy >* add(const std::string& group_name, const std::string& name, T& t) 
    {
        osg::ref_ptr<ConstraintAlterableT<T, Alterable::NoConstraintPolicy > > a = new ConstraintAlterableT<T, Alterable::NoConstraintPolicy>(name, instance(), t);
        instance()->addAlterable(group_name, a);
        return a;
    }
    
    /// add a value-reference as an Alterable, with a range-constraint, suitable for static variables
    template <class T> 
    static ConstraintAlterableT<T, Alterable::ConstraintByRangePolicy >* add(const std::string& group_name, const std::string& name, T& t, const doubleRange& constraining_range) 
    {
        osg::ref_ptr<ConstraintAlterableT<T, Alterable::ConstraintByRangePolicy > > a = new ConstraintAlterableT<T, Alterable::ConstraintByRangePolicy>(name, instance(), t, Alterable::ConstraintByRangePolicy(constraining_range));
        instance()->addAlterable(group_name, a);
        return a;
    }
    
    /// add a functor as an Alterable, useful for push-buttons, etc
    template <class T>
    static AlterableCallFunctor* add(const std::string& group_name, const std::string& name, T* holder, void(T::*fpt)(int))
    {
        osg::ref_ptr<AlterableCallFunctorT<T> > a = new AlterableCallFunctorT<T>(name, holder, fpt);
        instance()->addAlterable(group_name, a);
        return a;
    }
    
    
    /// dump all alterables to the given stream (for debug purposes)
    static void dump(std::ostream& s) {
        instance()->dumpTo(s);
    }
    
    /// add an implementation to the list of implementations
    static void addImplementation(AlterableControllerImplementation* impl) 
    { 
        impl->setController(instance());
        instance()->_impls.push_back(impl); 
        rebuild(); 
    }
    
    /// rebuild all Alterables
    static void rebuild() 
    {
        AlterableController* ctrl = instance();
        for(ImplementationList::iterator i = ctrl->_impls.begin(); i != ctrl->_impls.end(); ++i) {
            (*i)->rebuild();
        }
    }

protected:
    /// get the instance of the AlterableController-singleton
    static AlterableController* instance(bool erase = false);
    
    /// cleanup the AlterableController
    static void cleanup();
    
    /// add an Alterable-object to a given group
    void addAlterable(const std::string& group_name, Alterable* a)
    {
        AlterableGroups::iterator i = _groups.find(group_name);
        if (i == _groups.end()) {
            _groups[group_name] = AlterableGroup(group_name);
        }
        
        _groups[group_name].add(a);
    }
	
	

public:    
    
    /// get the num of groups
    unsigned int getNumGroups() const { return _groups.size(); }
    
    /// get all groups
    AlterableGroups& getGroups() { return _groups; }
    const AlterableGroups& getGroups() const { return _groups; }
    
    /// get the begin-iterator
    iterator begin() { return _groups.begin(); }
    
    // get the end-iterator
    iterator end() { return _groups.end(); }
    
private:
    AlterableController();
    void dumpTo(std::ostream& s);
    
    AlterableGroups _groups;
    ImplementationList _impls;
};

}