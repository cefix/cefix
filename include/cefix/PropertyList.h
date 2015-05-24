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
#ifndef __PROPERTY_LIST_HEADER__
#define __PROPERTY_LIST_HEADER__

#include <string.h>

#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <osg/Referenced>
#include <osg/ref_ptr>
#include <cefix/Log.h>
#include <cefix/tinyxml.h>


#include <osg/Vec2>
#include <osg/Vec3>
#include <osg/Vec4>
#include <osg/Quat>
#include <osg/Matrix>
#include <osgDB/FileNameUtils>
#include <cefix/Export.h>
#include <cefix/StringUtils.h>

namespace cefix {
	
    class PropertyList;
    class MultipleValueList;
	
	/**
	 * AbstractProperty defines an abstract class, which is the base class for the various Property-classes
	 */
	class CEFIX_EXPORT AbstractProperty : public osg::Referenced {
		/**
		 * different types of Properties
		 */
		public:
			enum PropertyType {
				UNDEFINED,			/**< the property is undefined, without type */
				BOOLEAN,
                INTEGER,			/**< the property is an integer */
				FLOAT,				/**< the property is a float */
				STRING,				/**< the property is a string */
				DOUBLE,				/**< the property is a double */
				VEC2F,				/**< the property is a osg::Vec2 */
				VEC3F,				/**< the property is a osg::Vec3 */
				VEC4F,				/**< the property is a osg::Vec4 */
                VEC2D,				/**< the property is a osg::Vec2 */
				VEC3D,				/**< the property is a osg::Vec3 */
				VEC4D,				/**< the property is a osg::Vec4 */
				MATRIXF,				/**< the property is a matrix */
                MATRIXD,
				QUAT,				/**< the property is a Quaternion */
				PROPERTY_LIST,		/**< the property is a property-list */
				MULTIPLE_VALUE_LIST,
				OBJECT				/**< the property is an object */
			};
			
			/**
			 * Constructor
			 */
			AbstractProperty() : osg::Referenced(), _xml_type(""), _parent(NULL) { setType(UNDEFINED); }
			/** d'tor */
			virtual ~AbstractProperty() {}
			
			/**
			 * Constructor
			 * @param type the type of this property
			 */
			AbstractProperty(PropertyType type) : osg::Referenced(), _xml_type(""), _parent(NULL) { setType(type); }
			
			/**
			 * @return returns the type of this property
			 */
			inline const PropertyType &getType() const{ return _type; }
			
			inline bool asBool() { return (asInt() != 0); }
			/**
			 * @return returns the property as integer
			 */
			virtual int asInt() { return 0;}
			
			/**
			 * @return returns the property as flaot
			 */
			virtual float asFloat() { return 0.0f; }
			
			/**
			 * @return returns the property as string
			 */
			virtual std::string asString() { return ""; }
			
			/**
			 * @return returns the property as string
			 */
			virtual std::wstring asWstring() { return L""; }
			
			/**
			 * @return returns the property as double
			 */
			virtual double asDouble() { return 0.0; }
            
            inline osg::Vec2 asVec2() { return asVec2f(); }
            inline osg::Vec3 asVec3() { return asVec3f(); }
            inline osg::Vec4 asVec4() { return asVec4f(); }
            inline osg::Matrix asMatrix() { return asMatrixf(); }
			
			virtual osg::Vec2f asVec2f() { return osg::Vec2f(0,0); }
			virtual osg::Vec3f asVec3f() { return osg::Vec3f(0,0,0); }
			virtual osg::Vec4f asVec4f() { return osg::Vec4f(0,0,0,0); }
			virtual osg::Vec2d asVec2d() { return osg::Vec2d(0,0); }
			virtual osg::Vec3d asVec3d() { return osg::Vec3d(0,0,0); }
			virtual osg::Vec4d asVec4d() { return osg::Vec4d(0,0,0,0); }
			
			virtual  osg::Quat asQuat() { return osg::Quat(); }
			
			virtual osg::Matrixf asMatrixf() { return osg::Matrixf::identity(); }
			virtual osg::Matrixd asMatrixd() { return osg::Matrixd::identity(); }
			
            
			/**
			 * @return returns the property as object
			 */
			virtual osg::Referenced* asObject() { return NULL;}
            
            PropertyList* asPropertyList();            
            
            MultipleValueList* asMultipleValueList();
            
            MultipleValueList* convertToMultipleValueList();
            			
			/** creates an xml-node representing this property and adds it to the root-element */
			virtual void addAsXml(TiXmlElement* root, std::string keyname) {}
			
