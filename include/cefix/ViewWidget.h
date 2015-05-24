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


#ifndef VIEW_WIDGET_HEADER
#define VIEW_WIDGET_HEADER

#include <cefix/AbstractWidget.h>
#include <cefix/Quad2DGeometry.h>
#include <cefix/PropertyList.h>
#include <osg/Geode>

namespace cefix {

class CEFIX_EXPORT ViewWidgetBase {

public:
	enum Mode {ORTHO, PERSPECTIVE, CUSTOM};
	enum RenderMode { DIRECT, BITMAP };
		
		
	virtual ~ViewWidgetBase() {}
};

class NoOpWidget : public AbstractWidget {
public:
	NoOpWidget(const std::string& identifier) : AbstractWidget(identifier) {}
	
	virtual void update() {};
};

template <class BASE>

class CEFIX_EXPORT ViewWidgetT : public BASE, public ViewWidgetBase {
	public:
		ViewWidgetT(const std::string identifier, const osg::Vec3& pos, const osg::Vec2& size, Mode mode = ORTHO, RenderMode rendermode = BITMAP) : 
			BASE(identifier),
			ViewWidgetBase(),
			_pos(pos), 
			_size(size),
			_mode(mode),
			_rendermode(rendermode),
			_verticalFieldOfView(45),
			_preRender(true)
		{
			init();
		}
		
		ViewWidgetT(cefix::PropertyList* pl) :
			BASE(pl->get("id")->asString()),
			ViewWidgetBase(),
			_pos(pl->get("position")->asVec3()), 
			_size(pl->get("size")->asVec2())
		{
			_mode = ORTHO;
			_rendermode = BITMAP;
			std::string modeStr = strToLower(pl->get("mode")->asString());
			if (modeStr == "perspective") _mode = PERSPECTIVE;
			else if (modeStr == "custom") _mode = CUSTOM;
			
			std::string renderModeStr = strToLower(pl->get("rendermode")->asString());
			if (renderModeStr == "direct") _rendermode = DIRECT;
			if (pl->hasKey("prerender")) _preRender = (pl->get("prerender")->asInt() !=0 );
			
			init();
		}
		
		
		
		osg::Camera* getCamera() { return _camera.get(); }
		
		virtual osg::Node* getNode() { return _group.get(); }
		
		virtual void move(const osg::Vec3& pos) { _pos = pos; applyMode(); }
		virtual void resize(float width, float height) { _size = osg::Vec2(width, height); applyMode(); }
		void setVerticalFieldOfView(double fov) { _verticalFieldOfView = fov; applyMode(); }
		
		Mode getMode() { return _mode; }
		
		const osg::Vec3& getPosition() const { return _pos; }
		float getWidth() { return _size[0]; }
		float getHeight() { return _size[1]; }
		
	protected:
		void init() {
			_group  = new osg::Group();
			_camera = new osg::Camera();
			_group->addChild(_camera.get());
			
			getCamera()->setClearMask(GL_DEPTH_BUFFER_BIT);
			getCamera()->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
			
			if (_rendermode == BITMAP) {
				setupBitmapRendering();
			}
			getCamera()->setRenderOrder((_preRender) ? osg::Camera::PRE_RENDER: osg::Camera::POST_RENDER);
			getCamera()->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
			applyMode();
		}
		
		void applyMode()
		{
			if (_rendermode == DIRECT)
				getCamera()->setViewport(new osg::Viewport(_pos[0], _pos[1], _size[0], _size[1]));
			else {
				getCamera()->setViewport(new osg::Viewport(0, 0, _size[0], _size[1]));
				_quad->setRect(_pos[0], _pos[1], _size[0], _size[1]);
				_quad->setLocZ(_pos[2]);
			}
				
			switch(getMode()) {
				case ORTHO:
					getCamera()->setProjectionMatrix( osg::Matrix::scale(2.0/_size[0], 2.0/_size[1], -2/1000.0f) * osg::Matrix::translate(-1, -1, 1) );
					getCamera()->setViewMatrix(osg::Matrix::identity());
					getCamera()->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
					break;
					
				case PERSPECTIVE:
					getCamera()->setProjectionMatrix(osg::Matrix::perspective(_verticalFieldOfView, _size[0] / (double)_size[1] ,1, 1000));
					break;
					
				case CUSTOM:
					applyCustomMode(); 
					break;
			}
		}
		
		virtual void applyCustomMode() {}

		void setupBitmapRendering()
		{

			osg::TextureRectangle* textureRect = new osg::TextureRectangle;
			textureRect->setTextureSize(_size[0], _size[1]);
			textureRect->setInternalFormat(GL_RGBA);
			textureRect->setFilter(osg::Texture2D::MIN_FILTER,osg::Texture2D::LINEAR);
			textureRect->setFilter(osg::Texture2D::MAG_FILTER,osg::Texture2D::LINEAR);
			
			getCamera()->attach(osg::Camera::COLOR_BUFFER, textureRect);
			getCamera()->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
			getCamera()->setClearColor(osg::Vec4(0,0,0,0));
			getCamera()->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			osg::Geode* geode = new osg::Geode();
			_quad = new cefix::Quad2DGeometry(_pos[0], _pos[1], _size[0], _size[1], _pos[2]);
			_quad->setTexture(textureRect);
			geode->addDrawable(_quad.get());
			_group->addChild(geode);

		}

		
	private:
		osg::Vec3					_pos;
		osg::Vec2					_size;
		Mode						_mode;
		RenderMode					_rendermode;
		double						_verticalFieldOfView;
		osg::ref_ptr<osg::Camera>	_camera;
		osg::ref_ptr<osg::Group>	_group;
		osg::ref_ptr<cefix::Quad2DGeometry> _quad;
		bool						_preRender;

};

typedef ViewWidgetT<NoOpWidget> ViewWidget;

} // end of namespace

#endif