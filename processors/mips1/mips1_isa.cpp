/**
 * @file      mips1_isa.cpp
 * @author    Sandro Rigo
 *            Marcus Bartholomeu
 *            Alexandro Baldassin (acasm information)
 *
 *            The ArchC Team
 *            http://www.archc.org/
 *
 *            Computer Systems Laboratory (LSC)
 *            IC-UNICAMP
 *            http://www.lsc.ic.unicamp.br/
 *
 * @version   1.0
 * @date      Mon, 19 Jun 2006 15:50:52 -0300
 * 
 * @brief     The ArchC i8051 functional model.
 * 
 * @attention Copyright (C) 2002-2006 --- The ArchC Team
 * 
 * This program is free software; you can redistribute it and/or modify 
 * it under the terms of the GNU General Public License as published by 
 * the Free Software Foundation; either version 2 of the License, or 
 * (at your option) any later version. 
 * 
 * This program is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU General Public License for more details. 
 * 
 * You should have received a copy of the GNU General Public License 
 * along with this program; if not, write to the Free Software 
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include  "mips1_isa.H"
#include  "mips1_isa_init.cpp"
#include  "mips1_bhv_macros.H"
#include  "pipeline_model.cpp"


//If you want debug information for this model, uncomment next line
//#define DEBUG_MODEL
#include "ac_debug_model.H"

/*MACROS DO TRABALHO*/
//Como usar macros.h: alterar arquivo com os macros da pipeline (pode ser com script bash)
#include "macros.h"

//!Added by Andre N. Filliettaz 1s2014p3
//!Log file
#include <fstream>

#ifndef PIPELINE_LENGTH
#define GEN_DIN 0 //1 - gera o DIN; 0 - Não gera
#define PIPELINE_LENGTH 7 // Número de estágios da Pipeline
#define FORWARD 1 //1 - Pipeline simula forwarding; 0 - Não simula
#define BRANCH 0 //0 - Branch Prediciton Desabilitado;
                 //1 - Saturating Counter com tabela de 8x2bit s/distinção de instruções
                 //2 - Saturating Counter com tabela de 8x2bit c/distinção de instruções
#endif

//!User defined macros to reference registers.
#define Ra 31
#define Sp 29

// 'using namespace' statement to allow access to all
// mips1-specific datatypes
using namespace mips1_parms;
//PipelineModel *pipeline;
FILE *cache;

//!Generic instruction behavior method.
void ac_behavior( instruction )
{
    dbg_printf("----- PC=%#x ----- %lld\n", (int) ac_pc, ac_instr_counter);
    //  dbg_printf("----- PC=%#x NPC=%#x ----- %lld\n", (int) ac_pc, (int)npc, ac_instr_counter);
    #ifndef NO_NEED_PC_UPDATE
    ac_pc = npc;
    npc = ac_pc + 4;
    #endif
    
    if (GEN_DIN) fprintf(cache, "2 %x\n", (int)ac_pc);
};

//! Instruction Format behavior methods.
void ac_behavior( Type_R ){}
void ac_behavior( Type_I ){}
void ac_behavior( Type_J ){}

//!Behavior called before starting simulation
void ac_behavior(begin)
{
    pipeline = new PipelineModel(PIPELINE_LENGTH, FORWARD, BRANCH);
    if (GEN_DIN) cache = fopen("cache.din","w");
    dbg_printf("@@@ begin behavior @@@\n");
    printf("%d", ADDIU);
    RB[0] = 0;
    npc = ac_pc + 4;
    
    // Is is not required by the architecture, but makes debug really easier
    for (int regNum = 0; regNum < 32; regNum ++)
        RB[regNum] = 0;
    hi = 0;
    lo = 0;
}

//!Behavior called after finishing simulation
void ac_behavior(end)
{
    ofstream log_file("cicles.txt", ios::app);
    //Termina de executar o que ficou na pipeline
    for(int i=0; i<pipeline->get_num_states(); i++){
        pipeline->inject_instr(pipeline->nop_instr);
    }
    dbg_printf("@@@ end behavior @@@\n");
    printf("\n\nIC = %lli\n\n", pipeline->ic);
    printf("\n\nDATA_HAZARD_NOPs = %lli\n\n", pipeline->data_hazard_ic);
    printf("\n\nCONTROL_HAZARD_NOPs = %lli\n\n", pipeline->control_hazard_ic);
    printf("\n\nBRANCH_PREDICTIONS = %lli\n\n", pipeline->branch_predictions);
    printf("\n\nBRANCH_MISPREDICTIONS = %lli\n\n", pipeline->branch_mispredictions);
    if (GEN_DIN) fclose(cache);

    // Log file
    log_file << pipeline->ic << endl;
    log_file.close();
}


