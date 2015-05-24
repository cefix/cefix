/*
 *  test_animation.cpp
 *  cefix
 *
 *  Created by Stephan Huber on 08.05.06.
 *  Copyright 2006 digitalmind.de // Stephan Maximilian Huber. All rights reserved.
 *
 */


 

#include <cefix/Log.h>
#include <osg/ShapeDrawable>
#include <osg/Geode>
#include <osg/Group>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osg/Material>


#include <cefix/AnimationFactory.h>
#include <cefix/AnimationBasedOnDuration.h>
#include <cefix/MathUtils.h>
#include <cefix/AppController.h>
#include <cefix/Timer.h>

osg::Geode* createCube() { 


    osg::Geode* geode = new osg::Geode();
    osg::Box* box = new osg::Box(osg::Vec3(0,0,0), 1.0f);
    osg::ShapeDrawable* drawable = new osg::ShapeDrawable(box);
    geode->addDrawable(drawable);
    
    return geode;

}


// Beispiel für eine Animationsklasse, die eine Matrix zeitbasiert interpoliert
class AnimateMatrixTransform : public cefix::AnimationBasedOnDuration {
    
    public:
        AnimateMatrixTransform(float startTime, float duration, osg::MatrixTransform* mat,
                osg::Matrix start_m, osg::Matrix end_m) : 
                AnimationBasedOnDuration(startTime, duration),
                _mat(mat),
                _start_m(start_m), 
                _end_m(end_m) 
        {
        }
        
        virtual void animate(float elapsed) {
            AnimationBasedOnDuration::animate(elapsed);
            osg::Matrix m;
            
            for (int i = 0; i < 4; i++) 
                for(int j = 0; j < 4; j++) {
                    m(i,j) = interpolate<float>(_start_m(i,j), _end_m(i,j));
                }
                
            _mat->setMatrix(m);
        }
        
    
    protected:
        osg::Matrix _start_m, _end_m;
        osg::ref_ptr<osg::MatrixTransform> _mat;
};

// beispiel für ne animationsklasse, die zeitbasiert von alpa a nach alpha b animiert
class AnimateMaterialAlpha : public cefix::AnimationBasedOnDuration {
    public:
        AnimateMaterialAlpha(float startTime, float duration, 
            osg::Material* mat, float startAlpha, float endAlpha) : 
            AnimationBasedOnDuration(startTime, duration), _mat(mat), _start_alpha(startAlpha), _end_alpha(endAlpha) 
        {
        }
        
        virtual void animate(float elapsed) {
            AnimationBasedOnDuration::animate(elapsed);
            _mat->setAlpha(osg::Material::FRONT_AND_BACK, interpolate<float>(_start_alpha, _end_alpha) );
        }
    protected:
    
        osg::ref_ptr<osg::Material> _mat;
        float _start_alpha, _end_alpha;
};

// beispiel für ne animationsklasse, die total benutzerdefiniert animiert
class AnimatePositionAttitudeTransform : public cefix::AnimationBase {
    public:
        AnimatePositionAttitudeTransform(float starttime, osg::PositionAttitudeTransform* pat) : cefix::AnimationBase(starttime), _pat(pat) {}
        
        virtual void init() {
        
            _scale = 1.0f;
            _attitude = _pat->getAttitude();
            animate(0);
        
        }
        
        virtual void animate(float elapsed_time) {
            
            if (cefix::randomf(1.0f) <0.01f) {
                _target = osg::Quat(cefix::randomf(360.0f), osg::Vec3(cefix::randomf(1.0f),cefix::randomf(1.0f),cefix::randomf(1.0f)));
                _target /= _target.length();
            }
            _scale = 0.5f - 0.5f * cos(elapsed_time/30.0 * osg::PI);
            _pat->setScale(osg::Vec3(_scale, _scale, _scale));
            _attitude.slerp(0.01,_attitude, _target);
            _pat->setAttitude(_attitude);
        }
    
