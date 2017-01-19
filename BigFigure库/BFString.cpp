#define _DLL_API_ 
#include "BFString.h"

BFString::BFString(const char * String)
{
	try
	{
		Detail = new BFSDetial;
		Detail->length = strlen(String);
		Detail->ReferCount = 1;
		try
		{
			Detail->String = new char[Detail->length + 1];
			strcpy(Detail->String, String);
		}
		catch (std::bad_alloc)
		{
			throw BFException(ERR_MEMORYALLOCATEDEXCEPTION, "Îª×Ö·û´®(BFString::String)·ÖÅäÄÚ´æÊ±Ê§°Ü", EXCEPTION_DETAIL);
		}
	}
	catch (std::bad_alloc)
	{
		delete Detail;
		throw BFException(ERR_MEMORYALLOCATEDEXCEPTION, "Îª×Ö·û´®(BFString)·ÖÅäÄÚ´æÊ±Ê§°Ü", EXCEPTION_DETAIL);
	}
	catch (BFException &e)
	{
		throw BFException(e);
	}
}

BFString::BFString(const char * String, size_t length)
{
	try
	{
		Detail = new BFSDetial();
		Detail->length = length;
		Detail->ReferCount = 1;
		try
		{
			Detail->String = new char[Detail->length + 1];
			strcpy(Detail->String, String);
		}
		catch (std::bad_alloc)
		{
			throw BFException(ERR_MEMORYALLOCATEDEXCEPTION, "Îª×Ö·û´®(BFString::String)·ÖÅäÄÚ´æÊ±Ê§°Ü", EXCEPTION_DETAIL);
		}
	}
	catch (std::bad_alloc)
	{
		delete Detail;
		throw BFException(ERR_MEMORYALLOCATEDEXCEPTION, "Îª×Ö·û´®(BFString)·ÖÅäÄÚ´æÊ±Ê§°Ü", EXCEPTION_DETAIL);
	}
	catch (BFException &e)
	{
		throw BFException(e);
	}
}

BFString::BFString(const BFString & Source)
{
	this->Detail = Source.Detail;
	this->Detail->ReferCount++;
}

BFString::~BFString()
{
	this->Detail->ReferCount--;
	if (!this->Detail->ReferCount)
	{
		delete[] Detail->String;
		delete Detail;
	}
}

char * BFString::c_str()
{
	return Detail->String;
}

std::ostream& operator<<(std::ostream &os, BFString &Source)
{
	os << Source.Detail->String;
	return os;
}
