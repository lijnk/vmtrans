#include <stdio.h>
#include "VM.h"

int main(int argc, char** argv)
{
	if(argc < 3)
	{
		printf("Usage: vmtrans <output> <input, ...>\n");
		return 1;
	}

	for(int i = 2; i < argc; i++)
	{
		vmstart(argv[i]);
	}

	vmasm(argc-2, argv+2, argv[1]);

	return 0;
}

