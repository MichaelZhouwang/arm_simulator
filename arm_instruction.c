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

#include "arm_instruction.h"
#include "arm_exception.h"
#include "arm_data_processing.h"
#include "arm_load_store.h"
#include "arm_branch_other.h"
#include "arm_constants.h"
#include "util.h"
#include <debug.h>
#include "arm_core.h"


// Immediate operand value
static inline uint32_t get_immediate(uint32_t ins, uint8_t* shift_C) {
	uint32_t imm_8 = ins & 255;
	uint8_t rotate_imm = (ins >> 8) & 15;
	uint32_t result = ror(imm_8,(rotate_imm * 2));
	if(rotate_imm == 0) *shift_C = arm_read_c(p);
	else *shift_C = get_bit(result,31);
	return result;
}
// Shifted register operand value
static inline uint32_t get_shifted(uint32_t ins, uint8_t* shift_C) {
		uint8_t rm = get_rm(ins);
    uint8_t shift_imm = get_shift_imm(ins);
		uint8_t shift_code = get_shift_code(ins);
    uint32_t result = arm_read_register(p, rm);
    if(shift_imm || shift_code) {
		  uint8_t bit4 = get_bit(ins,4);
    	uint8_t shift_value;
    	if(!bit4) shift_value = shift_imm;
    	else {
				uint8_t rs = get_rs(ins);
				shift_value = arm_read_register(p, rs);
		  }
		  result = shift(p, result, shift_code, shift_value);
		}
		return result;
}


// Condition field

inline uint8_t instruction_get_cond_field(uint32_t instruction) {
    return (uint8_t)(instruction>>28);
}

int instruction_check_cond_field(arm_core p, uint8_t field) {
    uint32_t cpsr = arm_read_cpsr(p);
	int n = get_bit(cpsr, N);
	int z = get_bit(cpsr, Z);
	int c = get_bit(cpsr, C);
	int v = get_bit(cpsr, V);
	int res;

	switch (field) {
		case 0 : res = (z == 1);           break;
		case 1 : res = (z == 0);           break;
		case 2 : res = (c == 1);           break;
		case 3 : res = (c == 0);           break;
		case 4 : res = (n == 1);           break;
		case 5 : res = (n == 0); 		   break;
		case 6 : res = (v == 1);           break;
		case 7 : res = (v == 0);           break;
		case 8 : res = (c == 1 && z == 0); break;
		case 9 : res = (c == 0 && z == 1); break;
		case 10: res = (n == v);           break;
		case 11: res = (n != v);           break;
		case 12: res = (z == 0 && n == v); break;
		case 13: res = (z == 1 || n != v); break;
        case 14: res =  1; break; // always
        case 15: res = -1; break; // undefined
        default: res =  0; break; // impossible
	}
	debug("condition : %x, %d\n", field, res);
    return res;
}

int instruction_check_condition(arm_core p, uint32_t inst) {
   uint8_t field = instruction_get_condition_field(inst);
    return instruction_check_cond_field(field);
}

// Instruction handlers

typedef int(* instruction_handler_t)(arm_core, uint32_t);

inline uint8_t instruction_get_handler_field(uint32_t instruction) {
    return (uint8_t)((instruction>>25) & 7);
}

instruction_handler_t instruction_field_get_handler(uint8_t field) {
    instruction_handler_t handler;
    switch(field) {
        case 0: handler = arm_data_processing_shift;         break;
        case 1: handler = arm_data_processing_immediate;     break;
        case 2: handler = arm_load_store_immediate;          break;
        case 3: handler = arm_load_store;                    break;
        case 4: handler = arm_load_store_multiple;           break;
        case 5: handler = arm_branch;                        break;
        case 6: handler = arm_coprocessor_load_store;        break;
        case 7: handler = arm_coprocessor_others_swi;        break;
        default: handler = NULL; break;
    }
    return handler;
}

// Execution of instructions

static int arm_execute_instruction(arm_core p) {
    int result;
    uint32_t instruction;
    uint8_t cond_field;
	uint8_t ins_class_field;
    instruction_handler_t handler = NULL;
    
    // We fetch the instruction
	debug("fetch\n");
    result = arm_fetch(p, &instruction);
    if (result) {
	    debug("error during fetch %x\n");
        return result;
    }

	debug("instruction %x\n", instruction);
    cond_field = instruction_get_cond_field(instruction);
    result = instruction_check_condition(p, cond_field);
	
    if (cond_field != 0x0f) {
        ins_class_field = instruction_get_handler_field(instruction);
		debug("handler : %x\n", ins_class_field);
        handler = instruction_field_get_handler(ins_class_field);
    } else {
		debug("The condition is undefined\n");
        handler = arm_miscellaneous;
    }
    
    return (handler != NULL) ? handler(p, instruction) : -1;
}

int arm_step(arm_core p) {
    int result;
    result = arm_execute_instruction(p);
    if (result) {
        arm_exception(p, result);
    }
    return result;
}

