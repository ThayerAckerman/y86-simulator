#include <string>
#include <cstdint>
#include "RegisterFile.h"
#include "PipeRegField.h"
#include "PipeReg.h"
#include "E.h"
#include "D.h"
#include "M.h"
#include "W.h"
#include "Stage.h"
#include "ExecuteStage.h"
#include "Status.h"
#include "Debug.h"
#include "Instructions.h"
#include "ConditionCodes.h"
#include "Tools.h"
#include "MemoryStage.h"


/*
 * doClockLow:
 * Performs the Fetch stage combinational logic that is performed when
 * the clock edge is low.
 *
 * @param: pregs - array of the pipeline register sets (F, D, E, M, W instances)
 * @param: stages - array of stages (FetchStage, DecodeStage, ExecuteStage,
 *         MemoryStage, WritebackStage instances)
 */
bool ExecuteStage::doClockLow(PipeReg ** pregs, Stage ** stages)
{
   E * ereg = (E *) pregs[EREG];
   M * mreg = (M *) pregs[MREG];
   W * wreg = (W *) pregs[WREG];

   uint64_t valE = 0;
   uint64_t stat = ereg->getstat()->getOutput();
   uint64_t icode = ereg->geticode()->getOutput();
   uint64_t ifun = ereg->getifun()->getOutput();
   uint64_t valA = ereg->getvalA()->getOutput();
   uint64_t valB = ereg->getvalB()->getOutput();
   uint64_t dstE = ereg->getdstE()->getOutput();
   uint64_t dstM = ereg->getdstM()->getOutput();
   uint64_t valC = ereg->getvalC()->getOutput();

   uint64_t ALUfun = alufun(icode, ifun);
   uint64_t ALUA = aluA(icode, valA, valC);
   uint64_t ALUB = aluB(icode, valB);

   e_valE = alu(ALUA, ALUB, ALUfun);
   valE = e_valE;

   MemoryStage * mStage = (MemoryStage *) stages[MSTAGE];
   uint64_t m_stat = mStage->getm_stat();
   uint64_t W_stat = wreg->getstat()->getOutput();

   if (set_cc(icode, m_stat, W_stat)) cc(ALUA, ALUB, ALUfun, valE);
   e_Cnd = cond(icode, ifun);

   M_bubble = calculateControlSignals(m_stat, W_stat);
   
   e_dstE = edstE(icode, e_Cnd, dstE);
   dstE = e_dstE;

   setMInput(mreg, stat, icode, e_Cnd, valE, valA, dstE, dstM);
   return false;
}

/* doClockHigh
 * applies the appropriate control signal to the F
 * and D register intances
 *
 * @param: pregs - array of the pipeline register (F, D, E, M, W instances)
 */
void ExecuteStage::doClockHigh(PipeReg ** pregs)
{
   if (M_bubble) bubble(pregs);
   else normalM(pregs);
}

void ExecuteStage::setMInput(M * mreg, uint64_t stat, uint64_t icode, uint64_t Cnd, 
                     uint64_t valE, uint64_t valA,
                     uint64_t dstE, uint64_t dstM)
{
   mreg->getstat()->setInput(stat);
   mreg->geticode()->setInput(icode);
   mreg->getCnd()->setInput(Cnd);
   mreg->getvalE()->setInput(valE);
   mreg->getvalA()->setInput(valA);
   mreg->getdstE()->setInput(dstE);
   mreg->getdstM()->setInput(dstM);
}

uint64_t ExecuteStage::aluA(uint64_t E_icode, uint64_t E_valA, uint64_t E_valC) {
   if (E_icode == IRRMOVQ || E_icode == IOPQ) return E_valA;
   else if (E_icode == IIRMOVQ || E_icode == IRMMOVQ || E_icode == IMRMOVQ) return E_valC;
   else if (E_icode == ICALL || E_icode == IPUSHQ) return -8;
   else if (E_icode == IRET || E_icode == IPOPQ) return 8;
   else return 0;
}

uint64_t ExecuteStage::aluB(uint64_t E_icode, uint64_t E_valB) {
   if(E_icode == IRMMOVQ || E_icode == IMRMOVQ || E_icode == IOPQ || E_icode == ICALL || E_icode == IPUSHQ || E_icode == IRET || E_icode == IPOPQ) return E_valB;
   else if(E_icode == IRRMOVQ || E_icode == IIRMOVQ) return 0;
   else return 0;
}

