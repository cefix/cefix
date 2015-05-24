/*
 *  NodeTimeline.cpp
 *  cefixSketch
 *
 *  Created by Stephan Huber on 04.11.09.
 *  Copyright 2009 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */

#include "NodeManipulationTimeline.h"
#include <cefix/Serializer.h>
#include <osg/Geode>
#include <osg/Billboard>
#include <osg/Switch>
#include <osg/LOD>
#include <osg/ProxyNode>
#include <osg/Projection>
#include <osg/CoordinateSystemNode>
#include <osg/ClipNode>
#include <osg/TexGenNode>
#include <osg/LightSource>
#include <osg/Sequence>
#include <osg/OccluderNode>
#include <osg/PagedLOD>
#include <osg/CameraView>
#include <osg/OcclusionQueryNode>

#include <cefix/FindNamedNodesVisitor.h>


namespace cefix {

void ManipulateNodeActionBase::writeTo(cefix::Serializer& serializer)
{
	serializer << "name_of_node_to_attach" << _nameOfNodeToAttach;
}


void ManipulateNodeActionBase::readFrom(cefix::Serializer& serializer)
{
	serializer >> "name_of_node_to_attach" >> _nameOfNodeToAttach;
}

bool ManipulateNodeActionBase::attachToNamedNode(const std::string& nameOfNode, osg::Node* node)
{
	FindNamedNodesVisitor::ResultVector results;
	FindNamedNodesVisitor v(nameOfNode, results);
	node->accept(v);
	
	if (results.size() == 0)
		return false;
	
	return attachNode(*results.begin());
	
}




NodeManipulationTimeline::NodeManipulationTimeline() 
:	cefix::Timeline()
{
}



void NodeManipulationTimeline::findManipulateNodeActions(cefix::Timeline* tl, std::vector<ManipulateNodeActionBase*>& found_actions)
{	
	for(cefix::Timeline::iterator i = tl->begin(); i != tl->end(); ++i)
	{
		ManipulateNodeActionBase* mnab = dynamic_cast<ManipulateNodeActionBase*>(i->second.get());
		if (mnab && (!mnab->getNameOfNodeToAttach().empty())) {
			found_actions.push_back(mnab);
			osg::notify(osg::DEBUG_INFO) << "found action: " << mnab << " for node-name " << mnab->getNameOfNodeToAttach() << std::endl;
		}	
		cefix::TimelineAsActionAdapter* taa = dynamic_cast<cefix::TimelineAsActionAdapter*>(i->second.get());
		if (taa) {
			findManipulateNodeActions(taa->getTimeline(), found_actions);
		}
	}

}

	

class AttachTimelinesNodeVisitor: public osg::NodeVisitor {
public:
	typedef std::multimap<std::string, ManipulateNodeActionBase*> ActionMap;
	typedef std::vector< std::pair<osg::Node*, ManipulateNodeActionBase*> > NodesToAttachVector;
	
	
	AttachTimelinesNodeVisitor(NodeManipulationTimeline* tl, std::vector<ManipulateNodeActionBase*>& found_nodes) : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN), _tl(tl)
	{
		for(std::vector<ManipulateNodeActionBase*>::iterator i = found_nodes.begin(); i != found_nodes.end(); ++i) {
			_actions.insert(std::make_pair((*i)->getNameOfNodeToAttach(), *i));
		}
	}
	
	template <typename NodeType> void findActionsAndAttachNode(NodeType* node, bool addToFoundNodes= false) 
	{
		//std::cout << node->getName() << " " << node->className() << std::endl;
		
		ActionMap::iterator i = _actions.find(node->getName());
		std::pair<ActionMap::iterator, ActionMap::iterator> range = _actions.equal_range(node->getName());
		std::vector<ActionMap::iterator> to_delete;
		for (ActionMap::iterator it = range.first; it != range.second; ++it) 
		{
			if (_tl->attachNodeToAction(node, it->second))
			{
				to_delete.push_back(it);
			} else 
			{
				if (addToFoundNodes) {
					osg::notify(osg::DEBUG_INFO) << "action " << it->second << " could not be added to " << it->first << ", missing transform? added for later " << typeid(NodeType).name() << std::endl;
					_foundNodes.push_back(std::make_pair(node, it->second));
				}
			}			
		}
		
		for(std::vector<ActionMap::iterator>::iterator i = to_delete.begin(); i != to_delete.end(); ++i) 
		{
			osg::notify(osg::DEBUG_INFO) << "action " << (*i)->second << " deleted from action-map" << std::endl;
			
			_actions.erase( *i );
		}
	}
	
