#include "VM.h"

char* _vm_ifn;
char* _vm_ofn;
char* _vm_sv;
char* _vm_fcn;
FILE* _vm_ofp;
int _vm_lbln=0;
int _vm_lcn=0;

void vmstart(char* fn)
{
	char* sp = strrchr(fn, '.');
	if(sp == NULL)
		sp = fn + strlen(fn);

	_vm_ifn = fn;
	_vm_fcn = NULL;

	_vm_ofn = (char*) calloc(sp-fn+3, sizeof(char));
	_vm_sv = (char*) calloc(sp-fn, sizeof(char));
	strncpy(_vm_ofn, fn, sp-fn);
	strncpy(_vm_sv, fn, sp-fn);
	strcat(_vm_ofn, ".s\0");

	_vm_ofp = fopen(_vm_ofn, "wb");

	if(_vm_lbln == 0)
		_vm_lbln = 1;

	if(_vm_lcn == 0)
		_vm_lcn = 1;
  
	vmtrans();
	fclose(_vm_ofp);
}

int vmtrans()
{
	FILE* ifp;
	int isz;
	char* ibuf;

	if((ifp = fopen(_vm_ifn, "rb")) == NULL)
		return 1;

	fseek(ifp, 0, SEEK_END);
	isz = ftell(ifp);
	rewind(ifp);

	ibuf = (char*) calloc(isz+1, sizeof(char));
	fread(ibuf, 1, isz, ifp);
	fclose(ifp);

	initParse(ibuf);
	printf("%s -> %s\n", _vm_ifn, _vm_ofn);

	while(advance())
	{
		char* tmp = arg1();
		switch(commandType())
		{
			case C_ARITHMETIC:
				writeArith(tmp);
				break;
			case C_PUSH:
				writePush(tmp, arg2());
				break;
			case C_POP:
				writePop(tmp, arg2());
				break;
			case C_LABEL:
				writeLabel(tmp);
				break;
      case C_GOTO:
				writeGoto(tmp);
				break;
			case C_IF:
				writeIf(tmp);
				break;
			case C_FUNCTION:
				writeFunction(tmp, arg2());
				break;
			case C_RETURN:
				writeReturn();
				break;
			case C_CALL:
				writeCall(tmp, arg2());
				break;
			default:
				printf("UNKNOWN\n");
				break;
		}
		if(tmp != NULL)
			free(tmp);
	}
	resetCmd();
	free(ibuf);

	return 0;
}

int vminitout(char* out)
{
	if((_vm_ofp = fopen(out, "wb")) == NULL)
		return 1;

	writeInit();

	return 0;
}

int vmout(char* in)
{
	FILE* ifp;
	int isz;
	char* ibuf;

	if((ifp = fopen(in, "rb")) == NULL)
		return 1;
	
	fseek(ifp, 0, SEEK_END);
	isz = ftell(ifp);
	rewind(ifp);

	ibuf = (char*) calloc(isz+1, sizeof(char));
	fread(ibuf, 1, isz, ifp);
	fclose(ifp);

	fwrite(ibuf, 1, isz, _vm_ofp);

	return 0;
}

int vmasm(int inc, char** in, char* out)
{
	if(vminitout(out) != 0)
		return 1;
	
	for(int i=0; i<inc; i++)
	{
		char* sp = strrchr(in[i], '.');
		if(sp == NULL)
			sp = in[i] + strlen(in[i]);

		_vm_ifn = (char*) calloc(sp-in[i]+3, sizeof(char));
		strncpy(_vm_ifn, in[i], sp-in[i]);
		strcat(_vm_ifn, ".s\0");

		printf("%s -> %s\n", _vm_ifn, out);
		if(vmout(_vm_ifn) != 0)
			return 1;

		remove(_vm_ifn);
	}

	fclose(_vm_ofp);
	return 0;
}

