#pragma once

#include "resources.hpp"
#include "petal.hpp"

namespace flower {
class Flower {
	using Material = std::shared_ptr<ofxLitSphere>;
	public:
		Flower(Material& base, Material& asort, Material& accent, const ofVec3f& position){
			_baseMaterial   = base;
			_asortMaterial  = asort;
			_accentMaterial = accent;
			
			_position = position;
		};
	
		Flower(
			Material& base,
			Material& asort,
			Material& accent,
			std::vector<std::shared_ptr<flower::Petal>>& array
		){
			ofVec3f center = ofVec3f::zero();
			for (auto&& petal : array) {
				center += petal->position();
			}
			center /= array.size();
			
			for (int i = 0; i < array.size(); i++) {
				auto&& petal = array[i];
				petal->position(center)
				      .orientation(
							  ofQuaternion(30.0f, ofVec3f(1, 0, 0))*
							  ofQuaternion(360.0f*float(i)/float(array.size()),ofVec3f(0, 1, 0)));
				// if(i == 0){
				// 	// petal->material(asort);
				// }else if(i == array.size()-1){
				// 	// petal->material(accent);
				// }
				if(i % 2 == 0){
					petal->material(accent);
				};
			}
			
			Flower(base, asort, accent, center);
		};
		
		virtual ~Flower(){};
		
		void draw(){
			
		};
		
		Flower& petals(std::vector<std::shared_ptr<flower::Petal>>& array){
			_petals = array;
			return *this;
		}
		
		std::vector<std::shared_ptr<flower::Petal>> petals(){
			return _petals;
		}
		
		Flower& position(const ofVec3f& position){
			_position = position;
			return *this;
		};
		
		Flower& orientation(const ofQuaternion& q){
			_orientation = q;
			return *this;
		}
		
		ofVec3f position()const{return _position;};
		ofQuaternion orientation()const{return _orientation;};

	private:
		std::vector<std::shared_ptr<flower::Petal>> _petals;
		
		Material _baseMaterial;
		Material _asortMaterial;
		Material _accentMaterial;
		
		ofVec3f _position;
		ofQuaternion _orientation;
};
} // namespace flower
