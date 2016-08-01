#include "resources.hpp"
map<string, shared_ptr<ofxAssimpModelLoader>> flower::Resources::resources_ = map<string, shared_ptr<ofxAssimpModelLoader>>();

shared_ptr<ofxAssimpModelLoader> flower::Resources::elem(string file_name){
	if (resources_.find(file_name) == resources_.end()) {
		// 					// Not Find
		resources_[file_name] = shared_ptr<ofxAssimpModelLoader>(new ofxAssimpModelLoader());
		if (!resources_[file_name]->loadModel(file_name)) {
			cout<<"LoadError"<<endl;
			// Load a model for error
			resources_[file_name]->loadModel("NoData/Body.x");
		}
		resources_[file_name]->setScaleNormalization(false);
	}
	return resources_[file_name];
}
