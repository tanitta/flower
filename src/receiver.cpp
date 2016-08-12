#include "receiver.hpp"

namespace flower {
	Receiver::Receiver(){};
	Receiver::~Receiver(){};
	
	void Receiver::setup(){
		_receiver.setup(8080);
	};
	
	void Receiver::update(){
		while( _receiver.hasWaitingMessages() ){
			ofxOscMessage message;
			_receiver.getNextMessage( &message );
			std::string address = message.getAddress();
			_messages[address] = message.getArgAsFloat(0);
		}
	};
	
	float Receiver::message(const std::string& address, const float& def){
		auto itr = _messages.find(address);
		if( itr != _messages.end() ) {
			return _messages[address];
		} else {
			std::cout<<"Warning : "<<address<<" is not found"<<std::endl;
			return def;
		}
	};
	
	ofxOscReceiver Receiver::_receiver;
	
	std::unordered_map<std::string, float> Receiver::_messages;
	
} // namespace flower