//!Instruction lb behavior method.
void ac_behavior( lb )
{
    pipeline->inject_instr(LB, rs, -1, rt, RB[rs]+ imm, 0);
    char byte;
    dbg_printf("lb r%d, %d(r%d)\n", rt, imm & 0xFFFF, rs);
    if (GEN_DIN) fprintf(cache, "0 %x\n", RB[rs]+ imm);
    byte = DM.read_byte(RB[rs]+ imm);
    RB[rt] = (ac_Sword)byte ;
    dbg_printf("Result = %#x\n", RB[rt]);
};

//!Instruction lbu behavior method.
void ac_behavior( lbu )
{
    pipeline->inject_instr(LBU, rs, -1, rt, RB[rs]+ imm, 0);
    unsigned char byte;
    dbg_printf("lbu r%d, %d(r%d)\n", rt, imm & 0xFFFF, rs);
    if (GEN_DIN) fprintf(cache, "0 %x\n", RB[rs]+ imm);
    byte = DM.read_byte(RB[rs]+ imm);
    RB[rt] = byte ;
    dbg_printf("Result = %#x\n", RB[rt]);
};

//!Instruction lh behavior method.
void ac_behavior( lh )
{
    pipeline->inject_instr(LH, rs, -1, rt, RB[rs]+ imm, 0);
    short int half;
    dbg_printf("lh r%d, %d(r%d)\n", rt, imm & 0xFFFF, rs);
    if (GEN_DIN) fprintf(cache, "0 %x\n", RB[rs]+ imm);
    half = DM.read_half(RB[rs]+ imm);
    RB[rt] = (ac_Sword)half ;
    dbg_printf("Result = %#x\n", RB[rt]);
};

//!Instruction lhu behavior method.
void ac_behavior( lhu )
{
    pipeline->inject_instr(LHU, rs, -1, rt, RB[rs]+ imm, 0);
    unsigned short int  half;
    if (GEN_DIN) fprintf(cache, "0 %x\n", RB[rs]+ imm);
    half = DM.read_half(RB[rs]+ imm);
    RB[rt] = half ;
    dbg_printf("Result = %#x\n", RB[rt]);
};

//!Instruction lw behavior method.
void ac_behavior( lw )
{
    pipeline->inject_instr(LW, rs, -1, rt, RB[rs]+ imm, 0);
    dbg_printf("lw r%d, %d(r%d)\n", rt, imm & 0xFFFF, rs);
    if (GEN_DIN) fprintf(cache, "0 %x\n", RB[rs]+ imm);
    RB[rt] = DM.read(RB[rs]+ imm);
    dbg_printf("Result = %#x\n", RB[rt]);
};

//!Instruction ll behavior method.
void ac_behavior( ll )
{
    pipeline->inject_instr(LW, rs, -1, rt, RB[rs]+ imm, 0);
    dbg_printf("ll r%d, %d(r%d)\n", rt, imm & 0xFFFF, rs);
    RB[rt] = DM.read(RB[rs]+ imm);
    LL = 1;
    LLAddr = RB[rs]+ imm;
    oldLL = RB[rt];
    dbg_printf("Result = %#x\n", RB[rt]);
};

//!Instruction lwl behavior method.
void ac_behavior( lwl )
{
    pipeline->inject_instr(LWL, rs, -1, rt, RB[rs]+ imm, 0);
    dbg_printf("lwl r%d, %d(r%d)\n", rt, imm & 0xFFFF, rs);
    unsigned int addr, offset;
    ac_Uword data;
    
    addr = RB[rs] + imm;
    offset = (addr & 0x3) * 8;
    if (GEN_DIN) fprintf(cache, "0 %x\n", addr & 0xFFFFFFFC);
    data = DM.read(addr & 0xFFFFFFFC);
    data <<= offset;
    data |= RB[rt] & ((1<<offset)-1);
    RB[rt] = data;
    dbg_printf("Result = %#x\n", RB[rt]);
};

//!Instruction lwr behavior method.
void ac_behavior( lwr )
{
    pipeline->inject_instr(LWR, rs, -1, rt, RB[rs]+ imm, 0);
    dbg_printf("lwr r%d, %d(r%d)\n", rt, imm & 0xFFFF, rs);
    unsigned int addr, offset;
    ac_Uword data;
    
    addr = RB[rs] + imm;
    offset = (3 - (addr & 0x3)) * 8;
    if (GEN_DIN) fprintf(cache, "0 %x\n", addr & 0xFFFFFFFC);
    data = DM.read(addr & 0xFFFFFFFC);
    data >>= offset;
    data |= RB[rt] & (0xFFFFFFFF << (32-offset));
    RB[rt] = data;
    dbg_printf("Result = %#x\n", RB[rt]);
};