			/** sets the optional XML-type */
			void setXmlType(std::string type) { _xml_type = type; }
			
			/** @return the optional XML-type */
			std::string getXmlType() const { return _xml_type; }
            
            /** sets the xml-id */
            void setXmlId(std::string ID) { _xml_id = ID; }
            
            /** @return the xml-id of this node (if available) */
            std::string getXmlId() const { return _xml_id; }
			
			PropertyList* getParent() const { return _parent; }
			
			std::string getKey();
		
		protected:

			/**
			 * sets the type of this property
			 * @param type the new type of this property
			 */
			void setType(PropertyType type);
            
            /**
             * adds the xml-id to the xmlelement e
             */
            inline void addXmlId(TiXmlElement* e) {
                if (!_xml_id.empty()) {
                    e->SetAttribute("id", _xml_id);
                }
            }
            
            inline void setVectorIndex(unsigned int i) { _vector_index = i; }
            inline unsigned int getVectorIndex() { return _vector_index; }
			
			void setParent(PropertyList* pl) { _parent = pl; }
			
			/**
			 * the type of the property
			 */
			PropertyType _type;
			std::string _xml_type;
            std::string _xml_id;
            unsigned int _vector_index;
			PropertyList* _parent;
			
		friend class PropertyList;
	};
    
    
    typedef std::vector<osg::ref_ptr<AbstractProperty> > FoundPropertiesVector;
	
    /**
     * this class encapsulates a vector of values with the same key.
     * it gets automagically populated by the property-list-class.
     */
	class CEFIX_EXPORT MultipleValueList : public AbstractProperty {
		public:
            typedef std::vector< osg::ref_ptr<AbstractProperty> > List;
            typedef List::iterator iterator;
            typedef List::const_iterator const_iterator;
            /** constructs a Property of type MULTIPLE_VALUE_LIST */
			MultipleValueList() : AbstractProperty(MULTIPLE_VALUE_LIST) {}
			
            /** add a property to the vector */
            inline void add(AbstractProperty* p) { 
                _list.push_back(p);
			}
			
            /** @return the nr. of items of this vector */
			inline unsigned int count() { return _list.size(); }
			
            /** removes a property from the vector */
			inline void remove(AbstractProperty* p) {
                List::iterator itr = std::find(_list.begin(), _list.end(), p);
                if (itr != _list.end())
                    _list.erase(itr);
			}
			
            
            /** @return item at position i */
			inline AbstractProperty* operator[](unsigned int i) { return get(i); }
                        
            /** @return item at position i */
			AbstractProperty* get(unsigned int i);
            
            /** @ return a reference to the map of stored properties */
            List &list() { return _list; }
			
			const List &list() const { return _list; }
            
			
            /** @return the vecotr */
			osg::Referenced* asObject() { return this; }
			virtual void addAsXml(TiXmlElement* root, std::string keyname);
						
			
		protected:
			virtual ~MultipleValueList() {}
            
            /// the vector of items
            List _list;
	};
			
	
	/**
	 * basic template class for the intrisinic classes, which are not derived from osg::Referenced
	 */
	template<class T> class AbstractPropertyT : public AbstractProperty {
		private:
			AbstractPropertyT() : AbstractProperty() {}
		public:
			AbstractPropertyT(PropertyType type) : AbstractProperty(type) {}
			AbstractPropertyT(PropertyType type, T data) : AbstractProperty(type), _data(data) {}
			/**
			 * @return returns the data for this property
			 */
			T &data() { return _data; }
			
			/** sets the data of this property */
			void setData(const T& data) { _data = data; }
			
		protected:
			virtual ~AbstractPropertyT() {}
			T _data;
	};
	
	/**
	 * template class for the intrisinic classes, which are not derived from osg::Referenced
	 */
	template<class T> class  PropertyT : public AbstractPropertyT<T> {
		private:
			PropertyT() : AbstractPropertyT<T>() {}
		public:
			PropertyT(AbstractProperty::PropertyType ttype, T data) : AbstractPropertyT<T>(ttype,data) {};
		protected:
			virtual ~PropertyT() {}
	};
	
