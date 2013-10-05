#include <stdio.h>
#include "VM.h"

int main(int argc, char** argv)
{
	if(argc < 2)
		return 1;

	for(int i = 1; i < argc; i++)
	{
		vmstart(argv[i]);
	}

	return 0;
}

