#pragma once
#include "ofxOsc.h"

namespace flower {
	class Receiver {
		public:
			Receiver();
			
			virtual ~Receiver();
			
			static void setup();
			static void update();
			static float message(const std::string& address, const float& def = 0.0f);
			
		private:
			static ofxOscReceiver  _receiver;
			static std::unordered_map<std::string, float> _messages;
	};
	
} // namespace flower
