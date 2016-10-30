#include "UIMenu.h"
#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))

UIMenu::UIMenu(uint8_t lcd_Addr,uint8_t lcd_cols,uint8_t lcd_rows, SerialComm* pserialComm) : lcd(lcd_Addr, lcd_cols, lcd_rows){
	displayMode=DISPLAY_MENU;
	serialComm = pserialComm;
	lastRefreshMillis = millis();
	lcd.init();
}

void UIMenu::print(uint8_t row,char* text){
	lcd.backlight();
	lcd.setCursor(0,row);
	lcd.print(text);
	//char line[16];
	//sprintf(line,"%-15s",text);
}

void UIMenu::print(uint8_t row,double text){
	lcd.backlight();
	lcd.setCursor(0,row);
	char line[16];
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

void UIMenu::loop(){
	if (displayMode==DISPLAY_MENU && backlightTimerId>0 && (millis()-backlightTimerId)>backlightTimerLength){
		backlightOff();
		backlightTimerId=-1;
	}

	unsigned long currentTime = millis();
	if (displayMode == DISPLAY_MENU && (lastRefreshMillis<currentTime-450) ){
		lastRefreshMillis = currentTime;
		char line[17];
		getMenuText(1,line,items[selected].label);
		lcd.setCursor(0,selectedLine);
		lcd.print(line);
	}
}

void UIMenu::display(){
	switch (displayMode){
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
		//Serial.printf("getMenuText %d %d %d\r\n",animOffset,animWay,strlen(from));
	}else{
		CString::rpad(to+1, from, 16);
	}
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
		CString::rpad(line, items[currentLine].label, 16);
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
	if (displayMode==DISPLAY_CALL_CONFIRM){
		lcd.setCursor(0,0);
		lcd.print("     In call    ");
		lcd.setCursor(0,1);
		lcd.print("                ");
		displayMode=2;
		serialComm->send((char*)"event:keyOk:Call:");
		displayMode=DISPLAY_INCALL;
	}else{
		lcd.setCursor(0,0);
		lcd.print(" Appeler? ");
		lcd.setCursor(0,1);
		lcd.print(items[selected].label);
		serialComm->send((char*)"event:keyOk:Confirmation");
		displayMode=DISPLAY_CALL_CONFIRM;
	}
	//Serial.print(items[selected].label);
	//Serial.print(":");
	//serialComm.send(items[selected].value);
}
