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

#include <debug.h>
#include "arm_exception.h"
#include "arm_constants.h"
#include "arm_core.h"
#include "util.h"

#ifdef BIG_ENDIAN_SIMULATOR
#define CP15_reg1_EEbit 1
#else
#define CP15_reg1_EEbit 0
#endif

#define High_vectors_configured 0

/////////////////////////////////////////////////////////////////////////////
// Helpers
/////////////////////////////////////////////////////////////////////////////

void arm_branch_exception_vector(arm_core p, int8_t vector) {
    int32_t address = vector;
    if (High_vectors_configured) {
        address |= 0xFFFF0000;
    } else {
        address &= 0x0000FFFF;
		address += 0x1000;
	}
    arm_write_usr_register(p, PC, address);
}

/////////////////////////////////////////////////////////////////////////////
// Exceptions handlers
/////////////////////////////////////////////////////////////////////////////

static void handle_reset(arm_core p) {
    debug("exception called : RESET\n");

    int32_t cpsr = arm_read_cpsr(p);
    cpsr = set_bits(cpsr, 4, 0, USR);        // Enter Supervisor mode
    //cpsr = clr_bit(cpsr, T);                  // Execute in ARM state
    cpsr = clr_bit(cpsr, F);                  // Disable fast interrupts
    cpsr = clr_bit(cpsr, I);                  // Disable normal interrupts
    cpsr = chg_bit(cpsr, E, CP15_reg1_EEbit); // Endianness on exception entry

    arm_write_cpsr(p, cpsr);

	//arm_write_usr_register(p, 15, 0);

    arm_branch_exception_vector(p, 0x0000);
}

static void handle_undefined_instruction(arm_core p) {
    debug("exception called : UNDEFINED INSTRUCTION\n");

    int32_t address_next_ins = arm_address_next_instruction(p);
    int32_t cpsr = arm_read_cpsr(p);
    int32_t cpsr_copie = cpsr;
    
    cpsr = set_bits(cpsr, 4, 0, UND);        // Enter Undef Instruction mode
    cpsr = clr_bit(cpsr, T);                  // Execute in ARM state
    cpsr = set_bit(cpsr, I);                  // Disable normal interrupts
    cpsr = chg_bit(cpsr, E, CP15_reg1_EEbit); // Endianness on exception entry
    
    arm_write_cpsr(p, cpsr);
    arm_write_register(p, LR, address_next_ins);
    arm_write_spsr(p, cpsr_copie);

    arm_branch_exception_vector(p, 0x0004);
}

static void handle_software_interrup(arm_core p) {
    debug("exception called : SOFTWARE INTERRUP\n");

    int32_t address_next_ins = arm_address_next_instruction(p);
    int32_t cpsr = arm_read_cpsr(p);
    int32_t cpsr_copie = cpsr;
    
    cpsr = set_bits(cpsr, 4, 0, SVC);        // Enter Supervisor mode
    cpsr = clr_bit(cpsr, T);                  // Execute in ARM state
    cpsr = set_bit(cpsr, I);                  // Disable normal interrupts
    cpsr = chg_bit(cpsr, E, CP15_reg1_EEbit); // Endianness on exception entry

    arm_write_cpsr(p, cpsr);
    arm_write_register(p, LR, address_next_ins);
    arm_write_spsr(p, cpsr_copie);

    arm_branch_exception_vector(p, 0x0008);
}

static void handle_prefetch_abord(arm_core p) {
    debug("exception called : PREFETCH ABORT\n");

    int32_t address_current_ins = arm_address_current_instruction(p);
    int32_t cpsr = arm_read_cpsr(p);
    int32_t cpsr_copie = cpsr;
    
    cpsr = set_bits(cpsr, 4, 0, ABT);        // Enter Abord mode
    cpsr = clr_bit(cpsr, T);                  // Execute in ARM state
    cpsr = set_bit(cpsr, I);                  // Disable normal interrupts
    cpsr = chg_bit(cpsr, E, CP15_reg1_EEbit); // Endianness on exception entry
    
    arm_write_cpsr(p, cpsr);
    arm_write_register(p, LR, address_current_ins + 4);
    arm_write_spsr(p, cpsr_copie);

    arm_branch_exception_vector(p, 0x000C);
}

