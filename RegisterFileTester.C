#include <iostream>
#include <assert.h>
#include "RegisterFile.h"
#include "RegisterFileTester.h"

/** 
 * RegisterFileTester constructor
 * Initializes the RegisterFile instance 
 *
*/
RegisterFileTester::RegisterFileTester()
{
   this->rf = RegisterFile::getInstance();
}

/** 
 * doRegisterFileTests
 * Calls each individual method to perform tests of the RegisterFile 
 * methods
 *
*/
void RegisterFileTester::doRegisterFileTests()
{
   assert(this->rf != NULL);

   doClearRegisterTests();
   doReadWriteRegisterTests();
   doReadWriteErrorTests();

   std::cout << "All RegisterFile tests passed.\n"; 
}

/*
 * doClearRegisterTests
 * Checks to make sure the registers have been initialized to 0
 */
void RegisterFileTester::doClearRegisterTests()
{
   int32_t regNumber;
   bool error;
   uint64_t value;
   for (regNumber = 0; regNumber < REGSIZE; regNumber++)
   {
       value = this->rf->readRegister(regNumber, error);
       //value should be 0 if registers initialized to 0
       assert(value == 0);
       //regNumber is valid thus error should be false
       assert(error == false);
   }
}

/** 
 * doReadWriteRegisterTests
 * Perform tests on RegisterFile::writeRegister and 
 * RegisterFile::readRegister
 *
*/
void RegisterFileTester::doReadWriteRegisterTests()
{
   int32_t regNumber;
   bool error;
   uint64_t svalue = 0x1122334455667788;
   uint64_t lvalue;
   for (regNumber = 0; regNumber < REGSIZE; regNumber++)
   {
       this->rf->writeRegister(svalue, regNumber, error);
       //regNumber is valid thus error should be false
       assert(error == false);
       lvalue = this->rf->readRegister(regNumber, error);
       //lvalue should be the value just stored
       assert(lvalue == svalue);
       //regNumber is valid thus error should be false
       assert(error == false);
   }
}

/** 
 * doReadWriteErrorTests
 * makes sure the RegisterFile::readRegister and 
 * RegisterFile::writeRegister handle out of bounds
 * register numbers by setting error to true
 */
void RegisterFileTester::doReadWriteErrorTests()
{
   bool error;
   uint64_t svalue = 0x1122334455667788;
   uint64_t lvalue;
   int32_t regNumber;
   //test using a range of bad register numbers
   for (regNumber = -31; regNumber < 32; regNumber++)
   {
      if (regNumber < 0 || regNumber >= REGSIZE)
      {
         this->rf->writeRegister(svalue, regNumber, error);
         //bad regNumber so error should be true
         assert(error == true);
         lvalue = this->rf->readRegister(regNumber, error);
         //bad regNumber so readRegister returns 0
         assert(lvalue == 0);
         //bad regNumber so error should be true
         assert(error == true);
      }
   }
}

