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

#ifndef CEFIX_RTT_UNIT_HEADER
#define CEFIX_RTT_UNIT_HEADER

#include <cefix/RttTools.h>

namespace cefix {

/** A RttUnit encapsulates all necessary stuff for rendering post-process-filters. A RttUnit has several inputs, and several outputs, 
 *  you can chain Units together, etc. It's main-usage are postprocessing-filters which opertes on screenaligned quads, and where 
 *  you want to feed the result of one unit into another unit
 */
class RttUnit : public osg::Group {
public:

	typedef std::map<std::string, osg::ref_ptr<osg::Texture> > TextureMap;
	typedef std::map<std::string, osg::ref_ptr<osg::Uniform> > UniformMap;
	typedef std::map<osg::Texture*, unsigned int> TextureUnitMap;
	
	/** every RttUnit needs a unique identifer, as the backend uses a cefix::Display to store the different cameras and matrices*/
	RttUnit(const std::string& identifier, unsigned int w, unsigned int h, bool createTextureRectangle=false);
	
	/** adds an Texture as input */
	void addInput(const std::string& name, osg::Texture* tex, bool createUniform = true);
	
	/** adds an quad2DGeometry as input, it is using the attached texture */
	void addInput(const std::string& name, cefix::Quad2DGeometry* geo, bool createUniform = true);
	
	/** adds another RttUnit as input */
	void addInput(RttUnit* input, bool createUniform = true, bool attachToScene = true);
	
	/** removes an input */
	void removeInput(const std::string& name);
	
	/** get number of inputs */
	unsigned int getNumInputs() const { return _inputs.size(); }
	
	/** get number of outputs */
	unsigned int getNumOutputs() const { return _inputs.size(); }
	
	/** get all inputs */
	TextureMap& getInputs() { return _inputs; }
	
	/** get all inputs */
	const TextureMap& getInputs() const { return _inputs; }
	
	/** get a specific input */
	osg::Texture* getInput(const std::string& name);
	
	/** get all outputs */
	TextureMap& getOutputs() { return _outputs; }
	
	/** get all outputs */
	const TextureMap& getOutputs() const { return _outputs; }
	
	/** get a specific output */
	osg::Texture* getOutput(const std::string& name);
	
	/** create a quad an texture it with named output */
	cefix::Quad2DGeometry* getOutputAsQuad(const std::string& name, unsigned int w = 0, unsigned int h = 0);
	
	/** create a quad an texture it with named output */
	cefix::Quad2DGeometry* getOutputAsQuad(const std::string& name, const osg::Vec4 &rect);
	
	/** get or create an uniform for named input */
	osg::Uniform* getOrCreateUniformForInput(const std::string& name);
	
	/** get an uniform for named input */
	osg::Uniform* getUniformForInput(const std::string& name);
	
	/** attach a set of shaders, name is their common name the code assumes .fs and .vs as extensions */
	bool attachShaders(const std::string& name);
	
	/** create an output texture */
	osg::Texture* createOutputTexture(const std::string& name, osg::Camera::BufferComponent buffer, GLenum internal_format, bool generate_mipmaps = false, unsigned int samples = 0);
	
	/** adds an output texture */
	osg::Texture* setOutputTexture(const std::string& name, osg::Texture* tex, osg::Camera::BufferComponent buffer, GLenum internal_format, bool generate_mipmaps = false, unsigned int samples = 0);
	
	/** attaches an uniform */
	void attachUniform(osg::Uniform* uniform);
	
	/** attaches an uniform for named intput */
	void attachUniform(const std::string& name) {
		attachUniform(getOrCreateUniformForInput(name));
	}

	bool createAsTextureRectangle() const { return _createAsTextureRectangle; }
	
	const std::string& getIdentifier() { return _display->getIdentifier(); }

protected:
	void setStateSet(osg::StateSet* ss) { _ss = ss; }
	Display* getDisplay() { return _display; }
	/** implemented by subclasses, gets called, when a new camera was added */
	virtual void cameraAdded() = 0;
	
	/** implemented by subclasses, gets called, when a new input was added */
	virtual void inputAdded(const std::string& name, osg::Texture* tex) = 0;
	
private:
	unsigned int			_width, _height;
	osg::ref_ptr<Display>	_display;
	TextureMap				_inputs, _outputs;
	bool					_createAsTextureRectangle;
	osg::StateSet*			_ss;
	bool					_cameraAdded;
	UniformMap				_uniforms;
protected:
	TextureUnitMap			_units;
};


/** a SceneRttUnit is a specialized RttUNit to render a 3d-scene into a set of outputs */

class SceneRttUnit : public RttUnit {
public:
	SceneRttUnit(const std::string& identifier, unsigned int w, unsigned int h, bool createTextureRectangle=false)
	:	RttUnit(identifier, w, h, createTextureRectangle)
	{
	}
	
	/** get the 2d-world */
	osg::Group* get3DWorld() { return getDisplay()->get3DWorld(); }
	
	/** get the 3d-world */
	osg::Group* get2DWorld() { return getDisplay()->get2DWorld(); }
	
	/** get the scene camera */
	osg::Camera* getSceneCamera() { return getDisplay()->get3DCamera(); }

protected:
	virtual void cameraAdded() {}
	virtual void inputAdded(const std::string& name, osg::Texture* tex) {}

};


/** a RenderRttUnit is a specialized RttUnit to render a screenaligned quad, where all inputs are attached to the quad */ 
class RenderRttUnit : public RttUnit {
public:
	RenderRttUnit(const std::string& identifier, unsigned int w, unsigned int h, bool createTextureRectangle=false);


protected:
	virtual void cameraAdded();
	virtual void inputAdded(const std::string& name, osg::Texture* tex);
private:
	osg::ref_ptr<cefix::Quad2DGeometry>	_geo;
};




}


#endif