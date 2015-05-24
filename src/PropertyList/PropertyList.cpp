/*
 *  PropertyList.cpp
 *  CommonTools
 *
 *  Created by Stephan Huber on 16.04.06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include <iomanip> 
#include <cefix/PropertyList.h>
#include <osg/io_utils>
#include <osgDB/FileUtils>
#include <osgDB/FileNameUtils>
#include <cefix/XMLWrapper.h>
#include <cefix/StringUtils.h>
#include <cefix/FileUtils.h>
#include <cefix/JSON.h>


namespace cefix {


	void AbstractProperty::setType(PropertyType type)
	{
		_type = type; 
		if(_xml_type.empty()) 
		{
			std::string xml_type("");
			switch(_type) {
				case AbstractProperty::UNDEFINED:	
					xml_type = "undefined";
					break;
                    
				case AbstractProperty::BOOLEAN:
					xml_type = "boolean";
					break;
                    
				case AbstractProperty::INTEGER:
					xml_type = "integer";
					break;
				case AbstractProperty::FLOAT:
					xml_type = "float";
					break;
				case AbstractProperty::DOUBLE:
					xml_type = "double";
					break;
					
				case AbstractProperty::STRING:
					xml_type = "string";
					break;
					
				case AbstractProperty::VEC2F:
					xml_type = "vec2f";
					break;

				case AbstractProperty::VEC2D:
					xml_type = "vec2d";
					break;

					
				case AbstractProperty::VEC3F:
					xml_type = "vec3f";
					break;

				case AbstractProperty::VEC3D:
					xml_type = "vec3d";
					break;
					
				case AbstractProperty::VEC4F:
					xml_type = "vec4f";
					break;

				case AbstractProperty::VEC4D:
					xml_type = "vec4d";
					break;
				
				case AbstractProperty::MATRIXF:
					xml_type = "matrixf";
					break;

				case AbstractProperty::MATRIXD:
					xml_type = "matrixd";
					break;
					
				case AbstractProperty::QUAT:
					xml_type = "quat";
					break;

				case AbstractProperty::PROPERTY_LIST:
					xml_type = "property_list";
					break;

				case AbstractProperty::MULTIPLE_VALUE_LIST:
					xml_type = "array";
					break;

				case AbstractProperty::OBJECT:
					xml_type = "object";
					break;
			}
			setXmlType(xml_type);
		}
	}
	
	std::string AbstractProperty::getKey() { return (_parent) ? _parent->getKeyAt(_vector_index) : ""; }

    PropertyList* AbstractProperty::asPropertyList() {
        osg::Referenced* o = asObject();
        if ((getType() == PROPERTY_LIST) && o) {
            return dynamic_cast<PropertyList*>(o);
        }
        return NULL;
    }
            
            
    MultipleValueList* AbstractProperty::asMultipleValueList() {
        if (getType() == MULTIPLE_VALUE_LIST) {
            return dynamic_cast<MultipleValueList*>(this);
        }
        return NULL;
    }
    
    
    MultipleValueList* AbstractProperty::convertToMultipleValueList() {
        if (getType() == MULTIPLE_VALUE_LIST) {
            return dynamic_cast<MultipleValueList*>(this);
        }
        MultipleValueList* mvl = new MultipleValueList();
        mvl->add(this);
        return mvl;
    }
        
	void PropertyT<bool>::addAsXml(TiXmlElement* root, std::string key) {
		TiXmlElement* e = new TiXmlElement(key); 
		std::string type = (getXmlType().empty()) ? "boolean" : getXmlType();
		e->SetAttribute("type", type);
		if (_data !=0)
			e->SetAttribute("value", "true");
		else
			e->SetAttribute("value", "false");
		addXmlId(e);
		root->LinkEndChild(e);
	}


	void PropertyT<int>::addAsXml(TiXmlElement* root, std::string key) {
		TiXmlElement* e = new TiXmlElement(key); 
		std::string type = (getXmlType().empty()) ? "integer" : getXmlType();
		e->SetAttribute("type", type);
		if (type == "boolean") {
			if (_data !=0)
				e->SetAttribute("value", "true");
			else
				e->SetAttribute("value", "false");
		}
		else
			e->SetAttribute("value", asString());
        addXmlId(e);
		root->LinkEndChild(e);
	}
	
	void PropertyT<unsigned int>::addAsXml(TiXmlElement* root, std::string key) {
		TiXmlElement* e = new TiXmlElement(key); 
		std::string type = (getXmlType().empty()) ? "integer" : getXmlType();
		e->SetAttribute("type", type);
		if (type == "boolean") {
			if (_data !=0)
				e->SetAttribute("value", "true");
			else
				e->SetAttribute("value", "false");
		}
		else
			e->SetAttribute("value", asString());
        addXmlId(e);
		root->LinkEndChild(e);
	}
	
    
	void PropertyT<float>::addAsXml(TiXmlElement* root, std::string key) {
		TiXmlElement* e = new TiXmlElement(key); 
		std::string type = (getXmlType().empty()) ? "float" : getXmlType();
		e->SetAttribute("type", type);
		e->SetAttribute("value", asString());
		addXmlId(e);
        		
		root->LinkEndChild(e);
	}
	
    
	void PropertyT<std::string>::addAsXml(TiXmlElement* root, std::string key) {
	
		TiXmlElement* e = new TiXmlElement(key); 
		std::string type = (getXmlType().empty()) ? "string" : getXmlType();
        if ((_data.length() < 160) && (_data.find(13) == std::string::npos) && (_data.find(10) == std::string::npos)) {
            e->SetAttribute("type", type);
            e->SetAttribute("value", asString());
        }
        else
            e->LinkEndChild(new TiXmlText(_data));
		addXmlId(e);
        		
		root->LinkEndChild(e);
	}
	
    
	void PropertyT<double>::addAsXml(TiXmlElement* root, std::string key) {
		TiXmlElement* e = new TiXmlElement(key); 
		std::string type = (getXmlType().empty()) ? "double" : getXmlType();
		e->SetAttribute("type", type);
		e->SetAttribute("value", asString());
        addXmlId(e);
        
		root->LinkEndChild(e);
	}
	
	void PropertyT<osg::Vec2f>::addAsXml(TiXmlElement* root, std::string key) {
		TiXmlElement* e = new TiXmlElement(key); 
		std::string type = (getXmlType().empty()) ? "vec2f" : getXmlType();
		e->SetAttribute("type", type);
		if (type=="size") {
			e->SetAttribute("width", floatToString(_data.x()));
			e->SetAttribute("height", floatToString(_data.y()));	
        } else {
			e->SetAttribute("x", floatToString(_data.x()));
			e->SetAttribute("y", floatToString(_data.y()));	
		}
		addXmlId(e);	
		root->LinkEndChild(e);
	}
    
    void PropertyT<osg::Vec2d>::addAsXml(TiXmlElement* root, std::string key) {
		TiXmlElement* e = new TiXmlElement(key); 
		std::string type = (getXmlType().empty()) ? "point" : getXmlType();
		e->SetAttribute("type", type);
		if (type=="size") {
			e->SetAttribute("width", doubleToString(_data.x()));
			e->SetAttribute("height", doubleToString(_data.y()));	
        } else {
			e->SetAttribute("x", doubleToString(_data.x()));
			e->SetAttribute("y", doubleToString(_data.y()));	
		}
		addXmlId(e);	
		root->LinkEndChild(e);
	}
	
	void PropertyT<osg::Vec3d>::addAsXml(TiXmlElement* root, std::string key) {
		TiXmlElement* e = new TiXmlElement(key); 
		std::string type = (getXmlType().empty()) ? "vector" : getXmlType();
		e->SetAttribute("type", type);
		e->SetAttribute("x", doubleToString(_data.x()));
		e->SetAttribute("y", doubleToString(_data.y()));	
		e->SetAttribute("z", doubleToString(_data.z()));	
        addXmlId(e);	
		root->LinkEndChild(e);
	}

    void PropertyT<osg::Vec3f>::addAsXml(TiXmlElement* root, std::string key) {
		TiXmlElement* e = new TiXmlElement(key); 
		std::string type = (getXmlType().empty()) ? "vec3f" : getXmlType();
		e->SetAttribute("type", type);
		e->SetAttribute("x", floatToString(_data.x()));
		e->SetAttribute("y", floatToString(_data.y()));	
		e->SetAttribute("z", floatToString(_data.z()));	
        addXmlId(e);	
		root->LinkEndChild(e);
	}


	void PropertyT<osg::Vec4d>::addAsXml(TiXmlElement* root, std::string key) {
    
        TiXmlElement* e = new TiXmlElement(key); 
		std::string type = (getXmlType().empty()) ? "vec4d" : getXmlType();
		e->SetAttribute("type", type);
        if (type=="rect") {
            e->SetAttribute("l", doubleToString(_data.x()));
            e->SetAttribute("t", doubleToString(_data.y()));
            e->SetAttribute("w", doubleToString(_data.z() - _data.x()));
            e->SetAttribute("h", doubleToString(_data.w() - _data.y()));
        } else if (type=="color") {
            if (_data[3] != 1.0f) {
                e->SetAttribute("r", doubleToString(_data.x()));
                e->SetAttribute("g", doubleToString(_data.y()));
                e->SetAttribute("b", doubleToString(_data.z()));
                e->SetAttribute("a", doubleToString(_data.w()));
            }
            else
                e->SetAttribute("value", vec4ToHexColor(_data));
        } else {
            e->SetAttribute("x", doubleToString(_data.x()));
            e->SetAttribute("y", doubleToString(_data.y()));
            e->SetAttribute("z", doubleToString(_data.z()));
            e->SetAttribute("w", doubleToString(_data.w()));
        }
        addXmlId(e);
        
		root->LinkEndChild(e);
	
	}
    
    void PropertyT<osg::Vec4f>::addAsXml(TiXmlElement* root, std::string key) {
    
        TiXmlElement* e = new TiXmlElement(key); 
		std::string type = (getXmlType().empty()) ? "vec4f" : getXmlType();
		e->SetAttribute("type", type);
        if (type=="rect") {
            e->SetAttribute("l", floatToString(_data.x()));
            e->SetAttribute("t", floatToString(_data.y()));
            e->SetAttribute("w", floatToString(_data.z() - _data.x()));
            e->SetAttribute("h", floatToString(_data.w() - _data.y()));
        } else if (type=="color") {
            if (_data[3] != 1.0f) {
                e->SetAttribute("r", floatToString(_data.x()));
                e->SetAttribute("g", floatToString(_data.y()));
                e->SetAttribute("b", floatToString(_data.z()));
                e->SetAttribute("a", floatToString(_data.w()));
            }
            else
                e->SetAttribute("value", vec4ToHexColor(_data));
        } else {
            e->SetAttribute("x", floatToString(_data.x()));
            e->SetAttribute("y", floatToString(_data.y()));
            e->SetAttribute("z", floatToString(_data.z()));
            e->SetAttribute("w", floatToString(_data.w()));
        }
        addXmlId(e);
        
		root->LinkEndChild(e);
	
	}
	
	void PropertyT<osg::Quat>::addAsXml(TiXmlElement* root, std::string key) {
    
        TiXmlElement* e = new TiXmlElement(key); 
		std::string type = (getXmlType().empty()) ? "quat" : getXmlType();
		e->SetAttribute("type", type);
		e->SetAttribute("x", doubleToString(_data.x()));
		e->SetAttribute("y", doubleToString(_data.y()));
		e->SetAttribute("z", doubleToString(_data.z()));
		e->SetAttribute("w", doubleToString(_data.w()));
        
        addXmlId(e);
        
		root->LinkEndChild(e);
	
	}
	
	void PropertyT<osg::Matrixf>::addAsXml(TiXmlElement* root, std::string key) {
    
        TiXmlElement* e = new TiXmlElement(key); 
		std::string type = (getXmlType().empty()) ? "matrixf" : getXmlType();
		e->SetAttribute("type", type);
	
		for(unsigned int y=0; y<4; ++y) {
			for(unsigned int x=0; x<4; ++x) {
				e->SetAttribute("i"+intToString(x)+intToString(y), floatToString(_data(x,y)));
			}
		}
        
        addXmlId(e);
        
		root->LinkEndChild(e);
	
	}
    
    void PropertyT<osg::Matrixd>::addAsXml(TiXmlElement* root, std::string key) {
    
        TiXmlElement* e = new TiXmlElement(key); 
		std::string type = (getXmlType().empty()) ? "matrixd" : getXmlType();
		e->SetAttribute("type", type);
	
		for(unsigned int y=0; y<4; ++y) {
			for(unsigned int x=0; x<4; ++x) {
				e->SetAttribute("i"+intToString(x)+intToString(y), doubleToString(_data(x,y)));
			}
		}
        
        addXmlId(e);
        
		root->LinkEndChild(e);
	
	}

	
	void PropertyT<osg::Referenced*>::addAsXml(TiXmlElement* root, std::string key) {
	
        PropertyList* pl = asPropertyList();
        if (!pl) {
            // could not handle normal objects
            return;
        }
        
        TiXmlElement* e = new TiXmlElement(key);
        pl->addToXmlNode(e);
        addXmlId(e);
        root->LinkEndChild(e);        
        
	}
    
    
    /** @return item at position i */
    AbstractProperty* MultipleValueList::get(unsigned int i)  {
        return (i < _list.size()) ? _list[i].get() : PropertyList::getUndefined();
    }
    
	void MultipleValueList::addAsXml(TiXmlElement* root, std::string key) {
        for(List::iterator i = _list.begin(); i != _list.end(); i++) {
            (*i)->addAsXml(root, key);
        }

	}
	
	// --------------------------------------------------------------------------------------------------------------------
	// ErrorHandler
	// --------------------------------------------------------------------------------------------------------------------

	PropertyList::ErrorHandler* PropertyList::ErrorHandler::instance(PropertyList::ErrorHandler* newhandler) {
		static osg::ref_ptr<ErrorHandler> s_error_handler = new PropertyList::ErrorHandler();
		if (newhandler)
			s_error_handler = newhandler;
		return s_error_handler.get();
	}
	
	// --------------------------------------------------------------------------------------------------------------------
	// IsInclusionAllowedCallback
	// --------------------------------------------------------------------------------------------------------------------

	PropertyList::IsInclusionAllowedCallback* PropertyList::IsInclusionAllowedCallback::instance(PropertyList::IsInclusionAllowedCallback* newcallback) {
		static osg::ref_ptr<IsInclusionAllowedCallback> s_inclusion_allowed_callback = new PropertyList::IsInclusionAllowedCallback();
		if (newcallback)
			s_inclusion_allowed_callback = newcallback;
		return s_inclusion_allowed_callback.get();
	}
	
	
	/**
	 * creates a propertylist out of a xml-file
	 */
	PropertyList::PropertyList(const std::string& xmlfilename, ReadXmlFileCallback* cb)  
	:	osg::Referenced(), 
		_multipleValuesAllowed(true),
		_readXmlFileCallback(cb),
		_parent(NULL)
	
	{
		loadFromXML(xmlfilename);
	}

	bool PropertyList::loadFromXML(const std::string& xmlfilename) {
        std::string filename = osgDB::findDataFile(xmlfilename);
        if (filename.empty()) {
            ErrorHandler::instance()->handle("Could not find xml-file " + xmlfilename);
			return false;
        }
        
		TiXmlDocument doc = TiXmlDocument(filename);
        doc.SetCondenseWhiteSpace(false); 
		
		if (_readXmlFileCallback.valid()) 
			_readXmlFileCallback->beforeLoad(filename);
		
		doc.LoadFile();
		if (doc.Error()) {
			std::ostringstream msg;
			msg.str("");
			msg << "Error parsing xml-file: "  << xmlfilename << " " << doc.ErrorDesc() << " at col: " << doc.ErrorCol() << " row: " << doc.ErrorRow();
			ErrorHandler::instance()->handle(msg.str());
			
			return false;
		}
		else { 
			if (_readXmlFileCallback.valid()) 
			_readXmlFileCallback->afterLoad(doc);
            _xmlFileName = filename;
			addFromXmlElement(doc.RootElement());
			return true;
		}
	}
	
	bool PropertyList::loadFromJSON(const std::string& xmlfilename) 
	{
		std::string str = cefix::readFileIntoString(osgDB::findDataFile(xmlfilename));
        
        JSON::ParseOptions options;
        options.mergeArrays = true;
		osg::ref_ptr<cefix::AbstractProperty> value = JSON::parse(str.c_str(), options);
		
        if (!value) 
			return false;
		
		if (value->getType() == AbstractProperty::PROPERTY_LIST) {
			cefix::PropertyList* pl = value->asPropertyList();
			for(unsigned int i = 0; i < pl->size(); ++i) {
				addValue(pl->getKeyAt(i), pl->get(i));
			}
		} 
		
		return true;
	}
    
    AbstractProperty* PropertyList::find(const std::string& path) {
		
		if (path.empty()) return NULL;
		
        std::vector<std::string> path_token;
        strTokenize(path, path_token, "/");
        
        return find(path_token);
    }
    
    
    void PropertyList::findAll(const std::string& path, FoundPropertiesVector& results) {
    
        AbstractProperty* ap = find(path);
        if (NULL == ap)         
            return;
            
        if (ap->getType() != AbstractProperty::MULTIPLE_VALUE_LIST) {
            results.push_back(ap);
        } 
        else {
            const MultipleValueList::List& list = ap->asMultipleValueList()->list();
            for(MultipleValueList::const_iterator itr = list.begin(); itr != list.end(); itr++) {
                results.push_back(*itr);
            }
        }
    
    }
    
    AbstractProperty* PropertyList::find(std::vector<std::string> &tokens, unsigned int tokenNdx) {
    
        std::string key = tokens[tokenNdx];
        AbstractProperty* p = get(key);
        
        // hamwa was jefunden?
        if (AbstractProperty::UNDEFINED == p->getType())
            return NULL;
        
        // sind wir am ende des tokenvectors und haben was jefunden?
        if (tokenNdx == tokens.size()-1)
            return p;
        
        if (AbstractProperty::PROPERTY_LIST == p->getType()) {
            PropertyList* pl = dynamic_cast<PropertyList*>(p->asObject());
            if (pl) 
                return pl->find(tokens, tokenNdx + 1);
            else
                return NULL;
        }
        
        if (AbstractProperty::MULTIPLE_VALUE_LIST == p->getType()) {
            MultipleValueList* pv = dynamic_cast<MultipleValueList*>(p);
            
            AbstractProperty* p2 = getUndefined();
            if ((pv)  && (tokenNdx + 1 < tokens.size())) {
                std::string ndx = tokens[tokenNdx + 1];
                p2 = pv->get(cefix::stringToInt(ndx));
            }
            if (AbstractProperty::UNDEFINED == p2->getType())
                    return NULL;
            else {
                //ist der propert eine liste, und wollen wir noch tiefer rein?
                if ((AbstractProperty::PROPERTY_LIST == p2->getType()) && (tokenNdx + 2 < tokens.size())) {
                    PropertyList* pl = dynamic_cast<PropertyList*>(p2->asObject());
                    if (pl) 
                        return pl->find(tokens, tokenNdx + 2); // +2, wir haben einen √ºbersprungen
                    else
                        return NULL; // hier sollten wir eigentlich nieee hin...
                }
                // seems to be a normal property, or finished with find
                else 
                    return p2;
            }
                
        }

        return NULL;
    }
        
	
	AbstractProperty* PropertyList::getUndefined() {
		static osg::ref_ptr<AbstractProperty> s_undefined = new AbstractProperty(AbstractProperty::UNDEFINED);
		
		return s_undefined.get();
	}
	
	AbstractProperty* PropertyList::replaceValue(const std::string& key, AbstractProperty* property) {
		osg::ref_ptr<AbstractProperty> savedprop = property;
		if (!hasKey(key)) return getUndefined();
		
		osg::ref_ptr<AbstractProperty> oldvalue = get(key);
		if (oldvalue->getType() == AbstractProperty::MULTIPLE_VALUE_LIST) {
			MultipleValueList* mvl = dynamic_cast<MultipleValueList*>(oldvalue.get());
			if (mvl) {
				osg::ref_ptr<AbstractProperty> ov = mvl->get(0);
				mvl->list()[0] = property;
				_vector[ov->getVectorIndex()] = property;
				_vector[ov->getVectorIndex()].setKey(key);
				property->setVectorIndex(ov->getVectorIndex());
				property->setXmlType(ov->getXmlType());
				property->setXmlId(ov->getXmlId());
				return property;
			}
		} else {
			_list[key] = property;
			_vector[oldvalue->getVectorIndex()] = property;
			_vector[oldvalue->getVectorIndex()].setKey(key);
			property->setVectorIndex(oldvalue->getVectorIndex());
			property->setXmlType(oldvalue->getXmlType());
			property->setXmlId(oldvalue->getXmlId());
		}
		property->setParent(this);
		return property;
	}
	
	AbstractProperty* PropertyList::addValue(const std::string& key, AbstractProperty* p) {
        bool has_key = hasKey(key);
        if ((!_multipleValuesAllowed) || (!has_key)) {
            if (!has_key) { // dem vector hinzufuegen
                p->setVectorIndex(_vector.size());
                _vector.push_back(p);
                _vector[p->getVectorIndex()].setKey(key);
            }
            else {
                // prop in _vector ersetzen
                AbstractProperty* tmp = _list[key].get();
                unsigned int temp_pos = tmp->getVectorIndex();
                p->setVectorIndex(temp_pos);
                _vector[temp_pos] = p;
                _vector[p->getVectorIndex()].setKey(key);
            }
            _list[key] = p; 
                
        }
        else {
            if ((*_list[key]).getType() ==  AbstractProperty::MULTIPLE_VALUE_LIST) {
                MultipleValueList* mpv = dynamic_cast<MultipleValueList*>(get(key)->asObject());
                mpv->add(p);
            }
            else {
                MultipleValueList* mpv = new MultipleValueList();
                AbstractProperty* old = get(key);
                
                mpv->add(old);
                mpv->add(p);
				mpv->setVectorIndex(_vector.size());
				mpv->setParent(this);
                _list[key] = mpv;
            }
            p->setVectorIndex(_vector.size());
            _vector.push_back(p);
            _vector[p->getVectorIndex()].setKey(key);
        }
        p->setParent(this);
        return p;
    }

	/**
	 * adds all properties from a xml-node
	 */
	void PropertyList::addFromXmlElement(TiXmlElement* node) {
		if (node == NULL)
			return;
		TiXmlNode* child = node->FirstChild();
		while(child) {
			TiXmlElement* subnode = child->ToElement();
            
			if (subnode) {
				std::string key = subnode->ValueStr();
                std::string xml_id = XML::getAttributeFromNode(subnode,"id");
                AbstractProperty* toAdd = NULL;
                
				if (XML::getElementCount(subnode) > 0) {
					osg::ref_ptr<PropertyList> l = new PropertyList(subnode, _xmlFileName, getReadXmlFileCallback());
					l->setParent(this);
					toAdd = new PropertyT<osg::Referenced*>(AbstractProperty::PROPERTY_LIST,l.get());
				}
				else if ((subnode->FirstAttribute() == NULL) && (subnode->FirstChild())) {
					TiXmlText* t = subnode->FirstChild()->ToText();
					if (t)
						toAdd = new PropertyT<std::string>(t->ValueStr());
				}
				else {
					std::string value = XML::getAttributeFromNode(subnode,"value").c_str();
					std::string valueType = strToLower(XML::getAttributeFromNode(subnode,"type"));
					if ((!valueType.empty()) && (!key.empty())) {
					
						//osg::notify(osg::DEBUG_INFO) << key << ": (" << valueType << ") " << value << std::endl;
                        
                        if (valueType == "include") 
						{
							bool absolute = (strToLower(XML::getAttributeFromNode(subnode,"rel")) == "absolute");
					
                            std::string path = osgDB::getFilePath(_xmlFileName);
							std::string xmlfile = (!absolute) ? path + "/" + value : value;
							
							// check via callback if the inclusion of this file is allowed
							if (IsInclusionAllowedCallback::instance()->allowFile(xmlfile)) {
								osg::ref_ptr<PropertyList> incl = new PropertyList(xmlfile, getReadXmlFileCallback() );
								if (incl.valid()) incl->setParent(this);
								toAdd = new PropertyT<osg::Referenced*>(AbstractProperty::PROPERTY_LIST,incl.get());
							} else
								toAdd = NULL;
                        } 
                        else if ((valueType == "boolean") || (valueType == "bool")) {
							bool  b = (strToLower(value) == "true") || (strToLower(value) == "1") ;
							toAdd = new PropertyT<bool>(b);
							toAdd->setXmlType("boolean");
							
						} 
                        else if (valueType == "integer") {
							toAdd = new PropertyT<int>(atoi(value.c_str()));
							
						} 
                        else if (valueType == "float") {
							toAdd = new PropertyT<float>((float)atof(value.c_str()));    
						
                        } 
                        else if (valueType == "double") {
							toAdd = new PropertyT<double>(strtod(value.c_str(), NULL));    
	
						} 
                        else if (valueType == "string") {
							toAdd =new PropertyT<std::string>( value);
						} 
                        else if (valueType == "symbol") {
							toAdd = new PropertyT<std::string>(strToLower(value));
							toAdd->setXmlType("symbol");
							
						} 
                        else if (valueType == "color") {
                            if (value.empty()) {
                                //scheint in rgba angegeben zu sein
                                float r,g,b,a;
                                r = atof(XML::getAttributeFromNode(subnode,"r").c_str());
                                g = atof(XML::getAttributeFromNode(subnode,"g").c_str());
                                b = atof(XML::getAttributeFromNode(subnode,"b").c_str());
                                a = atof(XML::getAttributeFromNode(subnode,"a").c_str());
							
                                toAdd = new PropertyT<osg::Vec4f>( osg::Vec4f(r,g,b,a));
                            }
                            else {
                                toAdd = new PropertyT<osg::Vec4f>(hexColorToVec4(value));
                            }                                
							toAdd->setXmlType("color");
						
						} 
                        else if (valueType == "rect") {
							float l,t,w,h;
							l = atof(XML::getAttributeFromNode(subnode,"l").c_str());
							t = atof(XML::getAttributeFromNode(subnode,"t").c_str());
							w = atof(XML::getAttributeFromNode(subnode,"w").c_str());
							h = atof(XML::getAttributeFromNode(subnode,"h").c_str());
							
							toAdd = new PropertyT<osg::Vec4d>(osg::Vec4d(l,t,l+w,t+h));
							toAdd->setXmlType("rect");
                        } 
                        else if ((valueType == "vector4") || (valueType == "vec4d")) {
							float x,y,z,w;
							x = atof(XML::getAttributeFromNode(subnode,"x").c_str());
							y = atof(XML::getAttributeFromNode(subnode,"y").c_str());
							z = atof(XML::getAttributeFromNode(subnode,"z").c_str());
							w = atof(XML::getAttributeFromNode(subnode,"w").c_str());
							
							toAdd = new PropertyT<osg::Vec4d>(osg::Vec4d(x,y,z,w));
                            toAdd->setXmlType(valueType);

						} else if (valueType == "vec4f") {
							float x,y,z,w;
							x = atof(XML::getAttributeFromNode(subnode,"x").c_str());
							y = atof(XML::getAttributeFromNode(subnode,"y").c_str());
							z = atof(XML::getAttributeFromNode(subnode,"z").c_str());
							w = atof(XML::getAttributeFromNode(subnode,"w").c_str());
							
							toAdd = new PropertyT<osg::Vec4f>(osg::Vec4f(x,y,z,w));
                            toAdd->setXmlType(valueType);

						} else if (valueType == "vector") {
							float x,y,z;
							x = atof(XML::getAttributeFromNode(subnode,"x").c_str());
							y = atof(XML::getAttributeFromNode(subnode,"y").c_str());
							z = atof(XML::getAttributeFromNode(subnode,"z").c_str());
							
							toAdd = new PropertyT<osg::Vec3d>( osg::Vec3d(x,y,z));
							toAdd->setXmlType("vector");
							
						} else if (valueType == "vec3f") {
							float x,y,z;
							x = atof(XML::getAttributeFromNode(subnode,"x").c_str());
							y = atof(XML::getAttributeFromNode(subnode,"y").c_str());
							z = atof(XML::getAttributeFromNode(subnode,"z").c_str());
							
							toAdd = new PropertyT<osg::Vec3f>( osg::Vec3f(x,y,z));
							toAdd->setXmlType("vec3f");
							
						} else if ((valueType == "vec3") || (valueType == "vec3d")) {
							float x,y,z;
							x = atof(XML::getAttributeFromNode(subnode,"x").c_str());
							y = atof(XML::getAttributeFromNode(subnode,"y").c_str());
							z = atof(XML::getAttributeFromNode(subnode,"z").c_str());
							
							toAdd = new PropertyT<osg::Vec3d>( osg::Vec3d(x,y,z));
							toAdd->setXmlType(valueType);
							
						} else if (valueType == "point") {
							float x,y;
							x = atof(XML::getAttributeFromNode(subnode,"x").c_str());
							y = atof(XML::getAttributeFromNode(subnode,"y").c_str());
						
							toAdd = new PropertyT<osg::Vec2d>( osg::Vec2d(x,y));
							toAdd->setXmlType("point");

						} 
						else if (valueType == "vec2") {
							float x,y;
							x = atof(XML::getAttributeFromNode(subnode,"x").c_str());
							y = atof(XML::getAttributeFromNode(subnode,"y").c_str());
						
							toAdd = new PropertyT<osg::Vec2d>( osg::Vec2d(x,y));
							toAdd->setXmlType("vec2");

						}
                        else if (valueType == "vec2f") {
							float x,y;
							x = atof(XML::getAttributeFromNode(subnode,"x").c_str());
							y = atof(XML::getAttributeFromNode(subnode,"y").c_str());
						
							toAdd = new PropertyT<osg::Vec2f>( osg::Vec2f(x,y));
							toAdd->setXmlType("vec2f");

						}
                        else if (valueType == "vec2d") {
							float x,y;
							x = atof(XML::getAttributeFromNode(subnode,"x").c_str());
							y = atof(XML::getAttributeFromNode(subnode,"y").c_str());
						
							toAdd = new PropertyT<osg::Vec2d>( osg::Vec2d(x,y));
							toAdd->setXmlType("vec2d");

						}else if (valueType == "size") {
							float x,y;
							x = atof(XML::getAttributeFromNode(subnode,"width").c_str());
							y = atof(XML::getAttributeFromNode(subnode,"height").c_str());
						
							toAdd = new PropertyT<osg::Vec2d>( osg::Vec2d(x,y));
							toAdd->setXmlType("size");

						}  else if (valueType == "quat") {
							float x,y,z,w;
							x = atof(XML::getAttributeFromNode(subnode,"x").c_str());
							y = atof(XML::getAttributeFromNode(subnode,"y").c_str());
							z = atof(XML::getAttributeFromNode(subnode,"z").c_str());
							w = atof(XML::getAttributeFromNode(subnode,"w").c_str());
						
							toAdd = new PropertyT<osg::Quat>( osg::Vec4d(x,y,z,w) );
							toAdd->setXmlType("quat");

						} else if ((valueType == "matrix") || (valueType == "matrixd")) {
							osg::Matrixd m;
							for(unsigned int y = 0; y < 4; ++y) {
								for(unsigned int x = 0; x < 4; ++x) {
									double f = strtod(XML::getAttributeFromNode(subnode,"i"+intToString(x)+intToString(y)).c_str(), NULL);
									m(x,y) = f;
								}
							}
													
							toAdd = new PropertyT<osg::Matrixd>( m );
							toAdd->setXmlType(valueType);

						} else if (valueType == "matrixf") {
							osg::Matrixf m;
							for(unsigned int y = 0; y < 4; ++y) {
								for(unsigned int x = 0; x < 4; ++x) {
									float f = strtod(XML::getAttributeFromNode(subnode,"i"+intToString(x)+intToString(y)).c_str(), NULL);
									m(x,y) = f;
								}
							}
													
							toAdd = new PropertyT<osg::Matrixf>( m );
							toAdd->setXmlType(valueType);

						}
                        else {
							log::error("PropertyList::addFromXmlElement") << "unknown type "
							<< valueType 
							<< " for tag: "
							<< key 
							<< std::endl;
						
						}
					}
				}
                
                // xml-id setzen
                if ((toAdd!= NULL) &&(!xml_id.empty())) {
                    toAdd->setXmlId(xml_id);
                }
                
                // der liste hinzufügen
                if ((toAdd != NULL) && (!key.empty())) {
                    addValue(key, toAdd);
                }
			}
			child = node->IterateChildren(child);
		}
	}
			
	/**
	 * dumps the property-list to std::out
	 */
	void PropertyList::dump(int level) {

		std::cout << *this << std::endl;
	}
	
	
    
    void PropertyList::saveAsXML(const std::string& filename, const std::string& rootname) {
    
        TiXmlDocument doc;
        doc.SetCondenseWhiteSpace(false);
        TiXmlElement* docroot = new TiXmlElement(rootname);
        doc.LinkEndChild(new TiXmlDeclaration("1.0", "UTF-8", "yes"));
        doc.LinkEndChild(docroot);
        
        addToXmlNode(docroot);
        
        doc.SaveFile(filename);
    }
	
	
	void PropertyList::saveAsJSON(const std::string& filename) 
	{
        std::ofstream myfile(filename.c_str(), std::ios::trunc | std::ios::out);
        std::string json = JSON::stringifyToUTF8(this);
		myfile << json;
		myfile.close();
    }
    
    void PropertyList::addToXmlNode(TiXmlElement* root) {
    
        // put all properties into the list
        for(PropertyVectorT::iterator i = _vector.begin(); i!= _vector.end(); i++) {
            AbstractProperty* ap = (*i).get();
            ap->addAsXml(root, i->key());
		}
        
    }
	
