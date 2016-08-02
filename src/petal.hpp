#pragma once

#include "resources.hpp"

namespace flower {
class Petal {
	using Material = std::shared_ptr<ofxLitSphere>;
	public:
		Petal(Material& m):_material(m){};
		virtual ~Petal(){};
		
		void draw(){
			_material->begin();
			flower::Resources::elem("petal.x")->drawFaces();
			ofDrawAxis(100.0f);
			_material->end();
		}
		
		Petal& material(Material& m){
			_material = m;
		};
		
		Material material(){
			return _material;
		};
		
		Petal& orientation(const ofQuaternion& q){
			_orientation = q;
			return *this;
		}
		
		Petal& position(const ofVec3f& p){
			_position = p;
			return *this;
		}
		
		ofVec3f position()const{return _position;};
		ofQuaternion orientation()const{return _orientation;};
		
	private:
		ofVec3f      _position;
		ofQuaternion _orientation;
		Material _material;
};
}
