#include "Code.h"

void writeArith(char* cmd)
{
	const struct {
		char* addr;
		char* hasm;
	} Dict[] = {
		{"add", "D=D+M\n"},
		{"sub", "D=D-M\n"},
		{"or", "D=D|M\n"},
		{"and", "D=D&M\n"},
		{"not", "M=!M\n"},
		{"neg", "M=-M\n"},
		{"eq", "D;JEQ\n"},
		{"gt", "D;JLT\n"},
		{"lt", "D;JGT\n"}
	};

	int i;
	for(i=0; i<9; i++)
	{
		if(strcmp(Dict[i].addr, cmd) == 0)
			break;
	}

	switch(i)
	{
		case 0:
		case 1:
		case 2:
		case 3:
			fwrite("@SP\nM=M-1\nA=M-1\nD=M\nA=A+1\n", 1, 26, _vm_ofp);
			fwrite(Dict[i].hasm, 1, strlen(Dict[i].hasm), _vm_ofp);
			fwrite("A=A-1\nM=D\n", 1, 10, _vm_ofp);
			break;
		case 4:
		case 5:
			fwrite("@SP\nA=M-1\n", 1, 10, _vm_ofp);
			fwrite(Dict[i].hasm, 1, strlen(Dict[i].hasm), _vm_ofp);
			break;
		case 6:
		case 7:
		case 8:
			fwrite("@SP\nAM=M-1\nD=M\nA=A-1\nM=D-M\n@CMP", 1, 29, _vm_ofp);
			writeNum(_vm_lbln);
			fwrite("\nD=A\n@SP\nA=M\nM=D\nA=A-1\nD=M\n@LCMP\n", 1, 33, _vm_ofp);
			fwrite(Dict[i].hasm, 1, strlen(Dict[i].hasm), _vm_ofp);
			fwrite("@SP\nA=M-1\nM=0\n(CMP", 1, 16, _vm_ofp);
			writeNum(_vm_lbln);
			fwrite(")\n", 1, 2, _vm_ofp);
			_vm_lbln++;
			break;
		default:
			return; //should never happen
	}
}

void writePush(char* seg, int index)
{
	const struct {
		char* addr;
		char* hasm;
	} Dict[] = {
		{"local", "@LCL\n"},
		{"argument", "@ARG\n"},
		{"this", "@THIS\n"},
		{"that", "@THAT\n"},
		{"pointer", "@THIS\n"},
		{"temp", "@5\n"},
		{"static", ""},
		{"constant", ""},
	};

	int i;

	for(i=0; i<8; i++)
	{
		if(strcmp(Dict[i].addr, seg) == 0)
			break;
	}

	switch(i)
	{
		case 0:
		case 1:
		case 2:
		case 3:
			fwrite(Dict[i].hasm, 1, strlen(Dict[i].hasm), _vm_ofp);
			fwrite("D=M\n@", 1, 5, _vm_ofp);
			writeNum(index);
			fwrite("\nA=D+A\nD=M\n", 1, 11, _vm_ofp);
			break;
		case 4:
		case 5:
			fwrite(Dict[i].hasm, 1, strlen(Dict[i].hasm), _vm_ofp);
			fwrite("D=A\n@", 1, 5, _vm_ofp);
			writeNum(index);
			fwrite("\nA=D+A\nD=M\n", 1, 11, _vm_ofp);
			break;
		case 6:
			fwrite("@", 1, 1, _vm_ofp);
			fwrite(_vm_sv, 1, strlen(_vm_sv), _vm_ofp);
			fwrite(".", 1, 1, _vm_ofp);
			writeNum(index);
			fwrite("\nD=M\n", 1, 5, _vm_ofp);
			break;
		case 7:
			fwrite("@", 1, 1, _vm_ofp);
			writeNum(index);
			fwrite("\nD=A\n", 1, 5, _vm_ofp);
			break;
		default:
			printf("Unknown push segment: \"%s\"\n", seg);
			return;
	}
	fwrite("@SP\nM=M+1\nA=M-1\nM=D\n", 1, 20, _vm_ofp);
}

void writePop(char* seg, int index)
{
	const struct {
		char* addr;
		char* hasm;
	} Dict[] = {
		{"local", "@LCL\n"},
		{"argument", "@ARG\n"},
		{"this", "@THIS\n"},
		{"that", "@THAT\n"},
		{"pointer", "@THIS\n"},
		{"temp", "@5\n"},
		{"static", ""}
	};

	int i;

	for(i=0; i<7; i++)
	{
		if(strcmp(Dict[i].addr, seg) == 0)
			break;
	}

	switch(i)
	{
		case 0:
		case 1:
		case 2:
		case 3:
			fwrite(Dict[i].hasm, 1, strlen(Dict[i].hasm), _vm_ofp);
			fwrite("D=M\n@", 1, 5, _vm_ofp);
			writeNum(index);
			fwrite("\nD=D+A\n", 1, 7, _vm_ofp);
			break;
		case 4:
		case 5:
			fwrite(Dict[i].hasm, 1, strlen(Dict[i].hasm), _vm_ofp);
			fwrite("D=A\n@", 1, 5, _vm_ofp);
			writeNum(index);
			fwrite("\nD=D+A\n", 1, 7, _vm_ofp);
			break;
		case 6:
			fwrite("@SP\nAM=M-1\nD=M\n@", 1, 16, _vm_ofp);
			fwrite(_vm_sv, 1, strlen(_vm_sv), _vm_ofp);
			fwrite(".", 1, 1, _vm_ofp);
			writeNum(index);
			fwrite("\nM=D\n", 1, 5, _vm_ofp);
			return;
		default:
			printf("Unknown pop segment: \"%s\"\n", seg);
			return;
	}
	fwrite("@SP\nM=M-1\nA=M+1\nM=D\nA=A-1\nD=M\nA=A+1\nA=M\nM=D\n", 1, 44, _vm_ofp);
}

