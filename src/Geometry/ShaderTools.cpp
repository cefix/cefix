/*
 *  ShaderTools.cpp
 *  cefixSketch
 *
 *  Created by Stephan Huber on 28.02.11.
 *  Copyright 2011 Digital Mind. All rights reserved.
 *
 */

#include "ShaderTools.h"
#include <cefix/Cache.h>
#include <osg/Program>
#include <cefix/FileUtils.h>
#include <cefix/Log.h>

#include <cefix/Pixel.h>
#include <cefix/StringUtils.h>

#include <osgDB/FileUtils>

namespace cefix {

bool ShaderTools::attachShaders(const std::string& basename, osg::StateSet* ss, const ReplacementMap& replacement_map, bool do_cache)
{
	typedef cefix::Cache<std::string, osg::Program > ProgramCache;
	static osg::ref_ptr<ProgramCache> s_cache = NULL;
    if (do_cache && s_cache==NULL) { 
        s_cache = new ProgramCache();
	}
    
	osg::Program* prog = (do_cache) ? s_cache->get(basename) : NULL;
	
	if (!prog) 
	{
		cefix::log::info("ShaderTools") << "loading shader " << basename <<  std::endl;

		std::string vert_src = cefix::readFileIntoString(osgDB::findDataFile(basename+ ".vs"));
		std::string frag_src = cefix::readFileIntoString(osgDB::findDataFile(basename+ ".fs"));
		std::string geom_src = cefix::readFileIntoString(osgDB::findDataFile(basename+ ".gs"));
        
        for(ReplacementMap::const_iterator i = replacement_map.begin(); i != replacement_map.end(); ++i) 
        {
            vert_src = cefix::strReplaceAll(vert_src, i->first, i->second);
            frag_src = cefix::strReplaceAll(frag_src, i->first, i->second);
            geom_src = cefix::strReplaceAll(geom_src, i->first, i->second);
        }
		
		prog = new osg::Program();
		bool is_valid(false);
		
		if (!vert_src.empty()) { is_valid = true; prog->addShader(new osg::Shader(osg::Shader::VERTEX, vert_src)); }
		if (!frag_src.empty()) { is_valid = true; prog->addShader(new osg::Shader(osg::Shader::FRAGMENT, frag_src)); }
		if (!geom_src.empty()) { is_valid = true; prog->addShader(new osg::Shader(osg::Shader::GEOMETRY, geom_src)); }
		
		if (is_valid && do_cache) {
			s_cache->add(basename, prog);
		} else {
			cefix::log::error("ShaderTools") << "could not load shaders from " << basename <<  std::endl;
			return false;
		}
	}
	
	ss->setAttributeAndModes(prog, osg::StateAttribute::ON);
	return true;
}

    
    
bool ShaderTools::precomputeGaussianKernel(osg::Image* image, unsigned int length, float sigma)
{
    float radius = length / 2.0;
    float sigma2 = 2.0 * sigma * sigma;
    float c = sqrt((1.0 / (sigma2 * osg::PI)));
    
    cefix::PixelT<float> p(image);
    if(p.s() != length)
        return false;
    
    for(int i = - radius; i <radius; ++i) {
        
        float weight = c * exp((i*i) / (-sigma2));
        p.setR(i+radius, 0, weight);
    }
    image->dirty();
    
    return true;
}
    
osg::Texture1D* ShaderTools::precomputeGaussianKernel(unsigned int alength, float sigma) 
{
    float length = osg::Image::computeNearestPowerOfTwo(alength);
    osg::Texture1D* tex = new osg::Texture1D();
    osg::ref_ptr<osg::Image> img = new osg::Image();
    img->allocateImage(length, 1, 1, GL_LUMINANCE, GL_FLOAT);
    
    precomputeGaussianKernel(img.get(), length, sigma);
    
    tex->setImage(img);
    tex->setFilter(osg::Texture1D::MIN_FILTER, osg::Texture1D::LINEAR);
	tex->setFilter(osg::Texture1D::MAG_FILTER, osg::Texture1D::LINEAR);	
	tex->setWrap(osg::Texture1D::WRAP_S, osg::Texture1D::CLAMP_TO_EDGE);
    tex->setInternalFormat(GL_LUMINANCE32F_ARB);
    
    return tex;
}

}
