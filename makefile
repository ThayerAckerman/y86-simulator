CC = g++
CFLAGS = -g -c -Wall -std=c++11
OBJ = yess.o Tools.o RegisterFile.o Loader.o \
ConditionCodes.o Memory.o PipeRegField.o FetchStage.o  F.o \
DecodeStage.o D.o ExecuteStage.o E.o \
MemoryStage.o M.o WritebackStage.o \
W.o Simulate.o PipeReg.o

.C.o:
	$(CC) $(CFLAGS) $< -o $@

yess: $(OBJ)

PipeReg.o: PipeReg.h

F.o: PipeReg.h PipeRegField.h F.h

D.o: PipeReg.h PipeRegField.h D.h

E.o: PipeReg.h PipeRegField.h E.h

M.o: PipeReg.h PipeRegField.h M.h

W.o: PipeReg.h PipeRegField.h W.h

FetchStage.o: FetchStage.h 

DecodeStage.o: DecodeStage.h

ExecuteStage.o: ExecuteStage.h

MemoryStage.o: MemoryStage.h

WritebackStage.o: WritebackStage.h

PipeRegField.o: PipeRegField.h

yess.o: Simulate.h Memory.h RegisterFile.h ConditionCodes.h Loader.h

Simulate.o: Simulate.h FetchStage.h DecodeStage.h ExecuteStage.h MemoryStage.h \
WritebackStage.h PipeRegField.h F.h D.h E.h M.h W.h

Memory.o: Tools.h Memory.h

Tools.o: Tools.h

RegisterFile.o: RegisterFile.h

ConditionCodes.o: ConditionCodes.h Tools.h

Loader.o: Loader.C Loader.h Memory.h

clean:
	rm $(OBJ)

run:
	make clean
	make yess
	./run.sh

