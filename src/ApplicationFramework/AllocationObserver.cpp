/*
 *  TextureAllocationObserver.cpp
 *  cefix
 *
 *  Created by Stephan Huber on 28.03.07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 
 
 */

#include<osg/Node>
#include<osg/Geode>
#include<osg/Billboard>
#include<osg/Group>
#include<osg/ProxyNode>
#include<osg/Projection>
#include<osg/CoordinateSystemNode>
#include<osg/ClipNode>
#include<osg/TexGenNode>
#include<osg/LightSource>
#include<osg/Transform>
#include<osg/Camera>
#include<osg/CameraView>
#include<osg/MatrixTransform>
#include<osg/PositionAttitudeTransform>
#include<osg/Switch>
#include<osg/Sequence>
#include<osg/LOD>
#include<osg/PagedLOD>
#include<osg/ClearNode>
#include<osg/OccluderNode>


#include "AllocationObserver.h"
#include <cefix/AnimationFactory.h>
#include <cefix/LineStatistics.h>
#include <cefix/Settings.h>
#include <cefix/DataFactory.h>

namespace cefix {
	
	class AddObservablesNodeVisitor : public osg::NodeVisitor {
		
	public:
		AddObservablesNodeVisitor() : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN ) { setTraversalMask(0xffffffff); }
		
		virtual void apply(osg::Node &node) {
			traverseStateSets(node.getStateSet());
			cefix::AllocationObserver::instance()->observe(&node); 
			traverse(node);
		}
		virtual void apply (osg::Geode &node){ 
			traverseStateSets(node.getStateSet());
			traverseDrawables(&node); 
			cefix::AllocationObserver::instance()->observe(&node); 
			traverse(node); 
		}
		
	private:
		void traverseStateSets(osg::StateSet* ss) {
			if (ss == NULL) 
				return;
			cefix::AllocationObserver::instance()->observe(ss); 
			
			osg::StateSet::AttributeList al = ss->getAttributeList();
			for(osg::StateSet::AttributeList::iterator i = al.begin(); i!= al.end(); ++i) {
				cefix::AllocationObserver::instance()->observe(i->second.first.get()); 
			}
			for(	osg::StateSet::TextureAttributeList::iterator i = ss->getTextureAttributeList().begin(); 
				i!= ss->getTextureAttributeList().end(); 
				++i) 
			{	
				osg::StateSet::AttributeList tl = (*i);
				for(osg::StateSet::AttributeList::iterator j = tl.begin(); j != tl.end(); ++j) {
					cefix::AllocationObserver::instance()->observe(j->second.first.get()); 
				}
			}
			
		}
		
		void traverseDrawables(osg::Geode* geode) {
			
			for(unsigned int i = 0; i < geode->getNumDrawables(); ++i) {
				osg::Drawable* drawable = geode->getDrawable(i);
				cefix::AllocationObserver::instance()->observe(drawable);
				traverseStateSets(drawable->getStateSet());
				
			}
		}
		
	};
	


// ----------------------------------------------------------------------------------------------------------
// instance
// ----------------------------------------------------------------------------------------------------------

AllocationObserver* AllocationObserver::instance() 
{
	static osg::ref_ptr<AllocationObserver> s_tao = NULL;
	static OpenThreads::Mutex mutex;
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mutex);
    
	if (!s_tao.valid()) {
		s_tao = new AllocationObserver();
    }
	
	return s_tao.get();
}

void AllocationObserver::addAsAnimation() 
{
	if (!_addedAsAnimation) {
		cefix::AnimationFactory::instance()->add(this);
		_addedAsAnimation = true;
	}
}

// ----------------------------------------------------------------------------------------------------------
// ctor
// ----------------------------------------------------------------------------------------------------------

AllocationObserver::AllocationObserver() :	
	cefix::AnimationBase(0, true),
	_last(0),
	_lastValue(0),
	_enabled(false),
	_prefsKey(""),
	_numSamples(500),
	_addedAsAnimation(false)
	
{
	_samplingTime = 1.0f;
}


void AllocationObserver::observe(const std::string& key, osg::Referenced* o)
{
	if (!_enabled) return;
    
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
	
	bool found = (_enabledKeys.find(key) != _enabledKeys.end());

	if ( (found ) && (isUnique(key, o)) ) 
    {
		_observed[key].push_back(o);
	} /*
	  else if (!found)
		std::cout << key << " " <<  "not found " << std::endl;
		*/
	
}

// ----------------------------------------------------------------------------------------------------------
// createStatistics
// ----------------------------------------------------------------------------------------------------------

void AllocationObserver::createStatistics() {
	_group = new StatisticsGroup();
	
	for(std::map<std::string, ObservedList>::iterator i = _observed.begin(); i != _observed.end(); ++i) {
		addStatistics(i->first);
	}
	
}


