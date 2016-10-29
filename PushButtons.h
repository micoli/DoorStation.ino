/*
 * menu.h
 *
 *  Created on: 24 oct. 2016
 *      Author: o.michaud
 */

#ifndef PUSHBUTTONS_H
#define PUSHBUTTONS_H

#include "Arduino.h"

typedef void (*Function) ();

class PushButtons {
	public:
		void setButtonHandler(int port,Function callback);
		void resetButtonHandler(int port);
		void loop();
	protected:
	private:
		Function handlers[32];
		int isset[32];
		int lastState[32];
};

#endif /* PUSHBUTTONS_H */
