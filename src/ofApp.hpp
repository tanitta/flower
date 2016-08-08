#pragma once

#include "ofMain.h"
#include "pharticle/pharticle.hpp";
#include "resources.hpp"
#include "ofxAssimpModelLoader.h"
#include "ofxLitSphere.h"
#include "ofxPostGlitch.h"

#include "flower.hpp"
#include "emitter.hpp"

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
		
		void setupPetals();
		void setupEmitters();
		
		void updatePetals();
		void updateEmitters();
		void updateParticles();
		
		void drawPetals();
		void drawEmitters();
		
		void addForce(pharticle::Particle& particle, Eigen::Vector3d force);
		void addForce(pharticle::Particle& particle, ofVec3f force);
		double _unitTime = 1.0;
};

//--------------------------------------------------------------
void ofApp::setup(){
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

void ofApp::setupPetals(){
	// for (int i = 0; i < 6; i++) {
	// 	auto p = std::shared_ptr<flower::Petal>(new flower::Petal(_baseMaterial));
	// 	const auto x = ofRandom(-200, 200);
	// 	const auto y = ofRandom(-200, 200);
	// 	const auto z = ofRandom(-200, 200);
	// 	p->position(ofVec3f(x, y, z))
	//       .orientation(ofQuaternion(ofRandom(-PI, PI)*100.0f, ofVec3f(x, y, z).normalized()));
	// 	_petals.push_back(p);
	// }
	//
	// flower::Flower flower(_baseMaterial, _asortMaterial, _accentMaterial, _petals);
}

void ofApp::setupEmitters(){
	for (int i = 0; i < 3; i++){
		auto emitter = std::shared_ptr<flower::Emitter>(new flower::Emitter(
			_petals,
			_baseMaterial,
			_asortMaterial,
			_accentMaterial,
			ofVec3f(i*100, 0, 0), 
			300, 
			5,
			1
		));
		_emitters.push_back(emitter);
	}
}
//--------------------------------------------------------------
void ofApp::update(){
	updatePetals();
	updateEmitters();
	updateParticles();
	
	_fbo.begin();
	_camera.begin();
	// ofBackground(0, 0, 255);
	
	// ofBackground(128, 128, 255);
	ofBackground(255);
	
	drawPetals();
	drawEmitters();
	
	_camera.end();
	_fbo.end();
}

void ofApp::updatePetals(){
	for (auto&& petal: _petals) {
		petal->update();
	}
	
	std::vector<std::shared_ptr<flower::Petal>> petals_tmp;
	for (auto&& petal: _petals) {
		if(!petal->shouldDie){
			petals_tmp.push_back(petal);
		}
	}
	_petals = petals_tmp;
}

void ofApp::updateEmitters(){
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
	
	
}

void ofApp::updateParticles(){
	std::vector<pharticle::Particle*> particles;
	
	for (auto& petal : _petals) {
		particles.push_back(&petal->particle());
	}
	
	for (auto& emitter: _emitters) {
		particles.push_back(&emitter->particle());
	}
	
	for (auto&& p : particles) {
		double distance = p->position_.norm();
		Eigen::Vector3d n = p->position_.normalized();
		Eigen::Vector3d vis = p->velocity_.dot(n) * n;
		Eigen::Vector3d force = (500-distance) * n * 0.1 - vis;
		// addForce(*p, force);

		double gain = 1.5;
		Eigen::Vector3d noiseLocalPositionEigen = p->position_/500.0 * gain;
		ofVec3f noiseLocalPosition(noiseLocalPositionEigen[0], noiseLocalPositionEigen[1], noiseLocalPositionEigen[2]);
		Eigen::Vector3d noiseForce(
			ofNoise(noiseLocalPosition + ofVec3f(0.5, 0.0, 0.0))-0.5, 
			ofNoise(noiseLocalPosition + ofVec3f(0.0, 0.5, 0.0))-0.5, 
			ofNoise(noiseLocalPosition + ofVec3f(0.0, 0.0, 0.5))-0.5
		);
		addForce(*p, noiseForce.normalized()*1.0);

		addForce(*p, -p->velocity_*0.01);
		
	}

	//register
	int id = 0;
	for (auto&& p : particles) {
		p->id_ = id;
		_engine.add(*p);
		id++;
	}
	_engine.update();
	// integrate
	// for (auto&& p : particles) {
	// 	p->integrate();
	// 	p->acceleration_ << 0,0,0;
	// }
}

void ofApp::drawPetals(){
	for (auto&& petal : _petals) {
		ofPushMatrix();
		
		ofTranslate(petal->position());
		
		ofVec3f qAxsis;
        float   angle;
		petal->orientation().getRotate(angle, qAxsis);
		ofRotate(angle, qAxsis.x, qAxsis.y, qAxsis.z);
		
		petal->draw();
		
		ofPopMatrix();
		petal->drawLine();
	}
}

void ofApp::drawEmitters(){
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
		
		emitter->draw();
		
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
	
	ofVec3f randomPoint(ofRandom(-500, 500), ofRandom(-500, 500), ofRandom(-500, 500));
	auto emitter = std::shared_ptr<flower::Emitter>(new flower::Emitter(
		_petals,
		_baseMaterial,
		_asortMaterial,
		_accentMaterial,
		randomPoint, 
		300, 
		5,
		ofRandom(0.4, 4)
	));
	_emitters.push_back(emitter);
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
