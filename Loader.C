#include <iostream>
#include <fstream>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include "Loader.h"
#include "Memory.h"

#define ADDRBEGIN 2
#define ADDREND 4
#define DATABEGIN 7
#define COMMENT 28

/* 
 * Loader
 * opens up the file named in argv[0] and loads the
 * contents into memory. If the file is able to be loaded,
 * then loaded is set to true.
 */
//This method is complete and does not need to be modified.  
Loader::Loader(int argc, char * argv[])
{
   std::ifstream inf;  //input file stream for reading from file
   int lineNumber = 1;
   lastAddress = -1;
   loaded = false;

   //if no file name given or filename badly formed, return without loading
   if (argc < 2 || badFile(argv[1])) return;  
   inf.open(argv[1]);

   //if file can't be opened, return without loading
   if (!inf.is_open()) return;  
  
   std::string line;
   while (getline(inf, line))
   {
      if (hasErrors(line))
      {
         std::cout << "Error on line " << std::dec << lineNumber 
              << ": " << line << std::endl;
         return;
      }
      if (hasAddress(line) && hasData(line)) loadLine(line);
      lineNumber++;
   }
   loaded = true;
}

/*
 * hasAddress
 * returns true if the line passed in has an address on it.
 * A line that has an address has a '0' in column 0.
 * It is assumed that the address has already been checked to
 * make sure it is properly formed.
 *
 * @param line - a string containing a line of valid input from 
 *               a .yo file
 * @return true, if the line has an address on it
 *         false, otherwise
 */
bool Loader::hasAddress(std::string line)
{
   return line.at(0) == 0x30;
}

/*
 * hasData
 * returns true if the line passed in has data on it.
 * A line that has data does not contain a space
 * at index DATABEGIN.
 * It is assumed that the data has already been checked to
 * make sure it is properly formed.
 *
 * @param line - a string containing a line of valid input from 
 *               a .yo file
 * @return true, if the line has data in it
 *         false, otherwise
 */
bool Loader::hasData(std::string line)
{
   return line.at(DATABEGIN) != 0x20;
}

/*
 * hasComment
 * returns true if line is at least COMMENT in length and 
 * line has a | at index COMMENT.
 * 
 * @param line - a string containing a line from a .yo file
 * @return true, if the line is long enough and has a | in index COMMENT
 *         false, otherwise
 */
bool Loader::hasComment(std::string line)
{
   return line.at(COMMENT) == 0x7C;
}

/*
 * loadLine
 * The line that is passed in contains an address and data.
 * This method loads that data into memory byte by byte
 * using the Memory::getInstance->putByte method.
 *
 * @param line - a string containing a line of valid input from 
 *               a .yo file. The line contains an address and
 *               a variable number of bytes of data (at least one)
 */
void Loader::loadLine(std::string line)
{
   //Hints:
   //Use the convert method to convert the characters
   //that represent the address into a number.
   //Also, use the convert method for each byte of data.
   
   int32_t address = Loader::convert(line, ADDRBEGIN, ADDREND - 1);

   bool imem_error;
   for (int i = DATABEGIN; line.at(i) != 0x20; i += 2) {
      lastAddress = address + (i - DATABEGIN)/2;
      Memory::getInstance()->putByte(Loader::convert(line, i, 2), lastAddress, imem_error);
   }

}

/*
 * convert
 * takes "len" characters from the line starting at character "start"
 * and converts them to a number, assuming they represent hex characters.
 * For example, if len is 2 and line[start] is '1' and 
 * line[start + 1] is 'a' then this function returns 26.
 * This function assumes that the line is long enough to hold the desired
 * characters and that the characters represent hex values.
 *
 * @param line - string of characters
 * @param start - starting index in line
 * @param len - represents the number of characters to retrieve
 */
int32_t Loader::convert(std::string line, int32_t start, int32_t len)
{
   // create the substring from line
   std::string substring = line.substr(start, len);

   // return the decimal value of the hex in the substring
   return (int32_t) strtoul(substring.c_str(), NULL, 16);
}

/*
 * hasErrors
 * Returns true if the line file has errors in it and false
 * otherwise.
 *
 * @param line - a string that contains a line from a .yo file
 * @return true, if the line has errors 
 *         false, otherwise
 */
