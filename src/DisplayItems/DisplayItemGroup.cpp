/*
 *  DisplayItemGroup.cpp
 *  AudiHeritage
 *
 *  Created by Stephan Huber on 22.05.06.
 *  Copyright 2006 digitalmind.de // Stephan Maximilian Huber. All rights reserved.
 *
 */

#include "DisplayItemGroup.h"
#include <cefix/PropertyList.h>
#include <cefix/Notify.h>

namespace cefix {

// ----------------------------------------------------------------------------------------------------------
// ctro
// ----------------------------------------------------------------------------------------------------------

DisplayItemGroup::Item::Item() : osg::Referenced() {

	static ID s_id_counter = 0;
	_id = s_id_counter++;
}


// --------------------------------------------------------------------------------------------------------------------
// error
// --------------------------------------------------------------------------------------------------------------------

void DisplayItemGroup::Item::error(std::string msg) 
{
    cefix::notify("addToErrorLog", msg);
}

    
// --------------------------------------------------------------------------------------------------------------------
// konstruktor
// --------------------------------------------------------------------------------------------------------------------

DisplayItemGroup::DisplayItemGroup() 
:	osg::Referenced(), 
	_isVisible(false), 
	_nodeMask(0xFFFFFFFF)
{
    _transform = new osg::MatrixTransform();
    _transform->setNodeMask(0);
	_animationController = new cefix::AnimationController();
}

DisplayItemGroup::~DisplayItemGroup() {
    
}

void DisplayItemGroup::addItem(Item* item) { 
    setAnimationStartTimeFor(item, _items.size() * 0.25f);
    _items.push_back(item); 
    osg::Node* n = item->getNode();
    if (n) _transform->addChild(n);
}

void DisplayItemGroup::removeItem(Item* item) {
    _transform->removeChild(item->getNode());
    for(ItemVector::iterator i = _items.begin(); i != _items.end(); ) {
        if ((*i).get() == item) {

            osg::Node* n = item->getNode();
            if (n) _transform->removeChild(n);
            i = _items.erase(i);
        } else {
            ++i;
        }
    }
}

// --------------------------------------------------------------------------------------------------------------------
// startet show-animation
// --------------------------------------------------------------------------------------------------------------------

void DisplayItemGroup::show(float starttime) {

    if (isVisible())
        return;
    _isVisible = true;
    languageChanged();
    _transform->setNodeMask(_nodeMask);
	
	if (_animationController->isFinished() == false) {
		_animationController->cancel();
	}
    _showFinished = false;
    createShowAnimation(_animationController.get(), starttime);
	
	_animationController->addCleanupFunctor(new AnimationController::CleanupFunctorT<DisplayItemGroup>(  
        this, 
        &DisplayItemGroup::finishShowAnimation
    ));
	
    _animationController->init();
    cefix::AnimationFactory::instance()->add(_animationController.get());
}

// --------------------------------------------------------------------------------------------------------------------
// startet hide-animation
// --------------------------------------------------------------------------------------------------------------------

void DisplayItemGroup::hide(float starttime) {    
    if (!isVisible())
        return;
	_hideFinished = false;
    _isVisible = false;
	
	if (_animationController->isFinished() == false) {
		_animationController->cancel();
	}
	
	createHideAnimation(_animationController.get(), starttime);
    _animationController->addCleanupFunctor(new  AnimationController::CleanupFunctorT<DisplayItemGroup>(  
        this, 
        &DisplayItemGroup::finishHideAnimation
    ));
    
    _animationController->init();
    cefix::AnimationFactory::instance()->add(_animationController.get());
}

// --------------------------------------------------------------------------------------------------------------------
// 
// --------------------------------------------------------------------------------------------------------------------

void DisplayItemGroup::finishHideAnimation() {
	// nur ausschalten, wenn wir wirklich nicht mehr sichtbar sind, kann ja sein, dass wir schon wieder am einblenden sind
	if (!isVisible() && _transform.valid()) 
		_transform->setNodeMask(0);
	_hideFinished = true;
}


// ----------------------------------------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------------------------------------

void DisplayItemGroup::finishShowAnimation() {
	_showFinished = true;
}



// --------------------------------------------------------------------------------------------------------------------
// languageChanged
// --------------------------------------------------------------------------------------------------------------------

void DisplayItemGroup::languageChanged() {
    for(ItemVector::iterator itr = _items.begin(); itr != _items.end(); itr++) {
        (*itr)->languageChanged();
    }
}
    


// --------------------------------------------------------------------------------------------------------------------
// erzeugt hide-animation
// --------------------------------------------------------------------------------------------------------------------

void DisplayItemGroup::createHideAnimation(cefix::AnimationController* ctrl, float starttime) {
    float maxstarttime = 0;
	for (ItemsAnimationStartTimeMap::iterator i = _starttimes.begin(); i != _starttimes.end(); i++) {
		maxstarttime = osg::maximum(maxstarttime, i->second);
	}

    for(int i = _items.size() - 1; i >=0; i--) {
        _items[i]->addAnimation(ctrl, starttime + maxstarttime -_starttimes[_items[i]->getId()], true);
	}
	//osg::notify(osg::ALWAYS) << "ContentLayerBase:: animation finished" << std::endl;
}

// --------------------------------------------------------------------------------------------------------------------
// erzeugt show-animation
// --------------------------------------------------------------------------------------------------------------------

void DisplayItemGroup::createShowAnimation(cefix::AnimationController* ctrl, float starttime)  {
    	
    for(unsigned int i = 0; i < _items.size(); i++) {
        _items[i]->addAnimation(ctrl, starttime + _starttimes[_items[i]->getId()], false);
    }
}


}