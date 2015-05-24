/*
 *  Frustum.cpp
 *  Circles
 *
 *  Created by Stephan Huber on 18.10.04.
 *  Copyright 2004 __MyCompanyName__. All rights reserved.
 *
 */

#include <osg/Math>
#include <cefix/Log.h>
#include <osg/io_utils>
#include "Frustum.h"

namespace cefix {

    bool Frustum::inside(const osg::Vec3 &v) {
                
        PlaneVector::iterator itr;
        double dist =0.0f;
        int count = 0;
        for (itr = _planes.begin(); itr !=_planes.end(); itr++,count++) {
            dist = (*itr).plane.distance(v);
            if (dist > 0) { // Punkt liegt auf der dem Frustum abgewandten Seite, d.h. sicher nicht im Frustum
                return false;
            }
        }
        
        return true;
    }

    double Frustum::getAngleBetween(const osg::Vec3 &a, const osg::Vec3 &b) {

        float dot = a * b;
        float mag = a.length() * b.length();
        
        double angle = acos(dot/mag);
        
		if (osg::isNaN(angle)) 
            return 0;
        else
            return angle;
    }


    bool Frustum::clipLine(const FrustumPlane &p, osg::Vec3 &v1, osg::Vec3 v2) {
        osg::Vec3 result;
        
        {
            osg::Vec3 vLineDir = v2-v1;
            vLineDir.normalize();
            
            double dist1 = p.plane.distance(v1);
            double dist2 = p.plane.distance(v2);
            if (dist1*dist2 >= 0) 
                return false; // punkte schneiden diese plane nicht
                
            double numerator = -dist1;
            double denominator = p.plane.getNormal() * vLineDir;
            
            if (0.0 == denominator) 
                result = v1;
            else {
                double dist = numerator/denominator;
                result = v1 + (vLineDir*dist);
            }
        }
        // so, jetzt gucken, ob das ding im polygon sitzt
        {
           osg::Vec3Array* vertices = p.getVertices();
            
            double angle = 0.0;
            int vertCount = vertices->size();
            osg::Vec3 a,b;
            for(int i = 0; i< vertCount; i++) {
                
                a = result - (*vertices)[i];
                b = result - (*vertices)[(i+1) % vertCount];
                
                angle += getAngleBetween(a,b);
            
            }
        
            if (angle >= (2*osg::PI*0.9999)) {
                v1 = result;
                return true;
            }
        }
        return false;
    }
        



    bool Frustum::project(osg::Vec3 &v1, osg::Vec3 &v2) {

        bool v1Inside, v2Inside;
        bool projected = false;
        osg::Vec3 ov1,ov2;
        ov1 = v1;
        ov2 = v2;
        
        v1Inside = inside(v1);
        v2Inside = inside(v2);

        if ((v1Inside) && (v2Inside))
            return true;

        // ok, einer oder beide punkte liegen ausserhalb des frustum-volumes, 
        // jetzt müssen wir die Schnittpunkte suchen
        
        PlaneVector::iterator itr;
         
        if (!v1Inside) {
            for (itr = _planes.begin(); itr !=_planes.end(); itr++) {
                if (clipLine((*itr), v1,ov2)) {
                    osg::notify(osg::DEBUG_INFO) << "clipped " << ov1 << " to " << v1 << " plane: " << ((*itr).name) << std::endl;
                    projected = true;
                    break;
                }
            }
        }

        if (!v2Inside) {
            for (itr = _planes.begin(); itr !=_planes.end(); itr++) {
                if (clipLine((*itr), v2,ov1)) {
                    osg::notify(osg::DEBUG_INFO) << "clipped " << ov2 << " to " << v2 << " plane: " << ((*itr).name) << std::endl;
                    projected = true;
                    break;
                }
                    
            }
        }

        return projected;
    }

}