bool Loader::hasErrors(std::string line)
{
   //checking for errors in a particular order can significantly 
   //simplify your code
   //1) line is at least COMMENT characters long and contains a '|' in 
   //   column COMMENT. If not, return true
   //   Hint: use hasComment
   if (!(line.length() >= COMMENT && hasComment(line))) {
      return true;
   }
   //printf("Passed check 1\n");
   //
   //2) check whether line has an address.  If it doesn't,
   //   return result of isSpaces (line must be all spaces up
   //   to the | character)
   //   Hint: use hasAddress and isSpaces
   if (!hasAddress(line)) {
      if (!isSpaces(line, 0, COMMENT - 1)) {
         return true;
      }
   }
   //printf("Passed check 2\n");
   //
   //3) return true if the address is invalid
   //   Hint: use errorAddress
   if (hasAddress(line)) {
      if (errorAddr(line)) {
         return true;
      }
   } 
   
   //printf("Passed check 3\n");
   //
   //4) check whether the line has data. If it doesn't
   //   return result of isSpaces (line must be all spaces from
   //   after the address up to the | character)
   //   Hint: use hasData and isSpaces
   if (!hasData(line)) {
      if (!isSpaces(line, ADDREND + 2, COMMENT - 1)) {
         return true;
      }
   }
   //printf("Passed check 4\n");
   //
   //5) if you get past 4), line has an address and data. Check to
   //   make sure the data is valid using errorData
   //   Hint: use errorData
   int32_t numDBytes;
   if (errorData(line, numDBytes)) {
      return true;
   }
   //
   //6) if you get past 5), line has a valid address and valid data.
   //   Make sure that the address on this line is > the last address
   //   stored to (lastAddress is a private data member)
   //   Hint: use convert to convert address to a number and compare
   //   to lastAddress
   if (hasAddress(line)) {
      int32_t newAddress = Loader::convert(line, ADDRBEGIN, ADDREND - 1);
      if (newAddress <= lastAddress) {
         return true;
      }


      //
      //7) Make sure that the last address of the data to be stored
      //   by this line doesn't exceed the memory size
      //   Hint: use numDBytes as set by errorData, MEMSIZE in Memory.h,
      //         and addr returned by convert
      if (newAddress + numDBytes > MEMSIZE) {
         return true;
      }
   }
   
   // if control reaches here, no errors found
   return false;
}

/*
 * errorData
 * Called when the line contains data. It returns true if the data
 * in the line is invalid. 
 *
 * Valid data consists of characters in the range
 * '0' .. '9','a' ... 'f', and 'A' .. 'F' (valid hex digits). 
 * The data digits start at index DATABEGIN.
 * The hex digits come in pairs, thus there must be an even number of them.
 * In addition, the characters after the last hex digit up to the
 * '|' character at index COMMENT must be spaces. 
 * If these conditions are met, errorData returns false, else errorData
 * returns true.
 *
 * @param line - input line from the .yo file
 * @return numDBytes is set to the number of data bytes on the line
 */
bool Loader::errorData(std::string line, int32_t & numDBytes)
{
   //Hint: use isxdigit and isSpaces
   int i = 0;
   for(i = DATABEGIN; line.at(i) != 0x20; i++) {
      if(!isxdigit(line.at(i))) {
         return true;
      }
   }
   int dataEnd = i;
   if((dataEnd - DATABEGIN) % 2 != 0) {
      return true;
   }
   if(!isSpaces(line, dataEnd, COMMENT - 1)) {
      return true;
   }
   numDBytes = (dataEnd - DATABEGIN) / 2;
   return false;
}

/*
 * errorAddr
 * This function is called when the line contains an address in order
 * to check whether the address is properly formed.  An address must be of
 * this format: 0xHHH: where HHH are valid hex digits.
 * 
 * @param line - input line from a .yo input file
 * @return true if the address is not properly formed and false otherwise
 */
bool Loader::errorAddr(std::string line)
{
   if (line.at(0) != 0x30 || line.at(1) != 0x78 || line.at(ADDREND + 1) != 0x3a || line.at(ADDREND + 2) != 0x20) {
      return true;
   }
   for (int i = ADDRBEGIN; i <= ADDREND; i++) {
      if (!isxdigit(line.at(i))) {
         return true;
      }
   }
   return false;
}

/* 
 * isSpaces
 * This function checks that characters in the line starting at 
 * index start and ending at index end are all spaces.
 * This can be used to check for errors
 *
 * @param line - string containing a line from a .yo file
 * @param start - starting index
 * @param end - ending index
 * @return true, if the characters in index from start to end are spaces
 *         false, otherwise
 */
bool Loader::isSpaces(std::string line, int32_t start, int32_t end)
{
   for(int i = start; i <= end; i++){
      if(line.at(i) != 0x20) {
         return false;
      }
   }
   return true;
}

/*
 * isLoaded
 * getter for the private loaded data member
 */
bool Loader::isLoaded()
{
   return loaded;  
}

/*
 * badFile
 * returns true if the name of the file passed in is an improperly 
 * formed .yo filename. A properly formed .yo file name is at least
 * four characters in length and ends with a .yo extension.
 *
 * @return true - if the filename is improperly formed
 *         false - otherwise
 */
bool Loader::badFile(std::string filename)
{
   //Hint: use std::string length method and C strcmp (or std::string find
   //      or std::string at or ...)
   int mainLength = 0;

   while(filename.at(mainLength + 1) != 0x2E) {
      mainLength++;
   }

   if(mainLength < 3) {
      return true;
   }
   else {
      return !(filename.at(mainLength + 2) == 0x79 && filename.at(mainLength + 3) == 0x6F);
   }
}
