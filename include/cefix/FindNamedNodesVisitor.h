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

#ifndef CEFIX_FIND_NAMED_NODES_VISITOR_HEADER
#define CEFIX_FIND_NAMED_NODES_VISITOR_HEADER

#include <osg/NodeVisitor>

namespace cefix {

class FindNamedNodesVisitor : public osg::NodeVisitor {
public:
	typedef std::vector<osg::Node*> ResultVector;
	
	FindNamedNodesVisitor(const std::string& name, ResultVector& results) : osg::NodeVisitor(TRAVERSE_ALL_CHILDREN), _name(name), _results(results) {}
	
	virtual void apply(osg::Node& node) 
	{
		if (node.getName() == _name)
		{
			_results.push_back(&node);
		}
		traverse(node);
	}
	
private:
	std::string _name;
	ResultVector& _results;

};


}

#endif
