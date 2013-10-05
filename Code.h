#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

void writeArith(char* cmd); //writes arithmetic command
void writePush(char* seg, int index); //writes push
void writePop(char* seg, int index); //writes pop
void writeNum(int n); //writes label number
void writeInit(); //writes initial code
void writeLabel(char* lab); //writes label
void writeGoto(char* lab); //writes goto
void writeIf(char* lab); //writes if-goto
void writeCall(char* fn, int args); //writes call
void writeReturn(); //writes return
void writeFunction(char* fn, int lcls); //writes function

extern FILE* _vm_ofp;
extern char* _vm_sv;
extern char* _vm_fcn;
extern int _vm_lbln;
extern int _vm_lcn;

