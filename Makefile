CFLAGS = -Wall -Wextra -pedantic -std=c99 -ggdb
OBJECTS = main.o Parser.o VM.o Code.o

vmtrans : $(OBJECTS)
	gcc -o vmtrans $(OBJECTS) -lm

.PHONY : clean
clean :
	rm -rf vmtrans $(OBJECTS)

