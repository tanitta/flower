#pragma once

#include "ofMain.h"
#include "pharticle/pharticle.hpp";
#include "resources.hpp"
#include "ofxAssimpModelLoader.h"
#include "ofxLitSphere.h"
#include "ofxPostGlitch.h"

#include "flower.hpp"
#include "emitter.hpp"
#include "receiver.hpp"

class ofApp : public ofBaseApp{
	using Material = std::shared_ptr<ofxLitSphere>;
	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
	private:
		ofEasyCam     _camera;
		ofxPostGlitch _glitch;
		ofFbo         _fbo;
		
		Material _baseMaterial;
		Material _asortMaterial;
		Material _accentMaterial;
		
		std::vector<std::shared_ptr<flower::Petal>>   _petals;
		std::vector<std::shared_ptr<flower::Emitter>> _emitters;
		
		Eigen::Vector3d _petalsCenter;

		pharticle::Engine _engine;
		
		void bloomEmitter();
		
		void setupPetals();
		void setupEmitters();
		
		void updateMaterial();
		void updateCamera();
		void updatePetals();
		void updateEmitters();
		void updateParticles();
		void updateEffects();
		
		void drawPetals();
		void drawEmitters();
		
		void addForce(pharticle::Particle& particle, Eigen::Vector3d force);
		void addForce(pharticle::Particle& particle, ofVec3f force);
		double _unitTime = 1.0;
		
		bool _bloomed = false;
};

//--------------------------------------------------------------
void ofApp::setup(){
	flower::Receiver::setup();
	// _engine.set_collision_reaction_force([&](pharticle::Particle& p1, pharticle::Particle& p2){
	// 	//separation
	// 	Eigen::Vector3d separationForce = Eigen::Vector3d(0.0, 0.0, 0.0);
	// 	auto distance = p2.radius_+p1.radius_ - (p2.position_ - p1.position_).norm();
	// 	auto n = (p2.position_ - p1.position_).normalized();
	// 	separationForce = distance*n*0.05;
	// 	// if(distance < 200.0){
	// 	// };
	// 	//
	// 	// //alignment
	// 	// Eigen::Vector3d alignmentForce = Eigen::Vector3d(0.0, 0.0, 0.0);
	// 	// alignmentForce = (p2.velocity_ - p1.velocity_)*0.02;
	// 	// std::cout<<separationForce+alignmentForce<<std::endl;
	// 	// // if(distance < 100.0){
	// 	// // 	// Eigen::Vector3d alignmentForce = -(p2.velocity_ - p1.velocity_)*0.01;
	// 	// // }
	// 	//
	// 	// Eigen::Vector3d force = separationForce + alignmentForce;
	// 	// return force;
	// 	return separationForce;
	// });
	_camera.disableMouseInput();
	
    ofEnableDepthTest();
    ofSetVerticalSync(true);
	
	ofDisableArbTex();
    _baseMaterial = std::shared_ptr<ofxLitSphere>(new ofxLitSphere());
    _asortMaterial = std::shared_ptr<ofxLitSphere>(new ofxLitSphere());
    _accentMaterial= std::shared_ptr<ofxLitSphere>(new ofxLitSphere());
	
    _baseMaterial->setup();
    _asortMaterial->setup();
    _accentMaterial->setup();
	//2 : yellow
	//4 : yellow
	//6 : black
	//8 : blue and yellow 
	//10: green
	//11: light green
	//12: silver and blue
	//13: pink
	//14: pink and blue
	//17: blue
	//19: white
	//20: mat black
	//21: red and white
	_baseMaterial->loadAt(12);
	_asortMaterial->loadAt(12);
	_accentMaterial->loadAt(17);
	
	ofEnableArbTex();
	_fbo.allocate(512, 512, GL_RGBA, 4);
	_glitch.setup(&_fbo);
	
	_glitch.setFx(OFXPOSTGLITCH_GLOW           , true);
	_glitch.setFx(OFXPOSTGLITCH_SHAKER         , true);
	_glitch.setFx(OFXPOSTGLITCH_CR_HIGHCONTRAST, true);
	
	
	// ofSetLineWidth(4);
	
	setupPetals();
	setupEmitters();
}

