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

#ifndef CEFIX_QUADTREE_HEADER
#define CEFIX_QUADTREE_HEADER

#include <osg/Referenced>
#include <list>
#include <vector>
#include <algorithm>
#include <osg/Geometry>
namespace cefix {

/// templated function which helps getting the position out of an arbitrary class. Specialize it for your classes, if needed */
template<class T, class VectorType> inline VectorType get_position_for_quad_tree(const T& t) 
{ 
	return t.getPosition(); 
}

/** QuadtreeNode-template class. A quadtree-node stores a data-node T and up to four child-quadtree-nodes
    MaxDepth is the maximal depth of the this quad-tree-node, 
	MaxChildsBeforeSubDivision is the amount of data-nodes a single quadtree-node can hold without subdividing itself
	*/
template <class T, typename ValueType = osg::Vec2::value_type, typename VectorType = osg::Vec2, int MaxDepth = 6, int MaxChildsBeforeSubdivison = 5>
class QuadTreeNode {

public:
	typedef typename std::list<T*> Childs;
	typedef QuadTreeNode<T,ValueType, VectorType, MaxDepth,MaxChildsBeforeSubdivison> Node;
	
	/** ctor
	    min_x, min_y, max_x, max_y defines the rectangular area covered by this node */
	QuadTreeNode(T* t, ValueType min_x, ValueType min_y, ValueType max_x, ValueType max_y, int depth, QuadTreeNode* parent = NULL) 
	:	_ts(),
		_parent(parent),
		_minX(min_x),
		_minY(min_y),
		_maxX(max_x),
		_maxY(max_y),
		_depth(depth),
		_hasChilds(false)
	{
		for(unsigned int i = 0; i < 4; ++i) 
		{
			_nodes[i] = create(NULL, i); 
		}
		if(t) _ts.push_back(t);
	}
	
