#include <stdlib.h>
#include <stdio.h>
#include "stringarray.h"

#define ARRAYLENGTH 30

StringArray initArray()
{
	StringArray array;
	array.length = ARRAYLENGTH;
	array.data = (String *)malloc(sizeof(String) * array.length);
	array.ptr = 0;
	return array;
}

void pushBack(StringArray *array, String str)
{
	if (array->ptr >= array->length)
	{
		array->length *= 2;
		array->data = (String *)realloc(array->data, sizeof(String) * array->length);
	}
	array->data[array->ptr++] = str;
}

long getArraySize(StringArray array)
{
	return array.ptr;
}

String getString(StringArray array, long ptr)
{
	if (array.ptr <= ptr)
	{
		fprintf(stderr, "StringArray out of index\n");
		exit(1);
	}
	return array.data[ptr];
}

StringArray split(String str, const char *pat)
{
	StringArray array = initArray();
	char *temp = str.data;
	char *ptr;
	long p = 0;

	while ((ptr = strstr(temp, pat)) != NULL)
	{
		String x = initString();
		while (temp != ptr)
		{
			addChar(&x, *temp);
			temp++;
			p++;
		}
		temp += strlen(pat);
		p++;
		pushBack(&array, x);
	}
	String x = initString();
	while (p < str.ptr)
	{
		addChar(&x, *temp);
		temp++;
		p++;
	}
	pushBack(&array, x);
	return array;
}

void freeArray(StringArray array)
{
	free(array.data);
}
