class MemoryStage: public Stage
{
   private:
      uint64_t m_valM;
      uint64_t m_stat;
      void setWInput(W * wreg, uint64_t stat, uint64_t icode,
                     uint64_t valE, uint64_t valM, uint64_t dstE, 
                     uint64_t dstM);
      uint64_t mem_addr(uint64_t M_icode, uint64_t M_valE, uint64_t M_valA);
      uint64_t buildm_stat(bool mem_error, uint64_t M_stat);            
   public:
      bool doClockLow(PipeReg ** pregs, Stage ** stages);
      void doClockHigh(PipeReg ** pregs);
      uint64_t getm_valM(); 
      uint64_t getm_stat();
};