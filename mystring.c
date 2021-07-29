#include <stdlib.h>
#include <string.h>
#include "mystring.h"
#include "stringarray.h"

#define STRINGLENGTH 30

String initString()
{
	String string;
	string.length = STRINGLENGTH;
	string.ptr = 0;
	string.data = (char *)malloc(sizeof(char) * string.length);
	return string;
}

void addChar(String *str, char ch)
{
	if (str->ptr >= str->length)
	{
		str->length *= 2;
		str->data = (char *)realloc(str->data, str->length);
	}
	str->data[str->ptr++] = ch;
}

void addString(String *dec, char *src)
{
	while (*src)
	{
		addChar(dec, *src);
		src++;
	}
}

int isEqual(String src, char *dec)
{
	for (long i = 0; i < src.ptr; i++)
	{
		if (src.data[i] != dec[i])
		{
			return 0;
		}
	}
	return 1;
}

long getStringSize(String str)
{
	return str.ptr;
}

void freeString(String str)
{
	free(str.data);
}
