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
#include "DecodeStage.h"
#include "Status.h"
#include "Debug.h"
#include "Instructions.h"
#include "ExecuteStage.h"
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
bool DecodeStage::doClockLow(PipeReg ** pregs, Stage ** stages)
{
   D * dreg = (D *) pregs[DREG];
   E * ereg = (E *) pregs[EREG];
   M * mreg = (M *) pregs[MREG];
   W * wreg = (W *) pregs[WREG];
   uint64_t valA = 0, valB = 0; 
   uint64_t dstE = RNONE, dstM = RNONE;
   uint64_t stat = dreg->getstat()->getOutput();
   uint64_t icode = dreg->geticode()->getOutput();
   uint64_t ifun = dreg->getifun()->getOutput();
   uint64_t valC = dreg->getvalC()->getOutput();
   uint64_t rB = dreg->getrB()->getOutput();
   uint64_t rA = dreg->getrA()->getOutput();
   uint64_t valP = dreg->getvalP()->getOutput();
   
   d_srcA = build_d_srcA(icode, rA);
   d_srcB = build_d_srcB(icode, rB);
   dstM = d_dstM(icode, rA);
   dstE = d_dstE(icode, rB);

   ExecuteStage * eStage = (ExecuteStage *) stages[ESTAGE];
   uint64_t e_dstE = eStage->gete_dstE();
   uint64_t e_valE = eStage->gete_valE();
   bool e_Cnd = eStage->gete_Cnd();

   MemoryStage * mStage = (MemoryStage *) stages[MSTAGE];
   uint64_t m_valM = mStage->getm_valM();

   uint64_t E_icode = ereg->geticode()->getOutput();
   uint64_t E_dstM = ereg->getdstM()->getOutput();
   calculateControlSignals(E_icode, E_dstM, d_srcA, d_srcB, e_Cnd);

   
   valA = d_valA(mreg, wreg, e_dstE, e_valE, d_srcA, icode, valP, m_valM);
   valB = d_valB(mreg, wreg, e_dstE, e_valE, d_srcB, m_valM);
   setEInput(ereg, stat, icode, ifun, valC, valA, valB, dstE, dstM, d_srcA, d_srcB);
   return false;
}

/* doClockHigh
 * applies the appropriate control signal to the F
 * and D register intances
 *
 * @param: pregs - array of the pipeline register (F, D, E, M, W instances)
 */
void DecodeStage::doClockHigh(PipeReg ** pregs)
{
   if (E_bubble) bubbleE(pregs);
   else normalE(pregs);
}

void DecodeStage::setEInput(E * ereg, uint64_t stat, uint64_t icode, 
                           uint64_t ifun, uint64_t valC, uint64_t valA,
                           uint64_t valB, uint64_t dstE, uint64_t dstM, 
                           uint64_t srcA, uint64_t srcB)
{
   ereg->getstat()->setInput(stat);
   ereg->geticode()->setInput(icode);
   ereg->getifun()->setInput(ifun);
   ereg->getvalC()->setInput(valC);
   ereg->getvalA()->setInput(valA);
   ereg->getvalB()->setInput(valB);
   ereg->getdstE()->setInput(dstE);
   ereg->getdstM()->setInput(dstM);
   ereg->getsrcA()->setInput(srcA);
   ereg->getsrcB()->setInput(srcB);
}

uint64_t DecodeStage::build_d_srcA(uint64_t D_icode, uint64_t D_rA) {
   uint64_t d_srcA;
   if (D_icode == IRRMOVQ || D_icode ==  IRMMOVQ || D_icode ==  IOPQ || D_icode == IPUSHQ) d_srcA = D_rA;
   else if (D_icode == IPOPQ || D_icode ==  IRET) d_srcA = RSP;
   else d_srcA = RNONE;
   return d_srcA;
}  

uint64_t DecodeStage::build_d_srcB(uint64_t D_icode, uint64_t D_rB) {
   uint64_t d_srcB;
   if (D_icode == IOPQ || D_icode ==  IRMMOVQ || D_icode ==  IMRMOVQ) d_srcB = D_rB;
   else if (D_icode == IPUSHQ || D_icode ==  IPOPQ || D_icode ==  ICALL || D_icode ==  IRET) d_srcB = RSP;
   else d_srcB = RNONE;
   return d_srcB;
}  

