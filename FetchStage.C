#include <string>
#include <cstdint>
#include "RegisterFile.h"
#include "PipeRegField.h"
#include "PipeReg.h"
#include "F.h"
#include "D.h"
#include "M.h"
#include "W.h"
#include "E.h"
#include "Stage.h"
#include "FetchStage.h"
#include "Status.h"
#include "Debug.h"
#include "Instructions.h"
#include "Memory.h"
#include "Tools.h"
#include "DecodeStage.h"
#include "ExecuteStage.h"

/*
 * doClockLow:
 * Performs the Fetch stage combinational logic that is performed when
 * the clock edge is low.
 *
 * @param: pregs - array of the pipeline register sets (F, D, E, M, W instances)
 * @param: stages - array of stages (FetchStage, DecodeStage, ExecuteStage,
 *         MemoryStage, WritebackStage instances)
 */
bool FetchStage::doClockLow(PipeReg ** pregs, Stage ** stages)
{
   F * freg = (F *) pregs[FREG];
   D * dreg = (D *) pregs[DREG];
   E * ereg = (E *) pregs[EREG];
   M * mreg = (M *) pregs[MREG];
   W * wreg = (W *) pregs[WREG];


   uint64_t f_pc = 0, icode = 0, ifun = 0, valC = 0, valP = 0;
   uint64_t rA = RNONE, rB = RNONE;

   //code missing here to select the value of the PC
   //and fetch the instruction from memory
   //Fetching the instruction will allow the icode, ifun,
   //rA, rB, and valC to be set.
   //The lab assignment describes what methods need to be
   //written.
   f_pc = selectPC(freg, mreg, wreg);
   bool error;
   uint64_t instructionByte = Memory::getInstance()->getByte(f_pc, error);
   icode = buildf_icode(error, Tools::getBits(instructionByte, 4, 7));
   ifun = buildf_ifun(error, Tools::getBits(instructionByte, 0, 3));

   bool instr_valid = (icode == INOP || icode == IHALT || icode == IRRMOVQ || icode == IIRMOVQ 
                        || icode == IRMMOVQ || icode == IMRMOVQ || icode == IOPQ || icode == IJXX 
                        || icode == ICALL || icode == IRET || icode == IPUSHQ || icode == IPOPQ);

   uint64_t stat = buildf_stat(error, instr_valid, icode);

   valP = PCIncrement(f_pc, needRegIds(icode), needValC(icode));

   
   //The value passed to setInput below will need to be changed

   if (needRegIds(icode)) getRegIds(rA, rB, Memory::getInstance()->getByte(f_pc + 1, error));

   if (needValC(icode)) buildValC(valC, f_pc, icode);

   uint64_t predictedPC = predictPC(icode, valC, valP);
   freg->getpredPC()->setInput(predictedPC);

   DecodeStage * dStage = (DecodeStage *) stages[DSTAGE];
   uint64_t d_srcA = dStage->getd_srcA();
   uint64_t d_srcB = dStage->getd_srcB();
   uint64_t E_icode = ereg->geticode()->getOutput();
   uint64_t E_dstM = ereg->getdstM()->getOutput();

   ExecuteStage * eStage = (ExecuteStage *) stages[ESTAGE];
   uint64_t e_Cnd = eStage->gete_Cnd();

   uint64_t D_icode = dreg->geticode()->getOutput();
   uint64_t M_icode = mreg->geticode()->getOutput();

   calculateControlSignals(E_icode, D_icode, M_icode, E_dstM, d_srcA, d_srcB, e_Cnd);

   //provide the input values for the D register
   setDInput(dreg, stat, icode, ifun, rA, rB, valC, valP);
   return false;
}

/* doClockHigh
 * applies the appropriate control signal to the F
 * and D register intances
 *
 * @param: pregs - array of the pipeline register (F, D, E, M, W instances)
 */
void FetchStage::doClockHigh(PipeReg ** pregs)
{
   if(D_bubble) bubbleD(pregs);
   else if(!D_stall) normalD(pregs);
}

/* setDInput
 * provides the input to potentially be stored in the D register
 * during doClockHigh
 *
 * @param: dreg - pointer to the D register instance
 * @param: stat - value to be stored in the stat pipeline register within D
 * @param: icode - value to be stored in the icode pipeline register within D
 * @param: ifun - value to be stored in the ifun pipeline register within D
 * @param: rA - value to be stored in the rA pipeline register within D
 * @param: rB - value to be stored in the rB pipeline register within D
 * @param: valC - value to be stored in the valC pipeline register within D
 * @param: valP - value to be stored in the valP pipeline register within D
*/
void FetchStage::setDInput(D * dreg, uint64_t stat, uint64_t icode, 
                           uint64_t ifun, uint64_t rA, uint64_t rB,
                           uint64_t valC, uint64_t valP)
{
   dreg->getstat()->setInput(stat);
   dreg->geticode()->setInput(icode);
   dreg->getifun()->setInput(ifun);
   dreg->getrA()->setInput(rA);
   dreg->getrB()->setInput(rB);
   dreg->getvalC()->setInput(valC);
   dreg->getvalP()->setInput(valP);
}

/* selectPC
* 
*/
uint64_t FetchStage::selectPC(F * freg, M * mreg, W * wreg)
{
   if (mreg->geticode()->getOutput() == IJXX && !(mreg->getCnd()->getOutput()))
   {
      return mreg->getvalA()->getOutput();
   }
   else if (wreg->geticode()->getOutput() == IRET)
   {
      return wreg->getvalM()->getOutput();
   }
   else
   {
      return freg->getpredPC()->getOutput();
   }
}

