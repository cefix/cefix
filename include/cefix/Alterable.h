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

#include <cefix/MathUtils.h>
#include <osg/observer_ptr>
#include <osg/ref_ptr>
#include <osg/Referenced>

namespace cefix {

/** 
 * a small data class holding information about the enclosing group of alterables
 */
class AlterableGroupInfo : public osg::Referenced {
public:
    AlterableGroupInfo(const std::string& name) : osg::Referenced(), _name(name) {}
    /// get the name of this group
    const std::string& getName() const { return _name; }
private:
    std::string _name;
};

/**
 * base class for Alterables. Alterables encapsulate a value which is alterable from the outside (widgets, osg, etc)
 * basically it encapsulates a reference to an object and a reference to a variable/object-property
 * An alterable can be composited of 2 or more values (like Vec2, which is a composite of 2 floats)
 */
class Alterable : public osg::Referenced {
public:
    /// type of representation of this alterable 
    enum RepresentationType { UNKNOWN = 0, VALUE, SWITCH, BUTTON, TEXT };
protected:
    /// ctor
    Alterable(const std::string& name, RepresentationType type) 
    :   osg::Referenced(),
        _name(name),
        _type(type),
        _readOnly(false)
    {
    }
    
public:

    class ValueChangedCallback : public osg::Referenced {
    public:
        ValueChangedCallback() : osg::Referenced() {}
        
        virtual void operator()(Alterable* alterable) = 0;
        
    };
    
    /// set the group-info
    void setGroupInfo(AlterableGroupInfo* info) { _groupInfo = info; }
    
    /// get num composites
    virtual unsigned int getNumComposites() const { return 1; }
    
    /// get name of composite for a given index
    virtual std::string getCompositeName(unsigned int composite_ndx) { return _name; }
    
    /// get a key (group + name of alterable + name of composite)
    virtual std::string getCompositeKey(unsigned int composite_ndx) { return createKey("/"+_groupInfo->getName()+"/"+getCompositeName(composite_ndx)); }
    
    /// is this alterable valid
    virtual bool isValid() { return true; }
    
    /// get the name of the alterable
    const std::string& getName() const { return _name; }
    
    /// get the float value of a composite-element
    virtual void setFloatValueAt(unsigned int composite_ndx, float f) {}
    
    /// set the float value of a composite-element
    virtual float getFloatValueAt(unsigned int composite_ndx) { return 0.0f; }
     
    /// get the range for an slider
    virtual doubleRange getSliderRange() { return doubleRange(0,1000); } 
    
    /// get the representation type
    RepresentationType getRepresentationType() { return _type; }
    
    /// isDirty is used to determine if the encapsulated value has changed since the last check
    virtual bool isDirty() { return false; }
    
    /// set to true, when this alterable is only readonly
    void setReadOnly(bool f) { _readOnly = f; }
    
    /// is this alterable readonly=
    bool isReadOnly() const { return _readOnly; }
    
    /// set a callback, the callback gets called, when the stored value got changed
    void setValueChangedCallback(ValueChangedCallback* cb) { _valueChangedCallback = cb; }
protected:
    
    void handleValueChangedCallback() { if (_valueChangedCallback.valid()) (*_valueChangedCallback)(this); }
    
    /// set the representation-type, this is a small hint for the implementation, so 
    /// they know how to represent the alterable in an interface
    void setRepresentationType(RepresentationType type) { _type = type; }
    
    /// create a key from a given string, replace all spaces to underscores and change the case to lower.
    std::string createKey(const std::string& name);

private:    
    std::string _name;
    osg::ref_ptr<AlterableGroupInfo> _groupInfo;
    RepresentationType _type;
    bool _readOnly;
    osg::ref_ptr<ValueChangedCallback> _valueChangedCallback;
public:
    
    /// NoConstraintPolicy, the alterable does not have any constraints
    class NoConstraintPolicy {
    public:
        NoConstraintPolicy() {}
        inline float operator()(float t)  { return t; }
        doubleRange getRange() const { return doubleRange(0,1000.0); }
    };
    
    /// ConstraintsByRangePolicy, the alterable is constrained by a min- and max-value
    class ConstraintByRangePolicy {
    public:
        ConstraintByRangePolicy(const  cefix::doubleRange& in_range) : _range(in_range) {}
        ConstraintByRangePolicy(double min_val, double max_val) : _range(min_val, max_val) {}
        
        inline float operator()(float t)  { return _range.clampTo(t); }
        const doubleRange& getRange() const { return _range; }
    private:
        doubleRange _range;
    };
    
    
};

/**
 * a special Alterable-class, encapsulating a functor to given method of an object, so it can route
 * push-buttons to a target
 */
class AlterableCallFunctor : public Alterable {
public:
    AlterableCallFunctor(const std::string& name, RepresentationType type) : Alterable(name, type) {}
    virtual void call(int i) = 0;
    void call() { call(1); }
};

/// templated inherited AlterableCallFunctor-class, stores the reference to an object/method
template<class T>
class AlterableCallFunctorT : public AlterableCallFunctor {
public:
    AlterableCallFunctorT(const std::string& name, T* holder, void(T::*fpt)(int))
    :   AlterableCallFunctor(name, BUTTON),
        _holder(holder),
        _fpt(fpt)
    {
    }
    virtual bool isValid() { return _holder.valid(); }
    
