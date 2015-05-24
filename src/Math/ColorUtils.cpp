/*
 *  ColorUtils.cpp
 *  cefix
 *
 *  Created by Stephan Huber on 29.03.07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "ColorUtils.h"
namespace cefix {


osg::Vec4 HSVtoRGB(float h, float s, float v) 
{ 
    
	int i; 
    float f, p, q, t,hTemp; 
	float r(0.0f),g(0.0f),b(0.0f);
    if( s == 0.0 || h == -1.0) // s==0? Totally unsaturated = grey so R,G and B all equal value 
    { 
      r = g = b = v; 
      return osg::Vec4(r,g,b,1); 
    } 
    hTemp = h/60.0f; 
    i = (int)floor( hTemp );                 // which sector 
    f = hTemp - i;                      // how far through sector 
    p = v * ( 1 - s ); 
    q = v * ( 1 - s * f ); 
    t = v * ( 1 - s * ( 1 - f ) ); 
  
    switch( i )  
    { 
	case 6:
    case 0:{r = v;g = t;b = p;break;} 
    case 1:{r = q;g = v;b = p;break;} 
    case 2:{r = p;g = v;b = t;break;} 
    case 3:{r = p;g = q;b = v;break;}  
    case 4:{r = t;g = p;b = v;break;} 
    case 5:{r = v;g = p;b = q;break;} 
    } 
	
	return osg::Vec4(r,g,b,1);
} 
  
void RGBtoHSV(osg::Vec4 rgb, float &h, float &s, float &v) 
 {
	float r(rgb[0]), g(rgb[1]), b(rgb[2]);
    float mn=r,mx=r; 
    int maxVal=0; 
  
    if (g > mx){ mx=g;maxVal=1;} 
    if (b > mx){ mx=b;maxVal=2;}  
    if (g < mn) mn=g; 
    if (b < mn) mn=b; 

    float  delta = mx - mn; 
  
    v = mx;  
    if( mx != 0 ) 
      s = delta / mx;  
    else  
    { 
      s = 0; 
      h = 0; 
      return; 
    } 
    if (s==0.0f) 
    { 
      h=-1; 
      return; 
    } 
    else 
    {  
      switch (maxVal) 
      { 
      case 0:{h = ( g - b ) / delta;break;}         // yel < h < mag 
      case 1:{h = 2 + ( b - r ) / delta;break;}     // cyan < h < yel 
      case 2:{h = 4 + ( r - g ) / delta;break;}     // mag < h < cyan 
      } 
    } 
  
    h *= 60; 
    if( h < 0 ) h += 360; 
}

}