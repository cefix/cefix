//
//  main.c
//  cefixTemplate
//
//  Created by Stephan Huber on 26.11.06.
//  Copyright __MyCompanyName__ 2006. All rights reserved.
//

#include <cefix/AppController.h>
#include <osg/ShapeDrawable>

#include <cefix/DataFactory.h>
#include <cefix/AnimationFactory.h>
#include <cefix/MathUtils.h>
#include <cefix/ColorUtils.h>

#include <cefix/ThreadPool.h>

class TestJob;
class SceneGraphUpdater : public osg::Referenced {
	public:
		SceneGraphUpdater(osg::Group* g)
		:	osg::Referenced(),
			_g(g)
		{
		}
		
		void jobFinished(TestJob* job);
	private:
		osg::ref_ptr<osg::Group>	_g;
};


class TestJob : public cefix::ThreadPool::Job {

	public:
		TestJob(SceneGraphUpdater* sgu) : cefix::ThreadPool::Job(), _sgu(sgu) {}
		
		virtual void operator()() {
		
			osg::Timer t;
			osg::Timer_t start = t.tick();
			float dur = cefix::randomf(1.0f) * 0.01;
			while(t.delta_s(start, t.tick()) < dur)
				;

			const float range(5.0f);	
			_geode = new osg::Geode();
			osg::ShapeDrawable* drawable = new osg::ShapeDrawable(
				new osg::Box(
					osg::Vec3(
						cefix::floatRange(-range,range).random(),
						cefix::floatRange(-range,range).random(),
						cefix::floatRange(-range,range).random()
					)
				, 0.3f)
			);
			drawable->setColor(cefix::HSVtoRGB(cefix::floatRange(0,360).random(), 1,1));
			_geode->addDrawable(drawable);
				
			callOnMainThread(_sgu.get(), &SceneGraphUpdater::jobFinished, this);
		}
		
		osg::Node* getNode() {
			return _geode.get();
		}
		
	private:
		osg::ref_ptr<SceneGraphUpdater>	_sgu;
		osg::ref_ptr<osg::Geode>		_geode;

};



void SceneGraphUpdater::jobFinished(TestJob* job) 
{
	_g->addChild(job->getNode());

}

class ThreadPoolObserver : public cefix::AnimationBase {

	public:
		ThreadPoolObserver(cefix::ThreadPool* pool, SceneGraphUpdater* sgu) : cefix::AnimationBase(), _pool(pool), _sgu(sgu) {}
		
	protected:
		virtual void animate(float elapsed) {
		
			if ((elapsed - _last) > 1)
			{
				
				if (elapsed < 20) {
					for (int i = 0; i < 30; ++i)
						_pool->addJob(new TestJob(_sgu.get()));
				
				} 
				
				std::cout << (*_pool) << std::endl;
				
				if (_pool->done())
					setFinishedFlag();
				_last = elapsed;
			}
		}
		
		osg::ref_ptr<cefix::ThreadPool> _pool;
		osg::ref_ptr<SceneGraphUpdater>	_sgu;
		float _last;
};





class MyAppController: public cefix::AppController {

	public:
		osg::Group * createWorld();
	
		cefix::ThreadPool* getPool() { return _pool.get(); }
		
	protected:
		virtual ~MyAppController() {
		}
		
		osg::ref_ptr<cefix::ThreadPool>	_pool;

};


osg::Group * MyAppController::createWorld() {
		
	osg::ref_ptr<osg::Group> g = new osg::Group();
	osg::ref_ptr<SceneGraphUpdater> sgu = new SceneGraphUpdater(g.get());
	
	_pool = new cefix::ThreadPool(4);
	_pool->startPausing();
	for(unsigned int i = 0; i < 3000; ++i) 
	{
		TestJob* job = new TestJob(sgu.get());
		_pool->addJob(job );
	}
	cefix::AnimationFactory::instance()->add(new ThreadPoolObserver(_pool.get(), sgu.get()));
	return g.get();
}





int main(int argc, char* argv[])
{
	cefix::DataFactory::instance()->setApplicationPath(argv[0]);
	osg::ref_ptr<MyAppController> app = new MyAppController();
	
	cefix::ThreadPool::setNotifySeverity(osg::ALWAYS);
	
	app->setUseOptimizerFlag(false);
	// create the world and apply it
    app->applyWorld(app->createWorld());
	
	app->getPool()->stopPausing();

    //app->requestPicking();
    //app->requestPostScriptWriter();
    
    //create a viewer-window/fullscreen
    app->realize();
	app->run();
	app = NULL;
	return 0;
}


