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
#include <limits.h>
#include "arm_instruction.h"
#include "arm_data_processing.h"
#include "arm_exception.h"
#include "arm_constants.h"
#include "arm_branch_other.h"
#include "util.h"
#include "debug.h"

typedef int(* dp_instruction_handler_t)(arm_core, uint8_t, int, int, uint8_t, uint8_t);

// Data processing instruction parsing
static inline int get_op_code(uint32_t ins) {
	return (ins >> 21) & 15;
}
static inline uint8_t get_rd(uint32_t ins) {
	return (ins >> 12) & 15;
}
static inline uint8_t get_rn(uint32_t ins) {
	return (ins >> 16) & 15;
}
static inline uint8_t get_rm(uint32_t ins) {
	return ins & 15;
}
static inline uint8_t get_S(uint32_t ins) {
	return (ins >> 20) & 1;
}
static inline uint8_t get_rs(uint32_t ins) {
	return (ins >> 8) & 15;
}
static inline uint8_t get_shift_imm(uint32_t ins) {
	return (ins >> 7) & 31;
}
static inline int get_shift_code(uint32_t ins) {
	return (ins >> 5) & 3;
}

// Decoding
dp_instruction_handler_t decode(int op_code) {
	switch(op_code) {
		case AND: return and; break;
		case EOR: return eor; break;
		case SUB: return sub; break;
		case RSB: return rsb; break;
		case ADD: return add; break;
		case ADC: return adc; break;
		case SBC: return sbc; break;
		case RSC: return rsc; break;
		case TST: return tst; break;
		case TEQ: return teq; break;
		case CMP: return cmp; break;
		case CMN: return cmn; break;
		case ORR: return orr; break;
		case MOV: return mov; break;
		case BIC: return bic; break;
		case MVN: return mvn; break;
	}
}

// Instructions

int and(arm_core p,uint8_t rd,int op1,int op2,uint8_t S, uint8_t shift_C) {
	int exception = 0;
	uint64_t result = op1 & op2;
	
	if(S) { // Flags update
		if(rd != 15){
			uint32_t cpsr = arm_read_cpsr(p);
			// N
			cpsr = get_bit(result ,31) ? set_n(cpsr) : clear_n(cpsr);
			// Z
			cpsr = result ? clear_z(cpsr) : set_z(cpsr);
			// C
			cpsr = shift_C ? set_c(cpsr) : clear_c(cpsr);
			// V
			// unaffected
			arm_write_cpsr(p, cpsr);
		}
		else {
			if(arm_current_mode_has_spsr(p)) {
				arm_write_cpsr(p,arm_read_spsr(p));
			}
			else {
				// UNPREDICTABLE
			}
		}
	}
	
	arm_write_register(p, rd, result);
	return exception;
}

int eor(arm_core p,uint8_t rd,int op1,int op2,uint8_t S, uint8_t shift_C) {
	int exception = 0;
	uint64_t result = op1 ^ op2;
	if(S) {
		if(rd != 15){
			uint32_t cpsr = arm_read_cpsr(p);
			// N
			cpsr = get_bit(result ,31) ? set_n(cpsr) : clear_n(cpsr);
			// Z
			cpsr = result ? clear_z(cpsr) : set_z(cpsr);
			// C
			cpsr = shift_C ? set_c(cpsr) : clear_c(cpsr);
			// V
			// unaffected	
			arm_write_cpsr(p, cpsr);
		}
		else {
			if(arm_current_mode_has_spsr(p)) {
				arm_write_cpsr(p,arm_read_spsr(p));
			}
			else {
				// UNPREDICTABLE
			}
		}
	}
	
	arm_write_register(p, rd, result);
	return exception;
}