uint64_t DecodeStage::d_dstE(uint64_t D_icode, uint64_t D_rB) {
   if (D_icode == IRRMOVQ || D_icode ==  IIRMOVQ || D_icode ==  IOPQ) return D_rB;
   else if (D_icode == IPUSHQ || D_icode ==  IPOPQ || D_icode ==  ICALL || D_icode ==  IRET) return RSP;
   else return RNONE;
} 

uint64_t DecodeStage::d_dstM(uint64_t D_icode, uint64_t D_rA) {
   uint64_t d_dstE;
   if (D_icode == IMRMOVQ || D_icode ==  IPOPQ) d_dstE = D_rA;
   else d_dstE = RNONE;
   return d_dstE;
} 

uint64_t DecodeStage::d_valA(M * mreg, W * wreg, uint64_t e_dstE, uint64_t e_valE, uint64_t d_srcA, uint64_t D_icode, uint64_t D_valP, uint64_t m_valM) {
   bool error;
   if(D_icode == ICALL || D_icode == IJXX) return D_valP;
   if(d_srcA == RNONE) return 0;
   else if(d_srcA == e_dstE) return e_valE;
   else if(d_srcA == mreg->getdstM()->getOutput()) return m_valM;
   else if(d_srcA == mreg->getdstE()->getOutput()) return mreg->getvalE()->getOutput();
   else if(d_srcA == wreg->getdstM()->getOutput()) return wreg->getvalM()->getOutput();
   else if(d_srcA == wreg->getdstE()->getOutput()) return wreg->getvalE()->getOutput();
   else return RegisterFile::getInstance()->readRegister(d_srcA, error);
} 

uint64_t DecodeStage::d_valB(M * mreg, W * wreg, uint64_t e_dstE, uint64_t e_valE, uint64_t d_srcB, uint64_t m_valM) {
   bool error;
   if(d_srcB == RNONE) return 0;
   if(d_srcB == e_dstE) return e_valE;
   else if(d_srcB == mreg->getdstM()->getOutput()) return m_valM;
   else if(d_srcB == mreg->getdstE()->getOutput()) return mreg->getvalE()->getOutput();
   else if(d_srcB == wreg->getdstM()->getOutput()) return wreg->getvalM()->getOutput();
   else if(d_srcB == wreg->getdstE()->getOutput()) return wreg->getvalE()->getOutput();
   else return RegisterFile::getInstance()->readRegister(d_srcB, error);
} 

bool DecodeStage::buildE_bubble(uint64_t E_icode, uint64_t E_dstM, uint64_t d_srcA, uint64_t d_srcB, uint64_t e_Cnd) {
   return (E_icode == IJXX && !e_Cnd)
            || ((E_icode == IMRMOVQ || E_icode == IPOPQ) && (E_dstM == d_srcA || E_dstM == d_srcB));
}

void DecodeStage::calculateControlSignals(uint64_t E_icode, uint64_t E_dstM, uint64_t d_srcA, uint64_t d_srcB, uint64_t e_Cnd) {
   E_bubble = buildE_bubble(E_icode, E_dstM, d_srcA, d_srcB, e_Cnd);
}

void DecodeStage::bubbleE(PipeReg ** pregs) {
   E * ereg = (E *) pregs[EREG];
   ereg->getstat()->bubble(SAOK);
   ereg->geticode()->bubble(INOP);
   ereg->getifun()->bubble();
   ereg->getvalC()->bubble();
   ereg->getvalA()->bubble();
   ereg->getvalB()->bubble();
   ereg->getdstE()->bubble(RNONE);
   ereg->getdstM()->bubble(RNONE);
   ereg->getsrcA()->bubble(RNONE);
   ereg->getsrcB()->bubble(RNONE);
}

void DecodeStage::normalE(PipeReg ** pregs) {
   E * ereg = (E *) pregs[EREG];
   ereg->getstat()->normal();
   ereg->geticode()->normal();
   ereg->getifun()->normal();
   ereg->getvalC()->normal();
   ereg->getvalA()->normal();
   ereg->getvalB()->normal();
   ereg->getdstE()->normal();
   ereg->getdstM()->normal();
   ereg->getsrcA()->normal();
   ereg->getsrcB()->normal();
}

uint64_t DecodeStage::getd_srcA() {
   return d_srcA;
}

uint64_t DecodeStage::getd_srcB() {
   return d_srcB;
}
