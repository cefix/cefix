/*
 *  AnimationController.cpp
 *  cefix
 *
 *  Created by Stephan Huber on 08.05.06.
 *  Copyright 2006 digitalmind.de // Stephan Maximilian Huber. All rights reserved.
 *
 */

#include <cefix/AnimationController.h>
#include <iomanip>
#include <iostream>

namespace cefix {

int AnimationController::stat_num_animated = 0;
int AnimationController::stat_num_deleted  = 0;

void AnimationController::animate_implementation(float delta_s) 
{
	
	AnimationBase* a = NULL;
	unsigned int cleaned(0);
	
	for(AnimationList::iterator i = _list.begin(); i != _list.end();) {
		
		a = (*i).get();
		//osg::notify(osg::ALWAYS) << "      s: " <<  a->getStartTime() << " n: " << delta_s << " f:" << a->isFinished() << std::endl;
		
		if (delta_s >= a->getStartTime()) {
			if (!a->isInited()) {
				a->init();
				a->_inited = true;
				++i;
			}
			else if (a->isFinished()) {
				a->cleanUp();
				i = _list.erase(i);
				++cleaned;
			}
			else {
				a->animate(delta_s - a->getStartTime());
				++i;
			}
		} else {
			++i;
		}
	}
	//osg::notify(osg::ALWAYS) << "  Animated: " << _list.size() << " to remove: " << toClean.size() << std::endl;
	stat_num_animated += _list.size();
	stat_num_deleted  += cleaned;
			
	_finished = (_list.size() == 0);
}

void AnimationController::dump(int delta) 
{
    for(AnimationList::iterator i = _list.begin(); i != _list.end();++i) 
    {
        cefix::AnimationController* ctrl = dynamic_cast<cefix::AnimationController*>((*i).get());
        if (ctrl)
            ctrl->dump(delta+3);
        else {
            std::cout << std::setw(delta) << " " << typeid(*(*i).get()).name() << std::endl;
        }

    }
}

}