	/**
	 * Property-class for properties derived from osg::Referenced
	 */
	template<> class CEFIX_EXPORT  PropertyT<osg::Referenced*> : public AbstractPropertyT<osg::Referenced*> {
		private:
			/**
			 * refcounting reference to the data
			 */
			osg::ref_ptr<osg::Referenced>  _data;
		
		public:
			/**
			 * Constructor without type, defaults to AbstractProperty::OBJECT
			 * @param data obj to attach to this property
			 */
			PropertyT(osg::Referenced* data) : AbstractPropertyT<osg::Referenced*>(AbstractProperty::OBJECT) { _data = data; }
			/**
			 * Constructor with type, defaults to AbstractProperty::OBJECT
			 * @param type type of this property, be careful, should only AbstractProperty::OBJECT or AbstractProperty::PROPERTY_LIST
			 * @param data obj to attach to this property
			 */
			PropertyT(PropertyType type, osg::Referenced* data) : AbstractPropertyT<osg::Referenced*>(type) { _data = data; }
			
			/**
			 * @return returns the data of this property
			 */
			osg::Referenced* data() { return _data.get(); }
			
			/** sets the data of this property */
			void setData(osg::Referenced* data) { _data = data; }
			
			virtual osg::Referenced* asObject() { return _data.get(); }
			virtual void addAsXml(TiXmlElement* root, std::string keyname);
		protected:
			virtual ~PropertyT() {}
	};
	
	/**
	 * specialised integer-property-class
	 */
	template<> class CEFIX_EXPORT  PropertyT<int> : public AbstractPropertyT<int> {
		public:
			PropertyT(int data) : AbstractPropertyT<int>(AbstractProperty::INTEGER, data) {}
			virtual int asInt() { return _data; }
			virtual float asFloat() { return (float) _data; }
			virtual std::string asString() { std::ostringstream ss; ss << _data; return ss.str(); }
			virtual double asDouble() { return (double) _data; }
			virtual void addAsXml(TiXmlElement* root, std::string keyname);
		protected:
			virtual ~PropertyT() {}
			
	};
	
	/**
	 * specialised integer-property-class
	 */
	template<> class CEFIX_EXPORT  PropertyT<unsigned int> : public AbstractPropertyT<unsigned int> {
		public:
			PropertyT(unsigned int data) : AbstractPropertyT<unsigned int>(AbstractProperty::INTEGER, data) {}
			virtual int asInt() { return _data; }
			virtual float asFloat() { return (float) _data; }
			virtual std::string asString() { std::ostringstream ss; ss << _data; return ss.str(); }
			virtual double asDouble() { return (double) _data; }
			virtual void addAsXml(TiXmlElement* root, std::string keyname);
		protected:
			virtual ~PropertyT() {}
			
	};
	
	/**
	 * specialised integer-property-class
	 */
	template<> class CEFIX_EXPORT  PropertyT<bool> : public AbstractPropertyT<int> {
		public:
			PropertyT(bool data) : AbstractPropertyT<int>(AbstractProperty::BOOLEAN, data) { setXmlType("boolean"); }
			virtual int asInt() { return _data; }
			virtual float asFloat() { return (float) _data; }
			virtual std::string asString() { std::ostringstream ss; ss << _data; return ss.str(); }
			virtual double asDouble() { return (double) _data; }
			virtual void addAsXml(TiXmlElement* root, std::string keyname);
		protected:
			virtual ~PropertyT() {}
			
	};
	
	/**
	 * specialised float-property-class
	 */
	template<> class CEFIX_EXPORT  PropertyT<float> : public AbstractPropertyT<float> {
		public:
			PropertyT(float data) : AbstractPropertyT<float>(AbstractProperty::FLOAT, data) {}
			virtual float asFloat() { return _data; }
			virtual int asInt() { return (int)_data; }
			virtual std::string asString() { std::ostringstream ss; ss << _data; return ss.str(); }
			virtual double asDouble() { return (double) _data; }
			virtual void addAsXml(TiXmlElement* root, std::string keyname);
		protected:
			virtual ~PropertyT() {}
	};
	
	// #### von Ralph, also Vorsicht!
	/**
	 * specialised bool-property-class
	 */
	 /*
	template<> class  PropertyT<bool> : public AbstractPropertyT<bool> {
		public:
			PropertyT(bool data) : AbstractPropertyT<bool>(AbstractProperty::BOOLEAN, data) {}
			virtual float asFloat() { return _data ? 1.0f:0.0f; }
			virtual int asInt() { return _data ? 1:0; }
			virtual std::string asString() { std::stringstream ss; _data ? (ss << "true"):(ss << "false"); return ss.str(); }
			virtual double asDouble() { return _data ? 1.0:0.0; }
			virtual void addAsXml(TiXmlElement* root, std::string keyname);
	};
	*/
	
