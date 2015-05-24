/*
 *  AlterableController.cpp
 *  cefix_alterable
 *
 *  Created by Stephan Huber on 02.09.11.
 *  Copyright 2011 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */

#include "AlterableController.h"
#include <cefix/DataFactory.h>



namespace cefix {


AlterableController::AlterableController() 
:   osg::Referenced() 
{
	cefix::DataFactory::instance()->addCleanupHandler(&AlterableController::cleanup);
}


void AlterableController::dumpTo(std::ostream& s)
{
    for(AlterableGroups::iterator i = _groups.begin(); i != _groups.end(); ++i) {
        s << i->first << std::endl;
        AlterableGroup::List& l = i->second.items;
        for(AlterableGroup::List::iterator j = l.begin(); j != l.end(); ++j) {
            Alterable* a = *j;
            for(unsigned int k = 0; k != a->getNumComposites(); ++k) {
                 s << a->getName() << " k: " << k << " key: " << a->getCompositeKey(k) << " name: " << a->getCompositeName(k) << std::endl;
            }
        }
    }
}

AlterableController* AlterableController::instance(bool erase)
{
    static osg::ref_ptr<AlterableController> s_ptr = NULL;
	
	if (erase) {
		s_ptr = NULL;
	} 
	else if (!s_ptr.valid()) 
		s_ptr = new AlterableController();
	
    return s_ptr.get(); 
}

void AlterableController::cleanup() {
	AlterableController::instance(true);
}

}