uint64_t ExecuteStage::alufun(uint64_t E_icode, uint64_t E_ifun) {
   if (E_icode == IOPQ) return E_ifun;
   else return ADDQ;
}

bool ExecuteStage::set_cc(uint64_t E_icode, uint64_t m_stat, uint64_t W_stat) {
   return (E_icode == IOPQ) && !(m_stat == SADR || m_stat == SINS || m_stat == SHLT) 
            && !(W_stat == SADR || W_stat == SINS || W_stat == SHLT);
}

uint64_t ExecuteStage::edstE(uint64_t E_icode, uint64_t e_Cnd, uint64_t E_dstE) {
   if (E_icode == IRRMOVQ && !e_Cnd) return RNONE;
   else return E_dstE;
}

uint64_t ExecuteStage::alu(uint64_t aluA, uint64_t aluB, uint64_t alufun) {
   if(alufun == ADDQ) return aluA + aluB;
   else if(alufun == SUBQ) return aluB - aluA;
   else if(alufun == XORQ) return aluA ^ aluB;
   else return aluA & aluB;
}

void ExecuteStage::cc(uint64_t aluA, uint64_t aluB, uint64_t alufun, uint64_t valE) {
   bool error;
   ConditionCodes * cc = ConditionCodes::getInstance();

   bool aluAS = Tools::getBits(aluA, 63, 63) == 1;
   bool aluBS = Tools::getBits(aluB, 63, 63) == 1;
   bool valES = Tools::getBits(valE, 63, 63) == 1;

   cc->setConditionCode((valE == 0), ZF, error);
   cc->setConditionCode(valES, SF, error);

   if (alufun == XORQ || alufun == ANDQ) cc->setConditionCode(0, OF, error);
   else if (alufun == ADDQ) {
      if (aluAS == aluBS) cc->setConditionCode(valES == !aluAS, OF, error);
      else cc->setConditionCode(false, OF, error);
   } 
   else {
      if (aluAS == !aluBS) cc->setConditionCode(valES == !aluBS, OF, error);
      else cc->setConditionCode(false, OF, error);
   }
}

uint64_t ExecuteStage::gete_dstE() {
   return e_dstE;
}

uint64_t ExecuteStage::gete_valE() {
   return e_valE;
}

uint64_t ExecuteStage::cond(uint64_t icode, uint64_t ifun) {
   bool error;
   bool zf = ConditionCodes::getInstance()->getConditionCode(ZF, error);
   bool of = ConditionCodes::getInstance()->getConditionCode(OF, error);
   bool sf = ConditionCodes::getInstance()->getConditionCode(SF, error);

   if (icode != IJXX && icode != ICMOVXX) return 0;
   else if (icode == UNCOND) return 1;
   else if (ifun == LESSEQ) return (sf ^ of) | zf;
   else if (ifun == LESS) return (sf ^ of);
   else if (ifun == EQUAL) return zf;
   else if (ifun == NOTEQUAL) return !zf;
   else if (ifun == GREATER) return !(sf ^ of) & !zf;
   else return !(sf ^ of);
}

bool ExecuteStage::calculateControlSignals(uint64_t m_stat, uint64_t W_stat) {
   bool result = (m_stat == SADR || m_stat == SINS || m_stat == SHLT) 
            || (W_stat == SADR || W_stat == SINS || W_stat == SHLT);
   return result;
}

void ExecuteStage::bubble(PipeReg ** pregs) {
   M * mreg = (M *) pregs[MREG];

   mreg->getstat()->bubble(SAOK);
   mreg->geticode()->bubble(INOP);
   mreg->getCnd()->bubble();
   mreg->getvalE()->bubble();
   mreg->getvalA()->bubble();
   mreg->getdstE()->bubble(RNONE);
   mreg->getdstM()->bubble(RNONE);
}

void ExecuteStage::normalM(PipeReg ** pregs) {
   M * mreg = (M *) pregs[MREG];
   
      mreg->getstat()->normal();
      mreg->geticode()->normal();
      mreg->getCnd()->normal();
      mreg->getvalE()->normal();
      mreg->getvalA()->normal();
      mreg->getdstE()->normal();
      mreg->getdstM()->normal();
}

bool ExecuteStage::gete_Cnd() {
   return e_Cnd;
}
