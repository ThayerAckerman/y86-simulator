
//size of memory
#define MEMSIZE 0x1000
class Memory 
{
   private:
      static Memory * memInstance;
      Memory();
      uint8_t mem[MEMSIZE];
   public:
      static Memory * getInstance();      
      uint64_t getLong(int32_t address, bool & error);
      uint8_t getByte(int32_t address, bool & error);
      void putLong(uint64_t value, int32_t address, bool & error);
      void putByte(uint8_t value, int32_t address, bool & error);
      void dump();
}; 
