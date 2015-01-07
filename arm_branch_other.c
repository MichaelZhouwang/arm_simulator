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
#include <debug.h>
#include <stdlib.h>

int arm_branch(arm_core p, uint32_t ins) {
    int32_t pc = arm_read_register(p,15);
    int32_t shift;
    
	debug("arm_branch pc:  %x\n", pc);
	if ((ins >> 24) & 1) {
		debug("Branch and Link\n");
		arm_write_register(p, 14, pc); //R14 <- R15
	} else {
		debug("Branch\n");
	}

	shift = ((((ins & 0xffffff) << 8) >> 8) << 2);
	pc += shift;
	debug("branch to pc:%x (shift: %x)\n", pc, shift);
	arm_write_register(p, 15, pc);

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
	uint8_t rd = ((ins >> 12) & 0x0f);
    // A COMPLETER SELON LA PAGE 226 ? (jeremy)
	if (rd == 15)
		return UNDEFINED_INSTRUCTION;

	//b19-b16 => SBO
	//b11-b0 => SBZ

	if ((ins >> 22) & 1) // R
		arm_write_register(p, rd, arm_read_spsr(p));
	else
		arm_write_register(p, rd, arm_read_cpsr(p));

	return 0;
}