//!Instruction sb behavior method.
void ac_behavior( sb )
{
    pipeline->inject_instr(SB, rs, -1, rt, 0, RB[rs] + imm);
    unsigned char byte;
    dbg_printf("sb r%d, %d(r%d)\n", rt, imm & 0xFFFF, rs);
    byte = RB[rt] & 0xFF;
    
    if (GEN_DIN) fprintf(cache, "1 %x\n", RB[rs] + imm);
    DM.write_byte(RB[rs] + imm, byte);
    dbg_printf("Result = %#x\n", (int) byte);
};

//!Instruction sh behavior method.
void ac_behavior( sh )
{
    pipeline->inject_instr(SH, rs, -1, rt, 0, RB[rs] + imm);
    unsigned short int half;
    dbg_printf("sh r%d, %d(r%d)\n", rt, imm & 0xFFFF, rs);
    half = RB[rt] & 0xFFFF;
    if (GEN_DIN) fprintf(cache, "1 %x\n", RB[rs] + imm);
    DM.write_half(RB[rs] + imm, half);
    dbg_printf("Result = %#x\n", (int) half);
};

//!Instruction sw behavior method.
void ac_behavior( sw )
{
    pipeline->inject_instr(SW, rs, -1, rt, 0, RB[rs] + imm);
    dbg_printf("sw r%d, %d(r%d)\n", rt, imm & 0xFFFF, rs);
    if (GEN_DIN) fprintf(cache, "1 %x\n", RB[rs] + imm);
    DM.write(RB[rs] + imm, RB[rt]);
    dbg_printf("Result = %#x\n", RB[rt]);
};

//!Instruction sc behavior method.
void ac_behavior( sc )
{
    pipeline->inject_instr(SW, rs, -1, rt, 0, RB[rs] + imm);
    dbg_printf("sc r%d, %d(r%d)\n", rt, imm & 0xFFFF, rs);  
    
    //Processador deve checar se o valor mudou
    unsigned int temp = DM.read(LLAddr);
    if (temp != oldLL)
    {
    LL = 0;
    }   
    if (LL == 1)
    {
    DM.write(RB[rs] + imm, RB[rt]);
    }
    RB[rt] = LL;
    dbg_printf("Result = %#x\n", RB[rt]);
};

//!Instruction swl behavior method.
void ac_behavior( swl )
{
    pipeline->inject_instr(SWL, rs, -1, rt, 0, RB[rs] + imm);
    dbg_printf("swl r%d, %d(r%d)\n", rt, imm & 0xFFFF, rs);
    unsigned int addr, offset;
    ac_Uword data;
    
    addr = RB[rs] + imm;
    offset = (addr & 0x3) * 8;
    data = RB[rt];
    data >>= offset;
    
    if (GEN_DIN) fprintf(cache, "0 %x\n", addr & 0xFFFFFFFC) & (0xFFFFFFFF << (32-offset));
    data |= DM.read(addr & 0xFFFFFFFC) & (0xFFFFFFFF << (32-offset));
    if (GEN_DIN) fprintf(cache, "1 %x\n", addr & 0xFFFFFFFC);
    DM.write(addr & 0xFFFFFFFC, data);
    dbg_printf("Result = %#x\n", data);
};

//!Instruction swr behavior method.
void ac_behavior( swr )
{
    pipeline->inject_instr(SWR, rs, -1, rt, 0, RB[rs] + imm);
    dbg_printf("swr r%d, %d(r%d)\n", rt, imm & 0xFFFF, rs);
    unsigned int addr, offset;
    ac_Uword data;
    
    addr = RB[rs] + imm;
    offset = (3 - (addr & 0x3)) * 8;
    data = RB[rt];
    data <<= offset;
    
    if (GEN_DIN) fprintf(cache, "0 %x\n", addr & 0xFFFFFFFC) & ((1<<offset)-1);
    data |= DM.read(addr & 0xFFFFFFFC) & ((1<<offset)-1);
    if (GEN_DIN) fprintf(cache, "1 %x\n", addr & 0xFFFFFFFC);
    DM.write(addr & 0xFFFFFFFC, data);
    dbg_printf("Result = %#x\n", data);
};

