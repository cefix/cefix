//
//  main.c
//  cefixTemplate
//
//  Created by Stephan Huber on 26.11.06.
//  Copyright __MyCompanyName__ 2006. All rights reserved.
//

#include <cefix/MouseLocationProvider.h>
#include <cefix/AppController.h>
#include <osg/ShapeDrawable>
#include <osg/MatrixTransform>
#include <osg/Depth>
#include <osg/BlendFunc>
#include <osg/io_utils>

#include <cefix/AnimationFactory.h>
#include <cefix/ColorUtils.h>
#include <cefix/MathUtils.h>
#include <cefix/Perlin.h>

#include <cefix/LineGridGeometry.h>
#include <cefix/VectorField.h>
#include <cefix/VectorFieldGeometry.h>

#include <cefix/ParticleProcessor.h>
#include <cefix/ParticleOperator.h>
#include <cefix/ParticleOperationStack.h>

#include "LineParticle.h"
#include <cefix/CommonParticleOperators.h>
#include <cefix/ParticleFountainEmitter.h>
#include <cefix/ParticleBoxPlacer.h>
#include <cefix/ParticleFixedDirectionShooter.h>
#include <cefix/ParticleRandomRateCounter.h>

#include <cefix/Quad2DGeometry.h>
#include <cefix/TextGeode.h>

class MotionBlurCallback : public osg::NodeCallback {

	public: 
		MotionBlurCallback(cefix::Quad2DGeometry* geo, osg::Vec4 backcolor, float persistence = 0.25) : osg::NodeCallback(), _geo(geo), _backcolor(backcolor), _clear(true), _persistence(persistence) {}
		
		virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) {
		
			if (_clear) {
				_geo->setColor(_backcolor);
				_clear = false; 
				_t0 = nv->getFrameStamp()->getReferenceTime();
			}
			else {
					double t = nv->getFrameStamp()->getReferenceTime();
			        double dt = fabs(t - _t0);
					_t0 = t;

					
					// compute the blur factor
					double s = powf(0.2, dt / _persistence);
					osg::Vec4 c = _backcolor;
					c[3] = 1-s;
					_geo->setColor(c);
			
			} 
		}

	private:
		osg::observer_ptr<cefix::Quad2DGeometry> _geo;
		osg::Vec4									_backcolor;
		bool _clear;
		double _t0;
		double _persistence;
};






class ParticleMouseAttractor : public  cefix::ParticleOperator<cefix::Particle> {
	public:
		ParticleMouseAttractor(float attractradius = 1.0f, float weight = 1.0f) : cefix::ParticleOperator<cefix::Particle>(weight), _attractRadius(attractradius*attractradius) {}
		
		inline void operator()(cefix::Particle* p, const cefix::Particle::time_t&) {
				cefix::WindowPickable* mlp = cefix::MouseLocationProvider::instance();
				osg::Vec3 pos = (mlp->getNear() + mlp->getFar()) / 2.0f;
				osg::Vec3 delta = pos - p->getPosition() - p->getVelocity();
				delta.normalize();
				//if (delta.length() > _attractRadius)
					p->addVelocity(delta  * getWeight());
			
		}
	private:
		osg::ref_ptr<cefix::Particle>	_attractor;
		float							_attractRadius;
};


class UpdateGeodeOperator : public cefix::ParticleOperator<LineParticle> {
	public: 
		UpdateGeodeOperator(osg::Geode* geode) : cefix::ParticleOperator<LineParticle>(), _geode(geode) { setOperatesOnEveryParticle(false); }
		
		inline void finish() {
			static int cnt = 0;
			++cnt;
			// update every geometry and their primitivesets
			osg::Geometry* geo;
			
			for(unsigned int i = 0;  i < _geode->getNumDrawables(); ++i) {
				geo = dynamic_cast<osg::Geometry*>(_geode->getDrawable(i));
				if (geo) {
					/*for (unsigned int j = 0; j < geo->getNumPrimitiveSets(); ++j) {
						da = dynamic_cast<osg::DrawArrays*>(geo->getPrimitiveSet(j));
						if (da) {
							vcount = geo->getVertexArray()->getNumElements();
							da->setCount(vcount);
							if (vcount > 40) {
								da->setCount(40);
								da->setFirst(vcount - 40);
							}
						}
								
					}*/
					//geo->dirtyDisplayList();
					geo->dirtyBound();
				}
			}
		}
	private:
		osg::ref_ptr<osg::Geode> _geode;
};


