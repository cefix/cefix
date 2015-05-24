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

#ifndef SERIALIZER_HEADER
#define SERIALIZER_HEADER

#include <map>
#include <string>
#include <set>
#include <vector>
#include <deque>
#include <list>
#include <osg/Array>
#include <cefix/PropertyList.h>
#include <cefix/ObjectFactory.h>
#include <cefix/KeyFrameInterpolator.h>
#include <cefix/MathUtils.h>


namespace cefix 
{

/** SerializerNoKeyException is thrown if you forget to preface your var with a key. 
    Be sure to do something along these lines: serializer << "varname" << var;
 */
class SerializerNoKeyException : public std::exception {};

/** The SerializerKeyNotFoundException is thrown if you are working in strict mode and a given key could not be found
 */
class SerializerKeyNotFoundException : public std::exception 
{
public:
	SerializerKeyNotFoundException(const std::string& key) : std::exception(), _key(key) {}
	virtual const char* what()  const throw() { std::string t("Could not find key "+_key); return t.c_str(); }
	const std::string& getKey() { return _key; }
	virtual ~SerializerKeyNotFoundException() throw() {}
private:
	std::string _key;
};

#define SERIALIZER_WRITE_IMPLEMENTATION(x) void write(x value) throw (SerializerNoKeyException) { if (!_hasKey) throw SerializerNoKeyException(); _current->add(_lastKey, value); _hasKey = false; }
#define SERIALIZER_READ_IMPLEMENTATION(type, func) \
void read(type& value) throw(SerializerNoKeyException) \
{ \
	if (!_hasKey || !_currentItem.valid()) throw SerializerNoKeyException(); \
	\
	value = _currentItem->func(); \
	popItem(); \
}


/**
  The Serializer-Class encapsulates all needed logic to serialize and deserialize objects
  into a cefix::PropertyList. It handles stl-vectors automatically, and can even handle 
  virtual inherited classes with some assistance by the coder, you have to register inherited
  classes via registerClass before reading or writing the class
  
*/
class Serializer {
private:
	/** factory creation base class */
	class RegistrarBase : public osg::Referenced{
	public:
		RegistrarBase() : osg::Referenced() {}
		
		virtual osg::Referenced* operator()() const = 0;
	};
	
	/** templated factory creation class */
	template<class T>
	class RegistrarT: public RegistrarBase {
	public:
		RegistrarT(): RegistrarBase() {}
		virtual osg::Referenced* operator()() const
		{
			return new T();
		}
	};


	typedef cefix::ObjectFactory<osg::Referenced, std::string, RegistrarBase> Factory;
	
public:

	/** small helper class encapsulating a default value */
	template <class T>
	class DefaultValue {
	public:
		DefaultValue(T& val, const T& default_value) : _val(val), _defaultValue(default_value) {}
		
		inline void setDefault() const { _val = _defaultValue; }
		inline T& value() const { return _val; }
	private:
		T& _val;
		T  _defaultValue;
	};

	/** ctor
	  @param pl Propertylist to read from to write into
	  @param strict if true, Serializer will throw an exception if a given key is not present in the propertylist
	*/
	Serializer(cefix::PropertyList* pl = new cefix::PropertyList(), bool strict=false);
	Serializer(const std::string& file, bool strict=false);
	
	/** 
	 * registers a class, so the Serializer can handle inherited classes from a pointer to its base-class
	 */
	template<class T>
	void registerClass(const std::string& aclassName = "")
	{
		std::string className( aclassName.empty() ? getClassName<T>() : aclassName);
		_factory->registerObjectClass(className, new RegistrarT<T>());
		_classNameMapping.insert(std::make_pair(getClassName<T>(), className));
	}
	template<class T, typename U>
	static DefaultValue<T> defaultValue(T& t, U d_val) { return DefaultValue<T>(t, d_val); }

	
	/** get the PropertyList used for storage */
	cefix::PropertyList* getPropertyList() { return _root.get(); }
	
	void setNotifyLevel(osg::NotifySeverity level) { _notify = level; }
	
	/** the operator << is used for writing into the serializer. Be sure to use it like serializer << "var-name" << var */
	template <class T>
	Serializer& operator<<(T& obj)
	{
		write(obj);
		return *this;
	}
	template <class T>
	Serializer& operator<<(const T& obj)
	{
		write(obj);
		return *this;
	}
	
