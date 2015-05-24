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

#ifndef PERLIN_HEADER
#define PERLIN_HEADER

#include <osg/Referenced>
#include <cefix/Export.h>
#include <osg/Vec2>
#include <osg/Vec3>

namespace cefix {

	/** Perlin class to compute the 1D,2D, or 3D-perlin-noise */
	
	class CEFIX_EXPORT Perlin : public osg::Referenced
	{
	public:
		/** ctor */
	  Perlin(int octaves,float freq,float amp,int seed);


	  /* 1D perlin-noise */
	  inline float get(float x)
	  {
		return perlin_noise_1D(x);
	  };


		/* 2D perlin-noise */
	  inline float get(float x,float y)
	  {
		float vec[2];
		vec[0] = x;
		vec[1] = y;
		return perlin_noise_2D(vec);
	  };

	  /* 3D perlin-noise */
	  float get(float x,float y,float z)
	  {
		float vec[3];
		vec[0] = x;
		vec[1] = y;
		vec[2] = z;
		return perlin_noise_3D(vec);
	  };
      
      inline float get(const osg::Vec3& p) { return get(p[0], p[1], p[2]); }
      inline float get(const osg::Vec2& p) { return get(p[0], p[1]); }
      
    virtual ~Perlin() {}

	private:
	  void init_perlin(int n,float p);
	  float perlin_noise_1D(float vec);
      float perlin_noise_2D(float vec[2]);
      float perlin_noise_3D(float vec[3]);

	  float noise1(float arg);
	  float noise2(float vec[2]);
	  float noise3(float vec[3]);
	  void normalize2(float v[2]);
	  void normalize3(float v[3]);
	  void init(void);

	  int   _octaves;
	  float _frequency;
	  float _amplitude;
	  int   _seed;
	  
	  #define SAMPLE_SIZE 1024
	  int p[SAMPLE_SIZE + SAMPLE_SIZE + 2];
	  float g3[SAMPLE_SIZE + SAMPLE_SIZE + 2][3];
	  float g2[SAMPLE_SIZE + SAMPLE_SIZE + 2][2];
	  float g1[SAMPLE_SIZE + SAMPLE_SIZE + 2];
	  bool  _start;

	};

}

#endif
