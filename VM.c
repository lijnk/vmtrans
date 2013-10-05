#include "VM.h"

char* _vm_ifn;
char* _vm_ofn;
FILE* _vm_ofp;
int _vm_lbln; //label number

void vmstart(char* fn)
{
	char* sp = strrchr(fn, '.');
	if(sp == NULL)
		sp = fn + strlen(fn);

	_vm_ifn = fn;

	_vm_ofn = (char*) calloc(sp-fn+3, sizeof(char));
	strncpy(_vm_ofn, fn, sp-fn);
	strcat(_vm_ofn, ".s\0");

	_vm_ofp = fopen(_vm_ofn, "wb");

	_vm_lbln = 0;
  
	printf("%s\n", _vm_ofn);
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
	printf("%s\n", _vm_ifn);

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
				printf("LABEL\n");
				break;
      case C_GOTO:
				printf("GOTO\n");
				break;
			case C_IF:
				printf("IF\n");
				break;
			case C_FUNCTION:
				printf("FUNCTION\n");
				break;
			case C_RETURN:
				printf("RETURN\n");
				break;
			case C_CALL:
				printf("CALL\n");
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

