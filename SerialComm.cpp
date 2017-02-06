#include "SerialComm.h"

SerialComm::SerialComm(SerialHandlerFunction phandler){
	handler=phandler;
}
void SerialComm::loop(){
	char serialRead[80];
	if (Serial.available()) {
		int l = Serial.readBytesUntil(13,serialRead,sizeof(serialRead));
		serialRead[l]=0;
		if(serialRead[l-1]==13){
			serialRead[l-1]=0;
		}
		Serial.print(l,DEC);
		if(strncmp(serialRead,"[[[",3)==0){
			handler(serialRead+3);
		}
	}
}

void SerialComm::send(char* str){
	Serial.print("]]]");
	Serial.println(str);
}
