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
// Available instructions
///////////////////////////////////////////////////////////////////////////////

static int and(arm_core p, uint8_t rd, uint32_t op1, uint32_t op2, uint8_t s,
                 uint8_t shift_c) {
	debug("AND r%d %d %d\n", rd, op1, op2);
	uint64_t result = op1 & op2;
	if (s) {
		if (rd != 15) {
		    update_nzcv(p, get_bit(result ,31), !result, shift_c, -1);
		} else {
			if(arm_current_mode_has_spsr(p)) {
				arm_write_cpsr(p,arm_read_spsr(p));
			} else {
				UNPREDICTABLE();
			}
		}
	}
	arm_write_register(p, rd, result);
	return 0;
}

static int eor(arm_core p, uint8_t rd, uint32_t op1, uint32_t op2, uint8_t s,
                 uint8_t shift_c) {
	debug("EOR r%d %d %d\n", rd, op1, op2);
    uint64_t result = op1 ^ op2;
	if (s) {
		if (rd != 15) {
		    update_nzcv(p, get_bit(result ,31), !result, shift_c, -1);
		} else {
			if(arm_current_mode_has_spsr(p)) {
				arm_write_cpsr(p,arm_read_spsr(p));
			} else {
				UNPREDICTABLE();
			}
		}
	}
	arm_write_register(p, rd, result);
	return 0;
}

static int sub(arm_core p, uint8_t rd, uint32_t op1, uint32_t op2, uint8_t s,
                 uint8_t shift_c) {
	debug("SUB r%d %d %d\n", rd, op1, op2);
	uint64_t result = op1 - op2;
	if (s) {
		if (rd != 15) {
		    int overflow = get_bit(op1,31) != get_bit(op2,31) &&
		                   get_bit(op1,31) != get_bit(result,31);
		    update_nzcv(p, get_bit(result, 31), // N
		                !result,             // Z
		                !(op1<op2),          // C
		                overflow);           // V
		} else {
			if (arm_current_mode_has_spsr(p)) {
				arm_write_cpsr(p,arm_read_spsr(p));
			} else {
				UNPREDICTABLE();
			}
		}
	}
	arm_write_register(p, rd, result);
	return 0;
}

static int rsb(arm_core p, uint8_t rd, uint32_t op1, uint32_t op2, uint8_t s,
                 uint8_t shift_c) {
	debug("RSB r%d %d %d\n", rd, op1, op2);
	uint64_t result = op2 - op1;
	if (s) {
		if (rd != 15) {
		    int overflow = get_bit(op1,31) != get_bit(op2,31) &&
		                   get_bit(op2,31) != get_bit(result,31);
		    update_nzcv(p, get_bit(result, 31), // N
		                !result,             // Z
		                !(op2<op1),          // C
		                overflow);           // V
		} else {
			if (arm_current_mode_has_spsr(p)) {
				arm_write_cpsr(p,arm_read_spsr(p));
			} else {
				UNPREDICTABLE();
			}
		}
	}
	arm_write_register(p, rd, result);
	return 0;
}

static int add(arm_core p, uint8_t rd, uint32_t op1, uint32_t op2, uint8_t s,
                 uint8_t shift_c) {
	debug("ADD r%d %d %d\n", rd, op1, op2);
	uint64_t result = op1 + op2;
	if (s) {
		if (rd != 15) {
		    int overflow = get_bit(op1,31) == get_bit(op2,31) &&
		                   get_bit(op2,31) != get_bit(result,31);
		    update_nzcv(p, get_bit(result, 31), // N
		                !result,             // Z
		                (result > UINT_MAX), // C
		                overflow);           // V
		} else {
			if (arm_current_mode_has_spsr(p)) {
				arm_write_cpsr(p,arm_read_spsr(p));
			} else {
				UNPREDICTABLE();
			}
		}
	}
	arm_write_register(p, rd, result);
	return 0;
}

static int adc(arm_core p, uint8_t rd, uint32_t op1, uint32_t op2, uint8_t s,
               uint8_t shift_c) {
	debug("ADC r%d %d %d\n", rd, op1, op2);
    int c = arm_read_c(p);
	uint64_t result = op1 + op2 + c;
	if (s) {
		if (rd != 15) {
		    int overflow = get_bit(op1,31) == get_bit(op2,31) &&
		                   get_bit(op2,31) != get_bit(result,31);
			update_nzcv(p, 
						get_bit(result, 31), // N
		                !result,             // Z
		                (result > UINT_MAX), // C
		                overflow);           // V
		}
		else {
			if(arm_current_mode_has_spsr(p)) {
				arm_write_cpsr(p,arm_read_spsr(p));
			} else {
				UNPREDICTABLE();
			}
		}
	}
	arm_write_register(p, rd, result);
	return 0;
}

