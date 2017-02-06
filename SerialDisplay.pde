#include <Wire.h>

#include "UIMenu.h"
#include "PushButtons.h"
#include "SerialComm.h"

SerialComm serialComm(treatCommand);
UIMenu uimenu(0x3F,16,2,&serialComm);
PushButtons pushButtons;

double millisDelta=0;

void setup(){
	Serial.begin(115200);
	pinMode(0, OUTPUT);
	pinMode(2, OUTPUT);

	uimenu.backlightOn();
	uimenu.addItem((char *)"01 Jaime"				,(char *)"01234567");
	uimenu.addItem((char *)"02 Velvet Frida Vowell"	,(char *)"01234567");
	uimenu.addItem((char *)"03 Frida Forne"			,(char *)"01234567");
	uimenu.addItem((char *)"04 Marlyn Muldowney 12"	,(char *)"01234567");
	uimenu.addItem((char *)"05 America Ausmus"		,(char *)"01234567");
	uimenu.addItem((char *)"06 Alissa Aberle"		,(char *)"01234567");
	uimenu.addItem((char *)"07 Clarissa Coltuithar"	,(char *)"01234567");
	uimenu.setHome(0,(char *)"-- L'ORATOIRE --");
	uimenu.setHome(1,(char *)"   873, Rapine  ");

	uimenu.print(1,uimenu.items[0].label);

	pushButtons.setButtonHandler(14,keyUp);
	pushButtons.setButtonHandler(13,keyOk);
	pushButtons.setButtonHandler(12,keyDown);

	uimenu.display();

	//Serial.println("event:started");
}

void loop(){
	uimenu.loop();
	pushButtons.loop();
	serialComm.loop();
}

void keyUp(){
	uimenu.keyUp();
}
void keyOk(){
	uimenu.keyOk();
}
void keyDown(){
	uimenu.keyDown();
}

void treatCommand(char* command){
	Serial.println(command);
	String cmd = getValueAt(command,';',0);
	//////////////////////////////////////////////////
	if(cmd.equals("help")){
		Serial.println("print;x;yyyyyy");
		Serial.println("backlight;1");
		Serial.println("clearItems");
		Serial.println("addItem;aaa;xxxx");
		Serial.println("setDisplayMode;1");
		Serial.println("cameraLight;0-256");
		Serial.println("clear");

	//////////////////////////////////////////////////
	}else if(cmd.equals("print")){
		uimenu.lcd.setCursor(0,getValueAt(command,';',1).toInt());
		uimenu.lcd.print(getValueAt(command,';',2));

	//////////////////////////////////////////////////
	}else if(cmd.equals("backlight")){
		if(getValueAt(command,';',1).toInt()){
			uimenu.lcd.backlight();
		}else{
			uimenu.lcd.noBacklight();
		}
	//////////////////////////////////////////////////
	}else if(cmd.equals("clearItems")){
		uimenu.clearItems();

	//////////////////////////////////////////////////
	}else if(cmd.equals("setDisplayMode")){
		uimenu.setDisplayMode(getValueAt(command,';',1).toInt());

	//////////////////////////////////////////////////
	}else if(cmd.equals("cameraLight")){
		uimenu.cameraLight(getValueAt(command,';',1).toInt());

	//////////////////////////////////////////////////
	}else if(cmd.equals("clear")){
		uimenu.print(0,(char*)"                ");
		uimenu.print(1,(char*)"                ");

	//////////////////////////////////////////////////
	}else if(cmd.equals("addItem")){
		char label[32];
		char value[20];
		getValueAt(command,';',1).toCharArray(label,32);
		getValueAt(command,';',2).toCharArray(value,20);
		uimenu.addItem(label,value);

	//////////////////////////////////////////////////
	}else if(cmd.equals("setClock")){
		millisDelta=getValueAt(command,';',1).toInt();

	}
}

String getValueAt(String data, char separator, int index){
	int found = 0;
	int strIndex[] = {0, -1};
	int maxIndex = data.length()-1;

	for(int i=0; i<=maxIndex && found<=index; i++){
		if(data.charAt(i)==separator || i==maxIndex){
			found++;
			strIndex[0] = strIndex[1]+1;
			strIndex[1] = (i == maxIndex) ? i+1 : i;
		}
	}

	return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}
