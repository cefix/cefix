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

#ifndef CEFIX_BINNING_3D_CONTAINER_HEADER
#define CEFIX_BINNING_3D_CONTAINER_HEADER

#include <vector>
#include <osg/Vec3>
#include <osg/Referenced>

namespace cefix {

/** Binning3DContainer stores elements into a 3D-Grid, with variable cell-widths and -heights. 
    Nice to have, if you have to check for neighbors, etc */

template <class T, typename VectorType, int CellX, int CellY, int CellZ>
class Binning3DContainer {

public:
	typedef std::vector<T*> Cell;
	typedef std::vector<Cell> Cells;
	
	/// c'tor
	Binning3DContainer(float dim_x, float dim_y, float dim_z)
	:	_x(dim_x), _y(dim_y), _z(dim_z)
	{
		_numXCells = ceil(_x / CellX);
		_numYCells = ceil(_y / CellY);
		_numZCells = ceil(_z / CellZ);
		clear();
	}
	
	/// adds an element, memory-management is your job
	void add(T* t) 
	{
		_cells[getIndex(t)].push_back(t);
		//std::cout << "add " << t << " at " << getIndex(t) << std::endl;
	}
	
	
	/// find all elements in a given volume and add them to the given vector
	void find_in_range(float xmin, float ymin, float zmin, float xmax, float ymax, float zmax, std::vector<T*>& results)
	{
		
		unsigned int x1 = getXCellIndex(xmin);
		unsigned int x2 = getXCellIndex(xmax);
		unsigned int y1 = getYCellIndex(ymin);
		unsigned int y2 = getYCellIndex(ymax);
		unsigned int z1 = getZCellIndex(zmin);
		unsigned int z2 = getZCellIndex(zmax);
		
		for(unsigned int z = z1; z <= z2; ++z) 
		{
			for(unsigned int y = y1; y <= y2; ++y)
			{
				for(unsigned int x = x1; x <= x2; ++x) 
				{
					Cell& cells = _cells[getIndex(x,y,z)];
					results.reserve(results.size() + cells.size());
					for(typename Cell::iterator i = cells.begin(); i != cells.end(); ++i) {
						results.push_back(*i);
					}
				}
			}
		}
	}
	
	/// find all elements in a given volume and add them to the given vector
	void find_in_range(const osg::Vec3& pos, float radius, std::vector<T*>& results) {
		find_in_range(pos[0] - radius, pos[1] - radius, pos[2] - radius, pos[0] + radius, pos[1] + radius, pos[2]+radius, results);
	}
	

	/// clears the grid
	void clear() {
		_cells.clear();
		_cells.resize(_numXCells*_numYCells*_numZCells);
	}
	
	// get the size of this container, may be slow
	unsigned int size() const
	{
		unsigned int s = 0;
		for(typename Cells::const_iterator i = _cells.begin(); i != _cells.end(); ++i) {
			s += _cells[i].size();
		}
		return s;
	}
	
	///d'tor
	virtual ~Binning3DContainer() 
	{	
		clear(); 
	}
	
private:
	inline unsigned int getXCellIndex(float x) const {
		return osg::clampTo<unsigned int>((x / _x) * _numXCells, 0, _numXCells-1);
	}
	
	inline unsigned int getYCellIndex(float y) const {
		return  osg::clampTo<unsigned int>((y / _y) * _numYCells, 0, _numYCells-1);
	}
	inline unsigned int getZCellIndex(float z) const {
		return  osg::clampTo<unsigned int>((z / _z) * _numZCells, 0, _numZCells-1);
	}
	
	inline unsigned int getIndex(T* t) 
	{
		const VectorType& p = t->getPosition();
		return getIndex(getXCellIndex(p.x()), getYCellIndex(p.y()), getZCellIndex(p.z())); 
	}
	
	inline unsigned int getIndex(unsigned int x, unsigned int y, unsigned int z) const {
		return z * _numXCells * _numYCells + y * _numXCells + x;
	}
	
	
	
private:
	float _x, _y, _z;
	unsigned int _numXCells, _numYCells, _numZCells;
	Cells _cells;

};
/** reference-counted Binning3DContainer stores elements into a 3D-Grid, with variable cell-widths and -heights. 
    Nice to have, if you have to check for neighbors, etc */

template <class T, class VectorType, int X, int Y, int Z>
class RefBinning3DContainer : public Binning3DContainer<T, VectorType, X, Y, Z>, public osg::Referenced
{
public:
	RefBinning3DContainer<T, VectorType, X, Y, Z>(float x, float y, float z) 
	:	Binning3DContainer<T, VectorType, X, Y, Z>(x, y, z), 
		osg::Referenced() 
	{
	}
};

}
#endif