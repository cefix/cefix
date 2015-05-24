/*
 *  FastBlur.cpp
 *  cefix
 *
 *  Created by Stephan Huber on 26.07.07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "FastBlur.h"

namespace cefix {



FastBlur::FastBlur(osg::Image* image, float r) : 
	_source(image), 
	_radius(r),
	_pixel(image)
{
	init();
}

void FastBlur::init() {
	_w=_source->s();
	_h=_source->t();
	_wm=_w-1;
	_hm=_h-1;
	_wh=_w*_h;
	int div=_radius+_radius+1;

	_r.resize(_wh);
	_g.resize(_wh);
	_b.resize(_wh);
	_a.resize(_wh);
	_vmin.resize(std::max(_w, _h));
	_vmax.resize(std::max(_w, _h));
	
	
	_dv.resize(256*div);
	for (int i=0;i<256*div;++i){
		_dv[i]=(i/div);
	}
}

void FastBlur::blur(osg::Image* dest) 
{
	if (_radius<1){
		return;
	}
	
	int yw, yi, y, x, rsum, gsum, bsum, asum,p1, p2;
	yw=yi=0;

	
	unsigned char r1,g1,b1, r2,g2, b2, a1, a2;
	for (y = 0; y < _h; ++y){
		rsum=gsum=bsum=asum=0;
		
		for(int i=-_radius;i<=_radius;i++){
			
			int tPix=std::min(_wm,std::max(i,0));
			_pixel.getRGBA(tPix, y, r1, g1, b1,a1);

			rsum += r1;
			gsum += g1;
			bsum += b1;
			asum += a1;
		}

		for (x = 0; x < _w; x++)
		{
			_r[yi] = _dv[rsum];
			_g[yi] = _dv[gsum];
			_b[yi] = _dv[bsum];
			_a[yi] = _dv[asum];

			if (y == 0)
			{
				_vmin[x] = std::min(x+_radius+1,_wm);
				_vmax[x] = std::max(x-_radius,0);
			}


			int t1 =  _vmin[x];
			_pixel.getRGBA(t1,y, r1, g1, b1, a1);
			int t2=_vmax[x];
			_pixel.getRGBA(t2,y, r2, g2, b2, a2);

			rsum+=((r1 - r2));
			gsum+=((g1 - g2));
			bsum+=((b1 - b2));
			asum+=((a1 - a2));
			++yi;
		}

		yw+=_w;
	}

	cefix::Pixel destPixel(dest);
	
	for (x=0;x<_w;x++){
		rsum=gsum=bsum=asum=0;
		int yp=-_radius*_w;
		
		
		for(int i=-_radius;i<=_radius;i++)
		{
			yi=std::max(0,yp)+x;
			rsum+=_r[yi];
			gsum+=_g[yi];
			bsum+=_b[yi];
			asum+=_a[yi];
			yp+=_w;
		}
	
	
		yi=x;
		
		for (y=0;y<_h;y++)
		{
			destPixel.setRGBA(x, y, _dv[rsum], _dv[gsum], _dv[bsum], _dv[asum]);

			//pix[yi]=0xff000000 | (dv[rsum]<<16) | (dv[gsum]<<8) | dv[bsum];

			if(x==0)
			{
				_vmin[y]=std::min(y+_radius+1,_hm)*_w;
				_vmax[y]=std::max(y-_radius,0)*_w;
			}
			p1=x+_vmin[y];
			p2=x+_vmax[y];

			rsum+=_r[p1]-_r[p2];
			gsum+=_g[p1]-_g[p2];
			bsum+=_b[p1]-_b[p2];
			asum+=_a[p1]-_a[p2];


			yi+=_w;
		  
		}
	}
	
	dest->dirty();
}



}


