/****************************************
 * 2D Spline Class
 * By Bill Perone (billperone@yahoo.com)
 * Original: 21-10-2003
 *   
 * Dependancies: matrix4, vector2 class, STL vector, STL list
 *
 * This code is provided "as is" and you can use it freely as long as 
 * credit is given to Bill Perone in the application it is used in
 ****************************************/


#include "Spline.h"

namespace cefix {



std::list<Spline1D::ArclenTableEntry>::iterator Spline1D::internalBuildArclenTable(std::list<Spline1D::ArclenTableEntry> &templist, 
																			       std::list<Spline1D::ArclenTableEntry>::iterator curr, unsigned int ctrlpt_offset)
{
	std::list<ArclenTableEntry>::iterator next= curr;
	ArclenTableEntry  middle;


	const unsigned int size= controlPoints.size();
	++next;
	middle.time= (curr->time + (next)->time)/2;	
	middle.pos= getPointOnSpline(controlPoints[ctrlpt_offset], controlPoints[(ctrlpt_offset+1)%size], 
							 controlPoints[(ctrlpt_offset+2)%size], controlPoints[(ctrlpt_offset+3)%size], 
							 middle.time - (float)ctrlpt_offset/_step); 
		
	float a, b, c;
	a= middle.pos-curr->pos;
	b= next->pos-middle.pos;
	c= next->pos-curr->pos;

	float error= a+b-c;
	if (error >= _maxError)
	{
		// recalculate the arclengths for the rest of the table
		std::list<ArclenTableEntry>::iterator i= next;
		while (i != templist.end())		
		{
			i->arclen+=error;			
			++i;
		} 

		// insert the mid point into the table
		middle.arclen= curr->arclen+a;
		templist.insert(next, middle);

		// recursive subdivision
		next= internalBuildArclenTable(templist, curr, ctrlpt_offset);
		next= internalBuildArclenTable(templist, next, ctrlpt_offset);
	}
	return next;
}


void Spline1D::buildArclenTable()
{
	std::list<ArclenTableEntry> templist;
	std::list<ArclenTableEntry>::iterator ai;

	ArclenTableEntry   entry;
	unsigned int size= controlPoints.size();
	

	arclentable.clear();

	// add the first entry to the arc table
	entry.time= 0;
	entry.arclen= 0;	
	entry.pos= getPointOnSpline(controlPoints[0], controlPoints[1], controlPoints[2%size], controlPoints[3%size], 0); 	
	templist.push_back(entry);

	// go through all the control point sets generating arclen table entries
	ai= templist.begin();
	int startDelta = getStartDelta();
	int endDelta = getEndDelta();
	
	for (unsigned int i= startDelta; i < controlPoints.size()-endDelta; i+=_step)	
	{
		// add the end entry to the list
		entry.time= (float)(i+_step)/_step; 
		entry.pos= getPointOnSpline(controlPoints[i%size], controlPoints[(i+1)%size], controlPoints[(i+2)%size], controlPoints[(i+3)%size], 1); 		
		entry.arclen= entry.pos-ai->pos + templist.back().arclen; // beginning approximation			
		templist.push_back(entry);

		// do the subdivision
		ai= internalBuildArclenTable(templist, ai, i);		
	}		
		
	ai= templist.begin();
	while (ai!=templist.end())
	{		
		ai->time/=templist.back().time;
		arclentable.push_back(*ai);		
		++ai;
	}	
}


float Spline1D::getArcPoint(float arc_val)
{
	// trivial cases
	if (arc_val==arclentable.front().arclen) return arclentable.front().pos;
	else 
	if (arc_val==arclentable.back().arclen) return arclentable.back().pos;

	unsigned int lower= getArcTableBisectionSearch(arc_val);	
	
	// perform a linear interpolation of 2 nearest position values
	float pos;	
	pos= ( (arclentable[lower+1].arclen-arc_val)*arclentable[lower].pos + 
		   (arc_val-arclentable[lower].arclen)*arclentable[lower+1].pos ) / 
		   (arclentable[lower+1].arclen - arclentable[lower].arclen);
	return pos;
}





std::list<Spline2D::ArclenTableEntry>::iterator Spline2D::internalBuildArclenTable(std::list<Spline2D::ArclenTableEntry> &templist, 
																			       std::list<Spline2D::ArclenTableEntry>::iterator curr, unsigned int ctrlpt_offset)
{
	std::list<ArclenTableEntry>::iterator next= curr;
	ArclenTableEntry  middle;


	const unsigned int size= controlPoints.size();
	++next;
	middle.time= (curr->time + (next)->time)/2;	
	middle.pos= getPointOnSpline(controlPoints[ctrlpt_offset], controlPoints[(ctrlpt_offset+1)%size], 
							   controlPoints[(ctrlpt_offset+2)%size], controlPoints[(ctrlpt_offset+3)%size], 
							   middle.time - (float)ctrlpt_offset/_step); 
		
	float a, b, c;
	a= (middle.pos-curr->pos).length();
	b= (next->pos-middle.pos).length();
	c= (next->pos-curr->pos).length();

	float error= a+b-c;
	if ((error >= _maxError)  || (templist.size() < 4))
	{
		// recalculate the arclengths for the rest of the table
		std::list<ArclenTableEntry>::iterator i= next;
		while (i != templist.end())		
		{
			i->arclen+=error;			
			++i;
		} 

		// insert the mid point into the table
		middle.arclen= curr->arclen+a;
		templist.insert(next, middle);

		// recursive subdivision
		next= internalBuildArclenTable(templist, curr, ctrlpt_offset);
		next= internalBuildArclenTable(templist, next, ctrlpt_offset);
	}
	return next;
}


void Spline2D::buildArclenTable()
{
	std::list<ArclenTableEntry> templist;
	std::list<ArclenTableEntry>::iterator ai;

	ArclenTableEntry   entry;
	unsigned int size= controlPoints.size();
	

	arclentable.clear();

	// add the first entry to the arc table
	entry.time= 0;
	entry.arclen= 0;	
	entry.pos= getPointOnSpline(controlPoints[0], controlPoints[1], controlPoints[2%size], controlPoints[3%size], 0); 
	templist.push_back(entry);

	// go through all the control point sets generating arclen table entries
	ai= templist.begin();
	
	int startDelta = getStartDelta();
	int endDelta = getEndDelta();
	
	for (unsigned int i= startDelta; i < controlPoints.size()-endDelta; i+=_step)
	{
		// add the end entry to the list
		entry.time= (float)(i+_step)/_step; 
		entry.pos= getPointOnSpline(controlPoints[i%size], controlPoints[(i+1)%size], controlPoints[(i+2)%size], controlPoints[(i+3)%size], 1); 
		entry.arclen= (entry.pos-ai->pos).length() + templist.back().arclen; // beginning approximation			
		templist.push_back(entry);

		// do the subdivision
		ai= internalBuildArclenTable(templist, ai, i);		
	}		
		
	ai= templist.begin();
	while (ai!=templist.end())
	{		
		ai->time/=templist.back().time;
		arclentable.push_back(*ai);		
		++ai;
	}	
}


osg::Vec2 Spline2D::getArcPoint(float arc_val)
{
	// trivial cases
	if (arc_val==arclentable.front().arclen) return arclentable.front().pos;
	else 
	if (arc_val==arclentable.back().arclen) return arclentable.back().pos;

	unsigned int lower= getArcTableBisectionSearch(arc_val);	
	
	// perform a linear interpolation of 2 nearest position values
	osg::Vec2 pos;	
	pos[0]= ( (arclentable[lower+1].arclen-arc_val)*arclentable[lower].pos[0] + 
		     (arc_val-arclentable[lower].arclen)*arclentable[lower+1].pos[0] ) / 
			 (arclentable[lower+1].arclen - arclentable[lower].arclen);
	pos[1]= ( (arclentable[lower+1].arclen-arc_val)*arclentable[lower].pos[1] + 
		     (arc_val-arclentable[lower].arclen)*arclentable[lower+1].pos[1] ) / 
			 (arclentable[lower+1].arclen - arclentable[lower].arclen);
	return pos;
}




std::list<Spline3D::ArclenTableEntry>::iterator Spline3D::internalBuildArclenTable(std::list<Spline3D::ArclenTableEntry> &templist, 
																			       std::list<Spline3D::ArclenTableEntry>::iterator curr, unsigned int ctrlpt_offset)
{
	std::list<ArclenTableEntry>::iterator next= curr;
	ArclenTableEntry  middle;


	const unsigned int size= controlPoints.size();
	++next;
	middle.time= (curr->time + (next)->time)/2;	
	middle.pos= getPointOnSpline(controlPoints[ctrlpt_offset], controlPoints[(ctrlpt_offset+1)%size], 
							   controlPoints[(ctrlpt_offset+2)%size], controlPoints[(ctrlpt_offset+3)%size], 
							   middle.time - (float)ctrlpt_offset/_step); 
			
	float a, b, c;
	a= (middle.pos-curr->pos).length();
	b= (next->pos-middle.pos).length();
	c= (next->pos-curr->pos).length();

	float error= a+b-c;
	if ((error >= _maxError) || (templist.size() < 4))
	{
		// recalculate the arclengths for the rest of the table
		std::list<ArclenTableEntry>::iterator i= next;
		while (i != templist.end())		
		{
			i->arclen+=error;			
			++i;
		} 

		// insert the mid point into the table
		middle.arclen= curr->arclen+a;
		templist.insert(next, middle);

		// recursive subdivision
		next= internalBuildArclenTable(templist, curr, ctrlpt_offset);
		next= internalBuildArclenTable(templist, next, ctrlpt_offset);
	}
	return next;
}


void Spline3D::buildArclenTable()
{
	std::list<ArclenTableEntry> templist;
	std::list<ArclenTableEntry>::iterator ai;

	ArclenTableEntry   entry;
	unsigned int size= controlPoints.size();
	

	arclentable.clear();

	// add the first entry to the arc table
	entry.time= 0;
	entry.arclen= 0;	
	entry.pos = getPointOnSpline(controlPoints[0], controlPoints[1], controlPoints[2], controlPoints[3], 0); 
	templist.push_back(entry);

	// go through all the control point sets generating arclen table entries
	ai= templist.begin();
	
	int startDelta = getStartDelta();
	int endDelta = getEndDelta();
	
	for (unsigned int i= startDelta; i < controlPoints.size()-endDelta; i+=_step)
	{
		// add the end entry to the list
		entry.time= (float)(i+_step)/_step; 
		entry.pos= getPointOnSpline(controlPoints[i%size], controlPoints[(i+1)%size], controlPoints[(i+2)%size], controlPoints[(i+3)%size], 1); 
		
		entry.arclen= (entry.pos-ai->pos).length() + templist.back().arclen; // beginning approximation			
		templist.push_back(entry);

		// do the subdivision
		ai= internalBuildArclenTable(templist, ai, i);		
	}		
		
	ai= templist.begin();
	while (ai!=templist.end())
	{		
		ai->time/=templist.back().time;
		arclentable.push_back(*ai);		
		++ai;
	}	
}


osg::Vec3 Spline3D::getArcPoint(float arc_val)
{
	// trivial cases
	if (arc_val==arclentable.front().arclen) return arclentable.front().pos;
	else 
	if (arc_val==arclentable.back().arclen) return arclentable.back().pos;

	unsigned int lower= getArcTableBisectionSearch(arc_val);	
	
	// perform a linear interpolation of 2 nearest position values
	osg::Vec3 pos;	
	pos[0]= ( (arclentable[lower+1].arclen-arc_val)*arclentable[lower].pos[0] + 
		     (arc_val-arclentable[lower].arclen)*arclentable[lower+1].pos[0] ) / 
			 (arclentable[lower+1].arclen - arclentable[lower].arclen);
	pos[1]= ( (arclentable[lower+1].arclen-arc_val)*arclentable[lower].pos[1] + 
		     (arc_val-arclentable[lower].arclen)*arclentable[lower+1].pos[1] ) / 
			 (arclentable[lower+1].arclen - arclentable[lower].arclen);
	pos[2]= ( (arclentable[lower+1].arclen-arc_val)*arclentable[lower].pos[2] + 
		     (arc_val-arclentable[lower].arclen)*arclentable[lower+1].pos[2] ) / 
			 (arclentable[lower+1].arclen - arclentable[lower].arclen);
	return pos;
}


}
