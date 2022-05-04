#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/time.h>

#define LOOP() while(1){}
#define debug() _debug(0)

#define APPEND_TO_BUFFER(buffer, s, offset) \
    for (int i = offset; i < strlen(s) + offset; i++) buffer[i] = s[i - offset];

#define FILL_ARRAY(buffer, num, size) memset(buffer, 0, num);

#define ARR_WRITE_STR(buffer, str, size) for (int i = 0; i < size; i++) buffer[i] = str[i];

typedef char* string;

typedef struct InputData {
    char*    path;
    char*    buffer;
    uint64_t size;
} InputData;

void printI(const char*fmt, ...);
void printE(const char*fmt, ...);
void throwE(char*f, int l, int p, const char*fmt, ...);
char*charToPtr(char chr);
int _isdigit(int a);
int _isalpha(int a);
bool _debug(int val);
char*parseStrInt(int64_t num);
char*concat(char*v0, char*v1, char*v2);
void writeAssebly(char*a);
uint64_t timeStamp();

InputData*InputFile(char*path);
char* str_hash(unsigned char *key);
char    *make_ptr(char c);
char    *copy_string(char*c);
char *format_string(char*c);
char *format_db_string(char*var, char*c);
char *str_replace(char *orig, char *rep, char *with);
void remove_comments(InputData*c);