	template <class T>
	Serializer& operator<<(T* obj)
	{
		write(*obj);
		return *this;
	}
		
	
	/** the operator >> is used for reading from the serializer. Be sure to use it like serializer >> "var-name" >> var */
	template <class T>
	Serializer& operator>>(T& obj)
	{
		read(obj);
		return *this;
	}
	
	template <class T>
	Serializer& operator>>(T* obj)
	{
		read(*obj);
		return *this;
	}
	
	/** the operator >> is used for reading from the serializer. Be sure to use it like serializer >> "var-name" >> var */
	template <class T>
	Serializer& operator>>(const DefaultValue<T>& obj)
	{
		read(obj);
		return *this;
	}
    
    cefix::PropertyList* getCurrentPropertyList() { return _current.get(); }
			
private:
	template <class T, T val> 
	struct member_wrapper{}; 

	template <class T> 
	static char test_for_writeTo(member_wrapper<void (T::*)(Serializer&), &T::writeTo>* p); 

	template <class T> 
	static double test_for_writeTo(...); 

	template <class T> 
	struct has_member_writeTo 
	{ 
	static const bool value = (1 == sizeof(test_for_writeTo<T>(0))); 
	}; 

	template <class T, bool>
	struct ReadWriteImplementation {
		static inline void write(Serializer& serializer, T& t) { t.writeTo(serializer); }
		static inline void read(Serializer& serializer, T& t) { t.readFrom(serializer); }
	};
	template <class T>
	struct ReadWriteImplementation<T, false> {
		static inline void write(Serializer& serializer, T& t) { writeToSerializer(serializer, t); }
		static inline void read(Serializer& serializer, T& t) { readFromSerializer(serializer, t); }
	};
	
	template <class T>
	void write(T& t) throw (SerializerNoKeyException)
	{
		if (!_hasKey) throw SerializerNoKeyException();
		
		pushAndCreateNewList(_lastKey);
		pushKey(_lastKey,false);
		std::string className = getClassName(t);
		if ( hasClassNameMapping(className) ) {
			_current->add("__CLASS", getClassNameMapping(className) );
		}
		ReadWriteImplementation<T, (1 == sizeof(test_for_writeTo<T>(0))) >::write(*this, t);
		popKey();
		popList();

	}

	template<class T>
	void write(osg::ref_ptr<T>& t) 
	{
		write(*t);
	}
	
	SERIALIZER_WRITE_IMPLEMENTATION( bool);
	SERIALIZER_WRITE_IMPLEMENTATION( int);
	SERIALIZER_WRITE_IMPLEMENTATION( unsigned int);
	SERIALIZER_WRITE_IMPLEMENTATION( float);
	SERIALIZER_WRITE_IMPLEMENTATION( double);
	SERIALIZER_WRITE_IMPLEMENTATION( std::string& );
	// TODO SERIALIZER_WRITE_IMPLEMENTATION(const std::wstring&);
	SERIALIZER_WRITE_IMPLEMENTATION( osg::Vec2f&);
	SERIALIZER_WRITE_IMPLEMENTATION( osg::Vec2d&);
	SERIALIZER_WRITE_IMPLEMENTATION( osg::Vec3f&);
	SERIALIZER_WRITE_IMPLEMENTATION( osg::Vec3d&);
	SERIALIZER_WRITE_IMPLEMENTATION( osg::Vec4f&);
	SERIALIZER_WRITE_IMPLEMENTATION( osg::Quat&);
	SERIALIZER_WRITE_IMPLEMENTATION( osg::Matrix&);
	
	SERIALIZER_WRITE_IMPLEMENTATION(const std::string&);
	// TODO SERIALIZER_WRITE_IMPLEMENTATION(const std::wstring&);
	
	SERIALIZER_WRITE_IMPLEMENTATION(const osg::Vec2f&);
	SERIALIZER_WRITE_IMPLEMENTATION(const osg::Vec2d&);
	SERIALIZER_WRITE_IMPLEMENTATION(const osg::Vec3f&);
	SERIALIZER_WRITE_IMPLEMENTATION(const osg::Vec3d&);
	SERIALIZER_WRITE_IMPLEMENTATION(const osg::Vec4f&);
	SERIALIZER_WRITE_IMPLEMENTATION(const osg::Quat&);
	SERIALIZER_WRITE_IMPLEMENTATION(const osg::Matrix&);
	
