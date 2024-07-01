#define REGSIZE 15  //size of register file
//Register numbers of Y86 registers
//When a special register, such as RSP, is
//accessed in the code, RSP instead of 4
//is used in the call.
#define RAX 0
#define RCX 1
#define RDX 2
#define RBX 3
#define RSP 4
#define RBP 5
#define RSI 6
#define RDI 7
#define R8  8
#define R9  9
#define R10 0xa
#define R11 0xb
#define R12 0xc
#define R13 0xd
#define R14 0xe
#define RNONE 0xf

class RegisterFile 
{
   private:
      static RegisterFile * regInstance;
      RegisterFile();
      uint64_t reg[REGSIZE];
   public:
      static RegisterFile * getInstance();      
      uint64_t readRegister(int32_t regNumber, bool & error);
      void writeRegister(uint64_t value, int32_t regNumber, 
                        bool & error);
      void dump();
}; 
