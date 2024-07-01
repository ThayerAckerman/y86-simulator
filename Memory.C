/**
 * Memory.C
 * Authors: Thayer Ackerman, Ashton Gragg
*/

#include <iostream>
#include <iomanip>
#include "Memory.h"
#include "Tools.h"

//memInstance will be initialized to the single instance
//of the Memory class
Memory * Memory::memInstance = NULL;

/** 
 * Memory constructor
 * initializes the mem array to 0
 */
Memory::Memory()
{
   // Set every value in the mem array equal to 0
   for (int32_t i = 0; i < MEMSIZE; i++) {
      mem[i] = 0;
   }
}

/**
 * getInstance
 * if memInstance is NULL then creates a Memory object
 * and sets memInstance to point to it; returns memInstance
 *
 * @return memInstance
 */
Memory * Memory::getInstance()
{
   // make memInstance from a new Memory object if it is null
   if(memInstance == NULL) {
      Memory * memObj = new Memory();
      memInstance = memObj;
   }
   return memInstance;
}

/**
 * getLong
 * returns the 64-bit word at the indicated address; sets imem_error
 * to false if the access is aligned and the address is within range;
 * otherwise sets imem_error to true
 *
 * @param address of 64-bit word; access must be aligned (address % 8 == 0)
 * @return imem_error is set to true or false
 * @return returns 64-bit word at the specified address or 0 if the
 *         access is not aligned or out of range
 */
uint64_t Memory::getLong(int32_t address, bool & imem_error)
{
   // get the long if the address in in range
   // NO LONGER CHECKING FOR ALIGNMENT
   if (address + 7 < MEMSIZE && address >= 0) {
      if (address % 8 == 0) imem_error = false;
      else imem_error = true;
      // build a long from 8 uint8_t
      uint64_t result;
      for (int i = 0; i < LONGSIZE; i++) {
         result = Tools::buildLong(&mem[address]);
      }
      return result;
   }
   // return 0 with an error if the address in not in range or aligned
   else {
      imem_error = true;
      return 0;
   }
}

/**
 * getByte
 * returns the byte (8-bits) at the indicated address if address
 * is within range and sets imem_error; otherwise sets imem_error to
 * true and returns 0
 *
 * @param address of byte
 * @return imem_error is set to true or false
 * @return byte at specified address or 0 if the address is out of range
 */
uint8_t Memory::getByte(int32_t address, bool & imem_error)
{
   // get the byte if the address is in range
   if (address < MEMSIZE && address >= 0) {
      imem_error = false;
      return mem[address];
   }
   // return 0 with an error if the address in not in range
   else {
      imem_error = true;
      return 0;
   }
}

/**
 * putLong
 * sets the 64-bit word in memory at the indicated address to the
 * value that is provided if the address is aligned and within range
 * and sets imem_error to false; otherwise sets 
 * imem_error to true
 *
 * @param 64-bit value to be stored in memory (mem array)
 * @param address of 64-bit word; access must be aligned (address % 8 == 0)
 * @return imem_error is set to true or false
 */
void Memory::putLong(uint64_t value, int32_t address, bool & imem_error)
{
   // set the long if the address in in range and aligned
   if (address % 8 == 0 && address + 7 < MEMSIZE && address >= 0) {
      imem_error = false;
      // split the long into 8 uint8_t in the correct places in memory
      for (int i = 0; i < LONGSIZE; i++) {
         mem[address + i] = Tools::getByte(value, i);
      }
   }
   // return 0 with an error if the address in not in range or aligned
   else {
      imem_error = true;
   }
}

/**
 * putByte
 * sets the byte (8-bits) in memory at the indicated address to the value
 * provided if the address is within range and sets imem_error to false; 
 * otherwise sets imem_error to true
 *
 * @param 8-bit value to be stored in memory (mem array)
 * @param address of byte
 * @return imem_error is set to true or false
 */

void Memory::putByte(uint8_t value, int32_t address, bool & imem_error)
{
   // get the byte if the address is in range
   if (address < MEMSIZE && address >= 0) {
      imem_error = false;
      mem[address] = value;
   }
   // return 0 with an error if the address in not in range
   else {
      imem_error = true;
   }
}

/**
 * dump
 * Output the contents of memory (mem array), four 64-bit words per line.
 * Rather than output memory that contains a lot of 0s, it outputs
 * a * after a line to indicate that the values in memory up to the next
 * line displayed are identical.
 */
void Memory::dump()
{
   uint64_t prevLine[4] = {0, 0, 0, 0};
   uint64_t currLine[4] = {0, 0, 0, 0};
   int32_t i;
   bool star = false;
   bool mem_error;

   //32 bytes per line (four 8-byte words)
   for (i = 0; i < MEMSIZE; i+=32)
   {
      //get the values for the current line
      for (int32_t j = 0; j < 4; j++) currLine[j] = getLong(i+j*8, mem_error);

      //if they are the same as the values in the previous line then
      //don't display them, but always display the first line
      if (i == 0 || currLine[0] != prevLine[0] || currLine[1] != prevLine[1] 
          || currLine[2] != prevLine[2] || currLine[3] != prevLine[3])
      {
         std::cout << std::endl << std::setw(3) << std::setfill('0') 
                   << std::hex << i << ": "; 
         for (int32_t j = 0; j < 4; j++) 
             std::cout << std::setw(16) << std::setfill('0') 
                       << std::hex << currLine[j] << " ";
         star = false;
      } else
      {
         //if this line is exactly like the previous line then
         //just print a * if one hasn't been printed already
         if (star == false) std::cout << "*";
         star = true;
      }
      for (int32_t j = 0; j < 4; j++) prevLine[j] = currLine[j];
   }
   std::cout << std::endl;
}