void ofApp::setupPetals(){}

void ofApp::setupEmitters(){}

//--------------------------------------------------------------
void ofApp::update(){
	flower::Receiver::update();
	
	updateMaterial();
	updateEmitters();
	updatePetals();
	updateParticles();
	updateEffects();
	updateCamera();
	
	_fbo.begin();
	_camera.begin();
	// ofBackground(0, 0, 255);
	
	// ofBackground(128, 128, 255);
	ofBackground(
		flower::Receiver::message("/background/r", 255),
		flower::Receiver::message("/background/g", 255),
		flower::Receiver::message("/background/b", 255)
	);
	
	drawPetals();
	drawEmitters();
	
	_camera.end();
	_fbo.end();
}
void ofApp::updateMaterial(){
	ofDisableArbTex();
	_baseMaterial->loadAt(flower::Receiver::message("/material/base", 2));
	_asortMaterial->loadAt(flower::Receiver::message("/material/asort", 2));
	_accentMaterial->loadAt(flower::Receiver::message("/material/accent", 12));
	ofEnableArbTex();
}

void ofApp::updateCamera(){
	ofVec3f target;
	if(_emitters.size()>0 && flower::Receiver::message("/camera/dynamicmode", 1.0f)>0.5f){
		target = _emitters.back()->position();
	}else{
		target = ofVec3f(
			flower::Receiver::message("/camera/target/x"), 
			flower::Receiver::message("/camera/target/y"), 
			flower::Receiver::message("/camera/target/z")
		);
	}
	_camera.orbit(
		flower::Receiver::message("/camera/orbit/longitude"), 
		flower::Receiver::message("/camera/orbit/latitude"), 
		flower::Receiver::message("/camera/orbit/radius", 1000), 
		target
		);
	_camera.setTarget(target);

}

void ofApp::updatePetals(){
	float lineWidth = flower::Receiver::message("/petal/line/width", 2.0f);
	ofColor lineStartColor(
		flower::Receiver::message("/petal/line/start/r", 0.0f), 
		flower::Receiver::message("/petal/line/start/g", 0.0f), 
		flower::Receiver::message("/petal/line/start/b", 0.0f)
	);
	ofColor lineFinishColor(
		flower::Receiver::message("/petal/line/finish/r", 0.0f), 
		flower::Receiver::message("/petal/line/finish/g", 0.0f), 
		flower::Receiver::message("/petal/line/finish/b", 0.0f)
	);
	for (auto&& petal: _petals) {
		petal->update(lineWidth, lineStartColor, lineFinishColor);
	}

	std::vector<std::shared_ptr<flower::Petal>> petals_tmp;
	for (auto&& petal: _petals) {
		if(!petal->shouldDie){
			petals_tmp.push_back(petal);
		}
	}
	_petals = petals_tmp;
	
	if(flower::Receiver::message("/petal/clear")>0.5f){
		_petals.clear();
	}
}

void ofApp::updateEmitters(){
	if(!_bloomed){
		if(flower::Receiver::message("/emitter/bloom")>0.5f){
			bloomEmitter();
			_bloomed = true;
		}
	}else{
		if(flower::Receiver::message("/emitter/bloom")<0.5f){
			_bloomed = false;
		}
	}
	
	for (auto&& emitter: _emitters) {
		emitter->update();
	}
	
	std::vector<std::shared_ptr<flower::Emitter>> emitters_tmp;
	for (auto&& emitter: _emitters) {
		if(!emitter->shouldDie){
			emitters_tmp.push_back(emitter);
		}
	}
	_emitters = emitters_tmp;
	
	
	if(flower::Receiver::message("/emitter/clear")>0.5f){
		_emitters.clear();
	}
}