namespace cefix {
	

	


}



class NodeParticle : public cefix::Particle {
	
	public:
		NodeParticle() : cefix::Particle() {
		
			_mat = new osg::MatrixTransform();
			
			osg::Geode* geode = new osg::Geode();
			osg::ShapeDrawable* shape = new osg::ShapeDrawable(new osg::Box(osg::Vec3(0,0,0), 1,1,2));
			shape->setColor(cefix::HSVtoRGB(cefix::randomf(360.0f), 1,1));
			geode->addDrawable(shape);
			_mat->addChild(geode);
			
			setLifeTime(5.0f);
		}
		
		inline void updateRepresentation() 
		{
			_mat->setMatrix(osg::Matrix::rotate(osg::Vec3(0,0,1), getDirection()) * osg::Matrix::translate(getPosition()));
		}

		osg::Node* getNode() { return _mat.get(); }

	protected:
		
		virtual ~NodeParticle() {
			for(unsigned int i = 0; i < _mat->getNumParents(); ++i) 
				_mat->getParent(i)->removeChild(_mat.get());
		}
	
	private:
		osg::ref_ptr<osg::MatrixTransform>	_mat;

};

class RotatePlaneUpdateCallback: public osg::NodeCallback {
	public:
		RotatePlaneUpdateCallback(osg::MatrixTransform* mat) : osg::NodeCallback(), _mat(mat) {}
		
		virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) {
		
			if (_mat.valid()) _mat->setMatrix(osg::Matrix::rotate(osg::DegreesToRadians(60.0f), 0, 1, 0) *  osg::Matrix::rotate(osg::DegreesToRadians(90.0f), 0, 0, 1) * osg::Matrix::translate(60, 0, 5) * osg::Matrix::rotate(osg::inRadians(_angle), 0, 0, 1)   );
			_angle += 0.01;
		}
		
	private:
		osg::observer_ptr<osg::MatrixTransform>	_mat;
		double									_angle;

};

namespace cefix {

class LineParticleNode : public LineParticle {

	public: 
		LineParticleNode() : LineParticle() {
		
		
			const float vertCount = 15;
			
			osg::Geode* geode = new osg::Geode();
					
			setVertexCount(vertCount);
			setLifeTime(7.0f);
			
			// jetzt erzeugen wir noch ne geometrie für dieses Partikel:
			osg::Geometry* geo = new osg::Geometry();
			osg::Vec4Array* colors = new osg::Vec4Array();
			osg::Vec4 c(cefix::HSVtoRGB(180+ cefix::randomf(40), 1,0.5));
			c[3] = 0.5f;
			
			
			for (unsigned int j = 0; j < vertCount; ++j){	
				//c[3] = 0.2f + j / (float)vertCount * 0.5f;
				colors->push_back(c);
			}

			geo->setVertexArray(getVertexArray());
			geo->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP, 0, vertCount));
			
			geo->setColorArray(colors);
			geo->setColorBinding(osg::Geometry::BIND_OVERALL);
			
			osg::Vec3Array* normals = new osg::Vec3Array();
			normals->push_back(osg::Vec3(0,1,0));
			geo->setNormalArray(normals);  			
			
			geode->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
			geode->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
			
			geode->addDrawable(geo);
			
			_geode = geode;
			_geometry = geo;
		}
		
		osg::Node* getNode() { return _geode.get(); }
		
		
		void updateRepresentation() {
			LineParticle::updateRepresentation();
			_geometry->dirtyBound();
			_geometry->dirtyDisplayList();
		}
		
	protected:
		virtual ~LineParticleNode() {
		
			for(unsigned int i = 0; i < _geode->getNumParents(); ++i) 
				_geode->getParent(i)->removeChild(_geode.get());
		}
		
	private:
		osg::ref_ptr<osg::Geode>	_geode;
		osg::ref_ptr<osg::Geometry>	_geometry;

};

}


class LineParticleEmitter : public cefix::ParticleEmitterT<
	cefix::LineParticleNode, 
	cefix::ParticleSteadyRateCounter<cefix::LineParticleNode>, 
	cefix::ParticleBoxPlacer<cefix::LineParticleNode>, 
	cefix::ParticleFixedDirectionShooter<cefix::LineParticleNode>
