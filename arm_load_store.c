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

// Compute offset according to manual p458

// [<Rn>, #+/-<offset_12>]
static uint32 arm_ls_compute_imm_offset(arm_core p, uint32_t ins) {	
	uint32_t rn = arm_ls_get_rn(ins);
	uint32_t offset = ins & 0x7FF;

	if (arm_ls_get_u(ins)) { 
		return rn + offset;
	} else {
	 	return rn - offset;
	}
}

// [<Rn>, +/-<Rm>]
// [<Rn>, +/-<Rm>, LSL #<shift_imm>]
// [<Rn>, +/-<Rm>, LSR #<shift_imm>]
// [<Rn>, +/-<Rm>, ASR #<shift_imm>]
// [<Rn>, +/-<Rm>, ROR #<shift_imm>]
// [<Rn>, +/-<Rm>, RRX]
static uint32 arm_ls_compute_reg_offset(arm_core p, uint32_t ins) {
	uint32_t rn = arm_ls_get_rn(ins);
	uint32_t rm = arm_ls_get_rm(ins);
	int shift = (ins >> 5) & 3;
	int shift_imm = (ins >> 7) & 15;
	int index;

	// TODO
	index = 

	if (arm_ls_get_u(ins)) {
		address = Rn + index;
	} else {
		address = Rn - index;
	}

// [<Rn>, #+/-<offset_12>]!
static uint32 arm_ls_compute_imm_pre(arm_core p, uint32_t ins) {
	uint32_t rn = arm_ls_get_rn(ins);
	uint32_t offset = ins & 0x7FF;
	uint32_t address;

	if (arm_ls_get_u(ins)) { 
		address = rn + offset;
	} else {
	 	address = rn - offset;
	}
	arm_write_register(p, rn, address);	

	return address;
}

static uint32 arm_ls_compute_reg_pre(arm_core p, uint32_t ins) {

}

// [<Rn>], #+/-<offset_12>
static uint32 arm_ls_compute_imm_post(arm_core p, uint32_t ins) {

}

static uint32 arm_ls_compute_reg_post(arm_core p, uint32_t ins) {

}

// Public

int arm_load_store(arm_core p, uint32_t ins) {
    debug("arm_load_store: %d\n", (int)ins);

	int rd = arm_ls_get_rd(ins);
	int rn = arm_ls_get_rn(ins);
	
	if (rn == 15) {
		return UNDEFINED_INSTRUCTION;
	}

	if (arm_ls_get_p(ins)) {
		address = arm_ls_compute_reg_post(p, ins);
	} else if (arm_ls_get_w(ins)) {
		address = arm_ls_compute_reg_pre(p, ins);
	} else {
		address = arm_ls_compute_reg_offset(p, ins);	
	}

	if (arm_ls_get_l(ins)) {
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
	
	uint32 address, value;
	int result;
	uint8_t rd = arm_ls_get_rd(ins);
	uint8_t rn = arm_ls_get_rn(ins);

	if (rn == 15) {
		return UNDEFINED_INSTRUCTION;
	}

	if (arm_ls_get_p(ins)) {	
		address = arm_ls_compute_imm_post(p, ins);
	} else if (arm_ls_get_w(ins)) {
		address = arm_ls_compute_imm_pre(p, ins);
	} else {
		address = arm_ls_compute_imm_offset(p, ins);
	}

	if (arm_ls_get_l(ins)) {
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
