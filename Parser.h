#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

enum {C_ARITHMETIC=0, C_PUSH, C_POP, C_LABEL, C_GOTO, C_IF, C_FUNCTION, C_CALL, C_RETURN};

void initParse(char* buf); //initialise parser
bool hasMoreCommands(); //checks if there's more commands in the stream
void resetCmd(); //resets _parser_com to NULL
bool advance(); //advances to next available command
int commandType(); //returns command type
char* arg1(); //returns argument 1 (all but return; arithmetic is arg 0)
int arg2(); //returns argument 2 (push, pop, function and call)
int toNum(char* s); //simple tonum function for strings
void debugString(char* s); //prints string in their ascii numbers

extern char* _parser_stream; //stream
extern char* _parser_com; //current command
extern int _parser_cur; //cursor location
extern int _parser_ln; //line number
extern int _parser_sz; //size