    virtual void call(int i) {
        (*_holder.*_fpt)(i);
        handleValueChangedCallback();
    }
    
private:
    osg::observer_ptr<T> _holder;
    void(T::*_fpt)(int);
        
};

/// a templated type traits class, encapsulating class-specific information, (num of composites, their names, etc)
template <class T> 
struct AlterableTypeTraits {
    static unsigned int getNumComposites() { return 1; }
    static  std::string getPartialCompositeName(unsigned int ndx) { return ""; }
	
	static inline float* getFloat(unsigned int ndx, const T& val) { return NULL; } 
    
    static inline float getFloatValueAt(unsigned int ndx, const T& t) { return t; }
    static inline void setFloatValueAt(unsigned int ndx, T& t, float f) { t = f; }
    
    static inline Alterable::RepresentationType getAlterableType() { return Alterable::VALUE; }
	typedef T type;
};

template <> 
struct AlterableTypeTraits<bool> {
    static unsigned int getNumComposites() { return 1; }
    static  std::string getPartialCompositeName(unsigned int ndx) { return ""; }
	
    static inline float getFloatValueAt(unsigned int ndx, const bool& t) { return t; }
    static inline void setFloatValueAt(unsigned int ndx, bool& t, float f) {  t = f; }
    
    static inline Alterable::RepresentationType getAlterableType() { return Alterable::SWITCH; }
	typedef bool type;
};


template <>
struct AlterableTypeTraits<osg::Vec4> {
    static unsigned int getNumComposites() { return 4; }
    static  std::string getPartialCompositeName(unsigned int ndx) { static const char* names[4] = { "R", "G", "B", "A" }; return names[ndx]; }
	
	static inline float getFloatValueAt(unsigned int ndx, const osg::Vec4& t) { return t[ndx]; }
    static inline void setFloatValueAt(unsigned int ndx, osg::Vec4& t, float f) { t[ndx] = f; }
    
    static inline Alterable::RepresentationType getAlterableType() { return Alterable::VALUE; }

	typedef float type;
};

template <>
struct AlterableTypeTraits<osg::Vec3> {
    static unsigned int getNumComposites() { return 3; }
    static  std::string getPartialCompositeName(unsigned int ndx) { static const char* names[3] = { "X", "Y", "Z" }; return names[ndx]; }
	
    static inline float getFloatValueAt(unsigned int ndx, const osg::Vec3& t) { return t[ndx]; }
    static inline void setFloatValueAt(unsigned int ndx, osg::Vec3& t, float f) { t[ndx] = f; }
    
    static inline Alterable::RepresentationType getAlterableType() { return Alterable::VALUE; }
    
	typedef float type;
};

template <>
struct AlterableTypeTraits<osg::Vec2> {
    static unsigned int getNumComposites() { return 2; }
    static  std::string getPartialCompositeName(unsigned int ndx) { static const char* names[2] = { "X", "Y" }; return names[ndx]; }
	
    static inline float getFloatValueAt(unsigned int ndx, const osg::Vec2& t) { return t[ndx]; }
    static inline void setFloatValueAt(unsigned int ndx, osg::Vec2& t, float f) {  t[ndx] = f; }
    
    static inline Alterable::RepresentationType getAlterableType() { return Alterable::VALUE; }
    
	typedef float type;
};

template <>
struct AlterableTypeTraits<std::string> {
    static unsigned int getNumComposites() { return 1; }
    static  std::string getPartialCompositeName(unsigned int ndx) { return ""; }
	
    static inline float getFloatValueAt(unsigned int ndx, const std::string& t) { return 0.0f; }
    static inline void setFloatValueAt(unsigned int ndx, std::string& t, float f) {}
    
    static inline Alterable::RepresentationType getAlterableType() { return Alterable::TEXT; }
    
	typedef float type;
};



/**
 * templated Alterable-class, encapsulating a value or object. It stores the holder of the value or object as an observer_ptr to track the lifetime
 * it uses the AlterableTypeTraits-class to retrieve some special propertied for a given class (num of composites, their names etc)
 * 
 * there exist some sepcialization for a range of classes, but not for all classes -- in case you'll have to implement AlterableTypeTraits by yourself
 */
template <class Type>
class AlterableT : public Alterable {
public:
    ///ctor
    AlterableT(const std::string& name, osg::Referenced* holder, Type& val)
    :   Alterable(name, AlterableTypeTraits<Type>::getAlterableType()),
        _holder(holder),
        _val(val)
    {
    }    
    
    /// an alterable is valid, if it's holder exists
    virtual bool isValid() { return _holder.valid(); }
    /// get the stored value
    const Type& getValue() const { return _val; }
    