/* needRegIds
* 
*/
bool FetchStage::needRegIds(uint64_t ficode)
{
   if (ficode == IRRMOVQ || ficode == IOPQ || ficode == IPUSHQ || ficode == IPOPQ || ficode == IIRMOVQ || ficode == IRMMOVQ || ficode == IMRMOVQ)
   {
      return true;
   }
   return false;
}

/* needValC
* 
*/
bool FetchStage::needValC(uint64_t ficode)
{
   if (ficode == IIRMOVQ || ficode == IRMMOVQ || ficode == IMRMOVQ || ficode == IJXX || ficode == ICALL)
   {
      return true;
   }
   return false;
}

/* predictPC
* 
*/
uint64_t FetchStage::predictPC(uint64_t ficode, uint64_t fvalC, uint64_t fvalP)
{
   if (ficode == IJXX || ficode == ICALL)
   {
      return fvalC;
   }
   else
   {
      return fvalP;
   }
} 

/*
*
*/
uint64_t FetchStage::PCIncrement(uint64_t f_pc, bool needRegIDs, bool needValC)
{
   if(needValC && !needRegIDs) 
   {
      return f_pc + (9);
   }
   else if(needValC && needRegIDs) 
   {
      return f_pc + (10);
   }
   else if(!needValC && needRegIDs)
   {
      return f_pc + (2);
   }
   else
   {
      return f_pc + 1;
   }
}

void FetchStage::getRegIds(uint64_t & rA, uint64_t & rB, uint64_t regByte) {
      rA = Tools::getBits(regByte, 4, 7);
      rB = Tools::getBits(regByte, 0, 3);
}

void FetchStage::buildValC(uint64_t & valC, uint64_t f_pc, uint64_t ficode) {
   bool error;

   uint64_t start_pc = f_pc;
   if (ficode <= IMRMOVQ) start_pc += 2;
   else start_pc += 1;
   valC = Memory::getInstance()->getLong(start_pc, error);
}

uint64_t FetchStage::buildf_stat(bool mem_error, uint64_t instr_valid, uint64_t f_icode) {
   if (mem_error) return SADR;
   else if (!instr_valid) return SINS;
   else if (f_icode == IHALT) return SHLT;
   else return SAOK;
}

uint64_t FetchStage::buildf_ifun(bool mem_error, uint64_t f_ifun) {
   if (mem_error) return FNONE;
   else return f_ifun;
}

uint64_t FetchStage::buildf_icode(bool mem_error, uint64_t f_icode) {
   if (mem_error) return INOP;
   else return f_icode;
}

bool FetchStage::buildF_stall (uint64_t E_icode, uint64_t D_icode, uint64_t M_icode, uint64_t E_dstM, uint64_t d_srcA, uint64_t d_srcB) {
   return ((E_icode == IMRMOVQ || E_icode == IPOPQ) && (E_dstM == d_srcA || E_dstM == d_srcB)) || (IRET == D_icode || IRET == E_icode || IRET == M_icode);
}

bool FetchStage::buildD_stall (uint64_t E_icode, uint64_t E_dstM, uint64_t d_srcA, uint64_t d_srcB) {
   return (E_icode == IMRMOVQ || E_icode == IPOPQ) && (E_dstM == d_srcA || E_dstM == d_srcB);
}

bool FetchStage::buildD_bubble(uint64_t E_icode, uint64_t D_icode, uint64_t M_icode, uint64_t E_dstM, uint64_t d_srcA, uint64_t d_srcB, bool e_Cnd) {
   return (E_icode == IJXX && !e_Cnd) || (!((E_icode == IMRMOVQ || E_icode == IPOPQ) && (E_dstM == d_srcA || E_dstM == d_srcB)) && (IRET == D_icode || IRET == E_icode || IRET == M_icode));
}

void FetchStage::calculateControlSignals(uint64_t E_icode, uint64_t D_icode, uint64_t M_icode, uint64_t E_dstM, uint64_t d_srcA, uint64_t d_srcB, bool e_Cnd) {
   F_stall = buildF_stall(E_icode, D_icode, M_icode, E_dstM, d_srcA, d_srcB);
   D_stall = buildD_stall(E_icode, E_dstM, d_srcA, d_srcB);
   D_bubble = buildD_bubble(E_icode, D_icode, M_icode, E_dstM, d_srcA, d_srcB, e_Cnd);
}

void FetchStage::bubbleD(PipeReg ** pregs) {
      F * freg = (F *) pregs[FREG];
      D * dreg = (D *) pregs[DREG];

      if (!F_stall) freg->getpredPC()->normal();

      dreg->getstat()->bubble(SAOK);
      dreg->geticode()->bubble(INOP);
      dreg->getifun()->bubble();
      dreg->getrA()->bubble(RNONE);
      dreg->getrB()->bubble(RNONE);
      dreg->getvalC()->bubble();
      dreg->getvalP()->bubble();
}

void FetchStage::normalD(PipeReg ** pregs) {
   F * freg = (F *) pregs[FREG];
   D * dreg = (D *) pregs[DREG];

   if (!F_stall) freg->getpredPC()->normal();
   
   dreg->getstat()->normal();
   dreg->geticode()->normal();
   dreg->getifun()->normal();
   dreg->getrA()->normal();
   dreg->getrB()->normal();
   dreg->getvalC()->normal();
   dreg->getvalP()->normal();
}