//!Instruction addi behavior method.
void ac_behavior( addi )
{
    pipeline->inject_instr(ADDI, rs, -1, rt, 0, 0);
    dbg_printf("addi r%d, r%d, %d\n", rt, rs, imm & 0xFFFF);
    RB[rt] = RB[rs] + imm;
    dbg_printf("Result = %#x\n", RB[rt]);
    //Test overflow
    if ( ((RB[rs] & 0x80000000) == (imm & 0x80000000)) &&
        ((imm & 0x80000000) != (RB[rt] & 0x80000000)) ) {
        fprintf(stderr, "EXCEPTION(addi): integer overflow.\n"); exit(EXIT_FAILURE);
        }
};

//!Instruction addiu behavior method.
void ac_behavior( addiu )
{
    pipeline->inject_instr(ADDIU, rs, -1, rt, 0, 0);
    dbg_printf("addiu r%d, r%d, %d\n", rt, rs, imm & 0xFFFF);
    RB[rt] = RB[rs] + imm;
    dbg_printf("Result = %#x\n", RB[rt]);
};

//!Instruction slti behavior method.
void ac_behavior( slti )
{
    pipeline->inject_instr(SLTI, rs, -1, rt, 0, 0);
    dbg_printf("slti r%d, r%d, %d\n", rt, rs, imm & 0xFFFF);
    // Set the RD if RS< IMM
    if( (ac_Sword) RB[rs] < (ac_Sword) imm )
        RB[rt] = 1;
    // Else reset RD
    else
        RB[rt] = 0;
    dbg_printf("Result = %#x\n", RB[rt]);
};

//!Instruction sltiu behavior method.
void ac_behavior( sltiu )
{
    pipeline->inject_instr(SLTIU, rs, -1, rt, 0, 0);
    dbg_printf("sltiu r%d, r%d, %d\n", rt, rs, imm & 0xFFFF);
    // Set the RD if RS< IMM
    if( (ac_Uword) RB[rs] < (ac_Uword) imm )
        RB[rt] = 1;
    // Else reset RD
    else
        RB[rt] = 0;
    dbg_printf("Result = %#x\n", RB[rt]);
};

//!Instruction andi behavior method.
void ac_behavior( andi )
{
    pipeline->inject_instr(ANDI, rs, -1, rt, 0, 0);	
    dbg_printf("andi r%d, r%d, %d\n", rt, rs, imm & 0xFFFF);
    RB[rt] = RB[rs] & (imm & 0xFFFF) ;
    dbg_printf("Result = %#x\n", RB[rt]);
};

//!Instruction ori behavior method.
void ac_behavior( ori )
{
    pipeline->inject_instr(ORI, rs, -1, rt, 0, 0);	
    dbg_printf("ori r%d, r%d, %d\n", rt, rs, imm & 0xFFFF);
    RB[rt] = RB[rs] | (imm & 0xFFFF) ;
    dbg_printf("Result = %#x\n", RB[rt]);
};

//!Instruction xori behavior method.
void ac_behavior( xori )
{
    pipeline->inject_instr(XORI, rs, -1, rt, 0, 0);	
    dbg_printf("xori r%d, r%d, %d\n", rt, rs, imm & 0xFFFF);
    RB[rt] = RB[rs] ^ (imm & 0xFFFF) ;
    dbg_printf("Result = %#x\n", RB[rt]);
};

//!Instruction lui behavior method.
void ac_behavior( lui )
{
    pipeline->inject_instr(LUI, rs, -1, rt, 0, 0);	
    dbg_printf("lui r%d, r%d, %d\n", rt, rs, imm & 0xFFFF);
    // Load a constant in the upper 16 bits of a register
    // To achieve the desired behaviour, the constant was shifted 16 bits left
    // and moved to the target register ( rt )
    RB[rt] = imm << 16;
    dbg_printf("Result = %#x\n", RB[rt]);
};

//!Instruction add behavior method.
void ac_behavior( add )
{
    pipeline->inject_instr(ADD, rs, rd, rt, 0, 0);
    dbg_printf("add r%d, r%d, r%d\n", rd, rs, rt);
    RB[rd] = RB[rs] + RB[rt];
    dbg_printf("Result = %#x\n", RB[rd]);
    //Test overflow
    if ( ((RB[rs] & 0x80000000) == (RB[rd] & 0x80000000)) &&
        ((RB[rd] & 0x80000000) != (RB[rt] & 0x80000000)) ) {
        fprintf(stderr, "EXCEPTION(add): integer overflow.\n"); exit(EXIT_FAILURE);
        }
};

//!Instruction addu behavior method.
void ac_behavior( addu )
{
    pipeline->inject_instr(ADDU, rs, rd, rt, 0, 0);
    dbg_printf("addu r%d, r%d, r%d\n", rd, rs, rt);
    RB[rd] = RB[rs] + RB[rt];
    //cout << "  RS: " << (unsigned int)RB[rs] << " RT: " << (unsigned int)RB[rt] << endl;
    //cout << "  Result =  " <<  (unsigned int)RB[rd] <<endl;
    dbg_printf("Result = %#x\n", RB[rd]);
};

