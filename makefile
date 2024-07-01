CC = g++
CFLAGS = -g -c -Wall -std=c++11 -O0
OBJ = lab4.o MemoryTester.o Memory.o Tools.o RegisterFile.o \
RegisterFileTester.o ConditionCodes.o ConditionCodesTester.o

lab4: $(OBJ)

lab4.o: MemoryTester.h Memory.h RegisterFile.h \
RegisterFileTester.h ConditionCodes.h ConditionCodesTester.h Tools.h

ConditionCodes.o: Tools.h

ConditionCodesTester.o: ConditionCodes.h

Memory.o: Tools.h

MemoryTester.o: Memory.h

RegisterFile.o: Tools.h

RegisterFileTester.o: RegisterFile.h

Tools.o: Tools.h

.C.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm $(OBJ) lab4

run:
	make clean
	make lab4
	./run.sh