// ----------------------------------------------------------------------------------------------------------
// adds all key-value-pairs of propertylist pl that are not present in this propertylist
// ----------------------------------------------------------------------------------------------------------

	void PropertyList::addNonExistingKeys(cefix::PropertyList* pl) {
		
		if (pl == NULL) return;
		
		for(unsigned int i  = 0 ; i < pl->size(); ++i) {
		
			std::string key = pl->getKeyAt(i);
			if (!hasKey(key)) {
				addValue(key, pl->get(i));
			} else if ((get(key)->getType() == AbstractProperty::PROPERTY_LIST) && (get(key)->getType() == pl->get(i)->getType())) {
				cefix::PropertyList* subpl = get(key)->asPropertyList();
				subpl->addNonExistingKeys(pl->get(i)->asPropertyList());
			}
		
		}
	
	}


std::ostream& operator<<(std::ostream& os, const PropertyList& pl)
{
	static unsigned int s_os_ident = 0;
	if(s_os_ident) {
		os << std::setw(s_os_ident*2) << " "; 
	}
	
	os << "{" << std::endl;
	s_os_ident++;
	for(unsigned int i = 0; i < pl.size(); ++i) 
	{
		os << std::setw(s_os_ident*2) << " ";
		os << pl.getKeyAt(i) << " : "; 
		AbstractProperty& property = *(pl.get(i));
		os << property << std::endl;
	}
	
	s_os_ident--;
	if(s_os_ident) {
		os << std::setw(s_os_ident*2) << " ";
	}
	os << "}" << std::endl;
	return os;
}