    void write(short int& i) { write((int)i); }
	void write(unsigned short int& i) { write((unsigned int)i); }
	
	template <class T>
	void writeContainer(T& v) 
	{
		if (!_hasKey) throw SerializerNoKeyException();
		pushAndCreateNewList(_lastKey);
		
		for(typename T::iterator i = v.begin(); i != v.end();++i) 
		{
			pushKey(_lastKey, true);
			write(*i);
			popKey();
		}
		
		popList();
	}
	
	template <class T> \
	void writeCollection(T& v) throw(SerializerNoKeyException)
	{
		if (!_hasKey) throw SerializerNoKeyException();
		pushAndCreateNewList(_lastKey);
		
		for(typename T::iterator i = v.begin(); i != v.end();++i) 
		{
			pushAndCreateNewList(_lastKey);
			pushKey("key", true);
			write(i->first);
			popKey();
			
			pushKey("value", true);
			write(i->second);
			popKey();
			popList();
		}
		
		popList();
	}

	
	
	template<class T, class Alloc> void write(std::deque<T, Alloc>& t) { writeContainer(t); }
	template<class T, class Alloc> void write(std::list<T, Alloc>& t) { writeContainer(t); }
	template<class T, class Alloc> void write(std::vector<T, Alloc>& t) { writeContainer(t); }
	template<typename T, osg::Array::Type ARRAYTYPE, int DataSize, int DataType>
	void write(osg::TemplateArray< T, ARRAYTYPE, DataSize, DataType >& t) { writeContainer(t); }
	
	template<class T, class Alloc, class Sort> void write(std::set<T, Alloc, Sort>& t) { writeContainer(t); }
	template<class T, class Alloc, class Sort> void write(std::multiset<T, Alloc, Sort>& t) { writeContainer(t); }

	template<class T, class Alloc, class Sort> void write(std::map<T, Alloc, Sort>& t) { writeCollection(t); }
	template<class T, class Alloc, class Sort> void write(std::multimap<T, Alloc, Sort>& t) { writeCollection(t); }
	
	
	template <class T>
	void read(T& t) throw (SerializerNoKeyException)
	{
		if (!_hasKey) throw SerializerNoKeyException();
		
		pushKey(_lastKey,false);
		ReadWriteImplementation<T, (1 == sizeof(test_for_writeTo<T>(0))) >::read(*this, t);
		popKey();
		popItem();
	}

	template<class T>
	void read(osg::ref_ptr<T>& t) 
	{
		if (_current.valid() && _current->hasKey("__CLASS")) {
			if (_factory->hasObjectClass(_current->get("__CLASS")->asString()))
				t = dynamic_cast<T*>(_factory->create(_current->get("__CLASS")->asString()));
			if (!t.valid()) 
			{
				log::error("Serializer") << "could not create object of class " << _current->get("__CLASS")->asString() << ", creating base-class!" << std::endl;
				t = new T();
			}
		} else
			t = new T();
		read(*t);
	}
	
	template<class T>
	void read(const DefaultValue<T>& t) {
		if (!_keyFound) {
			t.setDefault();
			popItem();
		}
		else {
			read(t.value());
		}
	}
	
	SERIALIZER_READ_IMPLEMENTATION(bool, asBool);
	SERIALIZER_READ_IMPLEMENTATION(int, asInt);
	SERIALIZER_READ_IMPLEMENTATION(unsigned int, asInt);
	SERIALIZER_READ_IMPLEMENTATION(float, asFloat);
	SERIALIZER_READ_IMPLEMENTATION(double, asDouble);
	SERIALIZER_READ_IMPLEMENTATION(osg::Vec2f, asVec2);
	SERIALIZER_READ_IMPLEMENTATION(osg::Vec2d, asVec2);
	SERIALIZER_READ_IMPLEMENTATION(osg::Vec3f, asVec3);
	SERIALIZER_READ_IMPLEMENTATION(osg::Vec3d, asVec3);
	SERIALIZER_READ_IMPLEMENTATION(osg::Vec4f, asVec4);
	SERIALIZER_READ_IMPLEMENTATION(osg::Quat, asQuat);
	SERIALIZER_READ_IMPLEMENTATION(osg::Matrix, asMatrix);
	SERIALIZER_READ_IMPLEMENTATION(std::string, asString);
	SERIALIZER_READ_IMPLEMENTATION(std::wstring, asWstring);
	
