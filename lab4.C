#include <iostream>
#include <string.h>
#include "Memory.h"
#include "RegisterFile.h"
#include "MemoryTester.h"
#include "RegisterFileTester.h"
#include "ConditionCodes.h"
#include "ConditionCodesTester.h"

void parseArgs(int argc, char * argv[], 
               bool& tstCC, bool& tstRF, bool& tstMem);

/* 
 * main written simply to test the methods
 * in the RegisterFile, ConditionCodes, and Memory
 * class.
 */
int main(int argc, char * argv[])
{
   bool tstCC = false, tstRF = false, 
        tstMem = false, tstAll = false;

   //parse user arguments to figure out what
   //to test
   parseArgs(argc, argv, tstCC, tstRF, tstMem);
   tstAll = !tstCC && !tstRF && !tstMem;

   //create objects of the testing classes
   RegisterFileTester regTest;
   ConditionCodesTester ccTest;
   MemoryTester memTest;

   //perform the tests
   if (tstRF || tstAll) regTest.doRegisterFileTests();
   if (tstCC || tstAll) ccTest.doConditionCodesTests();
   if (tstMem || tstAll) memTest.doMemoryTests();

   //output the final values of register, condition
   //codes and memory
   RegisterFile * rf = RegisterFile::getInstance();
   ConditionCodes * cc = ConditionCodes::getInstance();
   Memory * mem = Memory::getInstance();

   if ((tstRF || tstAll) && rf != NULL) rf->dump();
   if ((tstCC || tstAll) && cc != NULL) cc->dump();
   if ((tstMem || tstAll) && mem != NULL) mem->dump();
}

/*
 * examines the arguments passed to the program looking for
 * "-R", "-C", and "-M".  If argv[i] is "-R" then tstRF is set to
 * true.  If argv[i] is "-C" then tstCC is set to true.
 * If argv[i] is "-M" then tstMem is set to true.
 *
 *  params: argc - count of the number of command line arguments
 *          argv - array of c-strings that are the command
 *                 line arguments
 *  returns: none, but sets the bools passed by reference to
 *           this function
 */
void parseArgs(int argc, char * argv[], bool & tstCC, 
               bool & tstRF, bool & tstMem)
{
   int i;
   for (i = 1; i < argc; i++)
   {
      if (!strcmp(argv[i], "-C")) tstCC = true;
      if (!strcmp(argv[i], "-R")) tstRF = true;
      if (!strcmp(argv[i], "-M")) tstMem = true;
   }

}
