/**
 * RegisterFile.C
 * Authors: Thayer Ackerman, Ashton Gragg
*/

#include <iostream>
#include <iomanip>
#include "RegisterFile.h"
#include "Tools.h"

//regInstance will be initialized to the single RegisterFile
//object that is created
RegisterFile * RegisterFile::regInstance = NULL;

/**
 * RegisterFile constructor
 * initialize the contents of the reg array to 0
 */
RegisterFile::RegisterFile()
{
   //set the contents of the entire reg array to 0
   for (int32_t i = 0; i < REGSIZE; i++) {
      reg[i] = 0;
   }
}

/**
 * getInstance
 * if regInstance is NULL then creates a Register object
 * and sets regInstance to point it; returns regInstance
 *
 * @return regInstance, the pointer to the single RegisterFile
 *         instance
 */
RegisterFile * RegisterFile::getInstance()
{
   // make regInstance from a new RegisterFile object if it is null
   if (regInstance == NULL) {
      RegisterFile * regObj = new RegisterFile();
      regInstance = regObj;
   }
   return regInstance;
}

/**
 * readRegister
 * returns a register value from the reg array.
 * if regNumber is a valid register number then 
 * sets error to false and returns
 * the register value; otherwise sets error to
 * true and returns 0 
 *
 * @param register number
 * @returns reg[regNumber] if regNumber is valid, otherwise 0
 * @returns sets error to false if regNumber is valid, otherwise true
*/
uint64_t RegisterFile::readRegister(int32_t regNumber, bool & error)
{
   // get the register value if regNumber is in the correct range
   if (regNumber >= 0 && regNumber < REGSIZE) {
      error = false;
      return reg[regNumber];
   }
   // return 0 with an error if regNumber is not in the correct range
   else {
      error = true;
      return 0;
   }
   
}

/**
 * writeRegister
 * sets a register to a specified value if the regNumber is
 * within range and sets error to false; otherwise, sets error to true
 *
 * @param value to be stored in reg[regNumber]
 * @param number of register to be modified (index into reg array)
 * @returns sets error to false if regNumber is valid and true otherwise
 */
void RegisterFile::writeRegister(uint64_t value, int32_t regNumber, 
                                 bool & error)
{
   // set the register value if regNumber is in the correct range
   if (regNumber >= 0 && regNumber < REGSIZE) {
      reg[regNumber] = value;
      error = false;
   }
   // set error to true if the regNumber is not in the correct range
   else {
      error = true;
   }
}

/**
 * dump
 * output the contents of the reg array
 */
void RegisterFile::dump()
{
   std::string rnames[15] = {"%rax: ", "%rcx: ", "%rdx: ",  "%rbx: ",
                             "%rsp: ", "%rbp: ", "%rsi: ",  "%rdi: ", 
                             "% r8: ", "% r9: ", "%r10: ",  "%r11: ",
                             "%r12: ", "%r13: ", "%r14: "};  
   for (int32_t i = 0; i < REGSIZE; i+=4)
   {
      for (int32_t j = 0; j < 3; j++)
         std::cout << rnames[i + j] << std::hex << std::setw(16) 
                   << std::setfill('0') << reg[i + j] << ' ';
      if (i + 3 < REGSIZE) 
         std::cout << rnames[i + 3] << std::hex << std::setw(16) 
                   << std::setfill('0') << reg[i + 3] << std::endl;
      else
         std::cout << std::endl;
   }
}
