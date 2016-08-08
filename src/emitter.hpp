#pragma once

#include "pharticle/pharticle.hpp"

namespace flower {
class Emitter {
	using Material = std::shared_ptr<ofxLitSphere>;
	
	public:
		Emitter(
			std::vector<std::shared_ptr<flower::Petal>>& petals, 
			Material& base,
			Material& asort,
			Material& accent,
			ofVec3f position, 
			double age, 
			size_t numPetals, 
			float size
		):_petals(petals), _age(age){
			// flower::Flower flower(base, asort, accent, position, _drawingPetals);
			this->position(position);
			_size = size;
			_particle.mass_ = _size*_size*_size;
			_particle.radius_ = 100.0f*_size;
			addFlower(
					_drawingPetals,
				base, asort, accent,
				ofVec3f(0, 0, 0),
				numPetals
			);
			
			_petalMaterial = accent;
		};
		
		virtual ~Emitter(){};
		
		pharticle::Particle& particle(){
			return _particle;
		}
		
		void update(){
			
			if(_age>0){
				if(int(_age)%4 == 0){
					auto p = std::shared_ptr<flower::Petal>(new flower::Petal(_petalMaterial, 100));
					const auto x = ofRandom(-5, 5);
					const auto y = ofRandom(-5, 5);
					const auto z = ofRandom(-5, 5);
					p->position(ofVec3f(x, y, z) + position())
						.orientation(ofQuaternion(ofRandom(-PI, PI)*100.0f, ofVec3f(x, y, z).normalized()));
					_petals.push_back(p);

				}
				_age-=1.0;
			}else{
				shouldDie = true;
			}
		}
		
		void draw(){
			for (auto&& petal : _drawingPetals) {
				ofPushMatrix();
				ofScale(_size, _size, _size);
				ofTranslate(petal->position());
				ofVec3f qAxsis;
				float   angle;
				petal->orientation().getRotate(angle, qAxsis);
				ofRotate(angle, qAxsis.x, qAxsis.y, qAxsis.z);

				petal->draw();

				ofPopMatrix();
			}
		}
		
		Emitter& orientation(const ofQuaternion& q){
			_orientation = q;
			return *this;
		}
		
		ofQuaternion orientation()const{return _orientation;};
		
		Emitter& position(const ofVec3f& p){
			_particle.position_ = Eigen::Vector3d(p.x, p.y, p.z);
			return *this;
		}
		
		ofVec3f position()const{
			return ofVec3f(_particle.position_[0], _particle.position_[1], _particle.position_[2]);
		};
		
		bool shouldDie = false;
		
	private:
		std::vector<std::shared_ptr<flower::Petal>>& _petals;
		
		std::vector<std::shared_ptr<flower::Petal>> _drawingPetals;
		pharticle::Particle _particle;
		
		ofQuaternion _orientation;
		double _size;
		double _age;
		
		Material _petalMaterial;
};
} // namespace flower