    void read(short int& i) {
        int j; read(j); i = j;
    }
    
    void read(unsigned short int& i) {
        unsigned int j; read(j); i = j;
    }
	
	template <class T>
	void readPushBackContainer(T& v) 
	{
		if (!_hasKey) throw SerializerNoKeyException();
		cefix::PropertyList* pl = _currentItem->asPropertyList();
		v.clear();
		if (pl) 
		{
			for(cefix::PropertyList::iterator i = pl->begin(); i!=pl->end(); ++i) 
			{
				if ((*i)->getKey() == _lastKey) {
					pushItem((*i).get());
					
					typename T::value_type data;
					_hasKey = true;
					read(data);
					v.push_back(data);
				}
			}		
		}
		popItem();
	}
	
	template <class T>
	void readInsertContainer(T& v) 
	{
		if (!_hasKey) throw SerializerNoKeyException();
		cefix::PropertyList* pl = _currentItem->asPropertyList();
		v.clear();
		if (pl) 
		{
			for(cefix::PropertyList::iterator i = pl->begin(); i!=pl->end(); ++i) 
			{
				if ((*i)->getKey() == _lastKey) {
					pushItem((*i).get());
				
					typename T::value_type data;
					_hasKey = true;
					read(data);
					v.insert(data);
				}
			}		
		}
		popItem();
	}
	
	
	
	template <class T> 
	void readCollection(T& v) throw(SerializerNoKeyException)
	{
		if (!_hasKey) throw SerializerNoKeyException();
		v.clear();
		cefix::PropertyList* pl = _currentItem->asPropertyList();
		
		if (pl) 
		{
			for(cefix::PropertyList::iterator i = pl->begin(); i!=pl->end(); ++i) 
			{
				if ((*i)->getKey() == _lastKey) {
				
					cefix::PropertyList* subpl = (*i)->asPropertyList();
					if (!subpl) 
						continue;
						
					typename T::key_type key;
					typename T::mapped_type value;
					
					pushItem(subpl->get("key"));
					_hasKey = true;
					read(key);
					pushItem(subpl->get("value"));
					_hasKey = true;
					read(value);
					
					v.insert(std::make_pair(key, value));
				}
			}		
		}
		popItem();
	}

	
	
	template<class T, class Alloc> void read(std::deque<T, Alloc>& t) { readPushBackContainer(t); }
	template<class T, class Alloc> void read(std::list<T, Alloc>& t) { readPushBackContainer(t); }
	template<class T, class Alloc> void read(std::vector<T, Alloc>& t) { readPushBackContainer(t); }
	template<typename T, osg::Array::Type ARRAYTYPE, int DataSize, int DataType>
	void read(osg::TemplateArray< T, ARRAYTYPE, DataSize, DataType >& t) { readPushBackContainer(t); }
	
	template<class T, class Alloc, class Sort> void read(std::set<T, Alloc, Sort>& t) { readInsertContainer(t); }
	template<class T, class Alloc, class Sort> void read(std::multiset<T, Alloc, Sort>& t) { readInsertContainer(t); }

	template<class T, class Alloc, class Sort> void read(std::map<T, Alloc, Sort>& t) { readCollection(t); }
	template<class T, class Alloc, class Sort> void read(std::multimap<T, Alloc, Sort>& t) { readCollection(t); }


	
	
	
	void pushKey(const std::string& key, bool hasKey) {
		_keyStack.push_back(_lastKey);
		_lastKey = key;
		_hasKey = hasKey;
		
	}
	
	void popKey() {
		_lastKey = _keyStack.back();
		_keyStack.pop_back();
		_hasKey = false;
	}
	
	void pushAndCreateNewList(const std::string& key) {
		cefix::PropertyList * pl = new cefix::PropertyList();
		_current->add(key, pl);
		_listStack.push_back(_current);
		_hasKey = false;
		_current = pl;
	}
	
