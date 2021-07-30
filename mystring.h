#ifndef MYSTRING_H
#define MYSTRING_H

typedef struct
{
	char *data;
	long length;
	long ptr;
} String;

String initString();
void addChar(String *str, char ch);
void addString(String *dec, char *src, long len);
int isEqual(String src, char *dec);
long getStringSize(String str);
void freeString(String str);
char *toCString(String str);

#endif //MY_STRING
