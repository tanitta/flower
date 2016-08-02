#pragma once

#include "ofMain.h"
#include "resources.hpp"
#include "ofxAssimpModelLoader.h"
#include "ofxLitSphere.h"
#include "ofxPostGlitch.h"

#include "flower.hpp"

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
		void setupPetals();
		
		ofEasyCam     _camera;
		
		Material _baseMaterial;
		Material _asortMaterial;
		Material _accentMaterial;
		
		ofxPostGlitch _glitch;
		ofFbo         _fbo;
		float _angle;
		
		std::vector<std::shared_ptr<flower::Petal>> _petals;
};

//--------------------------------------------------------------
void ofApp::setup(){
	
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
	_fbo.allocate(512, 512);
	_glitch.setup(&_fbo);
	
	_glitch.setFx(OFXPOSTGLITCH_GLOW           , true);
	_glitch.setFx(OFXPOSTGLITCH_SHAKER         , true);
	_glitch.setFx(OFXPOSTGLITCH_CR_HIGHCONTRAST, true);
	
	_angle = 0.0f;
	
	setupPetals();
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

//--------------------------------------------------------------
void ofApp::update(){
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

	_angle += 0.1f;
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
	_fbo.allocate(w, h);
	_glitch.setup(&_fbo);
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
