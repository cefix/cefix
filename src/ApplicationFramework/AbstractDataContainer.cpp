/*
 *  AbstractDataContainer.cpp
 *  AudiHeritage
 *
 *  Created by Stephan Huber on 11.05.06.
 *  Copyright 2006 digitalmind.de // Stephan Maximilian Huber. All rights reserved.
 *
 */
#include <cefix/Notify.h>

#include <cefix/AbstractDataContainer.h>
#include <cefix/DataFactory.h>
#include <cefix/AllocationObserver.h>
#include <cefix/Settings.h>
#include <cefix/PropertyList.h>


namespace cefix {

AbstractDataContainer::AbstractDataContainer() : osg::Referenced(), _loadFinished(false), _queued(false), _key(""), _cancelled(false) {
    static unsigned int s_id = 0;
    _id = s_id++;
	
	cefix::AllocationObserver::instance()->observe(this);
}

AbstractDataContainer::~AbstractDataContainer()
{
}

void AbstractDataContainer::requestLoad() {
    _cancelled = false;
    DataFactory::instance()->addToLoadQueue(this);
}

bool AbstractDataContainer::setPropertyList(cefix::PropertyList* pl) 
{ 
	_pl = pl; 
	return isPropertyListApplicable(pl); 
}


std::string AbstractDataContainer::getFileName() const { 
	return (_pl.valid()) ? _pl->getXmlFileName() : ""; 
}

// --------------------------------------------------------------------------------------------------------------------
// error
// --------------------------------------------------------------------------------------------------------------------

void AbstractDataContainer::error(std::string error_msg) {
	std::string add_info = (_pl.valid()) ? " ("+_pl->getXmlFileName()+")" : getKey();
	std::string err =  add_info + " // "+error_msg;
	if (_queued) // fehlermeldungen beim laden via LoadQueue müssen speziell behandelt werdenv
		_errors.push_back(err);
	else
		cefix::notify("addToErrorLog",err);
}


// --------------------------------------------------------------------------------------------------------------------
// error
// --------------------------------------------------------------------------------------------------------------------

void AbstractDataContainer::error(std::string msg, const ErrorMessageVector& err_msg_vec) {
	if (err_msg_vec.size() == 0)
		return;
		
	for(ErrorMessageVector::const_iterator i = err_msg_vec.begin(); i != err_msg_vec.end(); i++) {
		error(msg + (*i));
	}
}

}