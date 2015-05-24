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

#ifndef ANIMATE_REMOVE_NODE_FROM_GROUP_HEADER_
#define ANIMATE_REMOVE_NODE_FROM_GROUP_HEADER_

#include <cefix/AnimationBase.h>
#include <osg/Node>
#include <osg/Group>

namespace cefix {

/** this class implements an "animation", when it's the right time, this object removes a node from a group */
class CEFIX_EXPORT AnimateRemoveNodeFromGroup : public cefix::AnimationBase {

	public:
        /** ctor
         *  @param starttime time to start the removal
         *  @param nodeToRemove the node to remove
         *  @param paentGroup   the group, from which the node gets removed */
		AnimateRemoveNodeFromGroup(	float starttime,
									osg::Node* nodeToRemove,
									osg::Group* parentGroup) :
			cefix::AnimationBase(starttime),
			_node(nodeToRemove),
			_group(parentGroup) 
		{
		}
        /** cleanup, called, when the animation is finished -- here the node gets removed. */
		virtual void cleanUp();
		
	protected:
        /** animate, only called once, so the cleanUp-handler can catchup and remove the node */
		virtual void animate(float elapsed_time) {
			setFinishedFlag();
		}
		
		virtual ~AnimateRemoveNodeFromGroup() {}
		
		osg::ref_ptr<osg::Node> _node;
		osg::ref_ptr<osg::Group> _group;
};

}
#endif
		
