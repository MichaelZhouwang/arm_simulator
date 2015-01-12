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

///////////////////////////////////////////////////////////////////////////////
// Helpers
///////////////////////////////////////////////////////////////////////////////

static uint32_t dp_lsl(arm_core p, uint32_t op, uint8_t value, uint8_t *shift_carry_out) {
	if (value == 0) {
	    *shift_carry_out = arm_read_c(p);
    } else {
        if(value < 32) {
            *shift_carry_out = get_bit(op, 32-value);
            op <<= value;
        } else {
            *shift_carry_out = (value == 32) ? get_bit(op, 0) : 0;
            op = 0;
        }				
    }
	return op;
}

static uint32_t dp_lsr(arm_core p, uint32_t op, uint8_t value, uint8_t *shift_carry_out) {
    if (value == 0) {
	    *shift_carry_out = arm_read_c(p);
	} else {
	    if (value < 32) {
		    *shift_carry_out = get_bit(op, value-1);
			op >>= value;
	    } else {
            *shift_carry_out = (value == 32) ? get_bit(op, 31) : 0;
	        op = 0;
		}				
	}
	return op;
}

static uint32_t dp_asr(arm_core p, uint32_t op, uint8_t value, uint8_t *shift_carry_out) {
    if (value == 0) {
	    *shift_carry_out = arm_read_c(p);
	} else {
		if(value < 32) {
			*shift_carry_out = get_bit(op, value-1);
			op = asr(op, value);
	    } else {
			*shift_carry_out = get_bit(op,31);
			op = (get_bit(op,31)) ? 0xFFFFFFFF : 0;
		}				
	}
	return op;
}

static uint32_t dp_ror_rrx(arm_core p, uint32_t op, uint8_t value, uint8_t *shift_carry_out) {
    if (value == 0) { // RRX
		//debug("C = %d\n", arm_read_c(p)); // A SUPPRIMER
		*shift_carry_out = get_bit(op,0);
	    op = (arm_read_c(p) << 31) | (op >> 1);
	} else { // ROR
		if (!get_bits(value,4,0)) {
	        *shift_carry_out = get_bit(op, 31);
	    } else {
		    *shift_carry_out = get_bit(op, get_bits(value,4,0) - 1);
			op = ror(op, value);
		}
	}
	return op;
}

uint32_t get_immediate(arm_core p, uint32_t ins, uint8_t *shift_carry_out) {
	uint32_t imm_8 = get_bits(ins,7,0);
	uint8_t rotate_imm = get_bits(ins,11,8);
	uint32_t res = ror(imm_8,(rotate_imm * 2));

	if (shift_carry_out) {
		if(rotate_imm == 0)
			*shift_carry_out = arm_read_c(p);
		else 
			*shift_carry_out = get_bit(res,31);
	}
	return res;
}

uint32_t get_shifted(arm_core p, uint32_t ins, uint8_t* shift_carry_out) {
    uint8_t shift_imm = get_bits(ins,11,7);
	uint8_t shift_code = get_bits(ins,6,5);
    uint32_t res = arm_read_register(p, get_bits(ins,3,0));
    if(!shift_imm && !shift_code) { // No shift
    	*shift_carry_out = arm_read_c(p);
    }
    else {
    	uint8_t shift_value;
    	if (!get_bit(ins,4)) {
    	     shift_value = shift_imm;
    	} else {
            shift_value = arm_read_register(p, get_bits(ins,11,8));
        }
        
        switch(shift_code) {
		    case 0: res = dp_lsl(p, res, shift_value, shift_carry_out);     break;
		    case 1: res = dp_lsr(p, res, shift_value, shift_carry_out);     break;
		    case 2: res = dp_asr(p, res, shift_value, shift_carry_out);     break;
		    case 3: res = dp_ror_rrx(p, res, shift_value, shift_carry_out); break;
		    default: break;
	    }
    }
    return res;
}


