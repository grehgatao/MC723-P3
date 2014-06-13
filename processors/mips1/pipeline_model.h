#include <stdio.h>
#include <vector>
#define HI 900
#define LO 99
#define LOHI 999

using namespace std;

#ifndef PIPELINE_H
#define PIPELINE_H
enum Operations{
    LB,
    LBU,
    LH,
    LHU,
    LW,
    LWL,
    LWR,
    SB,
    SH,
    SW,
    SWL,
    SWR,
    ADDI,
    ADDIU,
    SLTI,
    SLTIU,
    ANDI,
    ORI,
    XORI,
    LUI,
    ADD,
    ADDU,
    SUB,
    SUBU,
    SLT,
    SLTU,
    INSTR_AND,
    INSTR_OR,
    INSTR_XOR,
    INSTR_NOR,
    NOP,
    SLL,
    SRL,
    SRA,
    SLLV,
    SRLV,
    SRAV,
    MULT,
    MULTU,
    DIV,
    DIVU,
    MFHI,
    MTHI,
    MFLO,
    MTLO,
    J,
    JAL,
    JR,
    JALR,
    BEQ,
    BNE,
    BLEZ,
    BGTZ,
    BLTZ,
    BGEZ,
    BLTZAL,
    BGEZAL,
    SYSCALL,
    INSTR_BREAK
};

typedef struct instr{
    int op;
    int reg_read1;
    int reg_read2;
    int reg_write;
    unsigned int mem_read;
    unsigned int mem_write;
    bool branch_taken;
    unsigned char branch;
}TInstr;

class PipelineModel{
public:
    PipelineModel(int nstates, bool fw, int branchp);
    TInstr nop_instr; // Instrução NOP
    
    int get_num_states();
    void inject_instr(TInstr instr);
    void inject_instr(int op, int reg_read1, int reg_read2, int reg_write, int mem_read, int mem_write, bool branch_taken=false, char branch=0);
    void inject_branch_taken(bool taken);
    TInstr access_instr(int index);
    long long int ic;
    long long int data_hazard_ic;
    long long int control_hazard_ic;
    long long int branch_predictions;
    long long int branch_mispredictions;
    
private:
    int num_states; // Número de estágios do pipeline
    vector<TInstr> *states; // Fila de estágios do pipeline

    int if_stage; // Identifica as posições de cada estágio
    int id_stage;
    int mem_stage;
    int branch_stage;
    int ex_stage;
    int wb_stage;

    bool hazard; // Flag que habilita verificação de hazard
    bool forward; // Flag que habilita opção de forwarding
    int branch_prediction; //0 - Branch Prediciton Desabilitado;
                 //1 - Saturating Counter com tabela de 8x2bit s/distinção de instruções
                 //2 - Saturating Counter com tabela de 8x2bit c/distinção de instruções
    
    char branch_counters[256]; // Vetor de saturating counters para as posições de branch
};
#endif  //_PIPELINE_H