    protected:
        osg::ref_ptr<osg::PositionAttitudeTransform> _pat;
        float _scale;
        osg::Quat _attitude, _target;
};


class CleanupFunctorDemoHandler : public osg::Referenced {

	public:
		void calledWhenAnimationFinished() {
			osg::notify(osg::ALWAYS) << "Animation finished" << std::endl;
		}

};


class TestTimer : public cefix::Timer {

	public:
		TestTimer() : cefix::Timer(100, 10) {};
		
		virtual void timer() {
			std::cout << "testtimer :: " << getCount() << std::endl;
		}
};

class TestFunctorTimer : public osg::Referenced 
{
	public:
		void calledRegularly(cefix::Timer* timer) {
			std::cout << "calledRegularly " << timer->getCount() << std::endl;
		}
};


int main(int argc, char *argv[])
{
    osg::setNotifyLevel(osg::INFO);
    osg::Group* g = new osg::Group();
	
	CleanupFunctorDemoHandler* cfdh = new CleanupFunctorDemoHandler();
	
	// TimePolicyFunctor-Test:
	cefix::AnimationTimeBase::instance()->setImplementation(new cefix::AnimationTimeBase::FixedStepImplementation(0.001));
    
    // AnimationController erzeugen
    cefix::AnimationController* actrl = cefix::AnimationFactory::instance()->createController();
	
	osg::ref_ptr<TestTimer> tt = new TestTimer();
	tt->start();
	
	osg::ref_ptr<TestFunctorTimer> tft = new TestFunctorTimer();
	
	cefix::Timer* tft_timer = cefix::FunctorTimer::create(333, tft.get(), &TestFunctorTimer::calledRegularly);
	tft_timer->start();
    
    for (int i = 0; i < 10; i++) {
        osg::Geode* cube = createCube();
        // material setzen
        osg::StateSet* st = cube->getOrCreateStateSet();
        osg::Material* material = new osg::Material();
        material->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(1.0f, 0, 0,1.0f));
        st->setAttribute(material, osg::StateAttribute::OVERRIDE);
        st->setMode(GL_BLEND, osg::StateAttribute::ON);
        
        osg::MatrixTransform* mat = new osg::MatrixTransform();
        mat->addChild(cube);
        osg::Matrix start_m = osg::Matrix::translate(i*2-10,0,0);
        osg::Matrix end_m   = osg::Matrix::translate((10 - i)-5, -i, i-5) ;
        // Matrix animieren:
        actrl->add(new AnimateMatrixTransform(i, 3.0f, mat, start_m, end_m));
        // material animieren, ausblenden
        actrl->add(new AnimateMaterialAlpha(8 + i, 2.0f, material, 1.0f, 0.0f));
        // material animieren, einblenden
        actrl->add(new AnimateMaterialAlpha(12 + i, 2.0f, material, 0.0f, 1.0f));
        
        //und wieder zurückbewegen
        actrl->add(new AnimateMatrixTransform(12+i, 0.5f, mat, end_m, start_m));
		
		//register cleanup-functor
        actrl->addCleanupFunctor<CleanupFunctorDemoHandler>(cfdh, &CleanupFunctorDemoHandler::calledWhenAnimationFinished);
        
		mat->setMatrix(start_m);
        g->addChild(mat);
    }
    
    // alles initialisieren
    actrl->init();
    
    
    // so, jetzt zum spass noch einen zweiten Controller erzeugen:
    cefix::AnimationController* actrl2 = cefix::AnimationFactory::instance()->createController();
    osg::PositionAttitudeTransform* pat = new osg::PositionAttitudeTransform();
    pat->addChild(g);
    actrl2->add(new AnimatePositionAttitudeTransform(0, pat));
    
   
    {
		cefix::AppController* app = new cefix::AppController();
		app->setThreadingModel(osgViewer::ViewerBase::AutomaticSelection);
		app->applyWorld(g);
		app->run();
	}
	
	return 0;
}

