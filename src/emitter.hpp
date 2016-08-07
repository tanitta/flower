#pragma once

#include "pharticle/pharticle.hpp"

namespace flower {
class Emitter {
	public:
		Emitter(
			std::vector<std::shared_ptr<flower::Petal>>& petals, 
			Material& base,
			Material& asort,
			Material& accent,
			ofVec3f position, 
			size_t numPetals
		):_petals(petals){
			// flower::Flower flower(base, asort, accent, position, _drawingPetals);
			this->position(position);
			addFlower(
				_drawingPetals,
				base, asort, accent,
				ofVec3f(0, 0, 0),
				numPetals
			);
		};
		
		virtual ~Emitter(){};
		
		pharticle::Particle& particle(){
			return _particle;
		}
		
		void draw(){
			for (auto&& petal : _drawingPetals) {
				ofPushMatrix();
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
		
		
	private:
		std::vector<std::shared_ptr<flower::Petal>>& _petals;
		
		std::vector<std::shared_ptr<flower::Petal>> _drawingPetals;
		pharticle::Particle _particle;
		
		ofQuaternion _orientation;
};
} // namespace flower
