#include <cefix/AppController.h>
#include <cefix/MathUtils.h>
#include <cefix/LineStripGeometry.h>
#include <cefix/DataFactory.h>
#include <cefix/AnimationFactory.h>
#include <cefix/TextGeode.h>
#include <cefix/Quad2DGeometry.h>
#include <cefix/Tweening.h>
#include <cefix/FileUtils.h>
#include <osgDB/ReadFile>

namespace cefix {

	
	
	
	

}

template <class TweenerClass>
class TweenAnimation : public cefix::AnimationBase {

public:
	TweenAnimation(osg::Vec3Array* vertices) 
	:	cefix::AnimationBase(0),
		_v(vertices),
		_dt(0.0f),
		_lastElapsed(0.0f)
	{
		_v->resize(100);
		for(unsigned int i=0; i < 100; ++i) {
			(*_v)[i][0] = i;
			(*_v)[i][1] = i;
		}
	}

protected:
	virtual void animate(float elapsed) 
	{
		_dt += (elapsed - _lastElapsed);
		if(_dt > 3.0f) {
			_dt -= 3.0f;
			for(unsigned int i=0; i < 100; ++i) {
				(*_v)[i][0] = i;
				(*_v)[i][1] = i;
			}
		}
		_lastElapsed = elapsed;
		
		float x = osg::minimum(1.0f, _dt / 3.0f);;
		float y = cefix::tween<TweenerClass>(0.0f,100.0f, x);
		(*_v)[x*100][0] = x*100;
		(*_v)[x*100][1] = y;
	}
	
private:
	osg::Vec3Array* _v;
	float _dt, _lastElapsed;

};

class MyAppController: public cefix::AppController {

	public:
		
		template<class TweeningClass>
		osg::Node* createTweeningDemonstration(const std::string& name)
		{
			osg::Geode* geode = new osg::Geode();
			osg::Geometry* geo = new osg::Geometry();
			osg::Vec3Array* v = new osg::Vec3Array();
			v->resize(100);
			geo->setVertexArray(v);
			osg::Vec4Array* c = new osg::Vec4Array();
			c->push_back(osg::Vec4(0,1,1,1));
			geo->setColorArray(c);
			geo->setColorBinding(osg::Geometry::BIND_OVERALL);
			geo->setSupportsDisplayList(false);
			
			geo->addPrimitiveSet(new osg::DrawArrays(GL_LINE_STRIP, 0, 100));
			
			TweenAnimation<TweeningClass>* an = new TweenAnimation<TweeningClass>(v);
			cefix::AnimationFactory::instance()->add(an);
			
			geode->addDrawable(geo);
			
			cefix::Quad2DGeometry* quad = new cefix::Quad2DGeometry(0,0,100,100);
			quad->setColor(osg::Vec4(0.1,0.1,0.1,1));
			quad->setLocZ(-1);
			geode->addDrawable(quad);
			
			osg::Group* group = new osg::Group();
			group->addChild(geode);
			cefix::Utf8TextGeode*tg = new cefix::Utf8TextGeode("system.xml", 0, name);
			tg->setPosition(osg::Vec3(0,-10, 10));
			tg->setTextColor(osg::Vec4(1,1,1,1));
			group->addChild(tg);
			return group;
		}
		
		osg::Group * createWorld();
		
		virtual void realize() 
		{
			cefix::AppController::realize();
			std::string result;
			
			if (cefix::showSaveFileDialog(result, "test.html")) {
				std::cout << "save into " << result << std::endl;
			}
			
		}		
	protected:
		virtual ~MyAppController() {
		
		}
		
		
};


