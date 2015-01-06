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

// Fields parsing

inline uint8_t instruction_get_condition_field(uint32_t instruction) {
    return (uint8_t)(instruction>>28);
}

inline uint8_t instruction_get_handler_field(uint32_t instruction) {
    return (uint8_t)((instruction>>25) & 7);
}

int instruction_check_condition(arm_core p, uint8_t field) {
    int res;
    switch(field) {
        case 0:  res = is_z_set(p);                                     break;
        case 1:  res = is_z_clear(p);                                   break;
        case 2:  res = is_c_set(p);                                     break;
        case 3:  res = is_c_clear(p);                                   break;
        case 4:  res = is_n_set(p);                                     break;
        case 5:  res = is_n_clear(p);                                   break;
        case 6:  res = is_v_set(p);                                     break;
        case 7:  res = is_v_clear(p);                                   break;
        case 8:  res = is_c_set(p) && is_z_clear(p);                    break;
        case 9:  res = is_c_clear(p) && is_z_set(p);                    break;
        case 10: res = arm_read_n(p) == arm_read_v(p);                  break;
        case 11: res = arm_read_n(p) != arm_read_v(p);                  break;
        case 12: res = is_z_set(p) && arm_read_n(p) == arm_read_v(p);   break;
        case 13: res = is_z_clear(p) && arm_read_n(p) != arm_read_v(p); break;
        case 14: res =  1; break; // always
        case 15: res = -1; break; // undefined
        default: res =  0; break; // impossible
    }
    return res;
}

typedef int(* instruction_handler_t)(arm_core, uint32_t);

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
    result = arm_fetch(p, &instruction);
    if (result) {
        return result;
    }
    
    // We check the condition
    cond_field = instruction_get_condition_field(instruction);
    result = instruction_check_condition(p, field);
    if (result == 1) {
        // The condition was checked, the instruction will be handled
        ins_class_field = instruction_get_handler_field(instruction);
        handler = instruction_field_get_handler(field);
    } else if (result == -1) {
        // The condition is undefined
        handler = arm_miscellaneous;
    }
    
    return (handler) ? handler(p, instruction) : -1;
}

int arm_step(arm_core p) {
    int result;

    result = arm_execute_instruction(p);
    if (result) {
        arm_exception(p, result);
    }
    
    return result;
}

