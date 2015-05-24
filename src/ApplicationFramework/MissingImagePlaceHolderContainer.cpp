/*
 *  MissingImagePlaceHolderContainer.cpp
 *  AudiHeritage
 *
 *  Created by Stephan Huber on 20.05.06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */
#include <cefix/Notify.h>
#include <cefix/PropertyList.h>

#include "MissingImagePlaceHolderContainer.h"
namespace cefix {
void MissingImagePlaceHolderContainer::load() {
	
	std::string filename = _pl->get("Image")->asString();
	if (filename.empty())
		filename = "placeholder.png";
		
	_placeHolder = osgDB::readImageFile(filename);
	if (_placeHolder == NULL) 
		cefix::notify("addToErrorLog","MissingImagePlaceHolder " + filename + " could not be found!");


}
}