> {

	public:
		LineParticleEmitter(osg::Group* group) : 
			cefix::ParticleEmitterT<cefix::LineParticleNode, cefix::ParticleSteadyRateCounter<cefix::LineParticleNode>, cefix::ParticleBoxPlacer<cefix::LineParticleNode>, cefix::ParticleFixedDirectionShooter<cefix::LineParticleNode> >(
				new cefix::ParticleSteadyRateCounter<cefix::LineParticleNode>(50),
				new cefix::ParticleBoxPlacer<cefix::LineParticleNode>(osg::Vec3(10,10,10), 18,18,18),
				new cefix::ParticleFixedDirectionShooter<cefix::LineParticleNode>(osg::Vec3(10,0,0), cefix::floatRange(1.0f, 10.0f))),
			_root(group)
		{
		}
		
		virtual void mergeNewParticle(cefix::LineParticleNode* p, bool recycled) {
			if (!recycled) _root->addChild(p->getNode());
		}
		
		
	
	private:
		osg::ref_ptr<osg::Group>	_root;
};
template <class ParticleClass>
class FluidFrictionParticleEmitter : public cefix::ParticleEmitterT<ParticleClass, cefix::ParticleSteadyRateCounter<ParticleClass>, cefix::ParticleBoxPlacer<ParticleClass>, cefix::ParticleRadialShooter<ParticleClass> > {

	public:
		FluidFrictionParticleEmitter(osg::Group* group) : 
			cefix::ParticleEmitterT<ParticleClass, cefix::ParticleSteadyRateCounter<ParticleClass>, cefix::ParticleBoxPlacer<ParticleClass>, cefix::ParticleRadialShooter<ParticleClass> >(
				new cefix::ParticleSteadyRateCounter<ParticleClass>(200),
				new cefix::ParticleBoxPlacer<ParticleClass>(osg::Vec3(10,10,10), 20, 20, 20),
				new cefix::ParticleRadialShooter<ParticleClass>(cefix::floatRange(10,30))),
			_root(group)
		{
			this->getShooter()->setSpeedRange(0,0);
		}
		
		virtual void applySettings(ParticleClass* p, bool recycled) {
			float density = 1.0f;
			float radius = 0.25f * 10;
			p->setMass(1.28f * density*radius*radius*radius*osg::PI*4.0f/3.0f);
			p->setRadius(radius);
			p->setLifeTime(cefix::floatRange(3,15).random());
		}
		
		virtual void mergeNewParticle(ParticleClass* p, bool recycled) {
			if (!recycled)
				_root->addChild(p->getNode());
		}
		
		
	
	private:
		osg::ref_ptr<osg::Group>	_root;
};


class TextParticleNode : public cefix::Particle {
	public:
		TextParticleNode() : cefix::Particle() {
			setLifeTime(10);
			setMayRotate(true);
			_mat = new osg::MatrixTransform();
			if (cefix::randomf(1.0f) < 0.5f)
				_textgeode = new cefix::AsciiTextGeode("Palatino_32.xml", 24, "Partikel");
			else
				_textgeode = new cefix::AsciiTextGeode("VendettaLight_24.xml", 24, "sind toll");
				
			_textgeode->setTextColor(cefix::HSVtoRGB(cefix::randomf(360.0f), 1,0.7f));
			_textgeode->setTextAlignment(cefix::AsciiTextGeode::RenderIterator::ALIGN_CENTER);
			_mat->addChild(_textgeode.get());
		}
		
		osg::Node* getNode() { return _mat.get(); }
		
		inline void updateRepresentation() {
			_mat->setMatrix( osg::Matrix::rotate(getRotation()) * osg::Matrix::translate(getPosition()) );
			_textgeode->setAlpha(0.1f * (1 - getLifeScalar()));
			_mat->setNodeMask(isAlive() ? 0xffff : 0x0);
			
		}
		
	protected:
		virtual ~TextParticleNode() {
			for(unsigned int i = 0; i < _mat->getNumParents(); ++i) 
				_mat->getParent(i)->removeChild(_mat.get());
		}
	
	private:
		osg::ref_ptr<osg::MatrixTransform> _mat;
		osg::ref_ptr<cefix::AsciiTextGeode> _textgeode;
};


class TextParticleEmitter : public cefix::ParticleEmitterT<TextParticleNode, cefix::ParticleRandomRateCounter<TextParticleNode>, cefix::ParticlePointPlacer<TextParticleNode>, cefix::ParticleRadialShooter<TextParticleNode> > {

