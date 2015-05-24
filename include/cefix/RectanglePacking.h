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
 */

#ifndef RECTANGLE_PACKING_HEADER
#define RECTANGLE_PACKING_HEADER

#include <osg/Vec4>

namespace cefix {
  

template <class T>
class RectanglePacking 
{

	public:
		template <class U>
		struct Node {
			osg::Vec4 rect;
			U*	content;
			Node<U> *left, *right;
			
			Node(): rect(), content(NULL), left(NULL), right(NULL) {}
			~Node() { if (left) delete left; if (right) delete right; }
			
			inline float getWidth() const { return (rect[2]-rect[0]); }
			inline float getHeight() const { return (rect[3]-rect[1]); }
			
			inline float getLeft() const { return rect[0]; }
			inline float getRight() const { return rect[2]; }
			inline float getTop() const { return rect[1]; }
			inline float getBottom() const { return rect[3]; }
			
			Node<U>* insert(U* u, float w, float h) 
			{
				if (left && right) {
					Node<U>* newnode = left->insert(u,w,h);
					if (!newnode) newnode = right->insert(u,w,h);
					return newnode;
				}
				
				if (content) {
					//std::cout << "already content there" << std::endl;
					return NULL;
				}
				float width(getWidth()), height(getHeight());
				float dw = width  - w;
				float dh = height - h;
				
				
				if ((fabs(dw) < 0.0001 ) && (fabs(dh)< 0.0001)) 
				{
					content = u;
					return this;
				}
				if ((width < w-0.0005) || (height < h-0.0005)) {
					//std::cout << "rect to big: " << width <<"x"<<height<< "trying to insert " << w<<"x"<<h<<std::endl;
					return NULL;
				}
				
				left = new Node<U>();
				right = new Node<U>();
				
				if (dw > dh) {
					left->rect.set(getLeft(), getTop(), getLeft()+w,getBottom());
					right->rect.set(getLeft()+w, getTop(), getRight(),getBottom());
				} else {
					left->rect.set(getLeft(), getTop(), getRight(),getTop()+h);
					right->rect.set(getLeft(), getTop()+h, getRight(),getBottom());
				}
				
				return left->insert(u, w,h);
			}
		};
		
		typedef Node<T> node_type;
		
		RectanglePacking(unsigned int maxWidth, unsigned int maxHeight) 
		: 
			_root(NULL)
		{ 
			_root = new node_type();
			_root->rect.set(0,0,maxWidth, maxHeight);
		}
		
		node_type* insert(T* t, float w, float h) 
		{
			return _root->insert(t, w, h);
		}
		
		~RectanglePacking() 
		{
			delete _root;
		}
		
		
	private:
		Node<T>* _root;

};

}

#endif