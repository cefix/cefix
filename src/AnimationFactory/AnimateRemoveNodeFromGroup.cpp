/*
 *  AnimateRemoveNodeFromGroup.cpp
 *  AudiHeritage
 *
 *  Created by Stephan Huber on 21.05.06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include "AnimateRemoveNodeFromGroup.h"

namespace cefix {
void AnimateRemoveNodeFromGroup::cleanUp() {
	if (_group.valid()) {
		bool b = _group->removeChild(_node.get());
		osg::notify(osg::DEBUG_INFO) << "AnimateRemoveNodeFromGroup:: " << b << std::endl;
		_node = NULL;
		_group = NULL;
	}
}


}