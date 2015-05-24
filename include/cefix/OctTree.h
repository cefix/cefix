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

#ifndef CEFIX_OCTTREE_HEADER
#define CEFIX_OCTTREE_HEADER
#include <iomanip>
#include <osg/Referenced>
#include <list>
#include <vector>
#include <iostream>
#include <osg/Vec3>
#include <osg/Geometry>

namespace cefix {


/** 
 * templated function which helps getting the position out of an arbitrary class. 
 * Specialize it for your classes, if needed 
 */
template<class T, class VectorType> inline VectorType get_position_for_oct_tree(const T& t) 
{ 
	return t.getPosition(); 
}

/** OctTreeNode-template class. A octtree-node stores a data-node T and up to eight child-octtree-nodes
 *  MaxDepth is the maximal depth of the this octtree-node, 
 *	MaxChildsBeforeSubDivision is the amount of data-nodes a single octtree-node can hold 
 *	without subdividing itself
 */

template <class T, typename ValueType = int, typename VectorType = osg::Vec3, int MaxDepth = 6, int MaxChildsBeforeSubdivison = 5>
class OctTreeNode {

public:
	typedef typename std::list<T*> Childs;
	typedef OctTreeNode<T,ValueType, VectorType, MaxDepth,MaxChildsBeforeSubdivison> Node;
	
	///c'tor
	OctTreeNode(T* t, ValueType min_x, ValueType min_y, ValueType min_z, ValueType max_x, ValueType max_y, ValueType max_z, int depth, OctTreeNode* parent = NULL) 
	:	_ts(),
		_parent(parent),
		_minX(min_x),
		_minY(min_y),
		_minZ(min_z),
		_maxX(max_x),
		_maxY(max_y),
		_maxZ(max_z),
		_depth(depth),
		_hasChilds(false)
	{
		for(unsigned int i = 0; i < 8; ++i) 
		{
			_nodes[i] = NULL;
			//_nodes[i] = create(NULL, i); 
		}
		if(t) _ts.push_back(t);
	}
	
	/** adds an element, the tree stores only pointers to the elements, memory-management is your job
		(keep a ref_ptr around) */
	void add(T* t)
	{
		if (!_hasChilds && ((_ts.size() < MaxChildsBeforeSubdivison) || (_depth == MaxDepth))) {
			_ts.push_back(t);
			// std::cout << "added " << t << "(" << get_position_for_oct_tree<T, VectorType>(*t) << ") " << " into " << _minX<<"/"<<_minY<<"/"<<_minZ<<"x"<<_maxX<<"/"<<_maxY<<"/"<<_maxZ<<std::endl;
		}
		else 
		{
			for(typename Childs::const_iterator i = _ts.begin(); i != _ts.end(); ++i) {
				insert(*i);
			}
            
			insert(t);
			_ts.clear();
		}
	}
	/// d'tor
	~OctTreeNode() 
	{
		for(unsigned int i = 0; i < 8; ++i) 
		{
			if (_nodes[i]) 
			{ 
				delete _nodes[i]; 
				_nodes[i] = NULL; 
			}
		}
		_hasChilds = false;
	}
	
	
	/// removes an element
	void erase(T* t) 
	{
		_ts.erase(t);
		unsigned int ndx = computeIndex(get_position_for_oct_tree<T, VectorType>(*t)); 
		if (_nodes[ndx]) {
			_nodes[ndx]->erase(t);
			if(_nodes[ndx]->isEmpty()) {
				delete _nodes[ndx];
				_nodes[ndx] = NULL;
			}
		}
	}
	
	/// find all elements in a given range, add them to the results-vector
	void find_in_range(float minx, float miny, float minz, float maxx, float maxy, float maxz, std::vector<T*>& results) const 
	{
		if (!intersects(minx, miny, minz, maxx, maxy, maxz))
			return;
		for(typename Childs::const_iterator i = _ts.begin(); i != _ts.end(); ++i) {
			if (contains(get_position_for_oct_tree<T, VectorType>(**i), minx, miny, minz, maxx, maxy, maxz)) {
				results.push_back(*i);
			}
		}
		if (_hasChilds) {
			for(unsigned int i = 0; i < 8; ++i) {
				if (_nodes[i])
					_nodes[i]->find_in_range(minx, miny, minz, maxx, maxy, maxz, results);
			}
		}
	}
    