int sub(arm_core p,uint8_t rd,int op1,int op2,uint8_t S, uint8_t shift_C) {
	int exception = 0;
	uint64_t result = op1 - op2;
	if(S) {
		if(rd != 15){
			uint32_t cpsr = arm_read_cpsr(p);
			// N
			cpsr = get_bit(result ,31) ? set_n(cpsr) : clear_n(cpsr);
			// Z
			cpsr = result ? clear_z(cpsr) : set_z(cpsr);
			// C
			cpsr = (unsigned int)op1 < (unsigned int)op2 ? clear_c(cpsr) : set_c(cpsr);
			// V
			cpsr = get_bit(op1,31) != get_bit(op2,31) && get_bit(op1,31) != get_bit(result,31) ? set_v(cpsr) ; clear_v(cpsr);
			
			arm_write_cpsr(p, cpsr);
		}
		else {
			if(arm_current_mode_has_spsr(p)) {
				arm_write_cpsr(p,arm_read_spsr(p));
			}
			else {
				// UNPREDICTABLE
			}
		}
	}
	arm_write_register(p, rd, result);
	return exception;
}

int rsb(arm_core p,uint8_t rd,int op1,int op2,uint8_t S, uint8_t shift_C) {
	int exception = 0;
	uint64_t result = op2 - op1;
	if(S) {
		if(rd != 15){
			uint32_t cpsr = arm_read_cpsr(p);
			// N
			cpsr = get_bit(result ,31) ? set_n(cpsr) : clear_n(cpsr);
			// Z
			cpsr = result ? clear_z(cpsr) : set_z(cpsr);
			// C
			cpsr = (unsigned int)op2 < (unsigned int)op1 ? clear_c(cpsr) : set_c(cpsr);
			// V
			cpsr = get_bit(op1,31) != get_bit(op2,31) && get_bit(op2,31) != get_bit(result,31) ? set_v(cpsr) ; clear_v(cpsr);
			
			arm_write_cpsr(p, cpsr);
		}
		else {
			if(arm_current_mode_has_spsr(p)) {
				arm_write_cpsr(p,arm_read_spsr(p));
			}
			else {
				// UNPREDICTABLE
			}
		}
	}
	arm_write_register(p, rd, result);
	return exception;
}

int add(arm_core p,uint8_t rd,int op1,int op2,uint8_t S, uint8_t shift_C) {
	int exception = 0;
	uint64_t result = op1 + op2;
	if(S) {
		if(rd != 15){
			uint32_t cpsr = arm_read_cpsr(p);
			// N
			cpsr = get_bit(result ,31) ? set_n(cpsr) : clear_n(cpsr);
			// Z
			cpsr = result ? clear_z(cpsr) : set_z(cpsr);
			// C
			cpsr = result > UINT_MAX ? set_c(cpsr) : clear_c(cpsr);
			// V
			cpsr = get_bit(op1,31) = get_bit(op2,31) && get_bit(op2,31) != get_bit(result,31) ? set_v(cpsr) ; clear_v(cpsr);
			
			arm_write_cpsr(p, cpsr);
		}
		else {
			if(arm_current_mode_has_spsr(p)) {
				arm_write_cpsr(p,arm_read_spsr(p));
			}
			else {
				// UNPREDICTABLE
			}
		}
	}
	arm_write_register(p, rd, result);
	return exception;
}

int adc(arm_core p,uint8_t rd,int op1,int op2,uint8_t S, uint8_t shift_C) {
	int exception = 0;
	uint64_t result = op1 + op2 + arm_read_c(p);
	if(S) {
		if(rd != 15){
			uint32_t cpsr = arm_read_cpsr(p);
			// N
			cpsr = get_bit(result ,31) ? set_n(cpsr) : clear_n(cpsr);
			// Z
			cpsr = result ? clear_z(cpsr) : set_z(cpsr);
			// C
			cpsr = result > UINT_MAX ? set_c(cpsr) : clear_c(cpsr);
			// V
			cpsr = get_bit(op1,31) = get_bit(op2,31) && get_bit(op2,31) != get_bit(result,31) ? set_v(cpsr) ; clear_v(cpsr);
			
			arm_write_cpsr(p, cpsr);
		}
		else {
			if(arm_current_mode_has_spsr(p)) {
				arm_write_cpsr(p,arm_read_spsr(p));
			}
			else {
				// UNPREDICTABLE
			}
		}
	}
	arm_write_register(p, rd, result);
	return exception;
}

