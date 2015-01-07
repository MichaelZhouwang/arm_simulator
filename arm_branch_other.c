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

#include "arm_branch_other.h"
#include "arm_constants.h"
#include "util.h"
#include "arm_instruction.h"
#include <debug.h>
#include <stdlib.h>

int arm_branch(arm_core p, uint32_t ins) {
	if (instruction_check_condition(p, ins))
	{
		if (get_bit(ins, 24)) //Link
		{
			debug("Branch and Link\n");
			arm_write_register(p, 14, arm_read_register(p, 15)); //R14 = R15
		}
		else
			debug("Branch\n");

		int32_t add = ins & 0xffffff;
		add = add << 8;
		add = add >> 8;
		add = add << 2;
		add += arm_read_register(p, 15);

		debug("branch to add %x\n", add);

		arm_write_register(p, 15, add);
	}

    return 0;
}

int arm_coprocessor_others_swi(arm_core p, uint32_t ins) {
    debug("arm_coprocessor_other_swi: %d\n", (int)ins);
    
    if (get_bit(ins, 24)) {
        debug("==> swi instruction\n");
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
	if (instruction_check_condition(p, ins))
	{
		uint8_t rd = get_bits(ins, 15, 12);
		uint32_t op = get_immediate(ins);

		//if (rd == 15)
		//UNPREDICTABLE

		//b19-b16 => SBO
		//b11-b0 => SBZ

		if (get_bit(ins, 22)) // R
			arm_write_register(p, rd, arm_read_spsr(p));
		else
			arm_write_register(p, rd, arm_read_cpsr(p));
	}

	return 0;
}

int arm_msr(arm_core p, uint32_t ins) {
	if (instruction_check_condition(p, ins))
	{
		uint32_t byte_mask, mask = 0;

		uint32_t op = get_immediate(ins);
		
		byte_mask = (get_bit(ins, 16) ? 0x000000FF : 0) | //C
							 (get_bit(ins, 17) ? 0x0000FF00 : 0) | //X
							 (get_bit(ins, 18) ? 0x00FF0000 : 0) | //S
							 (get_bit(ins, 19) ? 0xFF000000 : 0);  //F

		if (get_bit(ins, 22)) //R
		{
			if (arm_current_mode_has_spsr(p))
			{
				mask = byte_mask & (UserMask | PrivMask | StateMask);
				arm_write_spsr(p, (arm_read_spsr(p) & ~mask) | (op & mask));
			}
			//else
				//UNPREDICTABLE
		}
		else
		{
			if (arm_in_a_privileged_mode(p))
			{
				if ((op & StateMask) == 0)
					mask = byte_mask & (UserMask & PrivMask);
				//else
					//UNPREDICTABLE

			}
			else
				mask = byte_mask & UserMask;
			arm_write_spsr(p, (arm_read_spsr(p) & ~mask) | (op & mask));
		}
		
	}

	return 0;
}