	/// find all elements in a given rang aroung pos, add result to a vector
    void find_in_range(const osg::Vec3& pos, float range, std::vector<T*>& results) {
        find_in_range(pos[0] - range, pos[1] - range, pos[2] - range, pos[0] + range, pos[1] + range, pos[2] + range, results);
    }
	
	
	/// update the tree from the stored elements' positions
	void update() 
	{
		for(unsigned int i = 0; i < 8; ++i) {
			if (_nodes[i]) _nodes[i]->update();
		}
		
		if (_ts.size() == 0) 
			return;
		
		Childs to_relocate;
		for(typename Childs::iterator i = _ts.begin(); i != _ts.end(); ) {
			if (contains(*i))
				++i;
			else {
				to_relocate.push_back(*i);
				i = _ts.erase(i);
			}
		}
		for(typename Childs::const_iterator i = to_relocate.begin(); i != to_relocate.end(); ++i) 
		{
			OctTreeNode* parent = this;
			while(parent && !parent->contains(*i))
				parent = parent->_parent;
			if (parent)
				parent->add(*i);
			else {
				// wenn wir hier landen, dann ist das partikel ausserhalb des Bereichs des OctTrees, deswegen erstmal wieder dazu, damit
				// wir es nicht verlieren
				_ts.push_back(*i);
			}
		}
	}
	
	/// get the size of the tree, the number of stored elements
	unsigned int size() const
	{
		unsigned int m = _ts.size();
		for(unsigned int i=0; i < 8; ++i) {
			if (_nodes[i]) m += _nodes[i]->size(); 
		}
		
		return m;
	}
	
	/// dump the contents of the tree into the console, for debugging purposes
	void dump(int depth) 
	{
		std::cout << std::setw(depth*3) << " " << "num childs: " << _ts.size() << std::endl;
		std::cout << std::setw(depth*3) << " " << "size      : " << _minX << "/" << _minY << "/" << _minZ << " - " << _maxX << "/" << _maxY << "/" << _maxZ << std::endl;
		for(typename Childs::iterator i = _ts.begin(); i != _ts.end(); ++i) 
		{
			std::cout << std::setw(depth*3) << " " << "          : " << get_position_for_oct_tree<T, VectorType>(**i) << std::endl;
		}
		
		for(unsigned int i = 0; i < 8; ++i) 
		{
			if(_nodes[i]) 
			{
				std::cout << std::setw(depth*3) << " " << i << ": " << std::endl;
				_nodes[i]->dump(depth+1);
			}
		}
	}
private:
	void insert(T* t) 
	{
		const VectorType& p = get_position_for_oct_tree<T,VectorType>(*t);
		int index = computeIndex(p);
		if (_nodes[index])
		{
			_nodes[index]->add(t);
		}
		else 
		{
			_nodes[index] = create(t, index);
		}
		_hasChilds = true;
	}
	
	OctTreeNode* create(T* t, int index) 
	{
		if (_depth >= MaxDepth) return NULL;
		
		float min_x(_minX), min_y(_minY), min_z(_minZ), max_x(_maxX), max_y(_maxY), max_z(_maxZ);
			
		if ((index == 0) || (index == 2) || (index == 4) || (index == 6))
			max_y = (_minY + _maxY) / static_cast<ValueType>(2);
		if ((index == 1) || (index == 3) || (index == 5) || (index == 7))
			min_y = (_minY + _maxY) / static_cast<ValueType>(2);
		
		if ((index == 0) || (index == 1) || (index == 4) || (index == 5))
			max_x = (_minX + _maxX) / static_cast<ValueType>(2);
		
		if ((index == 2) || (index == 3) || (index == 6) || (index == 7))
			min_x = (_minX + _maxX) / static_cast<ValueType>(2);
		
		if (index >= 4) 
			min_z = (_minZ + _maxZ) / static_cast<ValueType>(2);
		else 
			max_z = (_minZ + _maxZ) / static_cast<ValueType>(2);
			
		return new OctTreeNode(t, min_x, min_y, min_z, max_x, max_y, max_z, _depth+1, this);
	}
	
	
	bool isEmpty() {
		
		bool is_empty = true;
		for(unsigned int i = 0; i < 8; ++i) {
			if (_nodes[i]) {
				is_empty = false;
				break;
			}
		}
		
		return ((is_empty) && (_ts.size() == 0));
	}
	
	unsigned int computeIndex(const VectorType& p)
	{
		ValueType m_x = (_minX + _maxX) / static_cast<ValueType>(2);
		ValueType m_y = (_minY + _maxY) / static_cast<ValueType>(2);
		ValueType m_z = (_minZ + _maxZ) / static_cast<ValueType>(2);
		
		unsigned int ndx(0);
		if  (p.z() > m_z) 
			ndx +=4;
			
		if (p.x() > m_x) 
			ndx += 2;
		
		if  (p.y() > m_y) 
			ndx += 1;
		
		return ndx; 
	}
	
	inline bool contains(T* t) const {
		return contains(get_position_for_oct_tree<T, VectorType>(*t));
	}
	
	inline bool contains(const VectorType& p, float min_x, float min_y, float min_z, float max_x, float max_y, float max_z) const
	{
		return (
			(min_x <= p.x()) && (p.x() < max_x) && 
			(min_y <= p.y()) && (p.y() < max_y) && 
			(min_z <= p.z()) && (p.z() < max_z)
		);
	}
	
	inline bool contains(const VectorType& p) const
	{
		return contains(p, _minX, _minY, _minZ, _maxX, _maxY, _maxZ);
	}
	
