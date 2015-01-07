/*
Armator - simulateur de jeu d'instruction ARMv5T � but p�dagogique
Copyright (C) 2011 Guillaume Huard
Ce programme est libre, vous pouvez le redistribuer et/ou le modifier selon les
termes de la Licence Publique G�n�rale GNU publi�e par la Free Software
Foundation (version 2 ou bien toute autre version ult�rieure choisie par vous).

Ce programme est distribu� car potentiellement utile, mais SANS AUCUNE
GARANTIE, ni explicite ni implicite, y compris les garanties de
commercialisation ou d'adaptation dans un but sp�cifique. Reportez-vous � la
Licence Publique G�n�rale GNU pour plus de d�tails.

Vous devez avoir re�u une copie de la Licence Publique G�n�rale GNU en m�me
temps que ce programme ; si ce n'est pas le cas, �crivez � la Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307,
�tats-Unis.

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
	uint8_t field = instruction_get_cond_field(inst);
    return instruction_check_cond_field(p, field);
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
	    debug("error during fetch %d\n", result);
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