static int sbc(arm_core p, uint8_t rd, uint32_t op1, uint32_t op2, uint8_t s,
                 uint8_t shift_c) {
	debug("SBC r%d %d %d\n", rd, op1, op2);
    int c = arm_read_c(p);
	uint64_t result = op1 - op2 - !c;
	if (s) {
		if(rd != 15){
		    int overflow = get_bit(op1,31) != get_bit(op2,31) &&
		                   get_bit(op1,31) != get_bit(result,31);
		    update_nzcv(p,
						get_bit(result, 31),			// N
		                !result,						// Z
		                !(op1 < (op2+c)),				// C
		                overflow);						// V
		} else {
			if (arm_current_mode_has_spsr(p)) {
				arm_write_cpsr(p,arm_read_spsr(p));
			} else {
				UNPREDICTABLE();
			}
		}
	}
	arm_write_register(p, rd, result);
	return 0;
}

static int rsc(arm_core p, uint8_t rd, uint32_t op1, uint32_t op2, uint8_t s,
                 uint8_t shift_c) {
	debug("RSC r%d %d %d\n", rd, op1, op2);
    int c = arm_read_c(p);
	uint64_t result = op2 - op1 - !c;
	if (s) {
		if (rd != 15) {
		    int overflow = get_bit(op1,31) != get_bit(op2,31) &&
		                   get_bit(op2,31) != get_bit(result,31);
		    update_nzcv(p,
						get_bit(result, 31),			// N
		                !result,						// Z
		                !(op2 < (op1+c)),				// C
		                overflow);						// V
		}
		else {
			if (arm_current_mode_has_spsr(p)) {
				arm_write_cpsr(p,arm_read_spsr(p));
			} else {
				UNPREDICTABLE();
			}
		}
	}
	arm_write_register(p, rd, result);
	return 0;
}

static int tst(arm_core p, uint8_t rd, uint32_t op1, uint32_t op2, uint8_t s,
                 uint8_t shift_c) {
	debug("TST %d %d\n", op1, op2);
	uint64_t result = op1 & op2;
	update_nzcv(p, get_bit(result, 31), !result, shift_c, -1);
	return 0;
}

static int teq(arm_core p, uint8_t rd, uint32_t op1, uint32_t op2, uint8_t s,
                 uint8_t shift_c) {
	debug("TEQ %d %d\n", op1, op2);
	uint64_t result = op1 ^ op2;
	update_nzcv(p, get_bit(result, 31), !result, shift_c, -1);
	return 0;
}

static int cmp(arm_core p, uint8_t rd, uint32_t op1, uint32_t op2, uint8_t s,
                 uint8_t shift_c) {
	debug("CMP %d %d\n", op1, op2);
	uint64_t result = op1 - op2;
	int overflow = get_bit(op1,31) != get_bit(op2,31) &&
		           get_bit(op1,31) != get_bit(result, 31);
	update_nzcv(p, get_bit(result, 31), // N
		        !result,             // Z
		        !(op1 <  op2),       // C
		        overflow);           // V
	return 0;
}

static int cmn(arm_core p, uint8_t rd, uint32_t op1, uint32_t op2, uint8_t s,
                 uint8_t shift_c) {
	debug("CMN %d %d\n", op1, op2);
	uint64_t result = op1 + op2;
	int overflow = get_bit(op1,31) == get_bit(op2,31) &&
		           get_bit(op1,31) != get_bit(result, 31);
	update_nzcv(p, get_bit(result, 31), // N
		        !result,             // Z
		        (result > UINT_MAX), // C
		        overflow);           // V	
	return 0;
}

static int orr(arm_core p, uint8_t rd, uint32_t op1, uint32_t op2, uint8_t s,
                 uint8_t shift_c) {
	debug("ORR r%d %d %d\n", rd, op1, op2);
	uint64_t result = op1 | op2;
	if (s) {
		if(rd != 15) {
	        update_nzcv(p, get_bit(result, 31), !result, shift_c, -1);
		} else {
			if (arm_current_mode_has_spsr(p)) {
				arm_write_cpsr(p,arm_read_spsr(p));
			} else {
				UNPREDICTABLE();
			}
		}
	}
	arm_write_register(p, rd, result);
	return 0;
}

