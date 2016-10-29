#include "SerialComm.h"

SerialComm::SerialComm(SerialHandlerFunction phandler){
	handler=phandler;
}
void SerialComm::loop(){
	char serialRead[80];
	if (Serial.available()) {
		int l = Serial.readBytesUntil(13,serialRead,sizeof(serialRead));
		serialRead[l]=0;
		if(strncmp(serialRead,"[->[",4)==0){
			handler(serialRead+4);
		}
	}
}

void SerialComm::send(char* str){
	Serial.print("[<-[");
	Serial.println(str);
}
