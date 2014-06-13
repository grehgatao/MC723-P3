/******************************************************
 * ArchC Resources implementation file.               *
 * This file is automatically generated by ArchC      *
 * WITHOUT WARRANTY OF ANY KIND, either express       *
 * or implied.                                        *
 * For more information on ArchC, please visit:       *
 * http://www.archc.org                               *
 *                                                    *
 * The ArchC Team                                     *
 * Computer Systems Laboratory (LSC)                  *
 * IC-UNICAMP                                         *
 * http://www.lsc.ic.unicamp.br                       *
 ******************************************************/
 

#include  "mips1_arch.H"
#include  "mips1_arch_ref.H"

//!/Default constructor.
mips1_arch_ref::mips1_arch_ref(mips1_arch& arch) : ac_arch_ref<mips1_parms::ac_word, mips1_parms::ac_Hword>(arch),
  ac_pc(arch.ac_pc),
  DM(arch.DM),   RB(arch.RB),   npc(arch.npc),   hi(arch.hi),   lo(arch.lo), LL(arch.LL), LLAddr(arch.LLAddr), oldLL(arch.oldLL) {}