int sbc(arm_core p,uint8_t rd,int op1,int op2,uint8_t S, uint8_t shift_C) {
	int exception = 0;
	uint64_t result = op1 - op2 - !arm_read_c(p);
	if(S) {
		if(rd != 15){
			uint32_t cpsr = arm_read_cpsr(p);
			// N
			cpsr = get_bit(result ,31) ? set_n(cpsr) : clear_n(cpsr);
			// Z
			cpsr = result ? clear_z(cpsr) : set_z(cpsr);
			// C
			cpsr = (unsigned int)op1 < (unsigned int)(op2+arm_read_c(p)) ? clear_c(cpsr) : set_c(cpsr);
			// V
			cpsr = get_bit(op1,31) != get_bit(op2,31) && get_bit(op1,31) != get_bit(result,31) ? set_v(cpsr) ; clear_v(cpsr);
			
			arm_write_cpsr(p, cpsr);
		}
		else {
			if(arm_current_mode_has_spsr(p)) {
				arm_write_cpsr(p,arm_read_spsr(p));
			}
			else {
				// UNPREDICTABLE
			}
		}
	}
	arm_write_register(p, rd, result);
	return exception;
}

int rsc(arm_core p,uint8_t rd,int op1,int op2,uint8_t S, uint8_t shift_C) {
	int exception = 0;
	uint64_t result = op2 - op1 - !arm_read_c(p);
	if(S) {
		if(rd != 15){
			uint32_t cpsr = arm_read_cpsr(p);
			// N
			cpsr = get_bit(result ,31) ? set_n(cpsr) : clear_n(cpsr);
			// Z
			cpsr = result ? clear_z(cpsr) : set_z(cpsr);
			// C
			cpsr = (unsigned int)op2 < (unsigned int)(op1+arm_read_c(p)) ? clear_c(cpsr) : set_c(cpsr);
			// V
			cpsr = get_bit(op1,31) != get_bit(op2,31) && get_bit(op2,31) != get_bit(result,31) ? set_v(cpsr) ; clear_v(cpsr);
			
			arm_write_cpsr(p, cpsr);
		}
		else {
			if(arm_current_mode_has_spsr(p)) {
				arm_write_cpsr(p,arm_read_spsr(p));
			}
			else {
				// UNPREDICTABLE
			}
		}
	}
	arm_write_register(p, rd, result);
	return exception;
}

int tst(arm_core p,uint8_t rd,int op1,int op2,uint8_t S, uint8_t shift_C) {
	int exception = 0;
	uint64_t result = op1 & op2;
	uint32_t cpsr = arm_read_cpsr(p);
	// N
	cpsr = get_bit(result ,31) ? set_n(cpsr) : clear_n(cpsr);
	// Z
	cpsr = result ? clear_z(cpsr) : set_z(cpsr);
	// C
	cpsr = shift_C ? set_c(cpsr) : clear_c(cpsr);
	// V
	// unaffected
	arm_write_cpsr(p, cpsr);
	return exception;
}

int teq(arm_core p,uint8_t rd,int op1,int op2,uint8_t S, uint8_t shift_C) {
	int exception = 0;
	uint64_t result = op1 ^ op2;
	uint32_t cpsr = arm_read_cpsr(p);
	// N
	cpsr = get_bit(result ,31) ? set_n(cpsr) : clear_n(cpsr);
	// Z
	cpsr = result ? clear_z(cpsr) : set_z(cpsr);
	// C
	cpsr = shift_C ? set_c(cpsr) : clear_c(cpsr);
	// V
	// unaffected
	arm_write_cpsr(p, cpsr);
	return exception;
}

