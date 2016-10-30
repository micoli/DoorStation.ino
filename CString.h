/*
 * menu.h
 *
 *  Created on: 24 oct. 2016
 *      Author: o.michaud
 */

#ifndef CSTRING_H
#define CSTRING_H

#include "Arduino.h"

class CString {
	public:
		void static rpad(char * to,const char* from,int len);
};

#endif /* CSTRING_H */