	inline bool intersects(float min_x, float min_y, float min_z, float max_x, float max_y, float max_z) const
	{
		bool outside = 
			(max_x < _minX) || (max_y < _minY) || (max_z < _minZ) ||
			(min_x >= _maxX) || (min_y >= _maxY) || (min_z >= _maxZ);
		return !outside;
	}
	
	OctTreeNode* _nodes[8];
	OctTreeNode* _parent;
	Childs _ts;
	ValueType _minX, _minY, _minZ, _maxX, _maxY, _maxZ;
	int _depth;
	bool _hasChilds;

friend class OctTreeGeometry;

};


/** OctTree-template class. A octtree stores an octtree-node-node.
 *  MaxDepth is the maximal depth of  the octtree-nodes, 
 *	MaxChildsBeforeSubDivision is the amount of data-nodes a single octtree-node can hold 
 *	without subdividing itself
 */
template<class T, typename ValueType = int, class VectorType = osg::Vec3, int MaxDepth = 6, int MaxChildsBeforeSubDivision = 6>
class OctTree {
public:
	typedef OctTreeNode<T,ValueType, VectorType, MaxDepth,MaxChildsBeforeSubDivision> Node;
	
	/// c'tor
	OctTree(ValueType max_x, ValueType max_y, ValueType max_z) : _root(NULL), _maxX(max_x), _maxY(max_y),  _maxZ(max_z), _dirty(false), _cachedSize(0) {}
	
	/** adds an element, the tree stores only pointers to the elements, memory-management is your job
		(keep a ref_ptr around) */
	void add(T* t) 
	{ 
		if (!_root) 
		{ 
			_root = new Node(t, 0, 0, 0, _maxX, _maxY, _maxZ,0); 
		} 
		else 
		{ 
			_root->add(t); 
		} 
		_dirty = true;
	}
	
	/// removes an element
	void erase(T* t) {
		if (_root) _root->erase(t);
		_dirty = true;
	}
	
	/// find all elements in a given volume and add them to the given vector
	void find_in_range(float xmin, float ymin, float zmin, float xmax, float ymax, float zmax, std::vector<T*>&results) const
	{
		if (_root) 
		{
			_root->find_in_range(xmin, ymin, zmin, xmax, ymax, zmax, results);
		}
	}
    
	/// find all elements in a given volume and add them to the given vector
    void find_in_range(const osg::Vec3& pos, float range, std::vector<T*>& results) {
        _root->find_in_range(pos, range, results);
    }
	
	
	/// update the tree from all elements' positions
	void update() 
	{
		_dirty = true; 
		if(_root)
			_root->update();
	}
	
	
	/// remove all elements from the tree
	void clear() 
	{
		if (_root) 
			delete _root; 
		_root = NULL; 
	}
	
	// return the size of the tree, the number of stored elements, may be slow
	unsigned int size() const 
	{
		unsigned int m = _dirty ? (_root) ? _root->size() : 0 : _cachedSize;
		_dirty = false; 
		_cachedSize = m;
		return m;
	}
	
	/// dump the contents of this tree to the console
	void dump() {
		_root->dump(0);
	}
	
	/// d'tor
	virtual ~OctTree() 
	{ 
		clear();
	}
private: 
	 OctTreeNode<T, ValueType, VectorType, MaxDepth, MaxChildsBeforeSubDivision>* _root;
	 ValueType _maxX, _maxY, _maxZ;
	 mutable bool	_dirty;
	 mutable unsigned int _cachedSize;
friend class OctTreeGeometry;
};


template <class T, typename ValueType = int, class VectorType = osg::Vec3, int MaxDepth = 6, int MaxChildsBeforeSubDivision = 6>
class RefOctTree : public OctTree<T, ValueType, VectorType, MaxDepth, MaxChildsBeforeSubDivision> , public osg::Referenced {
public:
	RefOctTree(unsigned int x, unsigned int y, unsigned int z) : OctTree<T, ValueType, VectorType, MaxDepth, MaxChildsBeforeSubDivision>(x,y,z), osg::Referenced() {}

};



class OctTreeGeometry : public osg::Geometry 
{
public:
	OctTreeGeometry();
	
	template <class T>
	void visualize(const T& t) 
	{
		_vertices->clear();
		
		typename T::Node* root = t._root;
		if (root) addCube(root);
		
		_da->setCount(_vertices->size());
		dirtyDisplayList();
		dirtyBound();
	}

	
private:
	template <class T>
	void addCube(T* t) 
	{
		if (t->_ts.size() > 0) 
			drawCube(t->_minX, t->_minY, t->_minZ, t->_maxX, t->_maxY, t->_maxZ);
		for(unsigned int i = 0; i < 8; ++i) {
			if (t->_nodes[i])
				addCube(t->_nodes[i]);
		}
	}
	
	void drawCube(float min_x, float min_y, float min_z, float max_x, float max_y, float max_z);

	osg::ref_ptr<osg::Vec3Array> _vertices;
	osg::ref_ptr<osg::DrawArrays> _da;

};


}

#endif