void ofApp::updateParticles(){
	std::vector<pharticle::Particle*> particles;
	
	for (auto& petal : _petals) {
		particles.push_back(&petal->particle());
	}
	
	for (auto& emitter: _emitters) {
		particles.push_back(&emitter->particle());
	}
	
	double sphereRadius= flower::Receiver::message("/particle/sphere/radius", 500.0f);
	double sphereGain = flower::Receiver::message("/particle/sphere/gain", 1.0f);
	double noiseFreq= flower::Receiver::message("/particle/noise/freq", 1.5f);
	double noiseGain = flower::Receiver::message("/particle/noise/gain", 1.0f);
	double viscosity = flower::Receiver::message("/particle/viscosity", 0.01f);
	Eigen::Vector3d flow(
		flower::Receiver::message("/particle/flow/x"),
		flower::Receiver::message("/particle/flow/y"),
		flower::Receiver::message("/particle/flow/z")
	);
	for (auto&& p : particles) {
		double distance = p->position_.norm();
		Eigen::Vector3d n = p->position_.normalized();
		Eigen::Vector3d vis = p->velocity_.dot(n) * n;
		Eigen::Vector3d force = (sphereRadius-distance) * n * 0.1 - vis;
		addForce(*p, force*sphereGain);

		Eigen::Vector3d noiseLocalPositionEigen = p->position_/500.0 * noiseFreq;
		ofVec3f noiseLocalPosition(noiseLocalPositionEigen[0], noiseLocalPositionEigen[1], noiseLocalPositionEigen[2]);
		Eigen::Vector3d noiseForce(
			ofNoise(noiseLocalPosition + ofVec3f(0.5, 0.0, 0.0))-0.5, 
			ofNoise(noiseLocalPosition + ofVec3f(0.0, 0.5, 0.0))-0.5, 
			ofNoise(noiseLocalPosition + ofVec3f(0.0, 0.0, 0.5))-0.5
		);
		addForce(*p, noiseForce.normalized()*noiseGain);

		addForce(*p, -p->velocity_ * viscosity);
		
		addForce(*p, flow);
	}

	bool usingPharticle = flower::Receiver::message("/particle/usingpharticle") > 0.5f;
	if(usingPharticle){
		int id = 0;
		for (auto&& p : particles) {
			p->id_ = id;
			_engine.add(*p);
			id++;
		}
		_engine.update();
	}else{
		for (auto&& p : particles) {
			p->integrate();
			p->acceleration_ << 0,0,0;
		}
	}
}

void ofApp::updateEffects(){
	_glitch.setFx(OFXPOSTGLITCH_CONVERGENCE, flower::Receiver::message("/effect/convergence") > 0.5f );
	_glitch.setFx(OFXPOSTGLITCH_GLOW,        flower::Receiver::message("/effect/glow")        > 0.5f );
	_glitch.setFx(OFXPOSTGLITCH_SHAKER,      flower::Receiver::message("/effect/shaker")      > 0.5f );
	_glitch.setFx(OFXPOSTGLITCH_CUTSLIDER,   flower::Receiver::message("/effect/cutslider")   > 0.5f );
	_glitch.setFx(OFXPOSTGLITCH_TWIST,       flower::Receiver::message("/effect/twist")       > 0.5f );
	_glitch.setFx(OFXPOSTGLITCH_OUTLINE,     flower::Receiver::message("/effect/outline")     > 0.5f );
	_glitch.setFx(OFXPOSTGLITCH_NOISE,       flower::Receiver::message("/effect/noise")       > 0.5f );
	_glitch.setFx(OFXPOSTGLITCH_SLITSCAN,    flower::Receiver::message("/effect/slitscan")    > 0.5f );
	_glitch.setFx(OFXPOSTGLITCH_SWELL,       flower::Receiver::message("/effect/swell")       > 0.5f );
	_glitch.setFx(OFXPOSTGLITCH_INVERT,      flower::Receiver::message("/effect/invert")      > 0.5f );

	_glitch.setFx(OFXPOSTGLITCH_CR_HIGHCONTRAST, flower::Receiver::message("/effect/highcontrast") > 0.5f );
	_glitch.setFx(OFXPOSTGLITCH_CR_BLUERAISE,    flower::Receiver::message("/effect/blueraise")    > 0.5f );
	_glitch.setFx(OFXPOSTGLITCH_CR_REDRAISE,     flower::Receiver::message("/effect/redraise")     > 0.5f );
	_glitch.setFx(OFXPOSTGLITCH_CR_GREENRAISE,   flower::Receiver::message("/effect/greenraise")   > 0.5f );
	_glitch.setFx(OFXPOSTGLITCH_CR_BLUEINVERT,   flower::Receiver::message("/effect/blueinvert")   > 0.5f );
	_glitch.setFx(OFXPOSTGLITCH_CR_REDINVERT,    flower::Receiver::message("/effect/redinvert")    > 0.5f );
	_glitch.setFx(OFXPOSTGLITCH_CR_GREENINVERT,  flower::Receiver::message("/effect/greeninvert")  > 0.5f );
}

