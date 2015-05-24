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

#ifndef NODE_ANIMATION_HEADER
#define NODE_ANIMATION_HEADER

#include <osg/PositionAttitudeTransform>
#include <osg/MatrixTransform>
#include <cefix/Timeline.h>
#include <cefix/MathUtils.h>
#include <cefix/KeyFrameInterpolator.h>
#include <cefix/Serializer.h>
#include <cefix/LookAtTransform.h>

#include <cefix/EulerAngle.h>
#include <cefix/KeyframeBasedAction.h>
#include <cefix/SplineBasedAction.h>


namespace cefix {

/** base class for Timeline actions acting on osg-nodes */
class ManipulateNodeActionBase  {
public:
	/// ctor
	ManipulateNodeActionBase() : _nameOfNodeToAttach("") {}
	
	/// get the name of the node to attach
	const std::string& getNameOfNodeToAttach() const { return _nameOfNodeToAttach; }
	
	/// set the name of node to attach
	void setNameOfNodeToAttach(const std::string& name) { _nameOfNodeToAttach = name; }
	
	
	void writeTo(cefix::Serializer& serializer);
	void readFrom(cefix::Serializer& serializer);
	
	/// overridden by subclasses, should return true, if the given node is suitable for the action
	virtual bool attachNode(osg::Node* node) { return false; }
	
	bool attachToNamedNode(const std::string& name, osg::Node* node);
	
	virtual ~ManipulateNodeActionBase() {}
private:
	std::string _nameOfNodeToAttach;

};


/** template class encapsulating the storage of the node to act on and setting it automagically */
template <class NodeType>
class ManipulateNodeActionT : public ManipulateNodeActionBase {
public:
	/// ctor
	ManipulateNodeActionT() : ManipulateNodeActionBase(), _node(NULL) {}
	
	/// get the node
	inline NodeType* getNode() const { return _node.get(); }
	
	/// set the node
	inline void setNode(NodeType* node) { _node->setDataVariance(osg::Object::DYNAMIC); _node = node; }
	
	/// attach the node ( suitable for inherited classes of the core osg-classes, these are not handled by the NodeVisitor
	virtual bool attachNode(osg::Node* node) 
	{
		_node = dynamic_cast<NodeType*>(node);
        if (_node.valid()) _node->setDataVariance(osg::Object::DYNAMIC);
		return _node.valid();
	}
protected:
	osg::ref_ptr<NodeType> _node;
};



/// Bridge-template, cancatenating a ManipulateNodeActionT-template-class with a baseclass, forming one class
template <class BaseClass, class NodeType>
class ManipulateNodeAction : public BaseClass, public ManipulateNodeActionT<NodeType> {
public:
	typedef ManipulateNodeAction<NodeType, BaseClass> base_class;
	
	/// ctor
	ManipulateNodeAction() : BaseClass(), ManipulateNodeActionT<NodeType>() {}
	
	
	virtual void writeTo(cefix::Serializer& serializer) {
		BaseClass::writeTo(serializer);
		ManipulateNodeActionT<NodeType>::writeTo(serializer);
	}
	
	virtual void readFrom(cefix::Serializer& serializer)
	{
		BaseClass::readFrom(serializer);
		ManipulateNodeActionT<NodeType>::readFrom(serializer);
	}

};



/**
 * NodeManipulationTimeline is a subclassed Timeline qhich handles actions acting on nodes. It can attach named nodes of a scenegraph automatically to their corresponding actions, 
 * if their namesOfNodesToattach is setted right. 
 */
class NodeManipulationTimeline : public cefix::Timeline {

public:
	/// ctor
	NodeManipulationTimeline();
	
	/**
	 * attach Nodes to their actions, in a first step all actions are collected which have a nameOfNodeToAttach. Then in a next step, all nodes are traversed and every matched node's name is
	 * attached to the action, either via setNode or attachNode
	 */
	void attachNodesToActions(osg::Node* n);	
	
	/** set a default name of node to operate on for all actions */
	void setDefaultNameOfNodeToAttachTo(const std::string& name) {_defaultNameOfNodeToAttachTo = name; }
	
	/** apply the default name if applicable */
	virtual void newActionAdded(Action* action) 
	{ 
		ManipulateNodeActionBase* a = dynamic_cast<ManipulateNodeActionBase*>(action);
		if (a && a->getNameOfNodeToAttach().empty()) 
			a->setNameOfNodeToAttach(_defaultNameOfNodeToAttachTo); 
	}
	
	void readFrom(cefix::Serializer& serializer) { cefix::Timeline::readFrom(serializer); }
	void writeTo(cefix::Serializer& serializer) { cefix::Timeline::writeTo(serializer); }
	
	bool attachActionToNamedNode(ManipulateNodeActionBase* action, const std::string& nameOfNode, osg::Node* node);
		
protected:
	
	
	/** template helper function, tries to attach a particular node to an action */
	template <typename NodeType> bool attachNodeToAction(NodeType* node, ManipulateNodeActionBase* action) 
	{
		ManipulateNodeActionT<NodeType>* templated_action = dynamic_cast< ManipulateNodeActionT<NodeType>* >(action);
		if(templated_action) 
		{
			if (templated_action->getNode() == NULL) 
			{
				osg::notify(osg::DEBUG_INFO) << "action " << templated_action << " attached to node " << node << " " << node->getName() << "/" << node->className() << std::endl;
				templated_action->setNode(node);
			}
			// falls getNode schon was hat, gehen wir davon aus, dass diese Action versorgt ist
			return true;
		} else 
		{
			if (action->attachNode(node)) 
			{
				osg::notify(osg::DEBUG_INFO) << "action " << action << " attached to node " << node << " " << node->getName() << "/" << node->className() << std::endl;
				return true;
			}
		}
		return false;
	}
	
	/** insert a transform-node if necessary */
	bool replaceNodeWithTransformAndAttachToAction(osg::Node* node, ManipulateNodeActionBase* action) 
	{
		if (replaceNodeWithTransformAndAttachToAction<osg::PositionAttitudeTransform>(node, action))
			return true;
		
		if (replaceNodeWithTransformAndAttachToAction<osg::MatrixTransform>(node, action))
			return true;
						
		return false;
	}
	
	/** insert a transform-node if necessary */
	template <typename NodeType> bool replaceNodeWithTransformAndAttachToAction(osg::Node* node, ManipulateNodeActionBase* action)
	{
		ManipulateNodeActionT<NodeType>* templated_action = dynamic_cast< ManipulateNodeActionT<NodeType>* >(action);
		
		if (!templated_action) return false;
		
		osg::ref_ptr<NodeType> transform = new NodeType();
		osg::ref_ptr<osg::Node> storage = node;
		
		for(unsigned int i = 0; i < node->getNumParents(); ++i) {
			node->getParent(i)->replaceChild(node, transform.get());
		}
		
		transform->addChild(storage.get());
		
		return attachNodeToAction(transform.get(), action);
	}
	
	/** find all action with a nameOfNodeToAttach */
	void findManipulateNodeActions(cefix::Timeline* tl, std::vector<ManipulateNodeActionBase*>& found_actions);
	
	std::string _defaultNameOfNodeToAttachTo;
	
friend class AttachTimelinesNodeVisitor;
};

}

#endif