//!Instruction sub behavior method.
void ac_behavior( sub )
{
    pipeline->inject_instr(SUB, rs, rd, rt, 0, 0);
    dbg_printf("sub r%d, r%d, r%d\n", rd, rs, rt);
    RB[rd] = RB[rs] - RB[rt];
    dbg_printf("Result = %#x\n", RB[rd]);
    //TODO: test integer overflow exception for sub
};

//!Instruction subu behavior method.
void ac_behavior( subu )
{
    pipeline->inject_instr(SUBU, rs, rd, rt, 0, 0);
    dbg_printf("subu r%d, r%d, r%d\n", rd, rs, rt);
    RB[rd] = RB[rs] - RB[rt];
    dbg_printf("Result = %#x\n", RB[rd]);
};

//!Instruction slt behavior method.
void ac_behavior( slt )
{
    pipeline->inject_instr(SLT, rs, rd, rt, 0, 0);	
    dbg_printf("slt r%d, r%d, r%d\n", rd, rs, rt);
    // Set the RD if RS< RT
    if( (ac_Sword) RB[rs] < (ac_Sword) RB[rt] )
        RB[rd] = 1;
    // Else reset RD
    else
        RB[rd] = 0;
    dbg_printf("Result = %#x\n", RB[rd]);
};

//!Instruction sltu behavior method.
void ac_behavior( sltu )
{
    pipeline->inject_instr(SLTU, rs, rd, rt, 0, 0);
    dbg_printf("sltu r%d, r%d, r%d\n", rd, rs, rt);
    // Set the RD if RS < RT
    if( RB[rs] < RB[rt] )
        RB[rd] = 1;
    // Else reset RD
    else
        RB[rd] = 0;
    dbg_printf("Result = %#x\n", RB[rd]);
};

//!Instruction instr_and behavior method.
void ac_behavior( instr_and )
{
    pipeline->inject_instr(INSTR_AND, rs, rd, rt, 0, 0);
    dbg_printf("instr_and r%d, r%d, r%d\n", rd, rs, rt);
    RB[rd] = RB[rs] & RB[rt];
    dbg_printf("Result = %#x\n", RB[rd]);
};

//!Instruction instr_or behavior method.
void ac_behavior( instr_or )
{
    pipeline->inject_instr(INSTR_OR, rs, rd, rt, 0, 0);
    dbg_printf("instr_or r%d, r%d, r%d\n", rd, rs, rt);
    RB[rd] = RB[rs] | RB[rt];
    dbg_printf("Result = %#x\n", RB[rd]);
};

//!Instruction instr_xor behavior method.
void ac_behavior( instr_xor )
{
    pipeline->inject_instr(INSTR_XOR, rs, rd, rt, 0, 0);
    dbg_printf("instr_xor r%d, r%d, r%d\n", rd, rs, rt);
    RB[rd] = RB[rs] ^ RB[rt];
    dbg_printf("Result = %#x\n", RB[rd]);
};

//!Instruction instr_nor behavior method.
void ac_behavior( instr_nor )
{
    pipeline->inject_instr(INSTR_NOR, rs, rd, rt, 0, 0);
    dbg_printf("nor r%d, r%d, r%d\n", rd, rs, rt);
    RB[rd] = ~(RB[rs] | RB[rt]);
    dbg_printf("Result = %#x\n", RB[rd]);
};

//!Instruction nop behavior method.
void ac_behavior( nop )
{
    pipeline->inject_instr(NOP, -1, -1, -1, 0, 0);  
    dbg_printf("nop\n");
};

//!Instruction sll behavior method.
void ac_behavior( sll )
{
    pipeline->inject_instr(SLL, rs, rt, rd, 0, 0);  
    dbg_printf("sll r%d, r%d, %d\n", rd, rs, shamt);
    RB[rd] = RB[rt] << shamt;
    dbg_printf("Result = %#x\n", RB[rd]);
};

//!Instruction srl behavior method.
void ac_behavior( srl )
{
    pipeline->inject_instr(SRL, rs, rt, rd, 0, 0);
    dbg_printf("srl r%d, r%d, %d\n", rd, rs, shamt);
    RB[rd] = RB[rt] >> shamt;
    dbg_printf("Result = %#x\n", RB[rd]);
};

