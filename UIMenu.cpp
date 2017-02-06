#include "UIMenu.h"
#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))

UIMenu::UIMenu(uint8_t lcd_Addr,uint8_t lcd_cols,uint8_t lcd_rows, SerialComm* pserialComm) : lcd(lcd_Addr, lcd_cols, lcd_rows){
	displayMode=DISPLAY_HOME;
	serialComm = pserialComm;
	lastRefreshMillis = millis();
	lcd.init();
}

void UIMenu::print(uint8_t row,char* text){
	lcd.backlight();
	lcd.setCursor(0,row);
	char line[17];
	sprintf(line,"%-16s",text);
	lcd.print(line);
}

void UIMenu::print(uint8_t row,double text){
	lcd.backlight();
	lcd.setCursor(0,row);
	char line[17];
	sprintf(line,"%-16u",text);
	lcd.print(line);
}

void UIMenu::clearItems(){
	nbItems=0;
}

void UIMenu::addItem(char* label,char* value){
	strcpy(items[nbItems].label,label);
	strcpy(items[nbItems].value,value);
	nbItems++;
}

void UIMenu::backlightOn(){
	serialComm->send((char*)"event:backlightOn");
	lcd.backlight();
}

void UIMenu::backlightOff(){
	serialComm->send((char*)"event:backlightOff");
	lcd.noBacklight();
}

void UIMenu::restartTimer(){
	lcd.backlight();
	backlightTimerId=millis();
}

void UIMenu::cameraLight(unsigned int value){
	analogWrite(0,value);
	analogWrite(2,value);
}
void UIMenu::loop(){
	/*if (displayMode==DISPLAY_MENU && backlightTimerId>0 && (millis()-backlightTimerId)>backlightTimerLength){
		backlightOff();
		backlightTimerId=-1;
	}*/
	/*
	if (lastRefreshMillis2<millis()-30 ){
		f=f+fw*16;
		if(f==64){
			fw=-1;
		}
		if(f==0){
			fw=1;
		}
		lastRefreshMillis2=millis();
		Serial.print(f,DEC);
		Serial.println("");
	}
	*/
	if (displayMode==DISPLAY_MENU && backlightTimerId>0 && (millis()-backlightTimerId)>backlightTimerLength){
		displayMode=DISPLAY_HOME;
		backlightTimerId=-1;
		display();
	}

	unsigned long currentTime = millis();
	if (displayMode == DISPLAY_MENU && (lastRefreshMillis<currentTime-450) ){
		lastRefreshMillis = currentTime;
		char line[17];
		getMenuText(1,line,items[selected].label);
		print(selectedLine,line);
	}
}

void UIMenu::setDisplayMode(unsigned int mode){
	displayMode=mode;
	display();
}

void UIMenu::display(){
	switch (displayMode){
		case DISPLAY_HOME:
			displayHome();
		break;
		case DISPLAY_MENU:
			displayList();
		break;
	}
}

void UIMenu::getMenuText(bool isSelected,char* to,const char* from){
	to[0]=isSelected?'>':' ';
	if (isSelected && strlen(from)>15){
		unsigned int subOffset = strlen(from)-15;
		if (animWay==1){
			if (animOffset < subOffset){
				animOffset++;
			}else{
				animWay=-1;
			}
		}else{
			if (animOffset==0){
				animWay=1;
			}else{
				animOffset--;
			}
		}
		strncpy(to+1, from+animOffset, 15);
	}else{
		sprintf(to+1,"% -16s",from);
	}
}

void UIMenu::setHome(int l,char* str){
	strcpy(home[l],str);
}

void UIMenu::displayHome(){
	for(int i=0;i<2;i++){
		print(i,home[i]);
	}
	serialComm->send("event:home");
}

void UIMenu::displayList(){
	char line[17];
	int currentLine;
	for (int i=0;i<=1;i++){
		currentLine = offset+i;
		if (currentLine==nbItems){
			currentLine = 0;
		}
		if (currentLine==selected){
			selectedLine=i;
		}
		sprintf(line,"% -16s",items[currentLine].label);
		getMenuText(currentLine==selected, line, items[currentLine].label);
		print(i,line);
		Serial.printf("i:%d, Offset : %d, selected : %d, selectedLine :%d, currentLine :%d %s :: %s",
			i,
			offset,
			selected ,
			selectedLine,
			currentLine ,
			items[currentLine].label,
			currentLine==selected?"[::]":"    "
		);
		serialComm->send((char*)"");
	}
	restartTimer();
}

void UIMenu::initAnimation(){
	lastRefreshMillis=millis();
	animOffset=1;
	animWay=-1;
}

void UIMenu::resetList(){
	displayMode=DISPLAY_MENU;
	selected = 0;
	offset = 0;
	initAnimation();
	display();
}

void UIMenu::keyUp(){
	displayMode=DISPLAY_MENU;
	selected--;
	if (selected<0){
		selected=nbItems-1;//+selected;
	}
	offset=selected;
	initAnimation();
	display();
	serialComm->send((char*)"event:keyUp");
}

void UIMenu::keyDown(){
	displayMode=DISPLAY_MENU;
	selected=selected+1;
	if (selected==nbItems){
		selected=0;
		offset=nbItems-1;
	}else{
		offset=selected-1;
	}
	initAnimation();
	display();
	serialComm->send((char*)"event:keyDown");
}

void UIMenu::keyOk(){
	char event[64];
	if (displayMode==DISPLAY_HOME){
		displayMode=DISPLAY_MENU;
		return;
	}
	if (displayMode==DISPLAY_MENU){
		print(0,"  Appeler  ");
		print(1,items[selected].label);
		sprintf(event,"event:keyOk:CallConfirmation:%s:%s",items[selected].label,items[selected].value);
		serialComm->send(event);
		displayMode=DISPLAY_CALL_CONFIRM;
		return;
	}
	if (displayMode==DISPLAY_CALL_CONFIRM){
		print(0,"                ");
		print(1,"                ");
		displayMode=2;
		sprintf(event,"event:keyOk:Call:%s:%s",items[selected].label,items[selected].value);
		serialComm->send(event);
		displayMode=DISPLAY_INCALL;
		return;
	}
}