void writeNum(int n)
{
	int len = n==0?1:(int)log10((double)n) + 1;
	int temp = n;
	char* str = (char*) calloc(len+1, sizeof(char));
	
	for(int i = len-1; i>=0; i--)
	{
		str[i] = (temp % 10) + '0';
		temp /= 10;
	}
	fwrite(str, 1, len, _vm_ofp);
	free(str);
}

void writeInit()
{
	fwrite("@256\nD=A\n@SP\nM=D\n", 1, 17, _vm_ofp); //SP=256
	writeCall("Sys.init", 0); //call Sys.init
	fwrite("(LCMP)\n@SP\nA=M-1\nM=-1\nA=A+1\nA=M\n0;JMP\n", 1, 38, _vm_ofp); //comparison code
}

void writeLabel(char* lab)
{
	fwrite("(", 1, 1, _vm_ofp);
	fwrite(_vm_fcn, 1, strlen(_vm_fcn), _vm_ofp);
	fwrite("$", 1, 1, _vm_ofp);
	fwrite(lab, 1, strlen(lab), _vm_ofp);
	fwrite(")\n", 1, 2, _vm_ofp);
}

void writeGoto(char* lab)
{
	fwrite("@", 1, 1, _vm_ofp);
	fwrite(_vm_fcn, 1, strlen(_vm_fcn), _vm_ofp);
	fwrite("$", 1, 1, _vm_ofp);
	fwrite(lab, 1, strlen(lab), _vm_ofp);
	fwrite("\n0;JMP\n", 1, 7, _vm_ofp);
}

void writeIf(char* lab)
{
	fwrite("@SP\nAM=M-1\nD=M\n@", 1, 16, _vm_ofp);
	fwrite(_vm_fcn, 1, strlen(_vm_fcn), _vm_ofp);
	fwrite("$", 1, 1, _vm_ofp);
	fwrite(lab, 1, strlen(lab), _vm_ofp);
	fwrite("\nD;JNE\n", 1, 7, _vm_ofp);
}

void writeCall(char* fn, int args)
{
	fwrite("@LC", 1, 3, _vm_ofp); //push return address
	writeNum(_vm_lcn);
	fwrite("\nD=A\n@SP\nM=M+1\nA=M-1\nM=D\n", 1, 25, _vm_ofp);

	fwrite("@LCL\nD=M\n@SP\nM=M+1\nA=M-1\nM=D\n", 1, 29, _vm_ofp); //push lcl
	fwrite("@ARG\nD=M\n@SP\nM=M+1\nA=M-1\nM=D\n", 1, 29, _vm_ofp); //push arg
	fwrite("@THIS\nD=M\n@SP\nM=M+1\nA=M-1\nM=D\n", 1, 30, _vm_ofp); //push this
	fwrite("@THAT\nD=M\n@SP\nM=M+1\nA=M-1\nM=D\n", 1, 30, _vm_ofp); //push that

	fwrite("@SP\nD=M\n@", 1, 9, _vm_ofp); //arg = sp - args - 5
	writeNum(args);
	fwrite("\nD=D-A\n@5\nD=D-A\n@ARG\nM=D\n", 1, 25, _vm_ofp);

	fwrite("@SP\nD=M\n@LCL\nM=D\n@", 1, 18, _vm_ofp); //lcl = sp; goto f; set return address
	fwrite(fn, 1, strlen(fn), _vm_ofp);
	fwrite("\n0;JMP\n(LC", 1, 10, _vm_ofp);
	writeNum(_vm_lcn);
	fwrite(")\n", 1, 2, _vm_ofp);
	_vm_lcn++;
}

void writeReturn()
{
	fwrite("@LCL\nD=M\n@R15\nM=D\n", 1, 18, _vm_ofp); //FRAME=LCL
	fwrite("@5\nD=A\n@R15\nA=M-D\nD=M\n@R14\nM=D\n", 1, 31, _vm_ofp); //store return address
	fwrite("@SP\nA=M-1\nD=M\n@ARG\nA=M\nM=D\n", 1, 27, _vm_ofp); //*ARG=pop()
	fwrite("@ARG\nD=M+1\n@SP\nM=D\n", 1, 19, _vm_ofp); //SP=ARG+1
	fwrite("@R15\nAM=M-1\nD=M\n@THAT\nM=D\n", 1, 26, _vm_ofp); //restore THAT
	fwrite("@R15\nAM=M-1\nD=M\n@THIS\nM=D\n", 1, 26, _vm_ofp); //restore THIS
	fwrite("@R15\nAM=M-1\nD=M\n@ARG\nM=D\n", 1, 25, _vm_ofp); //restore ARG
	fwrite("@R15\nAM=M-1\nD=M\n@LCL\nM=D\n", 1, 25, _vm_ofp); //restore LCL
	fwrite("@R14\nA=M\n0;JMP\n", 1, 15, _vm_ofp); //return
}

void writeFunction(char* fn, int lcls)
{
	if(_vm_fcn != NULL)
		free(_vm_fcn);

	_vm_fcn = (char*) calloc(strlen(fn), sizeof(char));
	strncpy(_vm_fcn, fn, strlen(fn));

	fwrite("(", 1, 1, _vm_ofp);
	fwrite(fn, 1, strlen(fn), _vm_ofp);
	fwrite(")\n@SP\nA=M\n", 1, 10, _vm_ofp);

	for(int i = 0; i<lcls; i++)
		fwrite("M=0\nA=A+1\n", 1, 10, _vm_ofp);
	
	fwrite("D=A\n@SP\nM=D\n", 1, 12, _vm_ofp);
}

