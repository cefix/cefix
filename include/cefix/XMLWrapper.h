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

#ifndef __XMLWRAPPER_HEADER__
#define __XMLWRAPPER_HEADER__ 1
#include <string>
#include <iostream>
#include <cefix/tinyxml.h>
#include <cefix/Export.h>

namespace cefix {

class CEFIX_EXPORT XML {
	public:
	/**
	 * getStringValueFromSubNode returns the string value from a subnode
	 * @param node the parent-node
	 * @param subnodeName the name of the subnode
	 * @param defaultValue the default-value, if the subnode can't be found
	 *
	 * @return the stringvalue of the subnode
	 */
	static std::string getStringValueFromSubNode(TiXmlElement* node, std::string subnodeName, std::string defaultValue);
	
	/**
	 * gets the value of the attribute of the subnode with the name subnodeName
	 * @param node the parent-node
	 * @param subnodeName the name of the subnode
	 * @param attributeName the name of the attribute of the subnode
	 * @return the value of the attribute, if not found then empty
	 */
	static std::string getAttributeValueFromSubNode(TiXmlElement* node, std::string subnodeName, std::string attributeName);
	
	/**
	 * gets the value of the attribute of the node
	 * @param node the node
	 * @param attrName the name of the attribute of the subnode
	 * @return the value of the attribute, if not found then empty
	 */
	static std::string getAttributeFromNode(TiXmlElement* node, std::string attrName);
	
	/**
	 * returns the count of Elements inside node
	 * @param node the parent-node
	 * @return returns the amount of Elements inside node
	 */
	static int getElementCount(TiXmlElement* node);
};
	
}
#endif