static void handle_data_abort(arm_core p) {
    debug("exception called : DATA ABORT\n");

    int32_t cpsr = arm_read_cpsr(p);
    if (get_bit(cpsr, A)) {
        debug("data abord exceptions are disabled\n");
        return;
    }
    
    int32_t address_current_ins = arm_address_current_instruction(p);
    int32_t cpsr_copie = cpsr;

    cpsr = set_bits(cpsr, 4, 0, ABT);        // Enter Abord mode
    cpsr = clr_bit(cpsr, T);                  // Execute in ARM state
    cpsr = set_bit(cpsr, I);                  // Disable normal interrupts
    cpsr = chg_bit(cpsr, E, CP15_reg1_EEbit); // Endianness on exception entry
    
    arm_write_cpsr(p, cpsr);
    arm_write_register(p, LR, address_current_ins + 8);
    arm_write_spsr(p, cpsr_copie);

    arm_branch_exception_vector(p, 0x0010);
}

static void handle_irq(arm_core p) {
    debug("exception called : INTERRUPT REQUEST (IRQ)\n");

    int32_t cpsr = arm_read_cpsr(p);
    if (get_bit(cpsr, I)) {
        debug("IRQ exceptions are disabled\n");
        return;
    }
    
    int32_t address_next_ins = arm_address_next_instruction(p);
    int32_t cpsr_copie = cpsr;

    cpsr = set_bits(cpsr, 4, 0, IRQ);        // Enter IRQ mode
    cpsr = clr_bit(cpsr, T);                   // Execute in ARM state
    cpsr = set_bit(cpsr, I);                   // Disable normal interrupts
    cpsr = chg_bit(cpsr, E, CP15_reg1_EEbit); // Endianness on exception entry

    arm_write_cpsr(p, cpsr);
    arm_write_register(p, LR, address_next_ins + 4);
    arm_write_spsr(p, cpsr_copie);
    
    arm_branch_exception_vector(p, 0x0018);
}

static void handle_fiq(arm_core p) {
    debug("exception called : FAST INTERRUPT REQUEST (FIQ)\n");
    
    int32_t cpsr = arm_read_cpsr(p);
    if (get_bit(cpsr, F)) {
        debug("FIQ exceptions are disabled\n");
        return;
    }
    
    int32_t address_next_ins = arm_address_next_instruction(p);
    int32_t cpsr_copie = cpsr;
    
    cpsr = set_bits(cpsr, 4, 0, FIQ);        // Enter IRQ mode
    cpsr = clr_bit(cpsr, T);               // Execute in ARM state
    cpsr = set_bit(cpsr, F);               // Disable fast interrupts
    cpsr = set_bit(cpsr, I);               // Disable normal interrupts
    cpsr = chg_bit(cpsr, E, CP15_reg1_EEbit); // Endianness on exception entry
    
    arm_write_cpsr(p, cpsr);
    arm_write_register(p, LR, address_next_ins + 4);
    arm_write_spsr(p, cpsr_copie);

    arm_branch_exception_vector(p, 0x001C);
}

/////////////////////////////////////////////////////////////////////////////
// Exceptions management
/////////////////////////////////////////////////////////////////////////////

void arm_exception(arm_core p, unsigned char exception) {
    debug("arm_exception %s\n", arm_get_exception_name(exception));
       
    switch (exception) {
        case RESET:                 handle_reset(p);                 break;
        case UNDEFINED_INSTRUCTION: handle_undefined_instruction(p); break;
        case SOFTWARE_INTERRUPT:    handle_software_interrup(p);     break;
        case PREFETCH_ABORT:        handle_prefetch_abord(p);        break;
        case DATA_ABORT:            handle_data_abort(p);            break;
        case INTERRUPT:             handle_irq(p);                   break;
        case FAST_INTERRUPT:        handle_fiq(p);                   break;
        default: break;
    }
}