	/**
	 * specialised string-property-class
	 */
	template<> class CEFIX_EXPORT  PropertyT<std::string> : public AbstractPropertyT<std::string> {
		public:
			PropertyT(std::string data) : AbstractPropertyT<std::string>(AbstractProperty::STRING, data), _cached(L"") {}
			virtual float asFloat() { return (float)atof(_data.c_str()); }
			virtual int asInt() { return (int)atoi(_data.c_str()); }
			virtual std::string asString() {return _data; }
			virtual std::wstring asWstring() {
				if (_cached.empty()) {
					Unicode::fromUTF8toUCS4(_data, _cached);
				}
				return _cached;
			}
			virtual double asDouble() { return (double) atof(_data.c_str()); }
			virtual void addAsXml(TiXmlElement* root, std::string keyname);
		protected:
			virtual ~PropertyT() {}

			std::wstring _cached;
	};
	

	
	/**
	 * specialised double-property-class
	 */
	template<> class CEFIX_EXPORT  PropertyT<double> : public AbstractPropertyT<double> {
		public:
			PropertyT(double data) : AbstractPropertyT<double>(AbstractProperty::DOUBLE, data) {}
			virtual float asFloat() { return (float)_data; }
			virtual int asInt() { return (int)_data; }
			virtual std::string asString() { std::ostringstream ss; ss << _data; return ss.str(); }
			virtual double asDouble() { return _data; }
			virtual void addAsXml(TiXmlElement* root, std::string keyname);
		protected:
			virtual ~PropertyT() {}
	};
	
	/**
	 * specialised osg::Vec2f-property-class
	 */
	template<> class CEFIX_EXPORT  PropertyT<osg::Vec2d> : public AbstractPropertyT<osg::Vec2d> {
		public:
			PropertyT(osg::Vec2d data) : AbstractPropertyT<osg::Vec2d>(AbstractProperty::VEC2D, data) {}
			virtual osg::Vec2d asVec2d() { return _data; }
			virtual osg::Vec3d asVec3d() { return osg::Vec3d(_data[0], _data[1], 0); }
			virtual osg::Vec4d asVec4d() { return osg::Vec4d(_data[0], _data[1], 0, 0); }
            virtual osg::Vec2f asVec2f() { return _data; }
			virtual osg::Vec3f asVec3f() { return osg::Vec3f(_data[0], _data[1], 0); }
			virtual osg::Vec4f asVec4f() { return osg::Vec4f(_data[0], _data[1], 0, 0); }
			virtual void addAsXml(TiXmlElement* root, std::string keyname);
		protected:
			virtual ~PropertyT() {}
	};
    
    /**
	 * specialised osg::Vec2f-property-class
	 */
	template<> class CEFIX_EXPORT  PropertyT<osg::Vec2f> : public AbstractPropertyT<osg::Vec2f> {
		public:
			PropertyT(osg::Vec2f data) : AbstractPropertyT<osg::Vec2f>(AbstractProperty::VEC2F, data) {}
			virtual osg::Vec2d asVec2d() { return _data; }
			virtual osg::Vec3d asVec3d() { return osg::Vec3d(_data[0], _data[1], 0); }
			virtual osg::Vec4d asVec4d() { return osg::Vec4d(_data[0], _data[1], 0, 0); }
            virtual osg::Vec2f asVec2f() { return _data; }
			virtual osg::Vec3f asVec3f() { return osg::Vec3f(_data[0], _data[1], 0); }
			virtual osg::Vec4f asVec4f() { return osg::Vec4f(_data[0], _data[1], 0, 0); }
			virtual void addAsXml(TiXmlElement* root, std::string keyname);
		protected:
			virtual ~PropertyT() {}
	};
	
	/**
	 * specialised osg::Vec3-property-class
	 */
	template<> class CEFIX_EXPORT  PropertyT<osg::Vec3d> : public AbstractPropertyT<osg::Vec3d> {
		public:
			PropertyT(osg::Vec3d data) : AbstractPropertyT<osg::Vec3d>(AbstractProperty::VEC3D, data) {}
			virtual osg::Vec2d asVec2d() { return osg::Vec2d(_data[0], _data[1]); }
			virtual osg::Vec3d asVec3d() { return _data; }
			virtual osg::Vec4d asVec4d() { return osg::Vec4d(_data[0], _data[1], _data[2], 0); }
            virtual osg::Vec2f asVec2f() { return osg::Vec2f(_data[0], _data[1]); }
			virtual osg::Vec3f asVec3f() { return _data; }
			virtual osg::Vec4f asVec4f() { return osg::Vec4f(_data[0], _data[1], _data[2], 0); }
            
			virtual void addAsXml(TiXmlElement* root, std::string keyname);
		protected:
			virtual ~PropertyT() {}
	};
    
