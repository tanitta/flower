#pragma once

#include "pharticle/pharticle.hpp"

namespace flower {
class Emitter {
	public:
		Emitter(std::vector<std::shared_ptr<flower::Petal>>& petals):_petals(petals){};
		virtual ~Emitter(){};
		
		pharticle::Particle& particle(){
			return _particle;
		}
		
	private:
		std::vector<std::shared_ptr<flower::Petal>>& _petals;
		
		pharticle::Particle _particle;
};
} // namespace flower