	virtual void apply(osg::Node& node)							{	findActionsAndAttachNode(&node, true); }
	virtual void apply(osg::Geode& node)						{	findActionsAndAttachNode(&node); apply( (osg::Node&) node); }
	virtual void apply(osg::Billboard& node)					{	findActionsAndAttachNode(&node); apply( (osg::Geode&) node); }
        
	virtual void apply(osg::Group& node)						{	findActionsAndAttachNode(&node); apply( (osg::Node&) node); traverse(node); }
    virtual void apply(osg::ProxyNode& node)					{	findActionsAndAttachNode(&node); apply( (osg::Group&) node); }

    virtual void apply(osg::Projection& node)					{	findActionsAndAttachNode(&node); apply( (osg::Group&) node); }

    virtual void apply(osg::CoordinateSystemNode& node)			{	findActionsAndAttachNode(&node); apply( (osg::Group&) node); }

    virtual void apply(osg::ClipNode& node)						{	findActionsAndAttachNode(&node); apply( (osg::Group&) node); }
    virtual void apply(osg::TexGenNode& node)					{	findActionsAndAttachNode(&node); apply( (osg::Group&) node); }
    virtual void apply(osg::LightSource& node)					{	findActionsAndAttachNode(&node); apply( (osg::Group&) node); }

    virtual void apply(osg::Transform& node)					{	findActionsAndAttachNode(&node); apply( (osg::Group&) node); }
    virtual void apply(osg::Camera& node)						{	findActionsAndAttachNode(&node); apply( (osg::Transform&) node); }
    virtual void apply(osg::CameraView& node)					{	findActionsAndAttachNode(&node); apply( (osg::Transform&) node); }
    virtual void apply(osg::MatrixTransform& node)				{	findActionsAndAttachNode(&node); apply( (osg::Transform&) node); }
    virtual void apply(osg::PositionAttitudeTransform& node)	{	findActionsAndAttachNode(&node); apply( (osg::Transform&) node); }

    virtual void apply(osg::Switch& node)						{	findActionsAndAttachNode(&node); apply( (osg::Group&) node); }
    virtual void apply(osg::Sequence& node)						{	findActionsAndAttachNode(&node); apply( (osg::Group&) node); }
    virtual void apply(osg::LOD& node)							{	findActionsAndAttachNode(&node); apply( (osg::Group&) node); }
    virtual void apply(osg::PagedLOD& node)						{	findActionsAndAttachNode(&node); apply( (osg::LOD&) node); }
    virtual void apply(osg::ClearNode& node)					{	findActionsAndAttachNode(&node); apply( (osg::Group&) node); }
    virtual void apply(osg::OccluderNode& node)					{	findActionsAndAttachNode(&node); apply( (osg::Group&) node); }
    virtual void apply(osg::OcclusionQueryNode& node)			{	findActionsAndAttachNode(&node); apply( (osg::Group&) node); }

		
	virtual void doAttaches()
	{
		for(NodesToAttachVector::iterator i = _foundNodes.begin(); i != _foundNodes.end(); ++i) 
		{
			if (_tl->replaceNodeWithTransformAndAttachToAction(i->first, i->second) == false) {
				log::error("AttachTimelinesNodeVisitor") << "could not attach action " << i->second << " to node " << i->first << ": " << i->second->getNameOfNodeToAttach() << " / " << i->first->getName() << " " << i->first->className() << std::endl;
			}
		}
	}
	
private:
	ActionMap _actions;
	NodesToAttachVector _foundNodes;
	NodeManipulationTimeline* _tl;
};


void NodeManipulationTimeline::attachNodesToActions(osg::Node* n)
{
	std::vector<ManipulateNodeActionBase*> actions;
	findManipulateNodeActions(this, actions);
	AttachTimelinesNodeVisitor atlnv(this, actions);
	n->accept(atlnv);
	atlnv.doAttaches();
	
	init();
	
	
}




}