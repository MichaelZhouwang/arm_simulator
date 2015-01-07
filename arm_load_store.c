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

#include "arm_load_store.h"
#include "arm_exception.h"
#include "arm_constants.h"
#include "util.h"
#include "debug.h"

// Get different parts of instruction

static inline int arm_ls_get_p(uint32_t ins) {
	return (int)((ins >> 24) & 1);
}

static inline int arm_ls_get_u(uint32_t ins) {
	return (int)((ins >> 23) & 1);
}

static inline int arm_ls_get_b(uint32_t ins) {
	return (int)((ins >> 22) & 1);
}

static inline int arm_ls_get_w(uint32_t ins) {
	return (int)((ins >> 21) & 1);
}

static inline int arm_ls_get_l(uint32_t ins) {
	return (int)((ins >> 20) & 1);
}

static inline uint8_t arm_ls_get_rn(uint32_t ins) {
	return (uint8_t)((ins >> 16) & 0xF);
}

static inline uint8_t arm_ls_get_rd(uint32_t ins) {
	return (uint8_t)((ins >> 12) & 0xF);
}

static inline uint8_t arm_ls_get_rm(uint32_t ins) {
	return (uint8_t)(ins & 0xF);
}

static inline int arm_ls_get_shift(uint32_t ins) {
	return (int)((ins >> 5) & 3);
}

static inline int arm_ls_get_shift_imm(uint32_t ins) {
	return (int)((ins >> 7) & 15);
}

static inline int arm_ls_get_offset(uint32_t ins) {
	return (int)(ins & 0x7FF);
}


// Addressing mode

typedef enum {
    addressing_offset,
    addressing_pre_indexed,
    addressing_post_indexed
} addressing_t;

static inline addressing_t arm_ls_get_addressing(uint32_t ins) {
	if (arm_ls_get_p(ins)) {
	    return addressing_post_indexed;
	} else if (arm_ls_get_w(ins)) {
	    return addressing_pre_indexed;
	} else {
	    return addressing_offset;
	}
}

// Public

int arm_load_store(arm_core p, uint32_t ins) {
    debug("arm_load_store: %d\n", (int)ins);

	uint32 address, value;
	int result, index;
	uint8_t rd = arm_ls_get_rd(ins);
	uint8_t rn = arm_ls_get_rn(ins);
	uint8_t rm = arm_ls_get_rm(ins);
	int shift = arm_ls_get_shift(ins);
	int shift_imm = arm_ls_get_shift_imm(ins);
	addressing_t adressing = arm_ls_get_addressing(ins);
	
	if (adressing == addressing_post_indexed) {
       address = rn;
    }
	index = shift(/*complete*/); // TODO: add shift function created in util.h
	if (adressing != addressing_post_indexed ||
        instruction_check_condition(p, ins)) {
		address = (arm_ls_get_u(ins)) ? rn+index : rn-index;
    }
    if (adressing == addressing_pre_indexed &&
        instruction_check_condition(p, ins)) {
        rn = address;
    }
	
	if (arm_ls_get_l(ins)) { //TODO: Prise en charge de B
		result = arm_read_word(p, address, &value);
		if (result) arm_write_register(p, rd, value);
	} else { 
		value = arm_read_register(p, rd);
		result = arm_write_word(p, address, value);
	}

    return result;
}

int arm_load_store_immediate(arm_core p, uint32_t ins) {
    debug("arm_load_store_immediate: %d\n", (int)ins);
	    // GESTION DES EXEPTIONS ?
	uint32 address, value;
	int result;
	uint8_t rd = arm_ls_get_rd(ins); // PAS REG met val de REG
	uint8_t rn = arm_ls_get_rn(ins);
	uint32_t offset = arm_ls_get_offset(ins);
	addressing_t adressing = arm_ls_get_addressing(ins);

	if (adressing == addressing_post_indexed) {
       arm_write_register(p, rn, address);
    }
    if (adressing != addressing_post_indexed || 
        instruction_check_condition(p, ins)) {
        address = arm_ls_get_u(ins) ? rn+offset : rn-offset;
    }
    if (adressing == addressing_pre_indexed &&
        instruction_check_condition(p, ins)) {
       arm_write_register(p, rn, address);
    }
    if (rn == 15 && adressing == addressing_offset) {
		address += 8;
	}

	if (arm_ls_get_l(ins)) { //TODO: Prise en charge de B
		result = arm_read_word(p, address, &value);
		if (result) arm_write_register(p, rd, value);
	} else {
		value = arm_read_register(p, rd);
		result = arm_write_word(p, address, value);
	}
    return result;
}

int arm_load_store_multiple(arm_core p, uint32_t ins) {
    debug("arm_load_store_multiple: %d\n", (int)ins);
    return UNDEFINED_INSTRUCTION;
}

int arm_coprocessor_load_store(arm_core p, uint32_t ins) {
    debug("arm_coprocessor_load_store: %d\n", (int)ins);
    return UNDEFINED_INSTRUCTION;
}
