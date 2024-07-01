class DecodeStage: public Stage
{
   private:
      uint64_t d_srcA;
      uint64_t d_srcB;
      bool E_bubble;
      void setEInput(E * ereg, uint64_t stat, uint64_t icode, 
                           uint64_t ifun, uint64_t valC, uint64_t valA,
                           uint64_t valB, uint64_t dstE, uint64_t dstM, 
                           uint64_t srcA, uint64_t srcB);
      uint64_t build_d_srcA(uint64_t D_icode, uint64_t D_rA);
      uint64_t build_d_srcB(uint64_t D_icode, uint64_t D_rB);
      uint64_t d_dstE(uint64_t D_icode, uint64_t D_rB);
      uint64_t d_dstM(uint64_t D_icode, uint64_t D_rA);
      uint64_t d_valA(M * mreg, W * wreg, uint64_t eDstE, uint64_t eValE, uint64_t d_srcA, uint64_t D_icode, uint64_t D_valP, uint64_t m_valM);
      uint64_t d_valB(M * mreg, W * wreg, uint64_t eDstE, uint64_t eValE, uint64_t d_srcB, uint64_t m_valM);
      bool buildE_bubble(uint64_t E_icode, uint64_t E_dstM, uint64_t d_srcA, uint64_t d_srcB, uint64_t e_Cnd);
      void calculateControlSignals(uint64_t E_icode, uint64_t E_dstM, uint64_t d_srcA, uint64_t d_srcB, uint64_t e_Cnd);
      void bubbleE(PipeReg ** pregs);
      void normalE(PipeReg ** pregs);
   public:
      bool doClockLow(PipeReg ** pregs, Stage ** stages);
      void doClockHigh(PipeReg ** pregs);
      uint64_t getd_srcA();
      uint64_t getd_srcB();

};