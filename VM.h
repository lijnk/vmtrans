#pragma once

#include <stdio.h>
#include "Parser.h"
#include "Code.h"

void vmstart(char* fn); //starts new vm output
int vmtrans(); //translates vm into current file

int vminitout(char* out); //writes init code into output
int vmout(char* in); //concats asm files

int vmasm(int inc, char** in, char* out); //writes the final asm product

extern char* _vm_ifn; //input file name
extern char* _vm_ofn; //output file name
extern char* _vm_sv; //static var name
extern char* _vm_fcn; //function name
extern FILE* _vm_ofp; //output file buffer
extern int _vm_lbln; //label number
extern int _vm_lcn; //call number

/*
 * proposal:
 * open output file (file.s)
 * step through commands, translate and concat into file
 * close output file and repeat
 *
 * open each .s file and concat them all into a single .asm file (keep track of .s files)
 * remove the temp .s files
*/