static int mov(arm_core p, uint8_t rd, uint32_t op1, uint32_t op2, uint8_t s,
                 uint8_t shift_c) {
	debug("MOV r%d %d\n", rd, op2);

	uint64_t result = op2;
	if (s) {
		if (rd != 15) {
	        update_nzcv(p, get_bit(result, 31), !result, shift_c, -1);
		} else {
			if (arm_current_mode_has_spsr(p)) {
				arm_write_cpsr(p,arm_read_spsr(p));
			} else {
				UNPREDICTABLE();
			}
		}
	}
	arm_write_register(p, rd, result);
	return 0;
}

static int bic(arm_core p, uint8_t rd, uint32_t op1, uint32_t op2, uint8_t s,
                 uint8_t shift_c) {
	debug("BIC r%d %d %d\n", rd, op1, op2);
	uint64_t result = op1 & ~op2;
	if (s) {
		if (rd != 15) {
		    update_nzcv(p, get_bit(result, 31), !result, shift_c, -1);
		} else {
			if (arm_current_mode_has_spsr(p)) {
				arm_write_cpsr(p,arm_read_spsr(p));
			} else {
				UNPREDICTABLE();
			}
		}
	}
	arm_write_register(p, rd, result);
	return 0;
}

static int mvn(arm_core p, uint8_t rd, uint32_t op1, uint32_t op2, uint8_t s,
                 uint8_t shift_c) {
	debug("MVN r%d %d\n", rd, op2);
	uint64_t result = ~op2;
	if (s) {
		if (rd != 15) {
		    update_nzcv(p, get_bit(result, 31), !result, shift_c, -1);
		} else {
			if(arm_current_mode_has_spsr(p)) {
				arm_write_cpsr(p,arm_read_spsr(p));
			} else {
				UNPREDICTABLE();
			}
		}
	}
	arm_write_register(p, rd, result);
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
		case AND:  handler = and;  break;
		case EOR:  handler = eor;  break;
		case SUB:  handler = sub;  break;
		case RSB:  handler = rsb;  break;
		case ADD:  handler = add;  break;
		case ADC:  handler = adc;  break;
		case SBC:  handler = sbc;  break;
		case RSC:  handler = rsc;  break;
		case TST:  handler = tst;  break;
		case TEQ:  handler = teq;  break;
		case CMP: handler = cmp;  break;
		case CMN: handler = cmn;  break;
		case ORR: handler = orr;  break;
		case MOV: handler = mov;  break;
		case BIC: handler = bic;  break;
		case MVN: handler = mvn;  break;
		default: handler = NULL; break;
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
    
	uint8_t rd, s, shift_C;
	uint32_t op1, op2;
    
    dp_op_handler_t handler = arm_get_dp_handler(ins);
    
    rd = get_rd(ins);

	int op_code = get_op_code(ins);
    
    if(op_code != MOV && op_code != MVN) {
    	op1 = arm_read_register(p, get_rn(ins));
    } else {
    	op1 = 0;
    }
    
    op2 = get_shifted(p, ins, &shift_C);
    
    s = get_S(ins);

	debug("opcode:%x S:%d rd:r%d op1:%d op2:%d\n", op_code, s, rd, op1, op2);
    
    return (handler) ? handler(p, rd, op1, op2, s, shift_C) : 0;
}


int arm_data_processing_immediate(arm_core p, uint32_t ins) {
	debug("arm_data_processing_immediate\n");

	uint8_t rd, s, shift_C;
	uint32_t op1, op2;

	dp_op_handler_t handler = arm_get_dp_handler(ins);
	
	op2 = get_immediate(p, ins, &shift_C);	

	rd = get_rd(ins);

	int op_code = get_op_code(ins);

	if(op_code != MOV && op_code != MVN) {
		op1 = arm_read_register(p, get_rn(ins));
	} else {
		op1 = 0;
	}
	
	s = get_S(ins);

	debug("opcode:%x S:%d rd:r%d op1:%d op2:%d\n", op_code, s, rd, op1, op2);

	return (handler) ? handler(p, rd, op1, op2, s, shift_C) : 0;
}




// NE PAS SUPPRIMER TANT QUE LES TESTS SONT PAS PASSES


