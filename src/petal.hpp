#pragma once

#include "resources.hpp"
#include "pharticle/pharticle.hpp";

namespace flower {
class Petal {
	using Material = std::shared_ptr<ofxLitSphere>;
	
	public:
		Petal(Material& m, double age):_material(m), _age(age), _maxAge(age){
			// _particle.radius_ = 50.0f;
			_particle.mass_ = 2;
			
			_line.setStrokeColor( ofColor( 32, 32, 32 ) );
			_line.setFilled(false);
			_line.setStrokeWidth( 4 );
		};
		virtual ~Petal(){};

		void update(){
			if(_age>0){
				if(int(_age)%2 == 0){
					_line.setStrokeColor( ofColor( 255.0-220.0*_age/_maxAge ) );
					_line.setStrokeWidth( 5.0*_age/_maxAge+1.0 );
					_line.lineTo(_particle.position_[0], _particle.position_[1], _particle.position_[2]);
				}
			
				_age-=1.0;
			}else{
				shouldDie = true;
			}
		}

		void draw(){
			_material->begin();
			flower::Resources::elem("petal.x")->drawFaces();
			// flower::Resources::elem("petal.x")->drawWireframe();
			// ofDrawAxis(100.0f);
			_material->end();
		}
		
		void drawLine(){
			_line.draw();
		};

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
			_particle.position_ = Eigen::Vector3d(p.x, p.y, p.z);
			_line.moveTo(p);
			return *this;
		}
		
		ofVec3f position()const{
			return ofVec3f(_particle.position_[0], _particle.position_[1], _particle.position_[2]);
		};
		
		ofQuaternion orientation()const{return _orientation;};
		
		pharticle::Particle& particle(){
			return _particle;
		}
		
		bool shouldDie = false;
		
	private:
		ofQuaternion _orientation;
		Material _material;
		pharticle::Particle _particle;
		
		double _age;
		double _maxAge;
		
		ofPath _line;
};
}