	public:
		TextParticleEmitter(osg::Group* group, cefix::Particle::time_t lifetime) : 
			cefix::ParticleEmitterT<TextParticleNode, cefix::ParticleRandomRateCounter<TextParticleNode>, cefix::ParticlePointPlacer<TextParticleNode>, cefix::ParticleRadialShooter<TextParticleNode> >(
				new cefix::ParticleRandomRateCounter<TextParticleNode>(cefix::floatRange(1,30)),
				new cefix::ParticlePointPlacer<TextParticleNode>(),
				new cefix::ParticleRadialShooter<TextParticleNode>()),
			_root(group)
		{
			getShooter()->setInitialRotationSpeedRange(cefix::vec3Range(osg::Vec3(-1,-10,0), osg::Vec3(1,10,0)));
			setLifeTime(lifetime);
		}
		
		virtual void applySettings(TextParticleNode* p, bool recycled) {
			if (!recycled)	
				_root->addChild(p->getNode());
						
		}
		
		
	
	private:
		osg::ref_ptr<osg::Group>	_root;
};

			
		

class AnimateVectorFieldParticleOperator : public cefix::ParticleOperator<cefix::LineParticleNode> {

	public:
	AnimateVectorFieldParticleOperator(cefix::VectorFieldT<osg::Vec3>* field, cefix::VectorFieldGeometry* geo = NULL) : 
			cefix::ParticleOperator<cefix::LineParticleNode>(), 
			_field(field),
			_geo(geo)
		{
			_perlin = new cefix::Perlin(8,2.5f, osg::PI, 0);
			_t = 0;
		}
		
		inline void begin(const cefix::Particle::time_t& dt) 
		{
			std::cout << (*getGroup()) << std::endl;
			_t += dt;
			if (cefix::randomf(1.0f)  < 0.01f)
				_targetAngle = _perlin->get(_t);
			_angle = cefix::interpolate(_angle, _targetAngle, 0.1f);
			osg::Matrix m(osg::Matrix::rotate(_angle, 0,0,1));
			osg::Vec3Array* v = _field->getData();
			for(unsigned int i = 0; i <v->size(); ++i) {
				(*v)[i] = m * (*v)[i];
			}
			
			if (_geo.valid())
				_geo->update();
		}
		
		
	private:
		osg::ref_ptr<cefix::VectorFieldT<osg::Vec3> >	_field;
		osg::ref_ptr<cefix::Perlin>			_perlin;
		float _t;
		float _angle, _targetAngle;
		osg::ref_ptr<cefix::VectorFieldGeometry>	_geo;

};
		

class MyAppController: public cefix::AppController {

	public:
	
