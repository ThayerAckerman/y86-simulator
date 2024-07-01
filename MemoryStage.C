#include <string>
#include <cstdint>
#include "RegisterFile.h"
#include "PipeRegField.h"
#include "PipeReg.h"
#include "M.h"
#include "W.h"
#include "Stage.h"
#include "MemoryStage.h"
#include "Status.h"
#include "Debug.h"
#include "Instructions.h"
#include "Memory.h"

bool MemoryStage::doClockLow(PipeReg ** pregs, Stage ** stages)
{
   M * mreg = (M *) pregs[MREG];
   W * wreg = (W *) pregs[WREG];
   uint64_t valM = 0;
   uint64_t M_stat = mreg->getstat()->getOutput();
   uint64_t icode = mreg->geticode()->getOutput();
   uint64_t valE = mreg->getvalE()->getOutput();
   uint64_t valA = mreg->getvalA()->getOutput();
   uint64_t dstE = mreg->getdstE()->getOutput();
   uint64_t dstM = mreg->getdstM()->getOutput();

   bool mem_read = (icode == IMRMOVQ || icode == IPOPQ || icode == IRET);
   bool mem_write = (icode == IRMMOVQ || icode == IPUSHQ || icode == ICALL);

   uint64_t mem_address = mem_addr(icode, valE, valA);

   bool error;
   if (mem_read) {
      m_valM = Memory::getInstance()->getLong(mem_address, error);
      if (error) valM = 0;
      else valM = m_valM;
   }
   
   if (mem_write) Memory::getInstance()->putLong(valA, mem_address, error);

   m_stat = buildm_stat(error, M_stat);
   
   setWInput(wreg, m_stat, icode, valE, valM, dstE, dstM);
   return false;
}

void MemoryStage::doClockHigh(PipeReg ** pregs)
{
   W * wreg = (W *) pregs[WREG];
   
   wreg->getstat()->normal();
   wreg->geticode()->normal();
   wreg->getvalE()->normal();
   wreg->getvalM()->normal();
   wreg->getdstE()->normal();
   wreg->getdstM()->normal();
}

void MemoryStage::setWInput(W * wreg, uint64_t stat, uint64_t icode,
                     uint64_t valE, uint64_t valM, uint64_t dstE, 
                     uint64_t dstM)
{
   wreg->getstat()->setInput(stat);
   wreg->geticode()->setInput(icode);
   wreg->getvalE()->setInput(valE);
   wreg->getvalM()->setInput(valM);
   wreg->getdstE()->setInput(dstE);
   wreg->getdstM()->setInput(dstM);
}

uint64_t MemoryStage::mem_addr(uint64_t M_icode, uint64_t M_valE, uint64_t M_valA) {
   
   if (M_icode == IRMMOVQ || M_icode == IPUSHQ || M_icode == ICALL || M_icode == IMRMOVQ) return M_valE;
   else if (M_icode == IPOPQ || M_icode == IRET) return M_valA;
   else return 0;
}

uint64_t MemoryStage::buildm_stat(bool mem_error, uint64_t M_stat) {
   if (mem_error) return SADR;
   else return M_stat;
}

uint64_t MemoryStage::getm_valM() {
   return m_valM;
}

uint64_t MemoryStage::getm_stat() {
   return m_stat;
}
