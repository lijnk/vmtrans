#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

void writeArith(char* cmd); //writes arithmetic command
void writePush(char* seg, int index); //writes push
void writePop(char* seg, int index); //writes pop
void writeLabel(); //writes label number
void writeInit(); //writes initial code

extern FILE* _vm_ofp;
extern int _vm_lbln;