//!Instruction sra behavior method.
void ac_behavior( sra )
{
    pipeline->inject_instr(SRA, rs, rt, rd, 0, 0);
    dbg_printf("sra r%d, r%d, %d\n", rd, rs, shamt);
    RB[rd] = (ac_Sword) RB[rt] >> shamt;
    dbg_printf("Result = %#x\n", RB[rd]);
};

//!Instruction sllv behavior method.
void ac_behavior( sllv )
{
    pipeline->inject_instr(SLLV, rs, rt, rd, 0, 0);
    dbg_printf("sllv r%d, r%d, r%d\n", rd, rt, rs);
    RB[rd] = RB[rt] << (RB[rs] & 0x1F);
    dbg_printf("Result = %#x\n", RB[rd]);
};

//!Instruction srlv behavior method.
void ac_behavior( srlv )
{
    pipeline->inject_instr(SRLV, rs, rt, rd, 0, 0);
    dbg_printf("srlv r%d, r%d, r%d\n", rd, rt, rs);
    RB[rd] = RB[rt] >> (RB[rs] & 0x1F);
    dbg_printf("Result = %#x\n", RB[rd]);
};

//!Instruction srav behavior method.
void ac_behavior( srav )
{
    pipeline->inject_instr(SRAV, rs, rt, rd, 0, 0);
    dbg_printf("srav r%d, r%d, r%d\n", rd, rt, rs);
    RB[rd] = (ac_Sword) RB[rt] >> (RB[rs] & 0x1F);
    dbg_printf("Result = %#x\n", RB[rd]);
};

//!Instruction mult behavior method.
void ac_behavior( mult )
{
    pipeline->inject_instr(MULT, rs, rt, LO, 0, 0);
    dbg_printf("mult r%d, r%d\n", rs, rt);
    
    long long result;
    int half_result;
    
    result = (ac_Sword) RB[rs];
    result *= (ac_Sword) RB[rt];
    
    half_result = (result & 0xFFFFFFFF);
    // Register LO receives 32 less significant bits
    lo = half_result;
    
    half_result = ((result >> 32) & 0xFFFFFFFF);
    // Register HI receives 32 most significant bits
    hi = half_result ;
    
    dbg_printf("Result = %#llx\n", result);
};

//!Instruction multu behavior method.
void ac_behavior( multu )
{
    pipeline->inject_instr(MULTU, rs, rt, LO, 0, 0);
    dbg_printf("multu r%d, r%d\n", rs, rt);
    
    unsigned long long result;
    unsigned int half_result;
    
    result  = RB[rs];
    result *= RB[rt];
    
    half_result = (result & 0xFFFFFFFF);
    // Register LO receives 32 less significant bits
    lo = half_result;
    
    half_result = ((result>>32) & 0xFFFFFFFF);
    // Register HI receives 32 most significant bits
    hi = half_result ;
    
    dbg_printf("Result = %#llx\n", result);
};

//!Instruction div behavior method.
void ac_behavior( div )
{
    pipeline->inject_instr(DIV, rs, rt, LOHI, 0, 0);
    dbg_printf("div r%d, r%d\n", rs, rt);
    // Register LO receives quotient
    lo = (ac_Sword) RB[rs] / (ac_Sword) RB[rt];
    // Register HI receives remainder
    hi = (ac_Sword) RB[rs] % (ac_Sword) RB[rt];
};

//!Instruction divu behavior method.
void ac_behavior( divu )
{
    pipeline->inject_instr(DIVU, rs, rt, LOHI, 0, 0);
    dbg_printf("divu r%d, r%d\n", rs, rt);
    // Register LO receives quotient
    lo = RB[rs] / RB[rt];
    // Register HI receives remainder
    hi = RB[rs] % RB[rt];
};

//!Instruction mfhi behavior method.
void ac_behavior( mfhi )
{
    pipeline->inject_instr(MFHI, HI, -1, rd, 0, 0);
    dbg_printf("mfhi r%d\n", rd);
    RB[rd] = hi;
    dbg_printf("Result = %#x\n", RB[rd]);
};

//!Instruction mthi behavior method.
void ac_behavior( mthi )
{
    pipeline->inject_instr(MTHI, rs, -1, HI, 0, 0);
    dbg_printf("mthi r%d\n", rs);
    hi = RB[rs];
    dbg_printf("Result = %#x\n", hi);
};

//!Instruction mflo behavior method.
void ac_behavior( mflo )
{
    pipeline->inject_instr(MFLO, LO, -1, rd, 0, 0);
    dbg_printf("mflo r%d\n", rd);
    RB[rd] = lo;
    dbg_printf("Result = %#x\n", RB[rd]);
};

