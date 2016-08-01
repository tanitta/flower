#pragma once
#include "ofxAssimpModelLoader.h"
namespace flower {
	
class Resources {
	private:
		static map<string, shared_ptr<ofxAssimpModelLoader>> resources_;
	public:
		Resources(){};
		virtual ~Resources(){};

		static shared_ptr<ofxAssimpModelLoader> elem(string file_name);
};
} // namespace flower