osg::Group * MyAppController::createWorld() {

	//osg::Image* img = osgDB::readImageFile("H:/Dokumente und Einstellungen/stephan/Eigene Dateien/1234567890/0123456789/1234567890/cefix/audi/AudiIAA2009/media/infoscreen/items/r8series/items/r8/items/materialien/items/displayData/alu_schmiederaeder_ae_im_10_speichen_y_design_hochglanzpoliert_85x19_x_11x19.png");
	// std::cout << img << " " << img->getFileName() << std::endl;
		
	osg::ref_ptr<osg::Group> g = new osg::Group();
	
	std::vector< osg::ref_ptr<osg::Node> > nodes;
	std::vector< osg::ref_ptr<osg::Node> >::iterator itr;
	nodes.push_back(createTweeningDemonstration<cefix::LinearTweening<float> >("LinearTweening"));
	nodes.push_back(createTweeningDemonstration<cefix::EasyInOutTweening<float> >("EasyInOutTweening"));
	
	nodes.push_back(createTweeningDemonstration<cefix::QuadricInTweening<float> >("QuadricInTweening"));
	nodes.push_back(createTweeningDemonstration<cefix::QuadricOutTweening<float> >("QuadricOutTweening"));
	nodes.push_back(createTweeningDemonstration<cefix::QuadricInOutTweening<float> >("QuadricInOutTweening"));
	
	nodes.push_back(createTweeningDemonstration<cefix::CubicInTweening<float> >("CubicInTweening"));
	nodes.push_back(createTweeningDemonstration<cefix::CubicOutTweening<float> >("CubicOutTweening"));
	nodes.push_back(createTweeningDemonstration<cefix::CubicInOutTweening<float> >("CubicInOutTweening"));
	
	nodes.push_back(createTweeningDemonstration<cefix::QuartInTweening<float> >("QuartInTweening"));
	nodes.push_back(createTweeningDemonstration<cefix::QuartOutTweening<float> >("QuartOutTweening"));
	nodes.push_back(createTweeningDemonstration<cefix::QuartInOutTweening<float> >("QuartInOutTweening"));

	nodes.push_back(createTweeningDemonstration<cefix::QuintInTweening<float> >("QuintInTweening"));
	nodes.push_back(createTweeningDemonstration<cefix::QuintOutTweening<float> >("QuintOutTweening"));
	nodes.push_back(createTweeningDemonstration<cefix::QuintInOutTweening<float> >("QuintInOutTweening"));	
	
	nodes.push_back(createTweeningDemonstration<cefix::SineInTweening<float> >("SineInTweening"));
	nodes.push_back(createTweeningDemonstration<cefix::SineOutTweening<float> >("SineOutTweening"));
	nodes.push_back(createTweeningDemonstration<cefix::SineInOutTweening<float> >("SineInOutTweening"));	
	
	nodes.push_back(createTweeningDemonstration<cefix::CircularInTweening<float> >("CircularInTweening"));
	nodes.push_back(createTweeningDemonstration<cefix::CircularOutTweening<float> >("CircularOutTweening"));
	nodes.push_back(createTweeningDemonstration<cefix::CircularInOutTweening<float> >("CircularInOutTweening"));	
		
	nodes.push_back(createTweeningDemonstration<cefix::ElasticInTweening<float> >("ElasticInTweening"));
	nodes.push_back(createTweeningDemonstration<cefix::ElasticOutTweening<float> >("ElasticOutTweening"));
	nodes.push_back(createTweeningDemonstration<cefix::ElasticInOutTweening<float> >("ElasticInOutTweening"));	
	
	nodes.push_back(createTweeningDemonstration<cefix::OvershootInTweening<float> >("OvershootInTweening"));
	nodes.push_back(createTweeningDemonstration<cefix::OvershootOutTweening<float> >("OvershootOutTweening"));
	nodes.push_back(createTweeningDemonstration<cefix::OvershootInOutTweening<float> >("OvershootInOutTweening"));	
	
	nodes.push_back(createTweeningDemonstration<cefix::BounceInTweening<float> >("BounceInTweening"));
	nodes.push_back(createTweeningDemonstration<cefix::BounceOutTweening<float> >("BounceOutTweening"));
	nodes.push_back(createTweeningDemonstration<cefix::BounceInOutTweening<float> >("BounceInOutTweening"));	
	
	itr = nodes.begin();
	
	osg::MatrixTransform* root_mat = new osg::MatrixTransform();
	root_mat->setMatrix(osg::Matrix::translate(20,20,20));
	
	for(unsigned int y = 0; y < 5; ++y) {
		for(unsigned int x = 0; x < 8; ++x) 
		{
			if (itr != nodes.end()) {
				osg::MatrixTransform* mat = new osg::MatrixTransform();
				mat->setMatrix(osg::Matrix::translate(osg::Vec3(x*120, y* 120, 0)));
				mat->addChild(itr->get());
				root_mat->addChild(mat);
				++itr;
			}
		}
	}
	get2DLayer()->addChild(root_mat);
	get2DLayer()->getOrCreateStateSet()->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);
	
	return g.release();
}





int main(int argc, char* argv[])
{
	cefix::DataFactory::instance()->setApplicationPath(argv[0]);
	osg::ref_ptr<MyAppController> app = new MyAppController();
	osg::setNotifyLevel(osg::DEBUG_INFO);

	
	app->setUseOptimizerFlag(false);
	// create the world and apply it
    app->applyWorld(app->createWorld());
	


    //app->requestPicking();
    //app->requestPostScriptWriter();
    
    //create a viewer-window/fullscreen
    app->realize();
	app->run();
	app = NULL;
	return 0;
}
