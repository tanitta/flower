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
};

//--------------------------------------------------------------
void ofApp::setup(){
	_engine.set_collision_reaction_force([&](pharticle::Particle& p1, pharticle::Particle& p2){
		// //separation
		// Eigen::Vector3d separationForce = Eigen::Vector3d(0.0, 0.0, 0.0);
		// auto distance = (p2.position_ - p1.position_).norm();
		// if(distance < 200.0){
		// 	auto n = (p2.position_ - p1.position_).normalized();
		// 	auto depth = 200.0 - distance;
		// 	separationForce = depth*n*0.05;
		// };
		//
		// //alignment
		// Eigen::Vector3d alignmentForce = Eigen::Vector3d(0.0, 0.0, 0.0);
		// alignmentForce = (p2.velocity_ - p1.velocity_)*0.02;
		// std::cout<<separationForce+alignmentForce<<std::endl;
		// // if(distance < 100.0){
		// // 	// Eigen::Vector3d alignmentForce = -(p2.velocity_ - p1.velocity_)*0.01;
		// // }
		//
		// Eigen::Vector3d force = separationForce + alignmentForce;
		// return force;
		return Eigen::Vector3d(0, 0, 0);
	});
	
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
	_baseMaterial->loadAt(19);
	_asortMaterial->loadAt(19);
	_accentMaterial->loadAt(19);
	
	ofEnableArbTex();
	_fbo.allocate(512, 512, GL_RGBA, 4);
	_glitch.setup(&_fbo);
	
	_glitch.setFx(OFXPOSTGLITCH_GLOW           , true);
	_glitch.setFx(OFXPOSTGLITCH_SHAKER         , true);
	_glitch.setFx(OFXPOSTGLITCH_CR_HIGHCONTRAST, true);
	
	setupPetals();
	setupEmitters();
}

void ofApp::setupPetals(){
	for (int i = 0; i < 6; i++) {
		auto p = std::shared_ptr<flower::Petal>(new flower::Petal(_baseMaterial));
		const auto x = ofRandom(-200, 200);
		const auto y = ofRandom(-200, 200);
		const auto z = ofRandom(-200, 200);
		p->position(ofVec3f(x, y, z))
	      .orientation(ofQuaternion(ofRandom(-PI, PI)*100.0f, ofVec3f(x, y, z).normalized()));
		_petals.push_back(p);
	}
	
	flower::Flower flower(_baseMaterial, _asortMaterial, _accentMaterial, _petals);
}

void ofApp::setupEmitters(){
	for (int i = 0; i < 3; i++){
		auto emitter = std::shared_ptr<flower::Emitter>(new flower::Emitter(_petals));
		_emitters.push_back(emitter);
	}
}
//--------------------------------------------------------------
void ofApp::update(){
	updatePetals();
	
	_fbo.begin();
	_camera.begin();
	ofBackground(220);
	for (auto&& petal : _petals) {
		ofPushMatrix();
		
		ofTranslate(petal->position());
		
		ofVec3f qAxsis;
        float   angle;
		petal->orientation().getRotate(angle, qAxsis);
		ofRotate(angle, qAxsis.x, qAxsis.y, qAxsis.z);
		
		petal->draw();
		
		ofPopMatrix();
	}
	_camera.end();
	_fbo.end();
}

void ofApp::updatePetals(){
	int id = 0;
	for (auto& petal : _petals) {
		petal->particle().id_ = id;
		_engine.add(petal->particle());
		id++;
	}
	
	_engine.update();
}

void ofApp::updateEmitters(){
	
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
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	ofDisableArbTex();
    // _litSphere.loadNext();
	ofEnableArbTex();
	flower::addFlower(_petals, _baseMaterial, _asortMaterial, _accentMaterial, ofVec3f(ofRandom(-500, 500), ofRandom(-500, 500), ofRandom(-500, 500)), 5);
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