		void createVectorField(osg::Group* world) {
		
			// Line-Grid erzeugen:
			osg::Geode* geode = new osg::Geode();
			//geode->addDrawable(new cefix::LineGridGeometry(osg::Vec3(20,20,20), osg::Vec3(1,1,1), osg::Vec4(0,0.1,0,0.2f),  osg::Vec4(0,0.1,0,0.1f)));
				
			// Vektor-Feld erzeugen:
			cefix::VectorFieldT<osg::Vec3>* field = new cefix::VectorFieldT<osg::Vec3>(20,20,20);
			osg::Vec3 v;
			
			for (unsigned int z = 0; z < field->getDimZ(); ++z) {
				for (unsigned int y = 0; y < field->getDimY(); ++y)  {
					for (unsigned int x = 0; x < field->getDimX(); ++x) {
						// ein beispiel:
						v.set(0.9f * -(y-10) , 0.9f * (x-10), z / 30.0f);
						
						// ein anderes
						//v.set(sin(y * 0.5), cos(x * 0.5) , z/30.0f);
						v.normalize();
						//v = vm * v;
						
						field->put(x,y,z, v);
					}
				}
			}
			
			// debug-Geometrie für das Vektor-Feld ereugen
			cefix::VectorFieldGeometry* vfg = new cefix::VectorFieldGeometry(field, 1, osg::Vec4(0.5,0.5,0.5, 0.3f), osg::Vec4(0.5,0.5, 0.5, 0.1f));
			//geode->addDrawable(vfg);


		
			// Partikel-Prozessor erzeugen
			cefix::ParticleProcessor<cefix::LineParticleNode>* pp = new cefix::ParticleProcessor<cefix::LineParticleNode>();
			
			// Operation-Stack erzeugen
			cefix::ParticleOperationStack<cefix::LineParticleNode>* pos = new cefix::ParticleOperationStack<cefix::LineParticleNode>();
			pp->setOperationStack(pos);
			
			// partikel-operationen:
			
			pos->addOperation(new cefix::ParticleVectorFieldOperator<cefix::LineParticleNode>(field, 15.0f, 0.7f));
			pos->addOperation(new cefix::ParticleRigidDistractorOperator<cefix::LineParticleNode>(4.0f, 0.7f));
			
			//pos->addOperation(new cefix::RandomVelocityOperator<cefix::LineParticleNode>(osg::Vec3(10,10,10), 0.01f, 0.1f));
			
			pos->addOperation(new cefix::ParticleConstrainToSphereOperator<cefix::LineParticleNode>(osg::Vec3(10,10,10 ), 1.0f, true));
			pos->addOperation(new AnimateVectorFieldParticleOperator(field, vfg));
			
			// Emitter
			pp->setEmitter(new LineParticleEmitter(world));
			
			pp->run(0.1);
			
			// Processor als Animation hinzufügen
			cefix::AnimationFactory::instance()->add(new cefix::ParticleProcessorAnimationAdapter<cefix::LineParticleNode >(pp));
						
			// additives blending
			
			//geode->getOrCreateStateSet()->setAttribute(new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
			world->getOrCreateStateSet()->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);
			world->addChild(geode);

		}
		
				
		void createFluidFriction(osg::Group* world) {
			
			osg::Geode* geode = new osg::Geode();
			geode->addDrawable(new cefix::LineGridGeometry(osg::Vec3(20,20,20), osg::Vec3(1,1,1), osg::Vec4(0.8,0.8,0.8,0.2f),  osg::Vec4(0.9,0.9,0.9,0.1f)));
			world->addChild(geode);
			
			// Partikel-Prozessor erzeugen
			cefix::ParticleProcessor<TextParticleNode>* pp = new cefix::ParticleProcessor<TextParticleNode>();
			
			// Operation-Stack erzeugen
			cefix::ParticleOperationStack<TextParticleNode>* pos = new cefix::ParticleOperationStack<TextParticleNode>();
			pp->setOperationStack(pos);
			
			// partikel-operationen:
			cefix::FluidFrictionParticleOperator<TextParticleNode>* fluid = new cefix::FluidFrictionParticleOperator<TextParticleNode>(osg::Vec3(1.0,0,0));
			fluid->setFluidToAir();
			pos->addOperation(fluid);
			
			
			// Emitter
			pp->setEmitter(new FluidFrictionParticleEmitter<TextParticleNode>(world));
			
			// Processor als Animation hinzufügen
			cefix::AnimationFactory::instance()->add(new cefix::ParticleProcessorAnimationAdapter<TextParticleNode>(pp));
						
			// additives blending
			
			//geode->getOrCreateStateSet()->setAttribute(new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
			world->getOrCreateStateSet()->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);


		}
		
		void createTextParticleProcessor(osg::Group* world) {
			
			
			
			// Partikel-Prozessor erzeugen
			cefix::ParticleProcessor<TextParticleNode>* pp = new cefix::ParticleProcessor<TextParticleNode>();
			
			// wir wollen unsere Partikel recyclen, bessere Performance, hat aber ein paar fallstricke
			pp->enableParticleRecycling();
			
			// Operation-Stack erzeugen
			cefix::ParticleOperationStack<TextParticleNode>* pos = new cefix::ParticleOperationStack<TextParticleNode>();
			pp->setOperationStack(pos);
			
			// partikel-operationen:
			
			pos->addOperation(new cefix::RandomVelocityOperator<TextParticleNode>(osg::Vec3(10,10,10), 0.01f, 0.3f));
			pos->addOperation(new cefix::ParticleRigidDistractorOperator<TextParticleNode>(100.0f,0.3f));
			pos->addOperation(new cefix::FluidFrictionParticleOperator<TextParticleNode>());

			
			
			// Emitter
			pp->setEmitter(new TextParticleEmitter(world, 90));
			
			// Processor als Animation hinzufügen
			cefix::AnimationFactory::instance()->add(new cefix::ParticleProcessorAnimationAdapter<TextParticleNode>(pp));
						
			// additives blending
			
			// world->getOrCreateStateSet()->setAttribute(new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
			world->getOrCreateStateSet()->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);


		}
		
		void createFountain(osg::Group* world) {
		
			// Partikel-Prozessor erzeugen
			cefix::ParticleProcessor<NodeParticle>* pp = new cefix::ParticleProcessor<NodeParticle>();
			
			// Operation-Stack erzeugen
			cefix::ParticleOperationStack<NodeParticle>* pos = new cefix::ParticleOperationStack<NodeParticle>();
			pp->setOperationStack(pos);
			
			// Emitter erzeugen
			cefix::ParticleFountainEmitter<NodeParticle>* emitter = new cefix::ParticleFountainEmitter<NodeParticle>(world, osg::Vec3(0,0,3), 30, cefix::floatRange(120.0f, 200.0f), cefix::floatRange(-0.1f, 0.1f), cefix::floatRange(-0.1f,0.1f));
			pp->setEmitter(emitter); // dem emitter als Wurzel mitteilen, bewirkt, dass die Partikel lokal dazu abgeschossen werden
			
			
			// rotierende Ebene erzeugen:
			{
				osg::MatrixTransform* mat = new osg::MatrixTransform();
				mat->setUpdateCallback(new RotatePlaneUpdateCallback(mat));
				world->addChild(mat);
				
				osg::Geode* geode = new osg::Geode();
				geode->addDrawable(new osg::ShapeDrawable(new osg::Box(osg::Vec3(0,0,0), 30,30, 0.1)));
				mat->addChild(geode);
				emitter->trackNode(geode);
			}
			
			
			/// Partikel-Regeln:
			
			
			// Schwerkraft
			cefix::ParticleAccelerationOperator<NodeParticle>* pao = new cefix::ParticleAccelerationOperator<NodeParticle>();
			pao->setToGravity(0.3f);
			pos->addOperation(pao);
			
			// automatisiertes Sterben, wenn keine Bewegung mehr
			pos->addOperation(new cefix::ParticleRemoveWhenNoVelocityOperator<NodeParticle>(0.1f));
			
			// Reflektion an einer Ebene, ist noch buggy
			osg::Vec3 n(osg::Vec3(0,0,1));
			n.normalize();
			pos->addOperation(new cefix::ParticleReflectionOnPlaneOperator<NodeParticle>(osg::Plane(n, osg::Vec3(0,0,1)), 0.3f));
			
			
			// Processor als Animation hinzufügen
			cefix::AnimationFactory::instance()->add(new cefix::ParticleProcessorAnimationAdapter<NodeParticle>(pp));
			
			// Boden:
			osg::Geode* geode = new osg::Geode();
			geode->addDrawable(new osg::ShapeDrawable(new osg::Box(osg::Vec3(0,0,0), 500,500,0.1)));
			
			world->addChild(geode);
		
		}
		
		void createFlockParticleSystem(osg::Group* world) {
			
			cefix::ParticleProcessor<LineParticle>* pp = new cefix::ParticleProcessor<LineParticle>();
			cefix::ParticleOperationStack<LineParticle>* pos = new cefix::ParticleOperationStack<LineParticle>();
			//pos->addOperation(new ParticleVectorFieldOperator(vf));
			
			pp->setOperationStack(pos);
			
			osg::Geode* geode = new osg::Geode();
			LineParticle* attractor = NULL;
			unsigned int maxParticles = 300;
			unsigned int vertCount = 6;
			for(unsigned int i = 0; i < maxParticles; i++) {
				osg::Geometry* geo = new osg::Geometry();
				osg::Vec4Array* colors = new osg::Vec4Array();
				osg::Vec4 c(cefix::HSVtoRGB(i*360.0f/maxParticles, 1,1));
				c[3] = 0.2f;
				
				
				for (unsigned int j = 0; j < 1; ++j){	
					//c[3] = 0.2f + j / (float)vertCount * 0.5f;
					colors->push_back(c);
				}
				
				
				
				LineParticle* lp = new LineParticle(vertCount);
				lp->setPosition(osg::Vec3(cefix::randomf(50),cefix::randomf(50),cefix::randomf(50)));
				lp->setVelocity(osg::Vec3(cefix::randomf(30)-15,cefix::randomf(30)-15,cefix::randomf(30)-15));
				geo->setVertexArray(lp->getVertexArray());
				geo->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP, 0, vertCount));
				
				geo->setColorArray(colors);
				geo->setColorBinding(osg::Geometry::BIND_OVERALL);
				
				osg::Vec3Array* normals = new osg::Vec3Array();
				normals->push_back(osg::Vec3(0,1,0));
				geo->setNormalArray(normals);
				geo->setNormalBinding(osg::Geometry::BIND_OVERALL);
				
				geo->setSupportsDisplayList(false);
				geode->addDrawable(geo);
				geode->setCullingActive(false);
				pp->addParticle(lp);
				
				if (attractor == NULL)
					attractor = lp;
			}
			
			world->addChild(geode);
			geode->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
			geode->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
			geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
			
			// create the processing-stack
			//pos->addOperation(new cefix::ParticleAttractorOperator(attractor, 10.0f, 0.3f));
			
			//pos->addOperation(new ParticleMouseAttractor(0.01f, 1.0f));
			//pos->addOperation(new cefix::ParticleCentroidAttractorOperator(2.0f, 1.0f));
			pos->addOperation(new cefix::ParticleAttractedByNeighborsOperator<LineParticle>(4.0f, 0.1f));
			pos->addOperation(new cefix::ParticleAlignedDirectionOperator<LineParticle>(10.0f, 0.5f));
			pos->addOperation(new cefix::ParticleRigidDistractorOperator<LineParticle>(2.0f, 0.5f));
			pos->addOperation(new cefix::ParticleConstrainToSphereOperator<LineParticle>(osg::Vec3(0,0,0), 50, 0.5f));
			pos->addOperation(new cefix::RandomVelocityOperator<LineParticle>(osg::Vec3(10,10,10), 0.01f, 0.3f));
			pos->addOperation(new cefix::ParticleSpeedConstraintOperator<LineParticle>(10.0f,30.0f));
			pos->addOperation(new cefix::FluidFrictionParticleOperator<LineParticle>());

			//pos->addOperation(new cefix::ParticleSlowDownOperator(0.7f));
			pos->addOperation(new UpdateGeodeOperator(geode));
			
			cefix::AnimationFactory::instance()->add(new cefix::ParticleProcessorAnimationAdapter<LineParticle>(pp));
		}
		
		void createMotionBlur(osg::Vec4 backcolor, osg::Group* world) {
		
			osg::Camera* clearcam = getMainWindow()->createHudCamera(100,100);
			clearcam->setRenderOrder(osg::Camera::NESTED_RENDER);
			osg::Geode* geode = new osg::Geode();
			cefix::Quad2DGeometry* geo = new cefix::Quad2DGeometry(osg::Vec4(0,0,100,100));
			geo->setColor(backcolor);
			osg::StateSet* ss = geode->getOrCreateStateSet();
			ss->setMode(GL_BLEND,osg::StateAttribute::ON);
			ss->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
			ss->setAttribute(new osg::Depth(osg::Depth::LESS, 0.0f, 1.0f, false));
			ss->setBinNumber(-10);
			geode->addDrawable(geo);
			geode->setUpdateCallback(new MotionBlurCallback(geo, backcolor));
			clearcam->addChild(geode);
			world->addChild(clearcam);
		
		}
		
		
		osg::Group * createWorld() {
		
			// getMainWindow()->getCamera()->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);

			osg::Group* g = new osg::Group();
			osg::ClearNode* cn = new osg::ClearNode();
			cn->setClearColor(osg::Vec4(1.0, 1.0, 1.0, 0.0f));
			
			g->addChild(cn);
			if (0) {
				cn->setClearMask(GL_DEPTH_BUFFER_BIT);
				createMotionBlur(osg::Vec4(1.0, 1.0, 1.0, 1.0f), g);
			}
			//createFluidFriction(g);			
			//createFlockParticleSystem(g);
			//createFountain(g);
			//createVectorField(g);
			createTextParticleProcessor(g);
			

			return  g;
		}

};


int main(int argc, char* argv[])
{
    osg::ref_ptr<MyAppController> app = new MyAppController();
	
	// create the world and apply it
	app->setUseOptimizerFlag(false);
    //app->setThreadingModel(osgViewer::ViewerBase::AutomaticSelection);
	app->applyWorld(app->createWorld());
	
    //app->requestPicking();
    //app->requestPostScriptWriter();
    
    //create a viewer-window/fullscreen
    app->realize();
	app->run();

	return 0;
}


