/*
 *  GroupWidget.cpp
 *  cefixWidgets
 *
 *  Created by Stephan Huber on 04.06.07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "GroupWidget.h"
#include <cefix/WidgetFactory.h>
namespace cefix {


GroupWidget::GroupWidget(cefix::PropertyList* pl) : 
	AbstractWidget(pl->get("id")->asString()), 
	_group(new osg::Group()) ,
	_firstRun(true)
{
	_group->setName(pl->get("id")->asString()+" Widget");
	// group->setUserData(this); // add a reference of this WidgetGroup to the group, so we don't get deleted;
}

GroupWidget::~GroupWidget()
{
	clear();
}



void GroupWidget::add(AbstractWidget* widget) 
{ 
	if (!widget) return;
	WidgetMap::iterator i = _map.find(widget->getIdentifier());
	if (i != _map.end()) {
		if (i->second.get() == widget) {
			osg::notify(osg::INFO) << "GroupWidget :: Widget already member of this group! " << widget->getIdentifier() << std::endl;
			return;
		}
		std::string new_id = widget->getIdentifier();
		unsigned int cnt(0);
		while( _map.find(new_id) != _map.end()) {
			++cnt;
			new_id = widget->getIdentifier() + "_" + cefix::intToString(cnt);
		}
		osg::notify(osg::INFO) << "GroupWidget :: Renaming widget from " << widget->getIdentifier() << " to " << new_id <<  std::endl;
		widget->setIdentifier(new_id);
		add(widget);
	} else {
		_map[widget->getIdentifier()] = widget;
		_group->addChild(widget->getNode());
		widget->addParent(this);
	}
}



/** remove widget */
void GroupWidget::remove(AbstractWidget* widget) {
	if (widget) remove(widget->getIdentifier());
}




/** remove widget with given identifier */
void GroupWidget::remove(std::string identifier) {
	WidgetMap::iterator i = _map.find(identifier);
	if (i != _map.end()) {
		(*i).second->removeParent(this);
		_group->removeChild((*i).second->getNode());
		_map.erase(i);
	}
}



/** clear removes all children from this group */		
void GroupWidget::clear() 
{
	for(WidgetMap::iterator i = _map.begin(); i != _map.end(); ++i) 
	{
		(*i).second->removeParent(this);
		_group->removeChild((*i).second->getNode());
	}
	_map.clear();
}



/** update all widgets of this group */
void GroupWidget::update() {
	for (WidgetMap::iterator i = _map.begin(); i != _map.end(); ++i) {
		(*i).second->update();
	}
}



/** return the graphical representation for the scenegraph */
osg::Node* GroupWidget::getNode() { 
	if (_firstRun) {
		_firstRun = false;
		update();
	}
	return _group.get(); 
}


}

