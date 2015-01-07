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

static inline int get_p(uint32_t ins) {
	return (int)((ins >> 24) & 1);
}

static inline int get_u(uint32_t ins) {
	return (int)((ins >> 23) & 1);
}

static inline int get_b(uint32_t ins) {
	return (int)((ins >> 22) & 1);
}

static inline int get_w(uint32_t ins) {
	return (int)((ins >> 21) & 1);
}

static inline int get_l(uint32_t ins) {
	return (int)((ins >> 20) & 1);
}

static inline uint8_t get_rn(uint32_t ins) {
	return (uint8_t)((ins >> 16) & 0xF);
}

static inline uint8_t get_rd(uint32_t ins) {
	return (uint8_t)((ins >> 12) & 0xF);
}

static inline uint8_t get_rm(uint32_t ins) {
	return (uint8_t)(ins & 0xF);
}

static inline int get_shift(uint32_t ins) {
	return (int)((ins >> 5) & 0x3);
}

static inline int get_shift_imm(uint32_t ins) {
	return (int)((ins >> 7) & 0xF);
}

static inline int get_offset(uint32_t ins) {
	return (int)(ins & 0x7FF);
}

static inline int get_immed_h(uint32_t ins) {
	return (int)((ins >> 8) & 0xF);
}

static inline int get_immed_l(uint32_t ins) {
	return (int)(ins & 0xF);
}

static inline int get_s(uint32_t ins) {
	return (int)((ins >> 6) & 1);
}

static inline int get_h(uint32_t ins) {
	return (int)((ins >> 5) & 1);
}

// Load to reg / store from reg

int arm_load_to_reg(int8_t reg, int32_t address, int b) {
	int result;
	if (b) {		
		result = arm_read_byte(p, address, &value);
	} else {
		result = arm_read_word(p, address, &value);
	}
	return  result || arm_write_register(p, reg, value);
}

int arm_store_from_reg(int8_t reg, int32_t address, int b) {
	int value = arm_read_register(p, reg);
	if (b) {		
		return arm_write_byte(p, address, &value);
	} else {
		return arm_write_word(p, address, &value);
	}
}

// Load store
// TODO: Gestion de l'exeption data abort

int arm_load_store(arm_core p, uint32_t ins) {
    debug("arm_load_store: %d\n", (int)ins);

	uint32 address;
	int index;
	uint8_t rd = get_rd(ins);
	uint8_t rn = get_rn(ins);
	uint8_t rm = get_rm(ins);
	int val_rn = arm_read_register(p, rn);
	int val_rm = arm_read_register(p, rn);
	int shift = get_shift(ins);
	int shift_imm = get_shift_imm(ins);

	if (get_p(ins)) { // post_indexed
  		address = val_rn;
		if (instruction_check_condition(p, ins)) {
       		address = (get_u(ins)) ? val_rn + val_rm : val_rn - val_rm;   
    	}
	} else if (get_w(ins)) { // pre_indexed
        index = shift(p, val_rm, shift, shift_imm);
		address = (get_u(ins)) ? val_rn + index : val_rn - index;
		if (instruction_check_condition(p, ins)) {
			arm_write_register(p, rn, address);
    	}
    } else { // offset and scaled offser
        index = shift(p, val_rm, shift, shift_imm);
		address = (get_u(ins)) ? val_rn + index : val_rn - index;
		if (rn == 15) address += 8;        
    }
	
	if (get_l(ins)) {
		return arm_load_to_reg(rd, address, get_b(ins));
	} else { 
		return arm_store_from_reg(rd, address, get_b(ins));
	}
}

int arm_load_store_immediate(arm_core p, uint32_t ins) {
    debug("arm_load_store_immediate: %d\n", (int)ins);
	
	uint32 address;
	uint8_t rd = get_rd(ins);
	uint8_t rn = get_rn(ins);
	int val_rn = arm_read_register(p, rn);
	int val_rm = arm_read_register(p, rn);
	uint32_t offset = get_offset(ins);

	if (get_p(ins)) { // post_indexed
  		address = val_rn;
		if (instruction_check_condition(p, ins)) {
			val_rn = (get_u(ins)) ? val_rn+offset : val_rn-offset;
			arm_write_register(p, rn, val_rn);
		}
		if (rn == 15) {
			address += 8;
		}
	} else if (get_w(ins)) { // pre_indexed
    	address = (get_u(ins) ? val_rn + offset : val_rn - offset;      
		if (instruction_check_condition(p, ins)) {
			arm_write_register(p, rn, address);
		}           
    } else { // offset
    	address = (get_u(ins)) ? val_rn + offset : val_rn - offset;
    }

	if (get_l(ins)) {
		return arm_load_to_reg(rd, address, get_b(ins));
	} else { 
		return arm_store_from_reg(rd, address, get_b(ins));
	}
}

// Load store miscellaneous

int arm_load_store_miscellaneous(arm_core p, uint32_t ins) {
	debug("arm_load_store: %d\n", (int)ins);
	
	uint32 address;
	uint8_t rd = get_rd(ins);
	uint8_t rn = get_rn(ins);
	int val_rn = arm_read_register(p, rn);
	int val_rm = arm_read_register(p, rn);
	int offset_type = (get_bit(ins, 24) << 1) & get_bit(ins, 21);
	
	if (get_bit(ins, 22)) { // immediate
		if (offset_type == 0) { // post_indexed
		
		} else if (offset_type == 2) { // pre_indexed

		} else { // offset
			int offset_8 = (immedH << 4) OR immedL
			if U == 1 then
				address = Rn + offset_8
			else /* U == 0 */
				address = Rn - offset_8
		}
	} else { // register
		if (offset_type == 0) { // post_indexed
		
		} else if (offset_type == 2) { // pre_indexed

		} else { // offset

		}
	}
	

	
}

int arm_load_store_multiple(arm_core p, uint32_t ins) {
    debug("arm_load_store_multiple: %d\n", (int)ins);
    return UNDEFINED_INSTRUCTION;
}

int arm_coprocessor_load_store(arm_core p, uint32_t ins) {
    debug("arm_coprocessor_load_store: %d\n", (int)ins);
    return UNDEFINED_INSTRUCTION;
}