    /**
	 * specialised osg::Vec3-property-class
	 */
	template<> class CEFIX_EXPORT  PropertyT<osg::Vec3f> : public AbstractPropertyT<osg::Vec3f> {
		public:
			PropertyT(osg::Vec3f data) : AbstractPropertyT<osg::Vec3f>(AbstractProperty::VEC3F, data) {}
			virtual osg::Vec2d asVec2d() { return osg::Vec2d(_data[0], _data[1]); }
			virtual osg::Vec3d asVec3d() { return _data; }
			virtual osg::Vec4d asVec4d() { return osg::Vec4d(_data[0], _data[1], _data[2], 0); }
            virtual osg::Vec2f asVec2f() { return osg::Vec2f(_data[0], _data[1]); }
			virtual osg::Vec3f asVec3f() { return _data; }
			virtual osg::Vec4f asVec4f() { return osg::Vec4f(_data[0], _data[1], _data[2], 0); }
            
			virtual void addAsXml(TiXmlElement* root, std::string keyname);
		protected:
			virtual ~PropertyT() {}
	};
	
	/**
	 * specialised osg::Vec4-property-class
	 */
	template<> class CEFIX_EXPORT  PropertyT<osg::Vec4d> : public AbstractPropertyT<osg::Vec4d> {
		public:
			PropertyT(osg::Vec4d data) : AbstractPropertyT<osg::Vec4d>(AbstractProperty::VEC4D, data) {}
            
			virtual osg::Vec2d asVec2d() { return osg::Vec2(_data[0], _data[1]); }
			virtual osg::Vec3d asVec3d() { return osg::Vec3(_data[0], _data[1], _data[2]); }
			virtual osg::Vec4d asVec4d() { return _data; }
            virtual osg::Vec2f asVec2f() { return osg::Vec2(_data[0], _data[1]); }
			virtual osg::Vec3f asVec3f() { return osg::Vec3(_data[0], _data[1], _data[2]); }
			virtual osg::Vec4f asVec4f() { return _data; }
            
			virtual void addAsXml(TiXmlElement* root, std::string keyname);
		protected:
			virtual ~PropertyT() {}
	};

        /**
	 * specialised osg::Vec4-property-class
	 */
	template<> class CEFIX_EXPORT  PropertyT<osg::Vec4f> : public AbstractPropertyT<osg::Vec4f> {
		public:
			PropertyT(const osg::Vec4f& data) : AbstractPropertyT<osg::Vec4f>(AbstractProperty::VEC4F, data) {}
            
			virtual osg::Vec2d asVec2d() { return osg::Vec2(_data[0], _data[1]); }
			virtual osg::Vec3d asVec3d() { return osg::Vec3(_data[0], _data[1], _data[2]); }
			virtual osg::Vec4d asVec4d() { return _data; }
            virtual osg::Vec2f asVec2f() { return osg::Vec2(_data[0], _data[1]); }
			virtual osg::Vec3f asVec3f() { return osg::Vec3(_data[0], _data[1], _data[2]); }
			virtual osg::Vec4f asVec4f() { return _data; }
            
			virtual void addAsXml(TiXmlElement* root, std::string keyname);
		protected:
			virtual ~PropertyT() {}
	};

	/**
	 * specialised osg::Quaternion-property-class
	 */
	template<> class CEFIX_EXPORT  PropertyT<osg::Quat> : public AbstractPropertyT<osg::Quat> {
		public:
			PropertyT(osg::Quat data) : AbstractPropertyT<osg::Quat>(AbstractProperty::QUAT, data) {}
			virtual osg::Quat asQuat() { return _data; }
			virtual osg::Matrix asMatrix() { return osg::Matrix(_data); }
			virtual void addAsXml(TiXmlElement* root, std::string keyname);
		protected:
			virtual ~PropertyT() {}
	};
	
	/**
	 * specialised osg::Matrix-property-class
	 */
	template<> class CEFIX_EXPORT  PropertyT<osg::Matrixf> : public AbstractPropertyT<osg::Matrixf> {
		public:
			PropertyT(const osg::Matrixf& data) : AbstractPropertyT<osg::Matrixf>(AbstractProperty::MATRIXF, data) {}
			virtual osg::Quat asQuat() { osg::Quat q; q.get(_data); return q; }
			virtual osg::Matrixf asMatrixf() { return _data; }
            virtual osg::Matrixd asMatrixd() { return _data; }
			virtual void addAsXml(TiXmlElement* root, std::string keyname);
		protected:
			virtual ~PropertyT() {}
	};
    