///////////////////////////////////////////////////////////////////////////////
// Shift operands
///////////////////////////////////////////////////////////////////////////////
/*
static void logical_shift_left_imm(arm_core p, uint32_t ins, uint32_t *op,
             uint32_t *carry) {
	int shift_imm = get_bits(ins, 11, 7);
	int8_t rm = get_bits(ins, 3, 0);
	int32_t val_rm = arm_read_register(rm);

    if (shift_imm == 0) {
        *op = val_rm;
        *carry = arm_read_c(arm_core p);
    } else {
        op = val_rm >> shift_imm;
        *carry = get_bit(val_rm, 32-shift_imm);
	}
}

static void logical_shift_left_reg(arm_core p, uint32_t ins, uint32_t *op,
             uint32_t *carry) {
	int shift_imm = get_bits(ins, 11, 7);
	int8_t rm = get_bits(ins, 3, 0);
	int8_t rs = get_bits(ins, 11, 8)
	int32_t val_rm = arm_read_register(rm);
	int32_t val_rs = arm_read_register(rs);

    if (Rs[7:0] == 0) {
        *op = Rm
        *carry = C Flag
    } else if (Rs[7:0] < 32) {
        *op = Rm Logical_Shift_Left Rs[7:0]
        *carry = Rm[32 - Rs[7:0]]
    } else if (Rs[7:0] == 32) {
        *op = 0;
        *carry = get_bit(val_rm, 0);
    } else {
        *op = 0;
        *carry = 0;
	}
}

static void logical_shift_right_imm(arm_core p, uint32_t ins, uint32_t *op,
             uint32_t *carry) {
    if shift_imm == 0 then
         = 0
        *carry = Rm[31]
    else
        *op = Rm Logical_Shift_Right shift_imm
        *carry = Rm[shift_imm - 1]
}

static void logical_shift_right_reg(arm_core p, uint32_t ins, uint32_t *op,
             uint32_t *carry) {
    if Rs[7:0] == 0 then
        *op = Rm
        *carry = C Flag
    else if Rs[7:0] < 32 then
        *op = Rm Logical_Shift_Right Rs[7:0]
        *carry = Rm[Rs[7:0] - 1]
    else if Rs[7:0] == 32 then
        *op = 0
        *carry = Rm[31]
    else
        *op = 0
        *carry = 0
}
//
static void arithm_shift_right_imm(arm_core p, uint32_t ins, uint32_t *op,
             uint32_t *carry) {
    if shift_imm == 0 then
        if Rm[31] == 0 then
            *op = 0
            *carry = Rm[31]
        else
            *op = 0xFFFFFFFF
            *carry = Rm[31]
    else
        *op = Rm Arithmetic_Shift_Right <shift_imm>
        *carry = Rm[shift_imm - 1]
}

static void arithm_shift_right_reg(arm_core p, uint32_t ins, uint32_t *op,
             uint32_t *carry) {
    if Rs[7:0] == 0 then
        *op = Rm
        *carry = C Flag
    else if Rs[7:0] < 32 then
        *op = Rm Arithmetic_Shift_Right Rs[7:0]
        *carry = Rm[Rs[7:0] - 1]
    else
        if Rm[31] == 0 then
            *op = 0
            *carry = Rm[31]
        else
            *op = 0xFFFFFFFF
            *carry = Rm[31]
}

static void rotate_right_imm(arm_core p, uint32_t ins, uint32_t *op,
             uint32_t *carry) {
    if shift_imm == 0 then
        *op = (C Flag Logical_Shift_Left 31) OR (Rm Logical_Shift_Right 1)
        *carry = Rm[0]
    else 
        *op = Rm Rotate_Right shift_imm
        *carry = Rm[shift_imm - 1]
}

static void rotate_right_reg(arm_core p, uint32_t ins, uint32_t *op,
             uint32_t *carry) {
    if Rs[7:0] == 0 then
        *op = Rm
        *carry = C Flag
    else if Rs[4:0] == 0 then
        *op = Rm
        *carry = Rm[31]
    else
        *op = Rm Rotate_Right Rs[4:0]
        *carry = Rm[Rs[4:0] - 1]
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
    
    int result = 0;
    dp_shift_handler_t shift_handler = arm_dp_get_shift_handler(ins);
    if (shift_handler) {
        int32_t *op, carry_out;
        shift_handler(p, ins, &*op, &carry_out);
        dp_op_handler_t op_handler = arm_dp_get_op_handler(ins);
        if (op_handler) {
            result = op_handler();
        }
    }
    
    return result;
}

int arm_data_processing_immediate(arm_core p, uint32_t ins) {
    debug("arm_data_processing_immediate: %d\n", (int)ins);
    
    *op = immed_8 Rotate_Right (rotate_imm * 2)
    if rotate_imm == 0 then
        *carry = C flag
    else
        *carry = *op[31]
    
    // HANDLING
    dp_op_handler_t op_handler = arm_dp_get_op_handler(ins);
    return (op_handler) ? op_handler() : 0;
}*/

