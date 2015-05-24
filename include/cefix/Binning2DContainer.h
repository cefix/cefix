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

#ifndef CEFIX_BINNING_CONTAINER_HEADER
#define CEFIX_BINNING_CONTAINER_HEADER

#include <vector>
#include <osg/Referenced>
#include <osg/Math>
namespace cefix {


/** Binning2DContainer stores elements into a 2D-Grid, with variable cell-widths and -heights. 
    Nice to have, if you have to check for neighbors, etc */
	
template <class T, typename VectorType, int CellWidth, int CellHeight>
class Binning2DContainer {

public:
	typedef std::vector<T*> Cell;
	typedef std::vector<Cell> Cells;
	
	/// c'tor
	Binning2DContainer(float width, float height)
	:	_width(width),
		_height(height)
	{
		_numXCells = ceil(_width / CellWidth);
		_numYCells = ceil(_height / CellHeight);
		clear();
	}
	
	/// add an element, memory-management is your job
	void add(T* t) 
	{
		_cells[getIndex(t)].push_back(t);
	}
	
	/// find all elements in a given area and add them to a vector
	void find_in_range(float l, float t, float r, float b, std::vector<T*>& results)
	{
		
		unsigned int x1 = getXCellIndex(l);
		unsigned int x2 = getXCellIndex(r);
		unsigned int y1 = getYCellIndex(t);
		unsigned int y2 = getYCellIndex(b);
		for(unsigned int y = y1; y <= y2; ++y)
		{
			for(unsigned int x = x1; x <= x2; ++x) 
			{
				Cell& cells = _cells[getIndex(x,y)];
				for(typename Cell::iterator i = cells.begin(); i != cells.end(); ++i) {
					results.push_back(*i);
				}
			}
		}
	}
	
	/// clear all elements
	void clear() 
	{
		_cells.clear();
		_cells.resize(_numXCells*_numYCells);
	}
	
	/// get the size of this container, may be slow
	unsigned int size() const
	{
		unsigned int s = 0;
		for(typename Cells::const_iterator i = _cells.begin(); i != _cells.end(); ++i) {
			s += _cells[i].size();
		}
		return s;
	}
	
	/// d'tor
	virtual ~Binning2DContainer() 
	{ 
		clear(); 
	}
	
private:
	unsigned int getXCellIndex(float x) {
		return osg::clampTo<unsigned int>((x / _width) * _numXCells, 0, _numXCells-1);
	}
	
	unsigned int getYCellIndex(float y) {
		return  osg::clampTo<unsigned int>((y / _height) * _numYCells, 0, _numYCells-1);
	}
	
	unsigned int getIndex(T* t) 
	{
		const VectorType& p = t->getPosition();
		return getIndex(getXCellIndex(p.x()), getYCellIndex(p.y())); 
	}
	
	unsigned int getIndex(unsigned int x, unsigned int y) {
		return y * _numXCells + x;
	}
	
	
	
private:
	float _width, _height;
	unsigned int _numXCells, _numYCells;
	Cells _cells;

};


template <class T, class VectorType, int CellWidth, int CellHeight>
class RefBinning2DContainer : public Binning2DContainer<T, VectorType, CellWidth, CellHeight>, public osg::Referenced
{
public:
	RefBinning2DContainer<T, VectorType, CellWidth, CellHeight>(float width, float height) 
	:	Binning2DContainer<T, VectorType, CellWidth, CellHeight>(width, height), 
		osg::Referenced() 
	{
	}
};

}
#endif