//!Instruction mtlo behavior method.
void ac_behavior( mtlo )
{
    pipeline->inject_instr(MTLO, rs, -1, LO, 0, 0);
    dbg_printf("mtlo r%d\n", rs);
    lo = RB[rs];
    dbg_printf("Result = %#x\n", lo);
};

//!Instruction j behavior method.
void ac_behavior( j )
{
    pipeline->inject_instr(J, -1, -1, -1, 0, 0);
    dbg_printf("j %d\n", addr);
    addr = addr << 2;
    #ifndef NO_NEED_PC_UPDATE
    npc =  (ac_pc & 0xF0000000) | addr;
    #endif 
    dbg_printf("Target = %#x\n", (ac_pc & 0xF0000000) | addr );
};

//!Instruction jal behavior method.
void ac_behavior( jal )
{
    pipeline->inject_instr(JAL, -1, -1, Ra, 0, 0);
    dbg_printf("jal %d\n", addr);
    // Save the value of PC + 8 (return address) in $ra ($31) and
    // jump to the address given by PC(31...28)||(addr<<2)
    // It must also flush the instructions that were loaded into the pipeline
    RB[Ra] = ac_pc+4; //ac_pc is pc+4, we need pc+8
    
    addr = addr << 2;
    #ifndef NO_NEED_PC_UPDATE
    npc = (ac_pc & 0xF0000000) | addr;
    #endif 
    
    dbg_printf("Target = %#x\n", (ac_pc & 0xF0000000) | addr );
    dbg_printf("Return = %#x\n", ac_pc+4);
};

//!Instruction jr behavior method.
void ac_behavior( jr )
{
    pipeline->inject_instr(JR, rs, -1, -1, 0, 0);
    dbg_printf("jr r%d\n", rs);
    // Jump to the address stored on the register reg[RS]
    // It must also flush the instructions that were loaded into the pipeline
    #ifndef NO_NEED_PC_UPDATE
    npc = RB[rs], 1;
    #endif 
    dbg_printf("Target = %#x\n", RB[rs]);
};

//!Instruction jalr behavior method.
void ac_behavior( jalr )
{
    dbg_printf("jalr r%d, r%d\n", rd, rs);
    // Save the value of PC + 8(return address) in rd and
    // jump to the address given by [rs]
    
    #ifndef NO_NEED_PC_UPDATE
    npc = RB[rs], 1;
    #endif 
    dbg_printf("Target = %#x\n", RB[rs]);
    
    if( rd == 0 )  //If rd is not defined use default
        rd = Ra;
    RB[rd] = ac_pc+4;
    dbg_printf("Return = %#x\n", ac_pc+4);
    pipeline->inject_instr(JALR, rs, -1, rd, 0, 0);
};

//!Instruction beq behavior method.
void ac_behavior( beq )
{
    dbg_printf("beq r%d, r%d, %d\n", rt, rs, imm & 0xFFFF);
    if( RB[rs] == RB[rt] ){
        #ifndef NO_NEED_PC_UPDATE
        npc = ac_pc + (imm<<2);
        #endif 
        dbg_printf("Taken to %#x\n", ac_pc + (imm<<2));
	pipeline->inject_instr(BEQ, rs, rt, -1, 0, 0, true, (unsigned char)(ac_pc + (imm<<2)));
    }
    else{
        pipeline->inject_instr(BEQ, rs, rt, -1, 0, 0, false, (unsigned char)(ac_pc + (imm<<2)));
    };
};

//!Instruction bne behavior method.
void ac_behavior( bne )
{	
    dbg_printf("bne r%d, r%d, %d\n", rt, rs, imm & 0xFFFF);
    if( RB[rs] != RB[rt] ){
        #ifndef NO_NEED_PC_UPDATE
        npc = ac_pc + (imm<<2);
        #endif 
        dbg_printf("Taken to %#x\n", ac_pc + (imm<<2));
	pipeline->inject_instr(BNE, rs, rt, -1, 0, 0, true, (unsigned char)(ac_pc + (imm<<2)));
    }
    else{
	pipeline->inject_instr(BNE, rs, rt, -1, 0, 0, false, (unsigned char)(ac_pc + (imm<<2)));
    }
};

//!Instruction blez behavior method.
void ac_behavior( blez )
{
    dbg_printf("blez r%d, %d\n", rs, imm & 0xFFFF);
    if( (RB[rs] == 0 ) || (RB[rs]&0x80000000 ) ){
        #ifndef NO_NEED_PC_UPDATE
        npc = ac_pc + (imm<<2), 1;
        #endif 
        dbg_printf("Taken to %#x\n", ac_pc + (imm<<2));
	pipeline->inject_instr(BLEZ, rs, rt, -1, 0, 0, true, (unsigned char)(ac_pc + (imm<<2)));
    }
    else{
        pipeline->inject_instr(BLEZ, rs, rt, -1, 0, 0, false, (unsigned char)(ac_pc + (imm<<2)));
    }
};

