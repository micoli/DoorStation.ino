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
	//char line[16];
	//sprintf(line,"%-16s",text);
	lcd.print(text);
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

	if (displayMode == DISPLAY_MENU && (lastRefreshMillis+500>currentTime) ){
		Serial.printf("ee %d %d\n",currentTime,lastRefreshMillis);
		char line[17];
		lastRefreshMillis = currentTime;
		lcd.setCursor(0,selectedLine);
		getMenuText(1,line,items[selected].label);
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

void rpad(char * to,const char* from,int len){
	int atEnd=0;
	for(int j=0;j<=len-1;j++){
		if (from[j]==0){
			atEnd=1;
		}
		to[j]=atEnd?' ':from[j];
	}
	to[len]=0;
}

void UIMenu::getMenuText(int isSelected,char* to,const char* from){
	to[0]=isSelected?'>':' ';
	if (isSelected){
		if (sizeof(from)>15){
			if (animWay==1){
				if (offset<sizeof(from)-15){
					offset++;
				}else{
					animWay=-1;
					offset--;
				}
			}else{
				if (offset==0){
					animWay=1;
					offset=1;
				}else{
					offset--;
				}
			}
			strncpy(to+1, from+offset, 15);
			to[1] = '|';
		}else{
			rpad(to+1, from, 16);
		}
	}else{
		rpad(to+1, from, 16);
	}
}

void UIMenu::displayList(){
	char line[17];
	int currentLine;
	for(int i=0;i<=1;i++){
		currentLine = offset+i;
		if (currentLine==nbItems){
			currentLine = 0;
		}
		if(currentLine==selected){
			selectedLine=i;
		}
		lcd.setCursor(0,i);
		getMenuText(currentLine==selected,line,items[currentLine].label);
		lcd.print(line);
		Serial.printf("Offset : %d, selected : %d, currentLine :%d %s :: %s",
			offset,
			selected ,
			currentLine ,
			items[currentLine].label,
			currentLine==selected?"[::]":"    "
		);
		serialComm->send((char*)"");
	}
	restartTimer();
}

void UIMenu::initAnim(){
	lastRefreshMillis=millis();
	animOffset=0;
	animWay=1;
}

void UIMenu::resetList(){
	displayMode=DISPLAY_MENU;
	selected = 0;
	initAnim();
	display();
}

void UIMenu::keyUp(){
	displayMode=DISPLAY_MENU;
	selected--;
	if (selected<0){
		selected=nbItems-1;//+selected;
	}
	offset=selected;
	initAnim();
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
	initAnim();
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
