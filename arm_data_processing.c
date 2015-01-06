/*
Armator - simulateur de jeu d'instruction ARMv5T à but pédagogique
Copyright (C) 2011 Guillaume Huard
Ce programme est libre, vous pouvez le redistribuer et/ou le modifier selon les
termes de la Licence Publique Générale GNU publiée par la Free Software
Foundation (version 2 ou bien toute autre version ultérieure choisie par vous).

Ce programme est distribué car potentiellement utile, mais SANS AUCUNE
GARANTIE, ni explicite ni implicite, y compris les garanties de
commercialisation ou d'adaptation dans un but spécifique. Reportez-vous à la
Licence Publique Générale GNU pour plus de détails.

Vous devez avoir reçu une copie de la Licence Publique Générale GNU en même
temps que ce programme ; si ce n'est pas le cas, écrivez à la Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307,
États-Unis.

Contact: Guillaume.Huard@imag.fr
         ENSIMAG - Laboratoire LIG
         51 avenue Jean Kuntzmann
         38330 Montbonnot Saint-Martin
*/

#include "arm_data_processing.h"
#include "arm_exception.h"
#include "arm_constants.h"
#include "arm_branch_other.h"
#include "util.h"
#include "debug.h"

typedef enum {AND, EOR, SUB, RSB, ADD, ADC, SBC, RSC, TST, TEQ, CMP, CMN, ORR, MOV, BIC, MVN} op_code_t;
typedef int(* dp_instruction_handler_t)(arm_core, uint8_t, uint32_t, uint32_t, uint8_t);

uint8_t rd, rn, rm, S, rs, shift_imm, shift_code, bit4, bit7;
int op1, op2;

// Data processing instruction parsing
int get_op_code(uint32_t ins) {
	return (ins >> 21) & 15;
}
uint8_t get_rd(uint32_t ins) {
	return (ins >> 12) & 15;
}
uint8_t get_rn(uint32_t ins) {
	return (ins >> 16) & 15;
}
uint8_t get_rm(uint32_t ins) {
	return ins & 15;
}
uint8_t get_S(uint32_t ins) {
	return (ins >> 20) & 1;
}
uint8_t get_rs(uint32_t ins) {
	return (ins >> 8) & 15;
}
uint8_t get_shift_imm(uint32_t ins) {
	return (ins >> 7) & 31;
}
int get_shift_code(uint32_t ins) {
	return (ins >> 5) & 3;
}
uint8_t get_bit4(uint32_t ins) {
	return (ins >> 4) & 1;
}
uint8_t get_bit7(uint32_t ins) {
	return (ins >> 7) & 1;
}
// Immediate operand value
int get_immediate(uint32_t ins) {
	int imm_8 = ins & 255;
	int rotate_imm = (ins >> 8) & 15;
	return imm_8 >> (rotate_imm * 2) ;
}
// Shifted register operand value
void shift(int* op, int code, int value) {
	switch(code){
		case 0: // LSL
			*op <<= value;
			break;
		case 1: // LSR
			*op >>= value;
			break;
		case 2: // ASR
			
			break;
		case 3: // ROR
			break;
	}	
}



// Decoding
dp_instruction_handler_t decode(op_code_t op_code) {
	return and; // à compléter
}

// Decoding functions for different classes of instructions
int arm_data_processing_shift(arm_core p, uint32_t ins) {
    debug("arm_data_processing_shift: %d\n", (int)ins);
    
    op_code_t op_code = get_op_code(ins);
    rd = get_rd(ins);
    if(op_code != MOV && op_code != MVN) {
    	rn = get_rn(ins);
    	op1 = arm_read_register(p, rn);
    }
    // calcul de op2
    rm = get_rm(ins);
    shift_imm = get_shift_imm(ins);
    op2 = arm_read_register(p, rm);
    if(shift_imm) {
		  bit4 = get_bit4(ins);
		  shift_code = get_shift_code(ins);
    	int shift_value;
    	if(!bit4) shift_value = shift_imm;
    	else {
				rs = get_rs(ins);
				shift_value = arm_read_register(p, rs);
		  }
		  shift(&op2, shift_code, shift_value);
		}
    
    if(op_code == CMP || op_code == CMN  || op_code == TST || op_code == TEQ) {
    	S = 1;
    }
    else {
    	S = get_S(ins);
    }
    
    dp_instruction_handler_t handler = decode(op_code);
    handler(p, rd, op1, op2, S);
    return UNDEFINED_INSTRUCTION;
}


int arm_data_processing_immediate(arm_core p, uint32_t ins) {
    debug("arm_data_processing_immediate: %d\n", (int)ins);
    return UNDEFINED_INSTRUCTION;
}

