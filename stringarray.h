#ifndef STRINGARRAY_H
#define STRINGARRAY_H

#include "mystring.h"

typedef struct
{
	String *data;
	long length;
	long ptr;
} StringArray;

StringArray initArray();
void pushBack(StringArray *array, String str);
long getArraySize(StringArray array);
String getString(StringArray array, long ptr);
StringArray split(String str, const char *pat);
void freeArray(StringArray array);

#endif //STRINGARRAY_H
