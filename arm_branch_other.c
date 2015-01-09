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

#define LR 14
#define PC 15

int arm_branch(arm_core p, uint32_t ins) {
    debug("arm_branch\n");
    
    uint32_t val_pc = arm_read_register(p, PC);
	if (get_bit(ins, 24)) { //Link
	    debug("BL\n");
		arm_write_register(p, LR, val_pc - 4); //R14 = R15
	} else {
		debug("B\n");
    }
    
	//int32_t address = val_pc + ((((ins & 0xffffff) << 8) >> 8) << 2);
	uint32_t address = get_bits(ins, 23, 0);
	address = address << 8;
	address = asr(address,8);
	address = address << 2;
	address += val_pc ;

	debug("branch to address %x\n", address);
    arm_write_register(p, PC, address);
	
    return 0;
}






int arm_coprocessor_others_swi(arm_core p, uint32_t ins) {
    debug("arm_coprocessor_other_swi\n");
    
    if (get_bit(ins, 24)) {
        debug("SWI\n");
        // Here we implement the end of the simulation as swi 0x123456
        if ((ins & 0xFFFFFF) == 0x123456) {
            exit(0);
        }
        return SOFTWARE_INTERRUPT;
    }
    
    return UNDEFINED_INSTRUCTION;
}

int arm_miscellaneous(arm_core p, uint32_t ins) {
    debug("arm_miscellaneous: %d\n", (int)ins);
    return UNDEFINED_INSTRUCTION;
}

int arm_mrs(arm_core p, uint32_t ins) {
    debug("arm_mrs: %d\n", (int)ins);
    
	uint8_t rd = get_bits(ins, 15, 12);
    if (rd == 15)
		UNPREDICTABLE();
    if (get_bits(ins, 19, 16) != 0xF)
        SHOULD_BE_ONE();
    if (get_bits(ins, 11, 0) != 0)
        SHOULD_BE_ZERO();
    
    int32_t value;
    if (get_bit(ins, 22))
        value = arm_read_spsr(p);
    else
        value = arm_read_cpsr(p);
    
    arm_write_register(p, rd, value);
	return 0;
}

int arm_msr(arm_core p, uint32_t ins) {
    debug("arm_msr: %d\n", (int)ins);
	
	uint32_t op;
	uint32_t byte_mask, mask = 0;

	if (get_bit(ins, 25)) {
        op = get_immediate(p, ins, NULL);
    } else {
        uint8_t rm = get_bits(ins, 3, 0);
        op = arm_read_register(p, rm);
    }
    
	if ((op & UnallocMask) != 0) 
		UNPREDICTABLE();

	byte_mask = (get_bit(ins, 16) ? 0x000000FF : 0) | //C
				(get_bit(ins, 17) ? 0x0000FF00 : 0) | //X
				(get_bit(ins, 18) ? 0x00FF0000 : 0) | //S
				(get_bit(ins, 19) ? 0xFF000000 : 0);  //F

    if (get_bit(ins, 22)) { //R
        if (arm_current_mode_has_spsr(p)) {
	        mask = byte_mask & (UserMask | PrivMask | StateMask);
            arm_write_spsr(p, (arm_read_spsr(p) & ~mask) | (op & mask));
        } else {
            UNPREDICTABLE();
        }
    } else {
        if (arm_in_a_privileged_mode(p)) {
            if ((op & StateMask) == 0) {
                mask = byte_mask & (UserMask | PrivMask);
            } else {
                UNPREDICTABLE();
            }
        } else {
            mask = byte_mask & UserMask;
        }
        arm_write_cpsr(p, (arm_read_cpsr(p) & ~mask) | (op & mask));
    }
	return 0;
}