    /**
	 * specialised osg::Matrix-property-class
	 */
	template<> class CEFIX_EXPORT  PropertyT<osg::Matrixd> : public AbstractPropertyT<osg::Matrixd> {
		public:
			PropertyT(const osg::Matrixd& data) : AbstractPropertyT<osg::Matrixd>(AbstractProperty::MATRIXD, data) {}
			virtual osg::Quat asQuat() { osg::Quat q; q.get(_data); return q; }
			virtual osg::Matrixf asMatrixf() { return _data; }
            virtual osg::Matrixd asMatrixd() { return _data; }
			virtual void addAsXml(TiXmlElement* root, std::string keyname);
		protected:
			virtual ~PropertyT() {}
	};
	
			
	/**
	 * this class emulates a property list, which consists of a sorted list of keys -> values
	 * you can add different types and objects to this list transparently
	 */
	class CEFIX_EXPORT PropertyList : public osg::Referenced {
	
		public:
		
			/** small errorHandler-class, which gets called from the propertylist, if any errors occurs
			 *  to use your own errorhandling subclass this class and call PropertyList::instance(yourNewHandlingClassInstance)
			 */
			class CEFIX_EXPORT ErrorHandler : public osg::Referenced {
			
				public:
					static ErrorHandler* instance(ErrorHandler* newhandler = NULL);
					
					virtual void handle(std::string msg) {
						log::error("Propertylist") << msg << std::endl;
					}
			};
			
			/** isInclusionAllowedCallback is a static callback, which returns true, if the inclusion of a specific file is allowed */
			class CEFIX_EXPORT IsInclusionAllowedCallback : public osg::Referenced {
			
				public:
					static IsInclusionAllowedCallback* instance(IsInclusionAllowedCallback* newCallback = NULL);
					
					virtual bool allowFile(std::string xmlfilepath) {
						return true; // default is yes to all
					}
			};
			
			/** ReadXlFileCallback is a callback which gets called, when a xml has beend loaded. */
			class CEFIX_EXPORT ReadXmlFileCallback : public osg::Referenced {
				public:
					ReadXmlFileCallback() : osg::Referenced() {}
					
					virtual void beforeLoad(const std::string&) {};
					virtual void afterLoad(TiXmlDocument& doc) {};
			};
			
			/**
			 * Constructor - creates an empty list
			 */
			PropertyList() : osg::Referenced(), _multipleValuesAllowed(true), _parent(NULL) {}
			
			/**
			 * Constructor -- creates a Propertylist out of an xml node
			 */
			PropertyList(TiXmlElement* node, std::string partOfFile = "", ReadXmlFileCallback* cb = NULL ) : osg::Referenced(), _multipleValuesAllowed(true), _xmlFileName(partOfFile), _readXmlFileCallback(cb),_parent(NULL) { addFromXmlElement(node); }
			/**
			 * Constructor -- creates a Propertylist out of an xml file
			 */
			PropertyList(const std::string& xmlfilename, ReadXmlFileCallback* cb = NULL);
			
			/**
			 * loads an xml file into the propertylist
			 * @param xmlfilename the filename of the xml-file
			 * @return true, if succeded
			 */
			bool loadFromXML(const std::string& xmlfilename);
			
			/**
			 * loads a json file into the propertylist
			 * @param jsonfilename the filename of the json-file
			 * @return true, if succeded
			 */
			bool loadFromJSON(const std::string& jsonfilename);
			
			/**
			 * adds an property 
			 * @param key key of the prop
			 * @param p the property to add 
			 */
			AbstractProperty* addValue(const std::string &key, AbstractProperty* p);
			
			/**
			 * adds an value
			 */
			template <typename T>
            AbstractProperty* add(const std::string& key, const T& value) {
				return addValue(key, new PropertyT<T>(value));
			}
            
            AbstractProperty* add(const std::string& key, const char* value) {
				return addValue(key, new PropertyT<std::string>(value));
			}
            
            
			
			/**
			 * adds an object / property-list
			 * @param value an object based on osg::Referenced
			 */
			AbstractProperty* add(const std::string& key, osg::Referenced* value) 
			{
				if (dynamic_cast<AbstractProperty*>(value)) {
					AbstractProperty* p = dynamic_cast<AbstractProperty*>(value);
					addValue(key, p);
					return p;
				}
				PropertyList* list = dynamic_cast<PropertyList*>(value);
				if (list)
					if (list != this)
						return addValue(key, new PropertyT<osg::Referenced*>(AbstractProperty::PROPERTY_LIST, list));
					else {
						log::error("PropertyList::add") << "could not add propertylist! parent = child!" << std::endl;
						return get(key);
					}
				else
					return addValue(key, new PropertyT<osg::Referenced*>(value));
			}
			
			
			AbstractProperty* add(const std::string& key, cefix::PropertyList* value) {
				return add(key, static_cast<osg::Referenced*>(value));
			}
			
