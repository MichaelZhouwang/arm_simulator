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

#include <debug.h>
#include "arm_instruction.h"
#include "arm_exception.h"
#include "arm_data_processing.h"
#include "arm_load_store.h"
#include "arm_branch_other.h"
#include "arm_constants.h"
#include "util.h"
#include "arm_core.h"

///////////////////////////////////////////////////////////////////////////////
// Execution of instructions
///////////////////////////////////////////////////////////////////////////////

static int arm_execute_instruction(arm_core p) {
    debug("\n################## fetch ##################\n");
	
    uint32_t ins;
    int result = arm_fetch(p, &ins);
    if (result != NO_EXCEPTION) {
	    debug("error during fetch %d\n", result);
        return result;
    } else {   
	    debug("instruction %x\n", ins);
    }

    result = instruction_check_condition(p, ins);
    if (result == 0) {
    	debug("Cond false \n");
    	result = NO_EXCEPTION;
    } else if (result == 1) {
		debug("Cond true \n");
    	instruction_handler_t handler = instruction_get_handler(ins);
		result = (handler) ? handler(p, ins) : UNDEFINED_INSTRUCTION;
	} else if (result == -1) {
		debug("Cond undefined \n");
		result = arm_miscellaneous(p, ins);
	}
    
    return result;
}

int arm_step(arm_core p) {
    int result;
    result = arm_execute_instruction(p);
    if (result != NO_EXCEPTION) {
        arm_exception(p, result);
    }
    return result;
}

///////////////////////////////////////////////////////////////////////////////
// Condition field
///////////////////////////////////////////////////////////////////////////////

int instruction_check_condition(arm_core p, uint32_t ins) {
    uint32_t cpsr = arm_read_cpsr(p);
    int n = get_bit(cpsr, N);
    int z = get_bit(cpsr, Z);
    int c = get_bit(cpsr, C);
    int v = get_bit(cpsr, V);
    int cond_field = get_bits(ins, 31, 28);
    int result;
	
    switch (cond_field) {
        case 0 : result = (z == 1);           break;
        case 1 : result = (z == 0);           break;
        case 2 : result = (c == 1);           break;
        case 3 : result = (c == 0);           break;
        case 4 : result = (n == 1);           break;
        case 5 : result = (n == 0);           break;
        case 6 : result = (v == 1);           break;
        case 7 : result = (v == 0);           break;
        case 8 : result = (c == 1 && z == 0); break;
        case 9 : result = (c == 0 || z == 1); break;
        case 10: result = (n == v);           break;
        case 11: result = (n != v);           break;
        case 12: result = (z == 0 && n == v); break;
        case 13: result = (z == 1 || n != v); break;
        case 14: result =  1; break; // always
        case 15: result = -1; break; // undefined
        default: result =  0; break; // impossible
    }
    debug("condition : %x, %d\n", cond_field, result);
    return result;
}

///////////////////////////////////////////////////////////////////////////////
// Instruction handlers
///////////////////////////////////////////////////////////////////////////////

typedef int(* instruction_handler_t)(arm_core, uint32_t);

instruction_handler_t instruction_get_handler(uint32_t ins) {
    instruction_handler_t handler = NULL;
    switch(get_bits(ins, 27, 25)) {
        case 0:
            if (check_mask(ins, 0x0E000000, 0x00000090))
                handler = arm_load_store_miscellaneous;
            else if (check_mask(ins, 0x0E900FF0, 0x0120F000))
                handler =  arm_msr;
            else if (check_mask(ins, 0x0EB00FFF, 0x010F0000))
                handler = arm_mrs;
            else
                handler = arm_data_processing_shift;
            break;
        case 1:
            if (check_mask(ins, 0x0C900000,0x0320F000))
                handler = arm_msr;
            else
                handler = arm_data_processing_immediate;
            break;
        case 2: handler = arm_load_store;                    break;
        case 3: handler = arm_load_store;                    break;
        case 4: handler = arm_load_store_multiple;           break;
        case 5: handler = arm_branch;                        break;
        case 6: handler = arm_coprocessor_load_store;        break;
        case 7: handler = arm_coprocessor_others_swi;        break;
        default: break;
    }
    return handler;
}