int cmp(arm_core p,uint8_t rd,int op1,int op2,uint8_t S, uint8_t shift_C) {
	int exception = 0;
	uint64_t result = op1 - op2;
	uint32_t cpsr = arm_read_cpsr(p);
	// N
	cpsr = get_bit(result ,31) ? set_n(cpsr) : clear_n(cpsr);
	// Z
	cpsr = result ? clear_z(cpsr) : set_z(cpsr);
	// C
	cpsr = (unsigned int)op1 < (unsigned int)op2 ? clear_c(cpsr) : set_c(cpsr);
	// V
	cpsr = get_bit(op1,31) != get_bit(op2,31) && get_bit(op1,31) != get_bit(result,31) ? set_v(cpsr) ; clear_v(cpsr);
	
	arm_write_cpsr(p, cpsr);
	return exception;
}

int cmn(arm_core p,uint8_t rd,int op1,int op2,uint8_t S, uint8_t shift_C) {
	int exception = 0;
	uint64_t result = op1 + op2;
	uint32_t cpsr = arm_read_cpsr(p);
	// N
	cpsr = get_bit(result ,31) ? set_n(cpsr) : clear_n(cpsr);
	// Z
	cpsr = result ? clear_z(cpsr) : set_z(cpsr);
	// C
	cpsr = result > UINT_MAX ? set_c(cpsr) : clear_c(cpsr);
	// V
	cpsr = get_bit(op1,31) = get_bit(op2,31) && get_bit(op1,31) != get_bit(result,31) ? set_v(cpsr) ; clear_v(cpsr);
			
	arm_write_cpsr(p, cpsr);
	return exception;
}

int orr(arm_core p,uint8_t rd,int op1,int op2,uint8_t S, uint8_t shift_C) {
	int exception = 0;
	uint64_t result = op1 | op2;
	if(S) {
		if(rd != 15){
			uint32_t cpsr = arm_read_cpsr(p);
			// N
			cpsr = get_bit(result ,31) ? set_n(cpsr) : clear_n(cpsr);
			// Z
			cpsr = result ? clear_z(cpsr) : set_z(cpsr);
			// C
			cpsr = shift_C ? set_c(cpsr) : clear_c(cpsr);
			// V
			// unaffected	
			arm_write_cpsr(p, cpsr);
		}
		else {
			if(arm_current_mode_has_spsr(p)) {
				arm_write_cpsr(p,arm_read_spsr(p));
			}
			else {
				// UNPREDICTABLE
			}
		}
	}
	
	arm_write_register(p, rd, result);
	return exception;
}

int mov(arm_core p,uint8_t rd,int op1,int op2,uint8_t S, uint8_t shift_C) {
	int exception = 0;
	uint64_t result = op2;
	
	if(S) { // Flags update
		if(rd != 15){
			uint32_t cpsr = arm_read_cpsr(p);
			// N
			cpsr = get_bit(result ,31) ? set_n(cpsr) : clear_n(cpsr);
			// Z
			cpsr = result ? clear_z(cpsr) : set_z(cpsr);
			// C
			cpsr = shift_C ? set_c(cpsr) : clear_c(cpsr);
			// V
			// unaffected
			arm_write_cpsr(p, cpsr);
		}
		else {
			if(arm_current_mode_has_spsr(p)) {
				arm_write_cpsr(p,arm_read_spsr(p));
			}
			else {
				// UNPREDICTABLE
			}
		}
	}
	
	arm_write_register(p, rd, result);
	return exception;
}