    /// set the stored value
    void setValue(const Type& val) {
        _val = val;
        handleValueChangedCallback();
    }
	
    /// get the num of components (uses AlterableTypeTraits for getting this information)
    virtual unsigned int getNumComposites() const { return AlterableTypeTraits<Type>::getNumComposites(); }
    
    /// get the name of a specific composite
    virtual std::string getCompositeName(unsigned int composite_ndx) { 
        return (getNumComposites() == 1) ? getName() : getName()+"/"+AlterableTypeTraits<Type>::getPartialCompositeName(composite_ndx); 
    }
    /// set the float value for a given composite
    virtual void setFloatValueAt(unsigned int composite_ndx, float f) 
    {   
        if (!this->isReadOnly()) {
            AlterableTypeTraits<Type>::setFloatValueAt(composite_ndx, _val, applyConstraint(f));
            setValue(_val);
        }
    }
    
    /// get the float value for a given composite
    virtual float getFloatValueAt(unsigned int composite_ndx) { return AlterableTypeTraits<Type>::getFloatValueAt(composite_ndx, _val); }
    
    
    /// returns true, if the encapsulated value has changed
    virtual bool isDirty() { 
        bool result = (_val != _savedVal); 
        if(result) {
            _savedVal = _val;
        }
        return result; 
    }
    
    /// apply an optional constraint
    virtual float applyConstraint(float f) { return f; }
    
    
protected:
    osg::observer_ptr<osg::Referenced> _holder;
    Type &_val, _savedVal;
};


/// an alterable with a given constraintPolicy
template<class Type, class ConstraintPolicy>
class ConstraintAlterableT : public AlterableT<Type> {
public:
    /// ctor
    ConstraintAlterableT(const std::string& name, osg::Referenced* holder, Type& val, const ConstraintPolicy& constraintPolicy = ConstraintPolicy() )
    :   AlterableT<Type>(name, holder, val),
       _constraintPolicy(constraintPolicy)
    {
    }  
    /// get the slider range
    virtual doubleRange getSliderRange() { return _constraintPolicy.getRange(); } 
    
    /// apply the constraint-policy
    virtual float applyConstraint(float f) { return _constraintPolicy(f); }
    
private:
    ConstraintPolicy _constraintPolicy;
};


template <class T, class U, class V, class ConstraintPolicy>
class ConstraintGetterSetterAlterableT : public Alterable {
public:
	ConstraintGetterSetterAlterableT(
										const std::string& name, 
										T* holder, 
										V(T::*getter_fpt)() const,
										void(T::*setter_fpt)(V),
										const ConstraintPolicy& constraintPolicy = ConstraintPolicy()
									)
	:	Alterable(name, AlterableTypeTraits<U>::getAlterableType()),
		_t(holder),
		_getter_fpt(getter_fpt),
		_setter_fpt(setter_fpt),
		_constraintPolicy(constraintPolicy)
	{
	}
	
	/// an alterable is valid, if it's holder exists
    virtual bool isValid() { return _t.valid(); }
    /// get the stored value
    const U& getValue() { _val = (*_t.*_getter_fpt)(); return _val; }
    
    /// set the stored value
    void setValue(const U& val) {
        _val = val;
        (*_t.*_setter_fpt)(val);
        handleValueChangedCallback();
    }
	
    /// get the num of components (uses AlterableTypeTraits for getting this information)
    virtual unsigned int getNumComposites() const { return AlterableTypeTraits<U>::getNumComposites(); }
    
    /// get the name of a specific composite
    virtual std::string getCompositeName(unsigned int composite_ndx) { 
        return (getNumComposites() == 1) ? getName() : getName()+"/"+AlterableTypeTraits<U>::getPartialCompositeName(composite_ndx); 
    }
    /// set the float value for a given composite
    virtual void setFloatValueAt(unsigned int composite_ndx, float f) 
    {   
        if (!this->isReadOnly()) 
		{
            AlterableTypeTraits<U>::setFloatValueAt(composite_ndx, _val, applyConstraint(f)); 
			setValue(_val);
		}
    }
    
    /// get the float value for a given composite
    virtual float getFloatValueAt(unsigned int composite_ndx) 
	{ 
		_val = getValue(); 
		return AlterableTypeTraits<U>::getFloatValueAt(composite_ndx, _val); 
	}
    
    
    /// returns true, if the encapsulated value has changed
    virtual bool isDirty() { 
        bool result = (_val != _savedVal); 
        if(result) {
            _savedVal = _val;
        }
        return result; 
    }
    
	/// get the slider range
    virtual doubleRange getSliderRange() { return _constraintPolicy.getRange(); } 
    
    /// apply the constraint-policy
    virtual float applyConstraint(float f) { return _constraintPolicy(f); }

private:
	osg::observer_ptr<T> _t;
	V(T::*_getter_fpt)() const;
	void(T::*_setter_fpt)(V);
	ConstraintPolicy _constraintPolicy;
	U _val, _savedVal;
};

}