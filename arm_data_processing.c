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

#include "arm_data_processing.h"
#include "arm_exception.h"
#include "arm_constants.h"
#include "arm_branch_other.h"
#include "util.h"
#include "debug.h"

// Condition check
inline uint8_t instruction_get_condition_field(uint32_t instruction) {
    return (uint8_t)(instruction>>28);
}

int instruction_check_condition(arm_core p, uint8_t field) {
    int res;
    switch(field) {
        case 0:  res = is_z_set(p);                                     break;
        case 1:  res = is_z_clear(p);                                   break;
        case 2:  res = is_c_set(p);                                     break;
        case 3:  res = is_c_clear(p);                                   break;
        case 4:  res = is_n_set(p);                                     break;
        case 5:  res = is_n_clear(p);                                   break;
        case 6:  res = is_v_set(p);                                     break;
        case 7:  res = is_v_clear(p);                                   break;
        case 8:  res = is_c_set(p) && is_z_clear(p);                    break;
        case 9:  res = is_c_clear(p) && is_z_set(p);                    break;
        case 10: res = arm_read_n(p) == arm_read_v(p);                  break;
        case 11: res = arm_read_n(p) != arm_read_v(p);                  break;
        case 12: res = is_z_set(p) && arm_read_n(p) == arm_read_v(p);   break;
        case 13: res = is_z_clear(p) && arm_read_n(p) != arm_read_v(p); break;
        case 14: res =  1; break; // always
        case 15: res = -1; break; // undefined
        default: res =  0; break; // impossible
    }

	debug("condition : %x, %d\n", field, res);
    return res;
}



typedef int(* dp_instruction_handler_t)(arm_core, uint8_t, uint32_t, uint32_t, uint8_t);



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
static inline uint8_t get_bit4(uint32_t ins) {
	return (ins >> 4) & 1;
}
static inline uint8_t get_bit7(uint32_t ins) {
	return (ins >> 7) & 1;
}
static inline uint8_t get_bit25(uint32_t ins) {
	return (ins >> 25) & 1;
}
// Immediate operand value
static inline uint32_t get_immediate(uint32_t ins) {
	uint32_t imm_8 = ins & 255;
	uint8_t rotate_imm = (ins >> 8) & 15;
	return ror(imm_8,(rotate_imm * 2)) ;
}


// Decoding
dp_instruction_handler_t decode(int op_code) {
	switch(op_code) {
		case 0: return and; break;
		case 1: return eor; break;
		case 2: return sub; break;
		case 3: return rsb; break;
		case 4: return add; break;
		case 5: return adc; break;
		case 6: return sbc; break;
		case 7: return rsc; break;
		case 8: return tst; break;
		case 9: return teq; break;
		case 10: return cmp; break;
		case 11: return cmn; break;
		case 12: return orr; break;
		case 13: return mov; break;
		case 14: return bic; break;
		case 15: return mvn; break;
	}
}

// Instructions

void and(arm_core p,uint8_t rd,int op1,int op2,uint8_t S) {
	uint32_t result = op1 & op2;
	arm_write_register(p, rd, result);
	if(S) update_flags(p, result);
}

void eor(arm_core p,uint8_t rd,int op1,int op2,uint8_t S) {
	uint64_t result = op1 ^ op2;
	arm_write_register(p, rd, result);
}

void sub(arm_core p,uint8_t rd,int op1,int op2,uint8_t S) {
	uint64_t result = op1 - op2;
	arm_write_register(p, rd, result);
}

void rsb(arm_core p,uint8_t rd,int op1,int op2,uint8_t S) {
	uint64_t result = op2 - op1;
	arm_write_register(p, rd, result);
}

void add(arm_core p,uint8_t rd,int op1,int op2,uint8_t S) {
	uint64_t result = op1 + op2;
	arm_write_register(p, rd, result);
}

void adc(arm_core p,uint8_t rd,int op1,int op2,uint8_t S) {
	uint64_t result = op1 + op2 + is_c_set(p);
	arm_write_register(p, rd, result);
}

void sbc(arm_core p,uint8_t rd,int op1,int op2,uint8_t S) {
	uint64_t result = op1 - op2 - is_c_clear(p);
	arm_write_register(p, rd, result);
}

void rsc(arm_core p,uint8_t rd,int op1,int op2,uint8_t S) {
	uint64_t result = op2 - op1 - is_c_clear(p);
	arm_write_register(p, rd, result);
}

void tst(arm_core p,uint8_t rd,int op1,int op2,uint8_t S) {

}

void teq(arm_core p,uint8_t rd,int op1,int op2,uint8_t S) {

}

void cmp(arm_core p,uint8_t rd,int op1,int op2,uint8_t S) {

}

void cmn(arm_core p,uint8_t rd,int op1,int op2,uint8_t S) {

}

void orr(arm_core p,uint8_t rd,int op1,int op2,uint8_t S) {

}

void mov(arm_core p,uint8_t rd,int op1,int op2,uint8_t S) {

}

void bic(arm_core p,uint8_t rd,int op1,int op2,uint8_t S) {

}

void mvn(arm_core p,uint8_t rd,int op1,int op2,uint8_t S) {

}


// Decoding functions for various classes of instructions
int arm_data_processing_shift(arm_core p, uint32_t ins) {
    debug("arm_data_processing_shift: %d\n", (int)ins);    
    uint8_t rd, rn, rm, S, rs, shift_imm, shift_code, bit4, bit7;
		int op1, op2;
    uint8_t cond_field = instruction_get_condition_field(ins);
    int result = instruction_check_condition(p, cond_field);
    if(result) return result;
    
    // Parsing the instruction
    int op_code = get_op_code(ins);
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
    
    rm = get_rm(ins);
    shift_imm = get_shift_imm(ins);
		shift_code = get_shift_code(ins);
    op2 = arm_read_register(p, rm);
    if(shift_imm || shift_code) {
		  bit4 = get_bit4(ins);
    	uint8_t shift_value;
    	if(!bit4) shift_value = shift_imm;
    	else {
				rs = get_rs(ins);
				shift_value = arm_read_register(p, rs);
		  }
		  op2 = shift(p, op2, shift_code, shift_value);
		}
        
    // Specific instruction call
    handler(p, rd, op1, op2, S);
    return result;
}


int arm_data_processing_immediate(arm_core p, uint32_t ins) {
    debug("arm_data_processing_immediate: %d\n", (int)ins);
    uint8_t rd, rn, rm, S, rs, shift_imm, shift_code, bit4, bit7;
		int op1, op2;
    /*
    	Attention : bit25 == 0 &&	bit7 == 1 && bit4 == 1 	=> load/store (ou MRS à voir de plus près)
    	cf. p.443, 144 et 146
    	=> appel arm_load_store_miscellaneous(arm_core p, uint32_t ins)
    */
    
    return UNDEFINED_INSTRUCTION;
}

