/*
 *  Frustum.h
 *  Circles
 *
 *  Created by Stephan Huber on 18.10.04.
 *  Copyright 2004 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __FRUSTUM__HEADER__
#define __FRUSTUM__HEADER__
#include <osg/Referenced>
#include <osg/Array>
#include <osg/Matrix>
#include <osg/Plane>

namespace cefix {
    /**
     * FrustumPlane describe as plane of the frustum */
    class FrustumPlane {

        public:
            /** constructs a frustum plane describe by the vectors v1 - v4 */
            void set(osg::Vec3 v1, osg::Vec3 v2, osg::Vec3 v3, osg::Vec3 v4) {
                
                _vertices = new osg::Vec3Array();
                _vertices->push_back(v1);
                _vertices->push_back(v2);
                _vertices->push_back(v3);
                _vertices->push_back(v4);
                
                plane.set(v1,v2,v3);
            }
                
        public:
            osg::Plane                      plane;
            std::string                     name;
            osg::ref_ptr<osg::Vec3Array>    _vertices;
            
            osg::Vec3Array* getVertices() const { return _vertices.get();}
    };


    /**
     * the frustum class describes a frustum with 6 planes
     * it is used to decide which points are visible in the eps-rendering
     * it clips lines intersecting one of the frustum-planes
     */
    class Frustum : public osg::Referenced {

        public: 
            /** list of frustum planes */
            typedef std::vector<FrustumPlane>  PlaneVector;
             
            /** Constructor */
            Frustum() {}
            
            /** stores the projection-matrix and computes the frustum from a projection matrixx */
            void setMatrix(osg::RefMatrix * m) { _m = m; _computeFrustum();}
            
            /** projects two vectors with the help of the stored projection matrix 
            * and clips the line against the frustum
            * @param v1 startpoint
            * @param v2 endpoint of the line
            */
            bool project(osg::Vec3 &v1, osg::Vec3 &v2);
            
        protected:
            /** worker-method, computes the several frustum planes from a projection matrix */
            void _computeFrustum() {
                            
                osg::Matrix invP;
                invP.invert((*_m));
                
                // Einheitsürfel duch die Inverse Matrix der Projektion jagen, 
                // um die einzelnen Planes des Frsutum-Volumes bestimmen zu können

                osg::Vec3 f1(-1,-1,-1); f1 = f1*invP;
                osg::Vec3 f2(-1, 1,-1); f2 = f2*invP;
                osg::Vec3 f3( 1, 1,-1); f3 = f3*invP;
                osg::Vec3 f4( 1,-1,-1); f4 = f4*invP;

                osg::Vec3 b1(-1,-1,1); b1 = b1*invP;
                osg::Vec3 b2(-1, 1,1); b2 = b2*invP;
                osg::Vec3 b3( 1, 1,1); b3 = b3*invP;
                osg::Vec3 b4( 1,-1,1); b4 = b4*invP;
                           
                _planes.clear();
                {
                    FrustumPlane fpl;
                    fpl.set( f1, f4, f3,f2);
                    fpl.name = "near";
                    //fpl.set(    osg::Vec3(_left,    _bottom,    _znear  ),
                    //            osg::Vec3(_right,   _bottom,    _znear  ),
                    //            osg::Vec3(_right,   _top,       _znear  ),
                    //            osg::Vec3(_left,    _top,       _znear  )
                    //);
                    _planes.push_back(fpl);
                }
                 {
                    FrustumPlane fpl;
                    fpl.set( b4, b1, b2, b3);
                    
                    fpl.name = "far";
                    //fpl.set(    osg::Vec3(_left,    _bottom,    _zfar  ),
                    //            osg::Vec3(_right,   _bottom,    _zfar  ),
                    //            osg::Vec3(_right,   _top,       _zfar  ),
                    //             osg::Vec3(_left,    _top,       _zfar  )
                    //        );
                    
					//_planes.push_back(fpl);
                } 
                {
                    FrustumPlane fpl;
                    fpl.set( f1, b1, b4, f1);
                    
                    fpl.name = "bottom";
                    //fpl.set(    osg::Vec3(_left,    _bottom,    _znear  ),
                    //            osg::Vec3(_right,   _bottom,    _znear  ),
                    //            osg::Vec3(_right,   _bottom,    _zfar  ),
                    //            osg::Vec3(_left,    _bottom,    _zfar  )
                    //        );
                    _planes.push_back(fpl);
                } 
                {
                    FrustumPlane fpl;
                    fpl.set( f3, b3, b2, f2);
                    
                    fpl.name = "top";
                    //fpl.set(    osg::Vec3(_right,    _top,    _znear  ),
                    //            osg::Vec3(_left,   _top,    _znear  ),
                    //            osg::Vec3(_left,   _top,    _zfar  ),
                    //            osg::Vec3(_right,    _top,    _zfar  )
                    //        );
                    _planes.push_back(fpl);
                } 
                {
                    FrustumPlane fpl;
                    fpl.set( f2, b2, b1, f1);
                    fpl.name = "left";
                    //fpl.set(    osg::Vec3(_left,    _top,    _znear  ),
                    //            osg::Vec3(_left,    _bottom,       _znear  ),
                    //            osg::Vec3(_left,    _bottom,       _zfar  ),
                    //            osg::Vec3(_left,    _top,    _zfar  )
                    //        );
                    _planes.push_back(fpl);
                } 
                {
                    FrustumPlane fpl;
                    fpl.set( f4, b4, b3, f3);
                    
                    fpl.name = "right";
                    //fpl.set(    osg::Vec3(_right,    _bottom,    _znear  ),
                    //            osg::Vec3(_right,    _top,       _znear  ),
                    //            osg::Vec3(_right,    _top,       _zfar  ),
                    //            osg::Vec3(_right,    _bottom,    _zfar  )
                    //        );
                    _planes.push_back(fpl);
                } 
                
            }
            
            /** @return true if point is inside the frustum volume */
            bool inside(const osg::Vec3 &v);
            /** @return returns the angle enclosed by vector a and b */
            double getAngleBetween(const osg::Vec3 &a, const osg::Vec3 &b);        
            /**
             * clips a line with a specific frustum-plane
             * @param p frustum plane
             * @param v1 startpoint (the projected point is stored inside, if v1 is on the "wrong side")
             * @param v3 endpoint (the projected point is stored inside, if v1 is on the "wrong side")
             * @return true, if the line got clipped
             */
            bool clipLine(const FrustumPlane &p, osg::Vec3 &v1, osg::Vec3 v2);
            
            /** list of frustum planes describing the frustum volume */
            PlaneVector _planes;      
            /** current projection matrix */
            osg::RefMatrix* _m;
            
    };
}

#endif