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

		void update(const float& lineWidth, const ofColor& start, const ofColor& finish){
			if(_age>0){
				if(int(_age)%2 == 0){
					// _line.setStrokeColor( ofColor( 255.0-220.0*_age/_maxAge ) );
					_line.setStrokeColor( start*(_age/_maxAge) + finish*(1.0f - _age/_maxAge));
					_line.setStrokeWidth( lineWidth*(_age/_maxAge));
					_line.lineTo(_particle.position_[0], _particle.position_[1], _particle.position_[2]);
				}
			
				_age-=1.0;
			}else{
				shouldDie = true;
			}
		}

		void draw(const bool& isDrawingFaces){
			_material->begin();
			ofPushMatrix();
			float scale = _age/_maxAge;
			ofScale(scale, scale, scale);
			if(isDrawingFaces){
				flower::Resources::elem("petal.x")->drawFaces();
			}else{
				flower::Resources::elem("petal.x")->drawWireframe();
			}
			ofPopMatrix();
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