	/// add a T to this node, subdivide if necessary */
	void add(T* t)
	{
		if (!_hasChilds && ( (_depth == MaxDepth) || (_ts.size() < MaxChildsBeforeSubdivison))) {
			_ts.push_back(t);
			//std::cout << "added " << t << "(" << get_position_for_quad_tree(*t) << ") " << " into " << _minX<<"/"<<_minY<<"x"<<_maxX<<"/"<<_maxY << std::endl;
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
	
	/// dtor
	~QuadTreeNode() 
	{
		for(unsigned int i = 0; i < 4; ++i) 
		{
			if (_nodes[i]) 
			{ 
				delete _nodes[i]; 
				_nodes[i] = NULL; 
			}
		}
		_hasChilds = false;
	}
	
	
	/// remove T from this node
	void erase(T* t) 
	{
		typename Childs::iterator itr = std::find(_ts.begin(), _ts.end(), t);
        if (itr != _ts.end())
            _ts.erase(itr);
            
		unsigned int ndx = computeIndex(get_position_for_quad_tree<T, VectorType>(*t)); 
		if (_nodes[ndx]) {
			_nodes[ndx]->erase(t);
			if(_nodes[ndx]->isEmpty()) {
				delete _nodes[ndx];
				_nodes[ndx] = NULL;
			}
		}
	}
	
	
	/// find all T in the given range
	void find_in_range(ValueType l, ValueType t, ValueType r, ValueType b, std::vector<T*>& results) const 
	{
		if (!intersects(l,t,r,b))
			return;
		for(typename Childs::const_iterator i = _ts.begin(); i != _ts.end(); ++i) {
			if (contains(get_position_for_quad_tree<T, VectorType>(**i), l, t, r, b)) {
				results.push_back(*i);
			}
		}
		if (_hasChilds) {
			for(unsigned int i = 0; i < 4; ++i) {
				if (_nodes[i])
					_nodes[i]->find_in_range(l, t, r, b, results);
			}
		}
	}
	
	/// check all stored nodes if they have moved, and if so, move them to a new place in the quad-tree 
	void update() 
	{
		for(unsigned int i = 0; i < 4; ++i) {
			if (_nodes[i]) _nodes[i]->update();
		}
		
		
        for(typename Childs::iterator i = _ts.begin(); i != _ts.end(); ) {
			if (contains(*i)) 
            {
				++i;
			
            }
            else 
            {
				if (relocate(*i)) 
                {
                    i = _ts.erase(i);
                } 
                else 
                {
                    // wir konnten *i nicht unterbringen, deswegen sicherheitshalber in der liste lassen
                    ++i;
                }
			}
		}
	}
    
    inline void update(T* t) 
    {
        if (!contains(t)) {
            if (relocate(t)) {
                erase(t);
            }
        }
    }
    
    inline bool relocate(T* t) 
    {
        QuadTreeNode* parent = this;
        while(parent && !parent->contains(t))
            parent = parent->_parent;
        if (parent) {
            parent->add(t);
            return true;
        }
        return false;
    }
	
	
	/// get the size of the quad-tree
	unsigned int size() const
	{
		unsigned int m = _ts.size();
		for(unsigned int i=0; i < 4; ++i) {
			if (_nodes[i]) m += _nodes[i]->size(); 
		}
		
		return m;
	}
	
private:
	/// insert a T
	void insert(T* t) 
	{
		const VectorType& p = get_position_for_quad_tree<T,VectorType>(*t);
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
	
	
	/// create a child-node
	QuadTreeNode* create(T* t, int index) 
	{
		if (_depth >= MaxDepth) return NULL;
		
		float min_x(_minX), min_y(_minY), max_x(_maxX), max_y(_maxY);
			
		if ((index == 0) || (index == 2))
			max_x = (_minX + _maxX) / static_cast<ValueType>(2);
		if ((index == 1) || (index == 3))
			min_x = (_minX + _maxX) / static_cast<ValueType>(2);
		
		if ((index == 0) || (index == 1))
			max_y = (_minY + _maxY) / static_cast<ValueType>(2);
		
		if ((index == 2) || (index == 3))
			min_y = (_minY + _maxY) / static_cast<ValueType>(2);
		
		return new QuadTreeNode(t, min_x, min_y, max_x, max_y, _depth+1, this);
	}
	
	
	bool isEmpty() {
		
		bool is_empty = true;
		for(unsigned int i = 0; i < 4; ++i) {
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
		
		if (p.x() < m_x) {
			return (p.y() < m_y) ? 0 : 2;
		}
		return (p.y() < m_y) ? 1 : 3;
	}
	
	inline bool contains(T* t) const {
		return contains(get_position_for_quad_tree<T, VectorType>(*t));
	}
	
	inline bool contains(const VectorType& p, float l, float t, float r, float b) const
	{
		return ((l <= p.x()) && (p.x() < r) && (t <= p.y()) && (p.y() < b));
	}
	
	inline bool contains(const VectorType& p) const
	{
		return contains(p, _minX, _minY, _maxX, _maxY);
	}
	
	inline bool intersects(float l, float t, float r, float b) const
	{
		bool outside = (r < _minX) || (b < _minY) || (l >= _maxX) || (t >= _maxY);
		return !outside;
	}
	
	QuadTreeNode* _nodes[4];
	QuadTreeNode* _parent;
	Childs _ts;
	ValueType _minX, _minY, _maxX, _maxY;
	int _depth;
	bool _hasChilds;

friend class QuadTreeGeometry;

};


/** template class forming a quadtree, see quadTreeNode for more infos, this class holds the root-node of the quadtree and 
    channels all requests to it */
	
template<class T, typename ValueType = int, class VectorType = osg::Vec2, int MaxDepth = 6, int MaxChildsBeforeSubDivision = 6>
class QuadTree {
public:
	typedef QuadTreeNode<T,ValueType, VectorType, MaxDepth,MaxChildsBeforeSubDivision> Node;
	
	/// c'tor, width x height defines the dimensions this quad-tree can handle
	QuadTree(ValueType width, ValueType height) : _root(NULL), _left(0), _top(0), _width(width), _height(height), _dirty(false), _cachedSize(0) {}
    QuadTree(ValueType left, ValueType top, ValueType width, ValueType height) : _root(NULL), _left(left), _top(top), _width(width), _height(height), _dirty(false), _cachedSize(0) {}
	
	/** 
	 * add an element to the tree, the tree stores only a pointer, so do the memory management by yourself 
	 * (keep a ref_ptr to the object around)
	 */
	void add(T* t) 
	{ 
		if (!_root) 
		{ 
			_root = new Node(t, _left, _top, _left + _width, _top + _height,0); 
		} 
		else 
		{ 
			_root->add(t); 
		} 
		_dirty = true;
	}
	
	/// remove an element from the tree
	void erase(T* t) {
		if (_root) _root->erase(t);
		_dirty = true;
	}
	
	/// find all elements in a given range, for speed-reasons the given range is rectangular, all results are appended to the vector
	void find_in_range(float l, float t, float r, float b, std::vector<T*>&results) const
	{
		if (_root) 
		{
			_root->find_in_range(l, t, r, b, results);
		}
	}
	
	/// find all elements in a given range around p
	void find_in_range(const osg::Vec2& p, float range, std::vector<T*>&results) const
	{
		return find_in_range(p.x() - range, p.y() - range, p.x() + range, p.y() + range, results);
	}
	
	/// update the tree from all elements' positions
	void update() 
	{
		_dirty = true; 
		if(_root)
			_root->update();
	}
	
	/// remove all elements
	void clear() 
	{
		if (_root) 
			delete _root; 
		_root = NULL; 
	}
	
	/// get the size of the tree (the number of stored elements), may be slow
	unsigned int size() const 
	{
		unsigned int m = _dirty ? (_root) ? _root->size() : 0 : _cachedSize;
		_dirty = false; 
		_cachedSize = m;
		return m;
	}
	
	virtual ~QuadTree() 
	{ 
		clear();
	}
private: 
	 QuadTreeNode<T, ValueType, VectorType, MaxDepth, MaxChildsBeforeSubDivision>* _root;
	 ValueType _left, _top, _width, _height;
	 mutable bool	_dirty;
	 mutable unsigned int _cachedSize;
friend class QuadTreeGeometry;
};


/// a QuadTree inherited from osg::Referenced, so you can store it in a ref_ptr
template <class T, typename ValueType = int, class VectorType = osg::Vec2, int MaxDepth = 6, int MaxChildsBeforeSubDivision = 6>
class RefQuadTree : public QuadTree<T, ValueType, VectorType, MaxDepth, MaxChildsBeforeSubDivision> , public osg::Referenced {
public:
	RefQuadTree( ValueType w, ValueType h) : QuadTree<T, ValueType, VectorType, MaxDepth, MaxChildsBeforeSubDivision>(w,h), osg::Referenced() {}
    RefQuadTree(ValueType left, ValueType top, ValueType w, ValueType h) : QuadTree<T, ValueType, VectorType, MaxDepth, MaxChildsBeforeSubDivision>(left, top, w,h), osg::Referenced() {}
};


/// QuadTreeGeometry visualizes a QuadTree */
class QuadTreeGeometry : public osg::Geometry 
{
public:
	/// c'tor
	QuadTreeGeometry(float locz=999);
	
	
	/// visualize given quad-tree, vor every non-empty cell a red rectangle is drawn
	template <class T>
	void visualize(const T& t) 
	{
		_vertices->clear();
		
		typename T::Node* root = t._root;
		if (root) addRect(root);
		
		_da->setCount(_vertices->size());
		dirtyDisplayList();
		dirtyBound();
	}

	
private:
	template <class T>
	void addRect(T* t) 
	{
		drawRect(t->_minX, t->_minY, t->_maxX, t->_maxY);
		for(unsigned int i = 0; i < 4; ++i) {
			if ((t->_nodes[i]) && (t->_nodes[i]->size() > 0))
				addRect(t->_nodes[i]);
		}
	}
	
	void drawRect(float min_x, float min_y, float max_x, float max_y);

	osg::ref_ptr<osg::Vec3Array> _vertices;
	osg::ref_ptr<osg::DrawArrays> _da;
	float _locZ;

};


}

#endif