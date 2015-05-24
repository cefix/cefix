/*
 *  AbstractWidget.cpp
 *  cefixWidgets
 *
 *  Created by Stephan Huber on 03.06.07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "AbstractWidget.h"
#include <algorithm>
namespace cefix {

// ----------------------------------------------------------------------------------------------------------
// informResponder
// ----------------------------------------------------------------------------------------------------------

void AbstractWidget::informAttachedResponder(const std::string& actionIdentifier) {
	
	if (!_enabled) return;
	
	// callstack: um Endlosschleifen zu vermeiden, zählen wir mit, wer wen wann aufgerufen hat, wenn wir ein zweites mal für eine action aufgerufen werden, 
	// dann brechen wir wieder ab
	typedef std::vector<AbstractWidget*> Callstack;
	static Callstack callstack;
	for(Callstack::iterator i = callstack.begin(); i != callstack.end(); ++i) {
	
		if ((*i) == this) {
			// rekursiv wirds jetzt
			return;
		}
	}
	callstack.push_back(this);
	
	for (ResponderVector::iterator i = _responder.begin(); i != _responder.end(); ++i) {
		if ((*i).valid()) {
			(*i)->respondToAction(actionIdentifier, this);
		}
	}
	
	for (ObservedResponderVector::iterator i = _observedResponder.begin(); i != _observedResponder.end(); ++i) {
		if ((*i).valid()) {
			(*i)->respondToAction(actionIdentifier, this);
		}
	}
	callstack.pop_back();
}


// ----------------------------------------------------------------------------------------------------------
// removeResponder
// ----------------------------------------------------------------------------------------------------------

void AbstractWidget::removeResponder(Responder* responder) {
	for (ResponderVector::iterator i = _responder.begin(); i != _responder.end(); ) {
	
		if (((*i).valid() == false) || (*i) == responder) {
			i = _responder.erase(i);
		} else {
			++i;
		}
	}
	
	for (ObservedResponderVector::iterator i = _observedResponder.begin(); i != _observedResponder.end(); ) {
	
		if (((*i).valid() == false) || (*i) == responder) {
			i = _observedResponder.erase(i);
		} else {
			++i;
		}
	}
}



void AbstractWidget::addParent(AbstractWidget* w) {
	_parents.push_front(w);
	setParent( _parents.front() );
}



void AbstractWidget::removeParent(AbstractWidget* w) {
	std::list<AbstractWidget*>::iterator itr = std::find(_parents.begin(), _parents.end(), w);
	if (itr != _parents.end())
		_parents.erase(itr);
	
	itr = _parents.begin();
	setParent( (itr == _parents.end()) ? NULL : (*itr) );
}




}