void ofApp::drawPetals(){
	bool isDrawingFaces = flower::Receiver::message("/petal/isdrawingfaces", 1.0f)>0.5f;
	for (auto&& petal : _petals) {
		ofPushMatrix();
		
		ofTranslate(petal->position());
		
		ofVec3f qAxsis;
        float   angle;
		petal->orientation().getRotate(angle, qAxsis);
		ofRotate(angle, qAxsis.x, qAxsis.y, qAxsis.z);
		
		petal->draw(isDrawingFaces);
		
		ofPopMatrix();
		petal->drawLine();
	}
}

void ofApp::drawEmitters(){
	bool isDrawingFaces = flower::Receiver::message("/emitter/isdrawingfaces", 1.0f)>0.5f;
	ofSetColor(255, 0, 0);
	for (auto&& emitter: _emitters) {
		ofPushMatrix();
		
		ofTranslate(emitter->position());
		
		auto n = emitter->position().normalized();
		ofQuaternion q;
		q.makeRotate(ofVec3f(0, -1, 0), n);
		
		ofVec3f qAxsis;
		float   angle;
		q.getRotate(angle, qAxsis);
		ofRotate(angle, qAxsis.x, qAxsis.y, qAxsis.z);
		
		emitter->draw(isDrawingFaces);
		
		ofPopMatrix();
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	// _fbo.draw(0, 0);
	_glitch.generateFx();
	ofSetColor(255);
	_fbo.draw(0, 0);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (key == '1') _glitch.setFx(OFXPOSTGLITCH_CONVERGENCE	, true);
	if (key == '2') _glitch.setFx(OFXPOSTGLITCH_GLOW			, true);
	if (key == '3') _glitch.setFx(OFXPOSTGLITCH_SHAKER			, true);
	if (key == '4') _glitch.setFx(OFXPOSTGLITCH_CUTSLIDER		, true);
	if (key == '5') _glitch.setFx(OFXPOSTGLITCH_TWIST			, true);
	if (key == '6') _glitch.setFx(OFXPOSTGLITCH_OUTLINE		, true);
	if (key == '7') _glitch.setFx(OFXPOSTGLITCH_NOISE			, true);
	if (key == '8') _glitch.setFx(OFXPOSTGLITCH_SLITSCAN		, true);
	if (key == '9') _glitch.setFx(OFXPOSTGLITCH_SWELL			, true);
	if (key == '0') _glitch.setFx(OFXPOSTGLITCH_INVERT			, true);

	if (key == 'q') _glitch.setFx(OFXPOSTGLITCH_CR_HIGHCONTRAST, true);
	if (key == 'w') _glitch.setFx(OFXPOSTGLITCH_CR_BLUERAISE	, true);
	if (key == 'e') _glitch.setFx(OFXPOSTGLITCH_CR_REDRAISE	, true);
	if (key == 'r') _glitch.setFx(OFXPOSTGLITCH_CR_GREENRAISE	, true);
	if (key == 't') _glitch.setFx(OFXPOSTGLITCH_CR_BLUEINVERT	, true);
	if (key == 'y') _glitch.setFx(OFXPOSTGLITCH_CR_REDINVERT	, true);
	if (key == 'u') _glitch.setFx(OFXPOSTGLITCH_CR_GREENINVERT	, true);
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

	if (key == '1') _glitch.setFx(OFXPOSTGLITCH_CONVERGENCE	, false);
	if (key == '2') _glitch.setFx(OFXPOSTGLITCH_GLOW			, false);
	if (key == '3') _glitch.setFx(OFXPOSTGLITCH_SHAKER			, false);
	if (key == '4') _glitch.setFx(OFXPOSTGLITCH_CUTSLIDER		, false);
	if (key == '5') _glitch.setFx(OFXPOSTGLITCH_TWIST			, false);
	if (key == '6') _glitch.setFx(OFXPOSTGLITCH_OUTLINE		, false);
	if (key == '7') _glitch.setFx(OFXPOSTGLITCH_NOISE			, false);
	if (key == '8') _glitch.setFx(OFXPOSTGLITCH_SLITSCAN		, false);
	if (key == '9') _glitch.setFx(OFXPOSTGLITCH_SWELL			, false);
	if (key == '0') _glitch.setFx(OFXPOSTGLITCH_INVERT			, false);

	if (key == 'q') _glitch.setFx(OFXPOSTGLITCH_CR_HIGHCONTRAST, false);
	if (key == 'w') _glitch.setFx(OFXPOSTGLITCH_CR_BLUERAISE	, false);
	if (key == 'e') _glitch.setFx(OFXPOSTGLITCH_CR_REDRAISE	, false);
	if (key == 'r') _glitch.setFx(OFXPOSTGLITCH_CR_GREENRAISE	, false);
	if (key == 't') _glitch.setFx(OFXPOSTGLITCH_CR_BLUEINVERT	, false);
	if (key == 'y') _glitch.setFx(OFXPOSTGLITCH_CR_REDINVERT	, false);
	if (key == 'u') _glitch.setFx(OFXPOSTGLITCH_CR_GREENINVERT	, false);
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int buttonfloat ){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	ofDisableArbTex();
    // _litSphere.loadNext();
	ofEnableArbTex();
	// flower::addFlower(_petals, _baseMaterial, _asortMaterial, _accentMaterial, ofVec3f(ofRandom(-500, 500), ofRandom(-500, 500), ofRandom(-500, 500)), 5);
	// _emitters.push_back();
	
	bloomEmitter();
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
	ofEnableArbTex();
	_fbo.allocate(w, h, GL_RGBA, 4);
	_glitch.setup(&_fbo);
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

void ofApp::addForce(pharticle::Particle& particle, Eigen::Vector3d force){
	particle.velocity_ += force/particle.mass_*_unitTime;
}

void ofApp::addForce(pharticle::Particle& particle, ofVec3f force){
	addForce(particle, Eigen::Vector3d(force.x, force.y, force.z));
}

void ofApp::bloomEmitter(){
	float size = ofRandom(
		flower::Receiver::message("/emitter/size/min", 0.4),
		flower::Receiver::message("/emitter/size/max", 4.0)
	);
	float bloomingRange = flower::Receiver::message("/emitter/bloomingrange", 200);
	ofVec3f randomPoint(ofRandom(-bloomingRange, bloomingRange), ofRandom(-bloomingRange, bloomingRange), ofRandom(-bloomingRange, bloomingRange));
	auto emitter = std::shared_ptr<flower::Emitter>(new flower::Emitter(
		_petals,
		_baseMaterial,
		_asortMaterial,
		_accentMaterial,
		randomPoint, 
		flower::Receiver::message("/emitter/age", 300), 
		flower::Receiver::message("/emitter/numpetals", 5), 
		size, 
		flower::Receiver::message("/petal/age", 100)
	));
	_emitters.push_back(emitter);
}
// namespace flower {
// namespace boids {
// 	Eigen::Vector3d separation(pharticle::Particle& p1, pharticle::Particle& p2, double distance){
// 		auto n = (p2.position_ - p1.position_).normalized();
// 		auto depth = (p2.position_ - n*p2.radius_) - (p1.position_ - n*p1.radius_);
// 		return depth*0.1;
// 	}
//	
// 	Eigen::Vector3d alignment(pharticle::Particle& p1, pharticle::Particle& p2){
// 		return Eigen::Vector3d(0, 0, 0);
// 	}
//	
// 	Eigen::Vector3d cohesion(pharticle::Particle& p1, pharticle::Particle& p2){
// 		return Eigen::Vector3d(0, 0, 0);
// 	}
// } // namespace boids
// } // namespace flower
