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

#ifndef CEFIX_OPTIMIZER_CALLBACK
#define CEFIX_OPTIMIZER_CALLBACK

#include <osgUtil/Optimizer>
#include <cefix/TextGeode.h>
#include <cefix/Export.h>

namespace cefix {

class CEFIX_EXPORT CefixOptimizerCallback : public osgUtil::Optimizer::IsOperationPermissibleForObjectCallback {
	public:
		virtual bool isOperationPermissibleForObjectImplementation (const osgUtil::Optimizer *optimizer, const osg::Node *node, unsigned int option) const {
			if ((dynamic_cast<const TextGeodeBase*>(node)))
					return false;
		
			
			return true;
		}
	protected:
		virtual ~CefixOptimizerCallback() {}

};

}

#endif