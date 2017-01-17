#pragma once
/*
BFString本质上是一种字符串,但只提供了有限的功能,但速度比std::string要快
*/
#ifndef BFString_H
#define BFString_H

#define _CRT_SECURE_NO_WARNINGS
#include <string>
#include <cstring>
#include "BFException.h"

class BFString
{
private:
	struct BFSDetial
	{
		char *String;
		size_t length;
		int ReferCount;
	}*Detail;
public :
	BFString(const char *String);
	BFString(const BFString& Source);
	BFString(const char * String, size_t length);
	~BFString();
	
	char * c_str();
	friend std::ostream& operator<<(std::ostream &os, BFString &Source);

};
#endif