	void pushList(cefix::PropertyList* pl) {
		_listStack.push_back(_current);
		_hasKey = false;
		_current = pl;
	}

	
	void popList() 
	{
		_current = _listStack.back();
		_listStack.pop_back();
	}
	
	void pushItem(cefix::AbstractProperty* p) 
	{
		//std::cout <<  std::setw(3*_itemStack.size()) << " " << "before  " << _currentItem << "/" << _current << " " << _itemStack.size() << " KEY: " << _lastKey << std::endl;
		_itemStack.push_back(_currentItem);
		_listStack.push_back(_current);
		
		_currentItem = p;
		if (_currentItem->asPropertyList())
			_current = _currentItem->asPropertyList();
		
		//std::cout << std::setw(3*_itemStack.size()) << " " << "pushing " << _currentItem << "/" << _current << " " << _itemStack.size() <<  std::endl;
		//dumpCurrent(3*_itemStack.size());
	}
	
	void popItem() 
	{
		_currentItem = _itemStack.back();
		_itemStack.pop_back();
		
		_current = _listStack.back();
		_listStack.pop_back();
		
		//std::cout << std::setw(3*_itemStack.size()) << " " << "popping " << _currentItem << "/" << _current << " "<< _current->size() << " " << _itemStack.size() <<  std::endl;
		// dumpCurrent(3*_itemStack.size());
	}
	
	void dumpCurrent(int w) {
		std::cout << std::setw(w) << " " << ">> ";
		if (!_current.valid()) return;
		for(int i = 0;i < std::min<int>(3, _current->size()); ++i) std::cout << _current->getKeyAt(i) << " ";
		std::cout << std::endl;	
	}
	
	bool hasClassNameMapping(const std::string& className) 
	{
		return _classNameMapping.find( className ) != _classNameMapping.end();
	}
	
	std::string getClassNameMapping(const std::string& className) 
	{
		return _classNameMapping[ className ];
	}
	
	template <class T>
	const char* getClassName(T& t) {
		return typeid(t).name();
	}
	
	template <class T>
	const char* getClassName() {
		return typeid(T).name();
	}
	
	osg::ref_ptr<cefix::PropertyList> _root, _current;
	osg::ref_ptr<cefix::AbstractProperty> _currentItem;
	std::string _lastKey;
	bool  _hasKey;
	
	std::deque<std::string> _keyStack;
	std::deque< osg::ref_ptr<cefix::PropertyList> > _listStack;
	std::deque< osg::ref_ptr<cefix::AbstractProperty> > _itemStack;
	bool _strict;
	osg::ref_ptr<Factory> _factory;
	std::map<std::string, std::string> _classNameMapping;
	bool	_keyFound;
	osg::NotifySeverity	_notify;
};

template<> Serializer& Serializer::operator<<(const char* str);	
template<> Serializer& Serializer::operator<<(std::string& str);
template<> Serializer& Serializer::operator<<(const std::string& str);

template<> Serializer& Serializer::operator>>(const char* str);



template <class KEYFRAME>
void readFromSerializer(cefix::Serializer& serializer, cefix::KeyFrameInterpolator<KEYFRAME>& keyframes) 
{
	serializer >> "keyframes" >> keyframes.getKeyframes();
	keyframes.init();
}

template <class KEYFRAME>
void writeToSerializer(cefix::Serializer& serializer, cefix::KeyFrameInterpolator<KEYFRAME>& keyframes) 
{
	serializer << "keyframes" << keyframes.getKeyframes();
}

template <typename T>
void readFromSerializer(cefix::Serializer& serializer, cefix::range<T>& range) 
{
	T min, max;
	serializer >> "from" >> min;
	serializer >> "to" >> max;
	range.set(min,max);
}

template <typename T>
void writeToSerializer(cefix::Serializer& serializer, cefix::range<T>& range) 
{
	serializer << "from" << range.min();
	serializer << "to" << range.max();
}

template<typename T, typename U>
	void readFromSerializer(cefix::Serializer& serializer, std::pair<T,U>& p)
{
	serializer >> "first" >> p.first;
	serializer >> "second" >> p.second;
}
	
template<typename T, typename U>
void writeToSerializer(cefix::Serializer& serializer, std::pair<T,U>& p) 
{
	serializer << "first" << p.first;
	serializer << "second" << p.second;
}
	


}


#endif