//!Instruction bgtz behavior method.
void ac_behavior( bgtz )
{
    dbg_printf("bgtz r%d, %d\n", rs, imm & 0xFFFF);
    if( !(RB[rs] & 0x80000000) && (RB[rs]!=0) ){
        #ifndef NO_NEED_PC_UPDATE
        npc = ac_pc + (imm<<2);
        #endif 
        dbg_printf("Taken to %#x\n", ac_pc + (imm<<2));
	pipeline->inject_instr(BGTZ, rs, rt, -1, 0, 0, true, (unsigned char)(ac_pc + (imm<<2)));
    }
    else{
        pipeline->inject_instr(BGTZ, rs, rt, -1, 0, 0, false, (unsigned char)(ac_pc + (imm<<2)));
    }
};

//!Instruction bltz behavior method.
void ac_behavior( bltz )
{
    dbg_printf("bltz r%d, %d\n", rs, imm & 0xFFFF);
    if( RB[rs] & 0x80000000 ){
        #ifndef NO_NEED_PC_UPDATE
        npc = ac_pc + (imm<<2);
        #endif 
        dbg_printf("Taken to %#x\n", ac_pc + (imm<<2));
	pipeline->inject_instr(BLTZ, rs, rt, -1, 0, 0, true, (unsigned char)(ac_pc + (imm<<2)));
    }
    else{
        pipeline->inject_instr(BLTZ, rs, rt, -1, 0, 0, false, (unsigned char)(ac_pc + (imm<<2)));
    }
};

//!Instruction bgez behavior method.
void ac_behavior( bgez )
{
    dbg_printf("bgez r%d, %d\n", rs, imm & 0xFFFF);
    if( !(RB[rs] & 0x80000000) ){
        #ifndef NO_NEED_PC_UPDATE
        npc = ac_pc + (imm<<2);
        #endif 
        dbg_printf("Taken to %#x\n", ac_pc + (imm<<2));
	pipeline->inject_instr(BGEZ, rs, rt, -1, 0, 0, true, (unsigned char)(ac_pc + (imm<<2)));
    }
    else{
        pipeline->inject_instr(BGEZ, rs, rt, -1, 0, 0, false, (unsigned char)(ac_pc + (imm<<2)));
    }
};

//!Instruction bltzal behavior method.
void ac_behavior( bltzal )
{
    dbg_printf("bltzal r%d, %d\n", rs, imm & 0xFFFF);
    RB[Ra] = ac_pc+4; //ac_pc is pc+4, we need pc+8
    if( RB[rs] & 0x80000000 ){
        #ifndef NO_NEED_PC_UPDATE
        npc = ac_pc + (imm<<2);
        #endif 
        dbg_printf("Taken to %#x\n", ac_pc + (imm<<2));
	pipeline->inject_instr(BLTZAL, rs, rt, -1, 0, 0, true, (unsigned char)(ac_pc + (imm<<2)));
    }
    else{
        pipeline->inject_instr(BLTZAL, rs, rt, -1, 0, 0, false, (unsigned char)(ac_pc + (imm<<2)));
    }
    dbg_printf("Return = %#x\n", ac_pc+4);
    
};

//!Instruction bgezal behavior method.
void ac_behavior( bgezal )
{
    dbg_printf("bgezal r%d, %d\n", rs, imm & 0xFFFF);
    RB[Ra] = ac_pc+4; //ac_pc is pc+4, we need pc+8
    if( !(RB[rs] & 0x80000000) ){
        #ifndef NO_NEED_PC_UPDATE
        npc = ac_pc + (imm<<2);
        #endif 
        dbg_printf("Taken to %#x\n", ac_pc + (imm<<2));
	pipeline->inject_instr(BGEZAL, rs, rt, -1, 0, 0, true, (unsigned char)(ac_pc + (imm<<2)));
    }
    else{
        pipeline->inject_instr(BGEZAL, rs, rt, -1, 0, 0, false, (unsigned char)(ac_pc + (imm<<2)));
    }
    dbg_printf("Return = %#x\n", ac_pc+4);
};

//!Instruction sys_call behavior method.
void ac_behavior( sys_call )
{
    dbg_printf("syscall\n");
    stop();
}

//!Instruction instr_break behavior method.
void ac_behavior( instr_break )
{
    fprintf(stderr, "instr_break behavior not implemented.\n"); 
    exit(EXIT_FAILURE);
}
