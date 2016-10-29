/*
 * menu.h
 *
 *  Created on: 24 oct. 2016
 *      Author: o.michaud
 */

#ifndef SERIALCOMM_H
#define SERIALCOMM_H

#include "Arduino.h"

typedef void (*SerialHandlerFunction) (char*);

class SerialComm {
	public:
		SerialComm(SerialHandlerFunction handler);
		void loop();
		void send(char* str);
		void send(String str);
	private:
		int lastState[32];
		SerialHandlerFunction handler;
};

#endif /* SERIALCOMM_H */
