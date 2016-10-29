#include "PushButtons.h"

void PushButtons::setButtonHandler(int port, Function callback){
	pinMode(port, INPUT_PULLUP);
	handlers[port]=callback;
	isset[port]=1;
	lastState[port]=1;
}

void PushButtons::resetButtonHandler(int port){
	isset[port]=0;
}

void PushButtons::loop(){
	int found=0;
	for(int port =0;port<32;port++){
		if(isset[port]==1){
			if (digitalRead(port) == LOW) {
				found=1;
				if(lastState[port]==1){
					handlers[port]();
					lastState[port]=0;
				}
			}else{
				lastState[port]=1;
			}
		}
	}
	if(found){
		delay(200);
	}
}