			/**
			 * @param key key to look for
			 * @return returns true, if the property-list contains an item with the specified key
			 */
			inline bool hasKey(const std::string& key) {
				return (_list.find(key) != _list.end());
			}
			
			/**
			 * get returns a property for the specified key. If the key cannot be found a special property of type UNDEFINED is returned
			 * @param key the key to loop for
			 * @return the found property, if not found the undefined-property
			 */
			inline AbstractProperty* get(const std::string& key) {
				AbstractProperty* p = getProperty(key);
				return (p) ? p : getUndefined();
			}
			
			/** sets a value of an existing property with a specified key. If the property does not exists it will be added. A property of different type will be replaces 
			 *  @key the key of the property to set
			 *  @value the new value for the property with key 
			 */
			template<typename T> AbstractProperty* set(const std::string& key, const T& value) {
				
				PropertyT<T>* prop = dynamic_cast< PropertyT<T>* >(getProperty(key));
				if (prop) {
					prop->setData(value);
					return prop;
				} else {
					if (hasKey(key)) {
						return replaceValue(key, new PropertyT<T>(value));
					} else {
						return add(key, value);
					}
				}
			}
			
			AbstractProperty* set(const std::string& key, const char* value) {
				return set<std::string>(key, value);
			}
			
			/** removes property with key key from the list */
			void remove(const std::string& key) {
				AbstractProperty* p = getProperty(key);
				if ((p == NULL) ||( p->getType() == cefix::AbstractProperty::UNDEFINED))
					return;
				unsigned int ndx = p->getVectorIndex();
				
				// Die Vector-Indizes anpassen
				for(unsigned int i = ndx +1; i < _vector.size(); ++i) {
					_vector[i]->setVectorIndex(_vector[i]->getVectorIndex()-1);
				}
				_vector.erase(_vector.begin() + ndx);
				_list.erase(_list.find(key));
			}
            
            /**
             * find tries to find a property at the specified path
             * you can address hierarchical properties with
             * /proplist_1/sub_proplist_2/sub_proplist_3/property_key
             * @param path path to the property
             * @return NULL if the path cannot be resolved, key cannot be found
             */
            AbstractProperty* find(const std::string& path);
        
            /** 
             * same as find, will return an undefined property, when nothing is found, so findSafe("xxx")->asString() will work,
             * even when xxx is not found
             */
            AbstractProperty* findSafe(const std::string& path) { AbstractProperty* result = find(path); return result ? result : getUndefined(); }
			
            /**
             * findAll tries to find all childs of the given path and add them to a vector
             * you can address hierarchical properties with
             * /proplist_1/sub_proplist_2/sub_proplist_3/property_key
             * @param path path to the property
             * @param results the vector to add the results;
             */
            void findAll(const std::string& path, FoundPropertiesVector& results);

            
			/**
			 * returns a property for the specified key. If the key cannot be found a special property of type UNDEFINED is returned
			 * @param key the key to loop for
			 * @return the found property, if not found the undefined-property
			 */
			AbstractProperty*operator[](const std::string& key) { return get(key); }
			
			
			/**
			 * adds xml-elements as properties to this list
			 * @param node xml-node from which to add all subnodes
			 */
			void addFromXmlElement(TiXmlElement* node);

            /**
             * saves the property-list into a xml-file
             * @param filename the filename to write into
             * @param rootname the name of the root-tag
             */
			void saveAsXML(const std::string& filename, const std::string& rootname="propertylist");
			
			/**
             * saves the property-list into a json-file
             * @param filename the filename to write into
             * @param rootname the name of the root-tag
             */
			void saveAsJSON(const std::string& filename);
            
			/**
             * adds a propertylist as to an existing xml-node
             */
            void addToXmlNode(TiXmlElement* root);
			
			/**
			 * dumps the property list to std::out
			 */
			void dump(int level=0);
			
			/** sets the multipleValuesAllowedFlag (defaults to true) */
			inline void setMultipleValuesAllowedFlag(bool f) { _multipleValuesAllowed = f; }
			inline bool getMultipleValuesAllowedFlag() const { return _multipleValuesAllowed;}
            
            /**
             * get returns a property by its numerical index
             */
            inline AbstractProperty* get(unsigned int ndx) const { 
                if (ndx < _vector.size())
                    return _vector[ndx].get();
                else
                    return getUndefined();
            }
            
            /**
             * [] returns a property by its numerical index
             */
            inline AbstractProperty* operator[](unsigned int ndx) {
                return get(ndx);
            }
            
