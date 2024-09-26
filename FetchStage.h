//class to perform the combinational logic of
//the Fetch stage
class FetchStage: public Stage
{
   private:
      bool F_stall;
      bool D_stall;
      bool D_bubble;
      void setDInput(D * dreg, uint64_t stat, uint64_t icode, uint64_t ifun, 
                     uint64_t rA, uint64_t rB,
                     uint64_t valC, uint64_t valP);
      uint64_t selectPC(F * freg, M * mreg, W * wreg);
      bool needRegIds(uint64_t ficode);
      bool needValC(uint64_t ficode);
      uint64_t predictPC(uint64_t ficode, uint64_t fvalC, uint64_t fvalP);
      uint64_t PCIncrement(uint64_t f_pc, bool needRegIDs, bool needValC);
      void getRegIds(uint64_t & rA, uint64_t & rB, uint64_t regByte);
      void buildValC(uint64_t & valC, uint64_t f_pc, uint64_t ficode);
      uint64_t buildf_stat(bool mem_error, uint64_t instr_valid, uint64_t f_icode);
      uint64_t buildf_ifun(bool mem_error, uint64_t f_ifun);
      uint64_t buildf_icode(bool mem_error, uint64_t f_icode);
      bool buildF_stall (uint64_t E_icode, uint64_t D_icode, uint64_t M_icode, uint64_t E_dstM, uint64_t d_srcA, uint64_t d_srcB);
      bool buildD_stall (uint64_t E_icode, uint64_t E_dstM, uint64_t d_srcA, uint64_t d_srcB);
      void calculateControlSignals(uint64_t E_icode, uint64_t D_icode, uint64_t M_icode, uint64_t E_dstM, uint64_t d_srcA, uint64_t d_srcB, bool e_Cnd);
      bool buildD_bubble(uint64_t E_icode, uint64_t D_icode, uint64_t M_icode, uint64_t E_dstM, uint64_t d_srcA, uint64_t d_srcB, bool e_Cnd);
      void bubbleD(PipeReg ** pregs);
      void normalD(PipeReg ** pregs);
   public:
      bool doClockLow(PipeReg ** pregs, Stage ** stages);
      void doClockHigh(PipeReg ** pregs);
};
