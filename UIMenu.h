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


typedef struct menuItem{
	char label[32];
	char value[32];
};

const int DISPLAY_HOME=0;
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
		void addItem(char* label,char* value);
		void setHome(int l,char* str);

		void resetList();
		void displayHome();
		void displayList();
		void display();
		void keyUp();
		void keyDown();
		void keyOk();

		void backlightOn();
		void backlightOff();
		void loop();
		void initAnimation();
		void setDisplayMode(unsigned int displayMode);
		void cameraLight(unsigned int value);

		int nbItems=0;
		menuItem items[32];
		char home[2][32];
		double backlightTimerLength = 10000;
		LiquidCrystal_I2C lcd;
	protected:
		int selected = 0;
	private:
		SerialComm* serialComm;
		void restartTimer();
		int backlightTimerId = -1;
		int animWay=1;
		int selectedLine=-1;
		unsigned int displayMode=0;
		unsigned int offset = 0;
		unsigned int animOffset=0;
		unsigned long lastRefreshMillis=0;
		unsigned long lastRefreshMillis2=0;
};

#endif /* UIMENU_H */