int overflow_from_add(uint32_t op1, uint32_t op2, uint64_t res) {
    return get_bit(op1, 31) == get_bit(op2, 31) &&
		   get_bit(op1, 31) != get_bit(res, 31);
}

int overflow_from_sub(uint32_t op1, uint32_t op2, uint64_t res) {
    return get_bit(op1, 31) != get_bit(op2, 31) &&
		   get_bit(op1, 31) != get_bit(res, 31);
}

///////////////////////////////////////////////////////////////////////////////
// Available instructions
///////////////////////////////////////////////////////////////////////////////

static int and(arm_core p, uint8_t rd, uint32_t op1, uint32_t op2, uint8_t s,
                 uint8_t shift_carry_out) {
	debug("AND%s %s 0x%x 0x%x\n", s ? "S" : "", arm_get_register_name(rd), op1, op2);
	uint64_t res = op1 & op2;
	if (s) {
		if (rd != 15) {
		    update_nzcv(p, 
		                get_bit(res ,31), // Z
		                (res == 0),       // N
		                shift_carry_out,  // C
		                UNAFFECT_FLAG);   // V
		} else {
			if(arm_current_mode_has_spsr(p)) {
				arm_write_cpsr(p, arm_read_spsr(p));
			} else {
				UNPREDICTABLE();
			}
		}
	}
	arm_write_register(p, rd, res);
	return 0;
}

static int eor(arm_core p, uint8_t rd, uint32_t op1, uint32_t op2, uint8_t s,
                 uint8_t shift_carry_out) {
	debug("EOR%s %s 0x%x 0x%x\n", s ? "S" : "", arm_get_register_name(rd), op1, op2);
    uint64_t res = op1 ^ op2;
	if (s) {
		if (rd != 15) {
		    update_nzcv(p,
		                get_bit(res ,31), // N
		                (res == 0),       // Z
		                shift_carry_out,  // C
		                UNAFFECT_FLAG);   // V
		} else {
			if(arm_current_mode_has_spsr(p)) {
				arm_write_cpsr(p, arm_read_spsr(p));
			} else {
				UNPREDICTABLE();
			}
		}
	}
	arm_write_register(p, rd, res);
	return 0;
}

static int sub(arm_core p, uint8_t rd, uint32_t op1, uint32_t op2, uint8_t s,
                 uint8_t shift_carry_out) {
	debug("SUB%s %s 0x%x 0x%x\n", s ? "S" : "", arm_get_register_name(rd), op1, op2);
	uint64_t res = (uint64_t)op1 - (uint64_t)op2;
	if (s) {
		if (rd != 15) {
		    update_nzcv(p,
		                get_bit(res, 31),                  // N
		                (res == 0),                        // Z
		                (op1 >= op2),                      // C
		                overflow_from_sub(op1, op2, res)); // V
		} else {
			if (arm_current_mode_has_spsr(p)) {
				arm_write_cpsr(p,arm_read_spsr(p));
			} else {
				UNPREDICTABLE();
			}
		}
	}
	arm_write_register(p, rd, res);
	return 0;
}

static int rsb(arm_core p, uint8_t rd, uint32_t op1, uint32_t op2, uint8_t s,
                 uint8_t shift_carry_out) {
	debug("RSB%s %s 0x%x 0x%x\n", s ? "S" : "", arm_get_register_name(rd), op1, op2);
	uint64_t res = (uint64_t)op2 - (uint64_t)op1;
	if (s) {
		if (rd != 15) {
		    update_nzcv(p,
		                get_bit(res, 31),                  // N
		                (res == 0),                        // Z
		                (op2 >= op1),                      // C
		                overflow_from_sub(op2, op1, res)); // V
		} else {
			if (arm_current_mode_has_spsr(p)) {
				arm_write_cpsr(p,arm_read_spsr(p));
			} else {
				UNPREDICTABLE();
			}
		}
	}
	arm_write_register(p, rd, res);
	return 0;
}

