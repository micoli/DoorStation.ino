/*
 * menu.h
 *
 *  Created on: 24 oct. 2016
 *      Author: o.michaud
 */

#ifndef UIMENU_H
#define UIMENU_H

#include "Arduino.h"
#include <LiquidCrystal_I2C.h>
#include "SerialComm.h"
#include "CString.h"


typedef struct menuItem{
	char label[32];
	char value[32];
};

const int DISPLAY_MENU=1;
const int DISPLAY_CALL_CONFIRM=2;
const int DISPLAY_INCALL=3;

class UIMenu{
	public:
		UIMenu(uint8_t lcd_Addr,uint8_t lcd_cols,uint8_t lcd_rows, SerialComm * pserialComm);
		void print(uint8_t row,char * text);
		void print(uint8_t row,double text);
		void getMenuText(bool isSelected,char* to,const char* from);
		void clearItems();
		void displayList();
		void resetList();
		void display();
		void keyUp();
		void keyDown();
		void keyOk();

		void addItem(char* label,char* value);
		void backlightOn();
		void backlightOff();
		void loop();
		void initAnimation();
		int nbItems=0;
		menuItem items[32];
		double backlightTimerLength = 10000;
		LiquidCrystal_I2C lcd;
	protected:
		int selected = 0;
	private:
		SerialComm* serialComm;
		void restartTimer();
		int backlightTimerId = -1;
		int displayMode=0;
		int animWay=1;
		int selectedLine=-1;
		unsigned int offset = 0;
		unsigned int animOffset=0;
		unsigned long lastRefreshMillis=0;
};

#endif /* UIMENU_H */
