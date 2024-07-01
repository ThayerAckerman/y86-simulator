#include <string>
#include <cstdint>
#include "RegisterFile.h"
#include "PipeRegField.h"
#include "PipeReg.h"
#include "F.h"
#include "D.h"
#include "M.h"
#include "W.h"
#include "Stage.h"
#include "WritebackStage.h"
#include "Status.h"
#include "Debug.h"
#include "Instructions.h"


bool WritebackStage::doClockLow(PipeReg ** pregs, Stage ** stages)
{
   W * wreg = (W *) pregs[WREG];
   uint64_t stat = wreg->getstat()->getOutput();

   if (stat != SAOK) return true;
   return false;
}

void WritebackStage::doClockHigh(PipeReg ** pregs)
{
   W * wreg = (W *) pregs[WREG];
   uint64_t W_valE = wreg->getvalE()->getOutput();
   uint64_t W_dstE = wreg->getdstE()->getOutput();
   uint64_t W_valM = wreg->getvalM()->getOutput();
   uint64_t W_dstM = wreg->getdstM()->getOutput();

   bool error;
   RegisterFile::getInstance()->writeRegister(W_valE, W_dstE, error);
   RegisterFile::getInstance()->writeRegister(W_valM, W_dstM, error);
}
