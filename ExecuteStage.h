class ExecuteStage: public Stage
{
   private:
      uint64_t e_dstE;
      uint64_t e_valE;
      bool M_bubble;
      bool e_Cnd;
      void setMInput(M * mreg, uint64_t stat, uint64_t icode, uint64_t Cnd, 
                     uint64_t valE, uint64_t valA,
                     uint64_t dstE, uint64_t dstM);
      uint64_t aluA(uint64_t E_icode, uint64_t E_valA, uint64_t E_valC);
      uint64_t aluB(uint64_t E_icode, uint64_t E_valB);
      uint64_t alufun(uint64_t E_icode, uint64_t E_ifun);
      bool set_cc(uint64_t E_icode, uint64_t m_stat, uint64_t W_stat);
      uint64_t edstE(uint64_t E_icode, uint64_t e_Cnd, uint64_t E_dstE);
      uint64_t alu(uint64_t E_valA, uint64_t E_valB, uint64_t alufun);
      void cc(uint64_t E_valA, uint64_t E_valB, uint64_t alufun, uint64_t valE);
      uint64_t cond(uint64_t icode, uint64_t ifun);
      bool calculateControlSignals(uint64_t m_stat, uint64_t W_stat);
      void bubble(PipeReg ** pregs);
      void normalM(PipeReg ** pregs);
   public:
      bool doClockLow(PipeReg ** pregs, Stage ** stages);
      void doClockHigh(PipeReg ** pregs);
      uint64_t gete_dstE();
      uint64_t gete_valE();
      bool gete_Cnd();

};