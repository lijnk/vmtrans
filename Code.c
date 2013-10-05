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
		{"gt", "D;JLE\n"},
		{"lt", "D;JGE\n"}
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
			fwrite("@SP\nAM=M-1\nD=M\nA=A-1\nM=D-M\n@L", 1, 29, _vm_ofp);
			writeLabel();
			fwrite("\nD=A\n@SP\nM=D\nA=A-1\nD=M\n@LCMP\n", 1, 29, _vm_ofp);
			fwrite(Dict[i].hasm, 1, strlen(Dict[i].hasm), _vm_ofp);
			fwrite("@SP\nA=M-1\nM=0\n(L", 1, 16, _vm_ofp);
			writeLabel();
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
		{"constant", ""},
	};

	int i;
	int d = index==0?1:(int)log10((double)index);
	char* res;

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
			res = (char*) calloc(17+d, 1);
			fwrite(Dict[i].hasm, 1, strlen(Dict[i].hasm), _vm_ofp);
			sprintf(res, "D=M\n@%d\nA=D+A\nD=M\n", index);
			fwrite(res, 1, strlen(res), _vm_ofp);
			break;
		case 4:
		case 5:
			res = (char*) calloc(17+d, 1);
			fwrite(Dict[i].hasm, 1, strlen(Dict[i].hasm), _vm_ofp);
			sprintf(res, "D=A\n@%d\nA=D+A\nD=M\n", index);
			fwrite(res, 1, strlen(res), _vm_ofp);
			break;
		case 6:
			res = (char*) calloc(3+d, 1);
			sprintf(res, "@%d\nD=A\n", index);
			fwrite(res, 1, strlen(res), _vm_ofp);
			break;
		default:
			printf("Unknown segment: \"%s\"\n", seg);
			return;
	}
	free(res);
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
	};

	int i;
	int d = index==0?1:(int)log10((double)index);
	char* res;

	for(i=0; i<6; i++)
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
			res = (char*) calloc(7+d, 1);
			fwrite(Dict[i].hasm, 1, strlen(Dict[i].hasm), _vm_ofp);
			sprintf(res, "D=M\n@%d\n", index);
			fwrite(res, 1, strlen(res), _vm_ofp);
			break;
		case 4:
		case 5:
			res = (char*) calloc(7+d, 1);
			fwrite(Dict[i].hasm, 1, strlen(Dict[i].hasm), _vm_ofp);
			sprintf(res, "D=A\n@%d\n", index);
			fwrite(res, 1, strlen(res), _vm_ofp);
			break;
		default:
			printf("Unknown segment: \"%s\"\n", seg);
			return;
	}
	free(res);
	fwrite("D=D+A\n@SP\nM=M-1\nA=M+1\nM=D\nA=A-1\nD=M\nA=A+1\nA=M\nM=D\n", 1, 50, _vm_ofp);
}

void writeLabel()
{
	int len = (int)log10((double)_vm_lbln) + 1;
	int temp = _vm_lbln;
	char* str;

	str = (char*) calloc(len+1, 1);
	
	for(int i = len-1; i>=0; i++)
	{
		str[i] = temp % 10;
		temp /= 10;
	}
	fwrite(str, 1, len, _vm_ofp);
}

