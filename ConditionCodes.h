//The actual bits used within the codes register are
//arbitrary. Use OF, SF, ZF in your code, not 3, 6, 2
#define OF 3   //bit 3 of codes
#define SF 6   //bit 6 of codes
#define ZF 2   //bit 2 of codes

class ConditionCodes 
{
   private:
      static ConditionCodes * ccInstance;
      ConditionCodes();
      uint64_t codes;
   public:
      static ConditionCodes * getInstance();      
      bool getConditionCode(int32_t ccNum, bool & error);
      void setConditionCode(bool value, int32_t ccNum, 
                            bool & error);
      void dump();
}; 