int bic(arm_core p,uint8_t rd,int op1,int op2,uint8_t S, uint8_t shift_C) {
	int exception = 0;
	uint64_t result = op1 & ~op2;
	
	if(S) { // Flags update
		if(rd != 15){
			uint32_t cpsr = arm_read_cpsr(p);
			// N
			cpsr = get_bit(result ,31) ? set_n(cpsr) : clear_n(cpsr);
			// Z
			cpsr = result ? clear_z(cpsr) : set_z(cpsr);
			// C
			cpsr = shift_C ? set_c(cpsr) : clear_c(cpsr);
			// V
			// unaffected
			arm_write_cpsr(p, cpsr);
		}
		else {
			if(arm_current_mode_has_spsr(p)) {
				arm_write_cpsr(p,arm_read_spsr(p));
			}
			else {
				// UNPREDICTABLE
			}
		}
	}
	
	arm_write_register(p, rd, result);
	return exception;
}

int mvn(arm_core p,uint8_t rd,int op1,int op2,uint8_t S, uint8_t shift_C) {
	int exception = 0;
	uint64_t result = ~op2;
	
	if(S) { // Flags update
		if(rd != 15){
			uint32_t cpsr = arm_read_cpsr(p);
			// N
			cpsr = get_bit(result ,31) ? set_n(cpsr) : clear_n(cpsr);
			// Z
			cpsr = result ? clear_z(cpsr) : set_z(cpsr);
			// C
			cpsr = shift_C ? set_c(cpsr) : clear_c(cpsr);
			// V
			// unaffected
			arm_write_cpsr(p, cpsr);
		}
		else {
			if(arm_current_mode_has_spsr(p)) {
				arm_write_cpsr(p,arm_read_spsr(p));
			}
			else {
				// UNPREDICTABLE
			}
		}
	}
	
	arm_write_register(p, rd, result);
	return exception;
}


// Decoding functions for various classes of instructions
int arm_data_processing_shift(arm_core p, uint32_t ins) {
    debug("arm_data_processing_shift: %d\n", (int)ins);    
    
		uint8_t rd, S, shift_C;
		int op1, op2;
    int exception = 0;
    
    // Parsing the instruction
    int op_code = get_op_code(ins);
    dp_instruction_handler_t handler = decode(op_code);
    
    rd = get_rd(ins);
    
    if(op_code != MOV && op_code != MVN) {
    	op1 = arm_read_register(p, get_rn(ins));
    }
    
    op2 = get_shifted(ins, &shift_C);
    
    S = get_S(ins);
    
		// Redirecting on MRS and MSR
		if ((op_code == TST || op_code == TEQ || op_code == CMP || op_code == CMN) && !S)
		{
			if (get_bit(ins, 21))
				arm_msr(p, ins);
			else
				arm_mrs(p, ins);
		}
    else {
    // DP instruction call
    exception = handler(p, rd, op1, op2, S, shift_C);
    }
    return exception;
}


int arm_data_processing_immediate(arm_core p, uint32_t ins) {
    debug("arm_data_processing_immediate: %d\n", (int)ins);

	uint8_t rd, rn, rm, S, rs, shift_imm, shift_code, shift_C;
	int op1, op2;

    int op_code = get_op_code(ins);
	//recuperation de l'opperande
	if (get_bit(ins, 25)) //immediate
		op2 = get_immediate(ins);
	else //register
	{
		rm = get_bits(3, 0);
		op2 = arm_read_register(p, rm);
	}

	
		dp_instruction_handler_t handler = decode(op_code);

		rd = get_rd(ins);

		if(op_code != MOV && op_code != MVN) {
			rn = get_rn(ins);
			op1 = arm_read_register(p, rn);
		}
		
		if(op_code == CMP || op_code == CMN  || op_code == TST || op_code == TEQ) {
			S = 1;
		}
		else {
			S = get_S(ins);
		}

		handler(p, rd, op1, op2, S, shift_C);
	


    /*
    	Attention : bit25 == 0 &&	bit7 == 1 && bit4 == 1 	=> load/store 
    	cf. p.443, 144 et 146
    	=> appel arm_load_store_miscellaneous(arm_core p, uint32_t ins)
    */


    
    return 0;
}

