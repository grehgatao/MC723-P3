#include "pipeline_model.h"

PipelineModel::PipelineModel(int nstates, bool fw, int branchp) {
    nop_instr.op = NOP; // Constrói instrução NOP
    nop_instr.reg_read1 = -1;
    nop_instr.reg_read2 = -1;
    nop_instr.reg_write = -1;
    nop_instr.mem_read = 0;
    nop_instr.mem_write = 0;

    num_states = nstates;
    forward = fw;
    branch_prediction = branchp;

    if(num_states == 5) {
        if_stage = 0;
        id_stage = 1;
        ex_stage = 2;
        branch_stage = 2;
        mem_stage = 3;
        wb_stage = 4;
    }
    else if (num_states == 7) {
        //Estagio 0: Tradução de posição de memória p/PC
        if_stage = 1;
        id_stage = 2;
        ex_stage = 3;
        branch_stage = 3;
        //Estagio 4: Tradução de posição de memória p/read-write
        mem_stage = 5;
        wb_stage = 6;
    }
    else if (num_states == 13) {
        //Baseado na implementação do ARM Cortex A8
        if_stage = 1; // 2 estágios de Instruction fetch
        id_stage = 6; // 5 estágios de Instruction Decode
        ex_stage = 9;
        branch_stage = 12;
        mem_stage = 12;
        wb_stage = 12; // execute/mem/wb são paralelos, e hazards
        // de dados são minimizados em várias
        // interconexões entre os mesmos
    }
    else {
        fprintf(stderr,"ERRO: Número inválido de estágios da pipeline, use 5, 7 ou 13\n");
        throw -100;
    }

    ic = 0;
    data_hazard_ic = 0;
    control_hazard_ic = 0;
    branch_predictions = 0;
    branch_mispredictions = 0;
    for(int i=0; i<256; i++) {
        branch_counters[i]=0;
    }

    // Constrói fila de instruções preenchida com NOP
    states = new vector<TInstr>(num_states, nop_instr);
}

int PipelineModel::get_num_states() {
    return this->num_states;
}

void PipelineModel::inject_instr(TInstr instr) {

    // Adiciona a instrução na pipeline
    states->pop_back();
    states->insert(this->states->begin(), instr);
    ic++;

    //Verificação de casos
    int reg_read1, reg_read2, reg_write, reg_load;

    // Entra em verificação de hazards
    bool hazard = true;
    while(hazard) {
        hazard = false;
        // Control Hazard/Branch Prediction
        if (instr.op == BEQ ||
                instr.op == BNE ||
                instr.op == BLEZ ||
                instr.op == BGTZ ||
                instr.op == BLTZ ||
                instr.op == BGEZ ||
                instr.op == BLTZAL ||
                instr.op == BGEZAL
           ) {
            // Sem branch predicition
            if(branch_prediction == 0) {
                // Sempre considera que o branch não foi tomado
                // Caso contrário, dispara hazard
                if (instr.branch_taken) {
                    branch_mispredictions++;
                    for(int i=0; i<=branch_stage; i++) {
                        ic++;
                        control_hazard_ic++;
                    }
                }
                else {
                    branch_predictions++;
                }
            }
            // Com branch predicition saturating counter
            else {
                unsigned char table_index;
                // Cálculo de índice sem distinção de instruções
                if (branch_prediction == 1)
                    table_index = instr.branch;
                // Cálculo de índice com distinção de instruções
                else if (branch_prediction == 2) {
                    //Identificando 8 diferentes instr
                    table_index = instr.branch << 3;
                    switch (instr.op) {
                    case BEQ:
                        table_index += 0;
                        break;
                    case BNE:
                        table_index += 1;
                        break;
                    case BLEZ:
                        table_index += 2;
                        break;
                    case BGTZ:
                        table_index += 3;
                        break;
                    case BLTZ:
                        table_index += 4;
                        break;
                    case BGEZ:
                        table_index += 5;
                        break;
                    case BLTZAL:
                        table_index += 6;
                        break;
                    case BGEZAL:
                        table_index += 7;
                        break;
                    default:
                        fprintf(stderr, "OP INVALIDO!!!!\n");
                        throw -150;
                    }
                }
                // Joga erro caso branch_prediction != 0, 1 ou 2
                else {
                    fprintf(stderr, "ERRO: Definição inválida para branch prediction - Use 0, 1 ou 2\n");
                    throw -200;
                }
                if (instr.branch_taken) {
                    if (branch_counters[table_index] < 2) {
                        branch_mispredictions++;
                        for(int i=0; i<=ex_stage; i++) {
                            ic++;
                            control_hazard_ic++;
                        }
                    }
                    else {
                      branch_predictions++;
                    }
                    if (branch_counters[table_index] < 4)
                        branch_counters[table_index]++;
                }
                else {
                    if (branch_counters[table_index] >= 2) {
                        branch_mispredictions++;
                        for(int i=0; i<=ex_stage; i++) {
                            ic++;
                            control_hazard_ic++;
                        }
                    }
                    else {
                        branch_predictions++;
                    }
                    if (branch_counters[table_index] > 0)
                        branch_counters[table_index]--;
                }
            }
        }

        //Data hazard sem forwarding
        if(!forward) {
            for(int i=id_stage+1; i<=wb_stage; i++) {
                reg_read1 = states->at(id_stage).reg_read1;
                reg_read2 = states->at(id_stage).reg_read2;
                reg_write = states->at(i).reg_write;
                if ((reg_read1 == reg_write ||
                        reg_read2 == reg_write) && reg_write != -1) {
                    //Read B4 Write
                    hazard = true;
                    states->pop_back();
                    states->insert(this->states->begin()+id_stage+1, nop_instr);
                    ic++;
                    data_hazard_ic++;
                    break;
                }
            }
        }

        //Data hazard com forwarding em caso de load-use
        else if (states->at(ex_stage).op == LB ||
                 states->at(ex_stage).op == LBU ||
                 states->at(ex_stage).op == LH ||
                 states->at(ex_stage).op == LHU ||
                 states->at(ex_stage).op == LW ||
                 states->at(ex_stage).op == LWL ||
                 states->at(ex_stage).op == LWR) {
            reg_write = states->at(mem_stage).reg_write;
            if ((instr.reg_read1 == reg_write && instr.reg_read1 != -1) ||
                    (instr.reg_read2 == reg_write && instr.reg_read2 != -1)) {
                //Adiciona número de NOPs equivalentes à distância entre a saída
                //da leitura de memória e a operação na ALU
                //5 estágios: 1NOP; 7 estágios: 2NOP
                //13 estágios, por simplicidade, 3NOP
                for(int i=0; i<mem_stage-ex_stage; i++) {
                    states->pop_back();
                    states->insert(this->states->begin()+ex_stage+1, nop_instr);
                    ic++;
                    data_hazard_ic++;
                }
            }
        }
    }
}

void PipelineModel::inject_instr(int op, int reg_read1, int reg_read2, int reg_write,
                                 int mem_read, int mem_write, bool branch_taken, char branch) {
    TInstr instr;
    instr.op = op;
    instr.reg_read1 = reg_read1;
    instr.reg_read2 = reg_read2;
    instr.reg_write = reg_write;
    instr.mem_read = mem_read;
    instr.mem_write = mem_write;
    instr.branch_taken = branch_taken;
    instr.branch = branch;
    inject_instr(instr);
}

TInstr PipelineModel::access_instr(int index) {
    return states->at(index);
}
