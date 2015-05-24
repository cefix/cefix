/*
 *
 *      (\(\
 *     (='.')           cefix --
 *    o(_")")           a multipurpose library on top of OpenSceneGraph
 *  
 *
 *                      Copyright 2001-2011, stephanmaximilianhuber.com 
 *                      Stephan Maximilian Huber. 
 *
 *                      All rights reserved.
 *
 */

#ifndef MISSING_IMAGE_PLACEHOLDER_CONTAINER_
#define MISSING_IMAGE_PLACEHOLDER_CONTAINER_

#include <osgDB/ReadFile>
#include <osg/Image>
#include <cefix/AbstractDataContainer.h>

namespace cefix {

/**
 * MissingImagePlaceHolderContainer holds the data for the placeholder for missing
 * images. 
 * load this container as usual with
 * <code>
 * MissingImagePlaceHolderContainer* miphc = DataFactory::instance()->getDataFor<MissingImagePlaceHolderContainer>("MissingImagePlaceHolder", "preferences.xml");
 * osg::Image* image = miphc->getPlaceHolder();
 * </code>
 */
class CEFIX_EXPORT MissingImagePlaceHolderContainer : public AbstractDataContainer {
	
	public:
		/** constructor */
		MissingImagePlaceHolderContainer() : AbstractDataContainer(), _placeHolder(NULL) {}
		/** @return the placeholder image */
		osg::Image* getPlaceholder() { return _placeHolder.get(); }
		/** discard - not needed */
		void discard() {};
	protected:
        /** disable queuing the load */
        virtual bool needsQueued() { return false; }
        
        /** load the xmlfile into a propertylist */
        virtual void load();
		
		virtual ~MissingImagePlaceHolderContainer() {}
		        
        osg::ref_ptr<osg::Image> _placeHolder;

};

}

#endif