static int add(arm_core p, uint8_t rd, uint32_t op1, uint32_t op2, uint8_t s,
                 uint8_t shift_carry_out) {
	debug("ADD%s %s 0x%x 0x%x\n", s ? "S" : "", arm_get_register_name(rd), op1, op2);
	uint64_t res = (uint64_t)op1 + (uint64_t)op2;
	if (s) {
		if (rd != 15) {
		    update_nzcv(p, 
		                get_bit(res, 31),                      // N
		                (res == 0),                            // Z
		                (res > 0xFFFFFFFF),                    // C
		                overflow_from_add(op1, op2, res));     // V
		} else {
			if (arm_current_mode_has_spsr(p)) {
				arm_write_cpsr(p,arm_read_spsr(p));
			} else {
				UNPREDICTABLE();
			}
		}
	}
	arm_write_register(p, rd, res);
	return 0;
}

static int adc(arm_core p, uint8_t rd, uint32_t op1, uint32_t op2, uint8_t s,
               uint8_t shift_carry_out) {
	debug("ADC%s %s 0x%x 0x%x\n", s ? "S" : "", arm_get_register_name(rd), op1, op2);
    int c = arm_read_c(p);
	uint64_t res = (uint64_t)op1 + (uint64_t)op2 + (uint64_t)c;
	if (s) {
		if (rd != 15) {
			update_nzcv(p, 
						get_bit(res, 31),                  // N
		                (res == 0),                        // Z
		                (res > UINT_MAX),                  // C
		                overflow_from_add(op1, op2, res)); // V
		}
		else {
			if(arm_current_mode_has_spsr(p)) {
				arm_write_cpsr(p,arm_read_spsr(p));
			} else {
				UNPREDICTABLE();
			}
		}
	}
	arm_write_register(p, rd, res);
	return 0;
}

static int sbc(arm_core p, uint8_t rd, uint32_t op1, uint32_t op2, uint8_t s,
                 uint8_t shift_carry_out) {
	debug("SBC%s %s 0x%x 0x%x\n", s ? "S" : "", arm_get_register_name(rd), op1, op2);
    int c = arm_read_c(p);
	uint64_t res = (uint64_t)op1 - (uint64_t)op2 - !(uint64_t)c;
	if (s) {
		if(rd != 15){
		    update_nzcv(p,
						get_bit(res, 31),			       // N
		                (res == 0),						   // Z
		                (op1 >= (op2+c)),				   // C
		                overflow_from_sub(op1, op2, res)); // V
		} else {
			if (arm_current_mode_has_spsr(p)) {
				arm_write_cpsr(p,arm_read_spsr(p));
			} else {
				UNPREDICTABLE();
			}
		}
	}
	arm_write_register(p, rd, res);
	return 0;
}

static int rsc(arm_core p, uint8_t rd, uint32_t op1, uint32_t op2, uint8_t s,
                 uint8_t shift_carry_out) {
	debug("RSC%s %s 0x%x 0x%x\n", s ? "S" : "", arm_get_register_name(rd), op1, op2);
    int c = arm_read_c(p);
	uint64_t res = (uint64_t)op2 - (uint64_t)op1 - !(uint64_t)c;
	if (s) {
		if (rd != 15) {
		    update_nzcv(p,
						get_bit(res, 31),			       // N
		                (res == 0),						   // Z
		                (op2 >= (op1+c)),				   // C
		                overflow_from_sub(op2, op1, res)); // V
		}
		else {
			if (arm_current_mode_has_spsr(p)) {
				arm_write_cpsr(p,arm_read_spsr(p));
			} else {
				UNPREDICTABLE();
			}
		}
	}
	arm_write_register(p, rd, res);
	return 0;
}

