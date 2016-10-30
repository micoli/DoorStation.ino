#include "Cstring.h"

void static CString::rpad(char * to,const char* from,int len){
	int atEnd=0;
	for(int j=0;j<=len-1;j++){
		if (from[j]==0){
			atEnd=1;
		}
		to[j]=atEnd?' ':from[j];
	}
	to[len]=0;
}