std::ostream& operator<<(std::ostream& os, AbstractProperty& p) 
{
	switch (p.getType()) {
		case AbstractProperty::UNDEFINED:
			os << "UNDEFINED";
			break;
            
		case AbstractProperty::BOOLEAN:
			os << p.asBool();
			break;
            
		case AbstractProperty::INTEGER:
			os << p.asInt();
			break;
			
		case AbstractProperty::FLOAT:
			os << p.asFloat();
			break;
			
		case AbstractProperty::STRING:
			os << p.asString();
			break;
			
		case AbstractProperty::DOUBLE:
			os << p.asDouble();
			break;
		case AbstractProperty::VEC2F:
			os << p.asVec2f();
			break;
			
		case AbstractProperty::VEC3F:
			os << p.asVec3f();
			break;
			
		case AbstractProperty::VEC4F:
			os << p.asVec4f();
			break;
		
		case AbstractProperty::VEC2D:
			os << p.asVec2d();
			break;
			
		case AbstractProperty::VEC3D:
			os << p.asVec3d();
			break;
			
		case AbstractProperty::VEC4D:
			os << p.asVec4d();
			break;
			
		case AbstractProperty::MATRIXF:
			os << p.asMatrixf();
			break;
			
		case AbstractProperty::MATRIXD:
			os << p.asMatrixd();
			break;
			
		case AbstractProperty::QUAT:
			os << p.asQuat();
			break;
			
		case AbstractProperty::PROPERTY_LIST:
			{
				const PropertyList* pl = p.asPropertyList();
				os << std::endl << (*pl);
			}
			break;
		case AbstractProperty::MULTIPLE_VALUE_LIST:
			// todo
			break;
			
		case AbstractProperty::OBJECT:
			os << p.asObject();
			break;
	}
	
	return os;
}



} // end of namespace