static int tst(arm_core p, uint8_t rd, uint32_t op1, uint32_t op2, uint8_t s,
                 uint8_t shift_carry_out) {
	debug("TST 0x%x 0x%x\n", op1, op2);
	uint64_t res = op1 & op2;
	update_nzcv(p, get_bit(res, 31), (res == 0), shift_carry_out, UNAFFECT_FLAG);
	return 0;
}

static int teq(arm_core p, uint8_t rd, uint32_t op1, uint32_t op2, uint8_t s,
                 uint8_t shift_carry_out) {
	debug("TEQ 0x%x 0x%x\n", op1, op2);
	uint64_t res = op1 ^ op2;
	update_nzcv(p, get_bit(res, 31), (res == 0), shift_carry_out, UNAFFECT_FLAG);
	return 0;
}

static int cmp(arm_core p, uint8_t rd, uint32_t op1, uint32_t op2, uint8_t s,
                 uint8_t shift_carry_out) {
	debug("CMP 0x%x 0x%x\n", op1, op2);
	uint64_t res = (uint64_t)op1 - (uint64_t)op2;
	update_nzcv(p,
	            get_bit(res, 31),                  // N
		        (res == 0),                        // Z
		        (op1 >=  op2),                     // C
		        overflow_from_sub(op1, op2, res)); // V
	return 0;
}

static int cmn(arm_core p, uint8_t rd, uint32_t op1, uint32_t op2, uint8_t s,
                 uint8_t shift_carry_out) {
	debug("CMN 0x%x 0x%x\n", op1, op2);
	uint64_t res = (uint64_t)op1 + (uint64_t)op2;
	update_nzcv(p,
	            get_bit(res, 31),                  // N
		        (res == 0),                        // Z
		        (res > UINT_MAX),                  // C
		        overflow_from_add(op1, op2, res)); // V	
	return 0;
}

static int orr(arm_core p, uint8_t rd, uint32_t op1, uint32_t op2, uint8_t s,
                 uint8_t shift_carry_out) {
	debug("ORR%s %s 0x%x 0x%x\n", s ? "S" : "", arm_get_register_name(rd), op1, op2);
	uint64_t res = op1 | op2;
	if (s) {
		if(rd != 15) {
	        update_nzcv(p,
	                    get_bit(res, 31), // N
	                    (res == 0),       // Z
	                    shift_carry_out,  // C
	                    UNAFFECT_FLAG);   // V
		} else {
			if (arm_current_mode_has_spsr(p)) {
				arm_write_cpsr(p,arm_read_spsr(p));
			} else {
				UNPREDICTABLE();
			}
		}
	}
	arm_write_register(p, rd, res);
	return 0;
}

static int mov(arm_core p, uint8_t rd, uint32_t op1, uint32_t op2, uint8_t s,
                 uint8_t shift_carry_out) {
	debug("MOV%s %s 0x%x\n", s ? "S" : "", arm_get_register_name(rd), op2);
	uint64_t res = op2;
	if (s) {
		if (rd != 15) {
	        update_nzcv(p,
	                    get_bit(res, 31), // N
	                    (res == 0),       // Z
	                    shift_carry_out,  // C
	                    UNAFFECT_FLAG);   // V
		} else {
			if (arm_current_mode_has_spsr(p)) {
				arm_write_cpsr(p,arm_read_spsr(p));
			} else {
				UNPREDICTABLE();
			}
		}
	}
	arm_write_register(p, rd, res);
	return 0;
}

static int bic(arm_core p, uint8_t rd, uint32_t op1, uint32_t op2, uint8_t s,
                 uint8_t shift_carry_out) {
	debug("BIC%s %s 0x%x 0x%x\n", s ? "S" : "", arm_get_register_name(rd), op1, op2);
	uint64_t res = op1 & ~op2;
	if (s) {
		if (rd != 15) {
		    update_nzcv(p,
	                    get_bit(res, 31), // N
	                    (res == 0),       // Z
	                    shift_carry_out,  // C
	                    UNAFFECT_FLAG);   // V
		} else {
			if (arm_current_mode_has_spsr(p)) {
				arm_write_cpsr(p,arm_read_spsr(p));
			} else {
				UNPREDICTABLE();
			}
		}
	}
	arm_write_register(p, rd, res);
	return 0;
}

