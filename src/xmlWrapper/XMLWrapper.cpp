/*
 *  XMLWrapper.cpp
 *  NativeContext
 *
 *  Created by Stephan Huber on Wed Sep 04 2002.
 *  Copyright (c) 2002 digital mind. All rights reserved.
 *
 */


#include <cefix/XMLWrapper.h>

namespace cefix {

std::string XML::getStringValueFromSubNode(TiXmlElement* node, std::string subnodeName, std::string defaultValue) {
	
	TiXmlHandle h = TiXmlHandle(node);
	TiXmlText* subnode = h.FirstChild(subnodeName).FirstChild().Text();
	if (subnode) 
		return subnode->ValueStr();
	else
		return defaultValue;
}

std::string XML::getAttributeValueFromSubNode(TiXmlElement* node, std::string subnodeName, std::string attributeName) {
	
	TiXmlHandle h = TiXmlHandle(node);
	TiXmlElement* subnode = h.FirstChild(subnodeName).Element();
	if ((subnode) && (subnode->Attribute(attributeName)))
		return *subnode->Attribute(attributeName);
	else
		return "";
}

std::string XML::getAttributeFromNode(TiXmlElement* node, std::string attrName) {
	if (node->Attribute(attrName))
		return *node->Attribute(attrName);
	else
		return "";
}

int XML::getElementCount(TiXmlElement* node) {
	
	TiXmlNode* child = node->FirstChild();
	int cnt = 0;
	while(child) {
		if (child->ToElement())
			cnt++;
		child = node->IterateChildren(child);
	}
	return cnt;
}

}