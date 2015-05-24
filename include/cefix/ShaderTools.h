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

#ifndef CEFIX_SHADER_TOOLS_HEADER
#define CEFIX_SHADER_TOOLS_HEADER

#include <osg/Texture1D>

namespace cefix {

/** ShaderTools makes it easy to load shaders, create a program and attach the shaders to a StateSet, 
    there's even some code for text-replacing */
class ShaderTools {
public:
    typedef std::map<std::string, std::string> ReplacementMap;
	
	/** attach a set of shaders to a stateset, the extensions are '.gs', '.vs' and '.fs', only found shader-sources get attached
	    created programs are cached */
	static bool attachShaders(const std::string& base_name, osg::StateSet* ss, const ReplacementMap& replacement_map = ReplacementMap(), bool do_cache = true );
    
	/** compute an image with a gaussian kernel */
    static bool precomputeGaussianKernel(osg::Image* image, unsigned int length, float sigma);
	
	/** compute a texture1d with a gaussian kernel into an image */
    static osg::Texture1D* precomputeGaussianKernel(unsigned int length, float sigma);
private:
	ShaderTools() {}
};

}

#endif