static int mvn(arm_core p, uint8_t rd, uint32_t op1, uint32_t op2, uint8_t s,
                 uint8_t shift_carry_out) {
	debug("MVN%s %s 0x%x\n", s ? "S" : "", arm_get_register_name(rd), op2);
	uint64_t res = ~op2;
	if (s) {
		if (rd != 15) {
		    update_nzcv(p,
	                    get_bit(res, 31), // N
	                    (res == 0),       // Z
	                    shift_carry_out,  // C
	                    UNAFFECT_FLAG);   // V
		} else {
			if(arm_current_mode_has_spsr(p)) {
				arm_write_cpsr(p,arm_read_spsr(p));
			} else {
				UNPREDICTABLE();
			}
		}
	}
	arm_write_register(p, rd, res);
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// Operation code and handlers
///////////////////////////////////////////////////////////////////////////////

typedef int(* dp_op_handler_t)(arm_core, uint8_t, uint32_t, uint32_t,
     uint8_t, uint8_t);

typedef enum {
	AND,
	EOR,
	SUB,
	RSB,
	ADD,
	ADC,
	SBC,
	RSC,
	TST,
	TEQ,
	CMP,
	CMN,
	ORR,
	MOV,
	BIC,
	MVN
} op_code_t;

dp_op_handler_t arm_get_dp_handler(int32_t ins) {
	dp_op_handler_t handler;
	switch(get_bits(ins, 24, 21)) {
		case AND: handler = and;  break;
		case EOR: handler = eor;  break;
		case SUB: handler = sub;  break;
		case RSB: handler = rsb;  break;
		case ADD: handler = add;  break;
		case ADC: handler = adc;  break;
		case SBC: handler = sbc;  break;
		case RSC: handler = rsc;  break;
		case TST: handler = tst;  break;
		case TEQ: handler = teq;  break;
		case CMP: handler = cmp;  break;
		case CMN: handler = cmn;  break;
		case ORR: handler = orr;  break;
		case MOV: handler = mov;  break;
		case BIC: handler = bic;  break;
		case MVN: handler = mvn;  break;
		default:  handler = NULL; break;
	}
	return handler;
}

///////////////////////////////////////////////////////////////////////////////
// Parsing of different classes
///////////////////////////////////////////////////////////////////////////////

// Data processing instruction parsing

static inline int get_op_code(uint32_t ins) {
	return get_bits(ins, 24, 21);
}

static inline uint8_t get_rd(uint32_t ins) {
	return get_bits(ins, 15, 12);
}
static inline uint8_t get_rn(uint32_t ins) {
	return get_bits(ins, 19, 16);
}
static inline uint8_t get_S(uint32_t ins) {
	return get_bit(ins, 20);
}

int arm_data_processing_shift(arm_core p, uint32_t ins) {
    debug("arm_data_processing_shift\n");    
    
	uint8_t rd, s, shift_carry_out;
	uint32_t op1, op2;
    
    dp_op_handler_t handler = arm_get_dp_handler(ins);
    
    rd = get_rd(ins);

	int op_code = get_op_code(ins);
    
    if(op_code != MOV && op_code != MVN) {
    	op1 = arm_read_register(p, get_rn(ins));
    } else {
    	op1 = 0;
    }
    
    op2 = get_shifted(p, ins, &shift_carry_out);
    
    s = get_S(ins);

	debug("opcode:0x%x S:%d rd:%s op1:%d op2:%d\n", op_code, s, arm_get_register_name(rd), op1, op2);
    
    return (handler) ? handler(p, rd, op1, op2, s, shift_carry_out) : 0;
}


int arm_data_processing_immediate(arm_core p, uint32_t ins) {
	debug("arm_data_processing_immediate\n");

	uint8_t rd, s, shift_carry_out;
	uint32_t op1, op2;

	dp_op_handler_t handler = arm_get_dp_handler(ins);
	
	op2 = get_immediate(p, ins, &shift_carry_out);	

	rd = get_rd(ins);

	int op_code = get_op_code(ins);

	if(op_code != MOV && op_code != MVN) {
		op1 = arm_read_register(p, get_rn(ins));
	} else {
		op1 = 0;
	}
	
	s = get_S(ins);

	debug("opcode:0x%x S:%d rd:%s op1:%d op2:%d\n", op_code, s, arm_get_register_name(rd), op1, op2);

	return (handler) ? handler(p, rd, op1, op2, s, shift_carry_out) : 0;
}




// NE PAS SUPPRIMER TANT QUE LES TESTS SONT PAS PASSES

///////////////////////////////////////////////////////////////////////////////
// Shift operands
///////////////////////////////////////////////////////////////////////////////
/*
static void logical_shift_left_imm(arm_core p, uint32_t ins, uint32_t *op,
             uint32_t *shift_carry_out) {
	int shift_imm = get_bits(ins, 11, 7);
	int8_t rm = get_bits(ins, 3, 0);
	int32_t val_rm = arm_read_register(rm);

    if (shift_imm == 0) {
        *op = val_rm;
        *shift_carry_out = arm_read_c(arm_core p);
    } else {
        op = val_rm >> shift_imm;
        *shift_carry_out = get_bit(val_rm, 32-shift_imm);
	}
}

static void logical_shift_left_reg(arm_core p, uint32_t ins, uint32_t *op,
             uint32_t *shift_carry_out) {
	int shift_imm = get_bits(ins, 11, 7);
	int8_t rm = get_bits(ins, 3, 0);
	int8_t rs = get_bits(ins, 11, 8)
	int32_t val_rm = arm_read_register(rm);
	int32_t val_rs = arm_read_register(rs);

    if (Rs[7:0] == 0) {
        *op = Rm
        *shift_carry_out = C Flag
    } else if (Rs[7:0] < 32) {
        *op = Rm Logical_Shift_Left Rs[7:0]
        *shift_carry_out = Rm[32 - Rs[7:0]]
    } else if (Rs[7:0] == 32) {
        *op = 0;
        *shift_carry_out = get_bit(val_rm, 0);
    } else {
        *op = 0;
        *shift_carry_out = 0;
	}
}

static void logical_shift_right_imm(arm_core p, uint32_t ins, uint32_t *op,
             uint32_t *shift_carry_out) {
    if shift_imm == 0 then
         = 0
        *shift_carry_out = Rm[31]
    else
        *op = Rm Logical_Shift_Right shift_imm
        *shift_carry_out = Rm[shift_imm - 1]
}

static void logical_shift_right_reg(arm_core p, uint32_t ins, uint32_t *op,
             uint32_t *shift_carry_out) {
    if Rs[7:0] == 0 then
        *op = Rm
        *shift_carry_out = C Flag
    else if Rs[7:0] < 32 then
        *op = Rm Logical_Shift_Right Rs[7:0]
        *shift_carry_out = Rm[Rs[7:0] - 1]
    else if Rs[7:0] == 32 then
        *op = 0
        *shift_carry_out = Rm[31]
    else
        *op = 0
        *shift_carry_out = 0
}
//
static void arithm_shift_right_imm(arm_core p, uint32_t ins, uint32_t *op,
             uint32_t *shift_carry_out) {
    if shift_imm == 0 then
        if Rm[31] == 0 then
            *op = 0
            *shift_carry_out = Rm[31]
        else
            *op = 0xFFFFFFFF
            *shift_carry_out = Rm[31]
    else
        *op = Rm Arithmetic_Shift_Right <shift_imm>
        *shift_carry_out = Rm[shift_imm - 1]
}

static void arithm_shift_right_reg(arm_core p, uint32_t ins, uint32_t *op,
             uint32_t *shift_carry_out) {
    if Rs[7:0] == 0 then
        *op = Rm
        *shift_carry_out = C Flag
    else if Rs[7:0] < 32 then
        *op = Rm Arithmetic_Shift_Right Rs[7:0]
        *shift_carry_out = Rm[Rs[7:0] - 1]
    else
        if Rm[31] == 0 then
            *op = 0
            *shift_carry_out = Rm[31]
        else
            *op = 0xFFFFFFFF
            *shift_carry_out = Rm[31]
}

static void rotate_right_imm(arm_core p, uint32_t ins, uint32_t *op,
             uint32_t *shift_carry_out) {
    if shift_imm == 0 then
        *op = (C Flag Logical_Shift_Left 31) OR (Rm Logical_Shift_Right 1)
        *shift_carry_out = Rm[0]
    else 
        *op = Rm Rotate_Right shift_imm
        *shift_carry_out = Rm[shift_imm - 1]
}

static void rotate_right_reg(arm_core p, uint32_t ins, uint32_t *op,
             uint32_t *shift_carry_out) {
    if Rs[7:0] == 0 then
        *op = Rm
        *shift_carry_out = C Flag
    else if Rs[4:0] == 0 then
        *op = Rm
        *shift_carry_out = Rm[31]
    else
        *op = Rm Rotate_Right Rs[4:0]
        *shift_carry_out = Rm[Rs[4:0] - 1]
}
*/
///////////////////////////////////////////////////////////////////////////////
// Operation code and handlers
///////////////////////////////////////////////////////////////////////////////
/*
typedef void(* dp_shift_handler_t)(arm_core, uint32_t, uint32_t *, uint32_t *);

static dp_shift_handler_t arm_dp_get_shift_handler(int32_t ins) {
	dp_shift_handler_t handler;
	switch(get_bits(ins, 6, 4)) {
        case 0: handler = logical_shift_left_imm;  break;
        case 1: handler = logical_shift_left_reg;  break;
        case 2: handler = logical_shift_right_imm; break;
        case 3: handler = logical_shift_right_reg; break;
        case 4: handler = arithm_shift_right_imm;  break;
        case 5: handler = arithm_shift_right_reg;  break;
        case 6: handler = rotate_right_imm;        break;
        case 7: handler = rotate_right_reg;        break;
        default: handler = NULL; break; // impossible
	}
	return handler;
}
*/
///////////////////////////////////////////////////////////////////////////////
// Parsing of different classes
///////////////////////////////////////////////////////////////////////////////
/*
// complte handler arguments !!
int arm_data_processing_shift(arm_core p, uint32_t ins) {
    debug("arm_data_processing_shift: %d\n", (int)ins);
    
    int res = 0;
    dp_shift_handler_t shift_handler = arm_dp_get_shift_handler(ins);
    if (shift_handler) {
        int32_t *op, shift_carry_out;
        shift_handler(p, ins, &*op, &shift_carry_out);
        dp_op_handler_t op_handler = arm_dp_get_op_handler(ins);
        if (op_handler) {
            res = op_handler();
        }
    }
    
    return res;
}

int arm_data_processing_immediate(arm_core p, uint32_t ins) {
    debug("arm_data_processing_immediate: %d\n", (int)ins);
    
    *op = immed_8 Rotate_Right (rotate_imm * 2)
    if rotate_imm == 0 then
        *shift_carry_out = C flag
    else
        *shift_carry_out = *op[31]
    
    // HANDLING
    dp_op_handler_t op_handler = arm_dp_get_op_handler(ins);
    return (op_handler) ? op_handler() : 0;
}*/

