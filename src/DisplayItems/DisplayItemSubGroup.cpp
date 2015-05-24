/*
 *  DisplayItemSubGroup.cpp
 *  AudiHeritage
 *
 *  Created by Stephan Huber on 01.06.06.
 *  Copyright 2006 digitalmind.de // Stephan Maximilian Huber. All rights reserved.
 *
 */


#include "DisplayItemSubGroup.h"
#include <osg/observer_ptr>
#include <cefix/PropertyList.h>

namespace cefix {
	
	class AnimateSetNodeMask : public cefix::AnimationBase {
	public: 
		AnimateSetNodeMask(float starttime, osg::Node* node,  osg::Node::NodeMask nodemask) :
		cefix::AnimationBase(starttime),
		_node(node),
		_nodemask(nodemask)
		{
		}
		
		virtual void cleanUp() {
			if (_cancelled && _node.valid()) {
				// wir wurden gecancelt bevor wir die NodeMask setzen konnten
				_node->setNodeMask(_nodemask);
				_node = NULL;
			}
		}
		
	protected:
		virtual ~AnimateSetNodeMask() {}
		virtual void animate(float f) {
			if (_node.valid()) {
				_node->setNodeMask(_nodemask);
				_node = NULL;
			}
			setFinishedFlag();
		}
		
	private:
		osg::observer_ptr<osg::Node>	_node;
		osg::Node::NodeMask				_nodemask;
	};

void DisplayItemSubGroup::addAnimation(cefix::AnimationController* ctrl, float starttime, bool shouldHide) {
        
    cefix::AnimationController* sub_ctrl = new cefix::AnimationController(starttime);
    
    // show and hide hier nachbauen
    
    if (shouldHide) {
        _sublayer->createHideAnimation(sub_ctrl, 0.0f);
        _sublayer->setVisible(false);
        
        // noch einen cleanup-handler dazu, wenn wir fertig sind mit ausblenden ( setzt die nodemask wieder auf 0 )
        sub_ctrl->addCleanupFunctor(new  AnimationController::CleanupFunctorT<DisplayItemGroup>(  
            _sublayer.get(), 
            &DisplayItemGroup::finishHideAnimation
        ));
    }
    else {
        if (starttime == 0)
			_sublayer->getGroup()->setNodeMask(_sublayer->getNodeMask());
		else
			ctrl->add(new AnimateSetNodeMask(starttime, _sublayer->getGroup(), _sublayer->getNodeMask()));
        _sublayer->createShowAnimation(sub_ctrl, 0.0f);
        _sublayer->setVisible(true);
		sub_ctrl->addCleanupFunctor(new  AnimationController::CleanupFunctorT<DisplayItemGroup>(  
            _sublayer.get(), 
            &DisplayItemGroup::finishShowAnimation
        ));
    }
    
    ctrl->add(sub_ctrl);
}

}