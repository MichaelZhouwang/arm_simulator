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

#include "arm_branch_other.h"
#include "arm_constants.h"
#include "util.h"
#include "arm_instruction.h"
#include <debug.h>
#include <stdlib.h>

///////////////////////////////////////////////////////////////////////////////
// Available instructions
///////////////////////////////////////////////////////////////////////////////

int arm_branch(arm_core p, uint32_t ins) {
    debug("arm_branch\n");

    uint32_t val_pc = arm_read_register(p, PC);
    if (get_bit(ins, 24)) {
        arm_write_register(p, LR, val_pc-4);
    }     

    uint32_t signed_immed = get_bits(ins, 23, 0);
    uint32_t address = val_pc;
    address += sign_extend_24_to_30(signed_immed) << 2;
    debug("Adresse: %x\n", address);
    arm_write_register(p, PC, address);
	
    return 0;
}

int arm_coprocessor_others_swi(arm_core p, uint32_t ins) {
    debug("arm_coprocessor_other_swi\n");

    if (get_bit(ins, 24)) {
        // Here we implement the end of the simulation as swi 0x123456
        if ((ins & 0xFFFFFF) == 0x123456) {
            exit(0);
        }
        return SOFTWARE_INTERRUPT;
    }
    
    return UNDEFINED_INSTRUCTION;
}

int arm_miscellaneous(arm_core p, uint32_t ins) {
    debug("arm_miscellaneous: \n");
    return UNDEFINED_INSTRUCTION;
}

int arm_mrs(arm_core p, uint32_t ins) {
	debug("MRS ");
    uint8_t rd = get_bits(ins, 15, 12);
	debug_raw("rd:r%d ", rd);

    if (rd == 15)
        UNPREDICTABLE();
    /*if (get_bits(ins, 19, 16) != 0xF)
        SHOULD_BE_ONE();
    if (get_bits(ins, 11, 0) != 0x0)
        SHOULD_BE_ZERO();
	*/

    int32_t value;
    if (get_bit(ins, 22)){
        value = arm_read_spsr(p);
		debug_raw("SPSR:%x\n", value);
    } else {
        value = arm_read_cpsr(p);
		debug_raw("CPSR:%x\n", value);
	}

    arm_write_register(p, rd, value);
    return 0;
}

int arm_msr(arm_core p, uint32_t ins) {
    debug("MSR ");

    uint32_t operand;
    if (get_bit(ins, 25)) {
        int8_t immediate = get_bits(ins, 7, 0);
        int8_t rotate_imm = get_bits(ins, 11, 8);
        operand = ror(immediate, (rotate_imm * 2));
    } else {
        uint8_t rm = get_bits(ins, 3, 0);
        operand = arm_read_register(p, rm);
    }

    if ((operand & UnallocMask) != 0) 
        UNPREDICTABLE();

	debug_raw("operande:%x ", operand);

    uint32_t mask = 0;
    uint32_t byte_mask = (get_bit(ins, 16) ? 0x000000FF : 0) | //C
                         (get_bit(ins, 17) ? 0x0000FF00 : 0) | //X
                         (get_bit(ins, 18) ? 0x00FF0000 : 0) | //S
                         (get_bit(ins, 19) ? 0xFF000000 : 0);  //F

	debug_raw("byte mask:%x ", byte_mask);

    if (get_bit(ins, 22)) {
		debug_raw("SPSR ");
        if (arm_current_mode_has_spsr(p)) {
            mask = byte_mask & (UserMask | PrivMask | StateMask);
			debug_raw("mask:%x\n", mask);
            arm_write_spsr(p, (arm_read_spsr(p) & ~mask) | (operand & mask));
        } else {
            UNPREDICTABLE();
        }
    } else {
		debug_raw("CPSR ");
		if (arm_in_a_privileged_mode(p)) {
			debug_raw("mode privileged ");
			if ((operand & StateMask) == 0) {
				mask = byte_mask & (UserMask | PrivMask);
			} else {
				UNPREDICTABLE();
			}
		} else {
			debug_raw("mode user ");
			mask = byte_mask & UserMask;
		}
		debug_raw("mask:%x\n", mask);
		arm_write_cpsr(p, (arm_read_cpsr(p) & ~mask) | (operand & mask));
	}
	return 0;
}