// ----------------------------------------------------------------------------------------------------------
// addStatistics
// ----------------------------------------------------------------------------------------------------------

void AllocationObserver::addStatistics(std::string key) {
	unsigned int numValues = _numSamples;
	
	LineStatistics* ls = new LineStatistics(key, numValues);
	_group->addStatistic(key, ls);
	LineStatistics* lsref = new LineStatistics(key+" Refs", numValues);
	_group->addStatistic(key+" Refs", lsref);
}

// ----------------------------------------------------------------------------------------------------------
// checkList
// ----------------------------------------------------------------------------------------------------------

void AllocationObserver::checkList(const std::string& key, ObservedList& alist, bool addValue) 
{
    
	unsigned int deleted(0), active(0), refcounts(0);
	
	for(ObservedList::iterator i = alist.begin(); i!=alist.end(); ) {
	
		if ((*i).valid()) {
			++active;
				refcounts += (*i)->referenceCount();
			++i;
		} 
		else {
			i = alist.erase(i);
			++deleted;
		}
	}	
	if (!_group.valid())
		std::cout << std::setw(10) << key << ": " << std::setw(4) << active << " (" << std::setw(4) <<  refcounts << ") " << std::setw(2) << deleted << "-" << " | ";
	else {
		
		if (!_group->hasStatistic(key))
			addStatistics(key);
			
		if (addValue) {
			_group->addValue(key, active, deleted);
			_group->addValue(key+" Refs", refcounts, 0);
		} else {
			_group->mergeValue(key, active, deleted);
			_group->mergeValue(key+" Refs", refcounts, 0);
		}
	}	
}




// ----------------------------------------------------------------------------------------------------------
// animate
// ----------------------------------------------------------------------------------------------------------

void AllocationObserver::animate(float elapsed) {
	
	if (elapsed < _last + _samplingTime)
		return;
	
	bool newValue = (elapsed >= _lastValue + _samplingTime);

    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);

	// traverse scenegraph-node
	if (_observedNode.valid()) {
		AddObservablesNodeVisitor nv;
		_observedNode->accept(nv);
	}

	if (_group.valid()) _group->clear();	
	for(std::map<std::string, ObservedList>::iterator i = _observed.begin(); i != _observed.end(); ++i) {
		checkList(i->first, i->second, newValue);
	}
	if (!_group.valid()) 
		std::cout << std::endl;
	else
		_group->adjustLocations();
	_last = elapsed;
	if (newValue)
		_lastValue = elapsed;
	
	if (!_enabled) {
		_addedAsAnimation = false;
		setFinishedFlag();
		return;
	}
}	


// ----------------------------------------------------------------------------------------------------------
// isUnique
// ----------------------------------------------------------------------------------------------------------
bool AllocationObserver::isUnique(std::string key, osg::Referenced* o) {

	ObservedList& alist = _observed[key];
	
	for(ObservedList::iterator i = alist.begin(); i != alist.end(); ++i ) {
		if (((*i).valid()) && ((*i).get() == o)) {
			return false;
		}
	}
	
	return true;

}

void AllocationObserver::loadPrefs() 
{	
	cefix::PropertyList* pl = cefix::DataFactory::instance()->getPreferencesPropertyListFor(_prefsKey);
	if (pl == NULL) {
		return;
	}
	
	_enabledKeys.clear();

	if (pl->hasKey("enabled")) _enabled = (pl->get("enabled")->asInt() != 0);
	if (_enabled) addAsAnimation();
	
	if (pl->hasKey("numSamples")) _samplingTime = pl->get("numSamples")->asInt();
	if (pl->hasKey("samplingTime")) _samplingTime = pl->get("samplingTime")->asFloat();
	
	if (pl->hasKey("observe") && (pl->get("observe")->getType() == cefix::AbstractProperty::PROPERTY_LIST)) {
		pl = pl->get("observe")->asPropertyList();
		
		for(unsigned int i=0; i < pl->size(); ++i) {
			if ((pl->get(i)->getType() == cefix::AbstractProperty::INTEGER) && (pl->get(i)->asInt() != 0)) {
				osg::notify(osg::INFO) << "AllocationObserver :: observing '" << pl->getKeyAt(i) << "'" << std::endl;
				_enabledKeys.insert(std::make_pair(pl->getKeyAt(i), true));
			}
		};
	}
		
}

void AllocationObserver::dump(std::ostream& os)
{
	for(std::map<std::string, ObservedList>::iterator i = _observed.begin(); i != _observed.end(); ++i)
	{
		os << i->first << ": " << i->second.size() << std::endl;
		for(ObservedList::iterator j = i->second.begin(); j != i->second.end(); ++j) 
		{
			osg::Object* o = dynamic_cast<osg::Object*>((*j).get());
			if (o) os << "    " << o->getName() << " (" << o->className() << ")" << std::endl;
		}
	}
	
}

}