            /** @return the name of the key for the property at positon ndx */
            inline std::string getKeyAt(unsigned int ndx) const {
                if (ndx < _vector.size())
                    return _vector[ndx].key();
                else
                    return "";
            }
            
            /** @return the size of this propertylist */
            inline unsigned int size() const {
                return _vector.size();
            }
			
			/**
             * small helper class to encapsulate the key of a property
             */
            template<class T>class key_with_ref_ptr : public osg::ref_ptr<T> {
                public:
                    /** ctro */
                    key_with_ref_ptr() : osg::ref_ptr<T>(NULL), _key("") {}
                    /**ctor */
                    key_with_ref_ptr(const key_with_ref_ptr& rp) : osg::ref_ptr<T>(rp), _key(rp._key) {}
                    /** constructor */
                    key_with_ref_ptr(T* t) : osg::ref_ptr<T>(t), _key("") {}
                    /** sets the key*/
                    void setKey(std::string key) { _key = key; }
                    /** @return key of this property*/
                    std::string key() const { return _key; }
                protected:
                    std::string _key;
            };
			
			/** @return the data of the Abstractproperty at key, or the default value, if the property could not be found */
			template <typename T> T get(std::string key, T defaultval) {
				PropertyT<T>* p = dynamic_cast< PropertyT<T>* >(get(key));
				if (p != NULL)
					return p->data();
				else
					return defaultval;
			}
			
			/** @return the filename of the xmlfile, from which this propertylist was built */
			const std::string& getXmlFileName() const { return _xmlFileName; }
			
			/** @return the filepath to the xmlfile, from which this propertylist was built */
			std::string getXmlFilePath() const { return osgDB::getFilePath(_xmlFileName); }
			
			/** adds all key-value-pairs of propertylist pl that are not present in this propertylist */
			void addNonExistingKeys(cefix::PropertyList* pl);
			
			void setReadXmlFileCallback(ReadXmlFileCallback* cb ) { _readXmlFileCallback = cb; }
			
			ReadXmlFileCallback* getReadXmlFileCallback() const { return _readXmlFileCallback.get(); }
			
			PropertyList* getParent() const { return _parent; }
			
			/** 
             * properties are stored also in a vector
             */
            typedef std::vector<key_with_ref_ptr<AbstractProperty> > PropertyVectorT;
			typedef PropertyVectorT::const_iterator const_iterator ;
			typedef const_iterator iterator;
			
			const_iterator begin() const {
				return _vector.begin();
			}
			
			const_iterator end() const {
				return _vector.end();
			}
		
		protected:
			AbstractProperty* replaceValue(const std::string& key, AbstractProperty* property);
			virtual ~PropertyList() {}
        
            struct ltstr
            {
              bool operator()(std::string s1, std::string s2) const
              {
				  return cefix::stricmp(s1.c_str(), s2.c_str()) < 0;
              }
            };
                    
			/**
			 * map of key -> refcounted AbstractProperties
			 */
			typedef std::map<std::string, osg::ref_ptr<AbstractProperty>, ltstr > PropertyListT;
            
            
            
            /**
			 * gets a property from a key
			 * @param key key to retrieve
			 * @return returns the property-object, NULL if not found
			 */
			AbstractProperty* getProperty(const std::string &key) { PropertyListT::iterator itr = _list.find(key); return (itr != _list.end()) ? (itr->second.get()) : NULL; }
			
            /**
			 * returns the value out of an AbstractProperty, does only work with non-object-types
			 * @param ap the property to look inside
			 * @return the value encapsulated in the property
			 */
			template <class T> T &getValue(AbstractProperty* ap) {
				PropertyT<T>* p = dynamic_cast< PropertyT<T>* >(ap);
				if (p != NULL)
					return p->data();
			}
			/**
			 * returns the special undefined property
			 */
			static AbstractProperty* getUndefined();
            
            AbstractProperty* find(std::vector<std::string> &tokens, unsigned int tokenNdx = 0);
			
			void setParent(PropertyList* pl ) { _parent = pl; }
			
			            
			/**
			 * the list...
			 */
			PropertyListT       _list;
            PropertyVectorT     _vector;
			bool                _multipleValuesAllowed;
            std::string         _xmlFileName;
			osg::ref_ptr<ReadXmlFileCallback> _readXmlFileCallback;
			PropertyList*		_parent;
            
        friend class MultipleValueList;
		friend class JSON;
	
	};


std::ostream& operator<<(std::ostream& os, const PropertyList& pl);
std::ostream& operator<<(std::ostream& os, AbstractProperty& property);

}

#endif
