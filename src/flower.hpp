#pragma once
#include "renderer.hpp";
namespace flower {
class Flower {
	public:
		Flower(){};
		
		virtual ~Flower(){};
		
		void draw(){
			flower::Resources::elem("petal.x")->drawFaces();
		}
};
} // namespace flower
