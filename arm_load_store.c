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
#include <assert.h>

///////////////////////////////////////////////////////////////////////////////
// Available instructions
///////////////////////////////////////////////////////////////////////////////

static int ldr(arm_core p, uint8_t rd, uint32_t address) {
    uint32_t value;
	int result = arm_read_word(p, address, &value);
    if (!result) {
        if (rd == 15) {
            value &= 0xFFFFFFFE;
            uint32_t cpsr = arm_read_cpsr(p);
            cpsr &= ~(1 << 5);
            cpsr |= ((value & 1) << 5);
            arm_write_cpsr(p, cpsr);
        }
        arm_write_register(p, rd, value);
    }
    return result;
}

static int str(arm_core p, uint8_t rd, uint32_t address) {
    uint32_t value = arm_read_register(p, rd);
    return arm_write_word(p, address, value);
}

static int ldrb(arm_core p, uint8_t rd, uint32_t address) {
    uint8_t value;
    int result = arm_read_byte(p, address, &value);
    if (!result)
        arm_write_register(p, rd, value);
    return result;
}

static int strb(arm_core p, uint8_t rd, uint32_t address) {
    uint8_t value = (arm_read_register(p, rd) & 0xFF);
    return arm_write_byte(p, address, value);
}

static int ldrt(arm_core p, uint8_t rd, uint32_t address) {
    uint32_t value;
    int result = arm_read_word(p, address, &value);
    if (!result)
        arm_write_register(p, rd, value);
    return result;
}

static int strt(arm_core p, uint8_t rd, uint32_t address) {
    uint32_t value = arm_read_register(p, rd);
    return arm_write_word(p, address, value);
}

static int ldrbt(arm_core p, uint8_t rd, uint32_t address) {
    uint8_t value;
    int result = arm_read_byte(p, address, &value);
    if (!result)
        arm_write_register(p, rd, (uint32_t)value);
    return result;
}

static int strbt(arm_core p, uint8_t rd, uint32_t address) {
    uint8_t value;
    int result = arm_read_byte(p, address, &value);
    if (!result)
        arm_write_register(p, rd, (uint32_t)value);
    return result;
}

static int ldrh(arm_core p, uint8_t rd, uint32_t address) {
    uint16_t value;
    int result = arm_read_half(p, address, &value);
    if (!result)
        arm_write_register(p, rd, (uint32_t)value);
    return result;
}

static int strh(arm_core p, uint8_t rd, uint32_t address) {
    uint16_t value = (arm_read_register(p, rd) & 0xFFFF);
    return arm_write_half(p, address, value);
}

static int ldrsh(arm_core p, uint8_t rd, uint32_t address) {
    uint32_t value;
    int result = arm_read_half(p, address, (uint16_t*)&value);
    if (!result) {
        if (value & (1<<15)) {
            value |= 0xFFFF0000; 
        } else {
            value &= 0X0000FFFFF;
        } 
        arm_write_register(p, rd, value);
    }
    return result;
}

static int ldrsb(arm_core p, uint8_t rd, uint32_t address) {
    uint8_t value;
    int result = arm_read_byte(p, address, &value);
    if (!result) {
        if (value & (1<<7)) {
            value |= 0xFFFFFF00; 
        } else {
            value &= 0X0000F00FF;
        }
        arm_write_register(p, rd, (uint32_t)value);
    }
    return result;
}

static int ldrd(arm_core p, uint8_t rd, uint32_t address) {
    if (is_odd(rd) || (rd == 14) || (address & 3)) {
        UNPREDICTABLE();
        return 0;
    }
    uint32_t value;
    int result = arm_read_word(p, address, &value);
    if (!result) arm_write_register(p, rd, value);
    if (!result) result = arm_read_word(p, address+4, &value);
    if (!result) arm_write_register(p, rd+1, value);
    return result;
}

static int strd(arm_core p, uint8_t rd, uint32_t address) {
    if (is_odd(rd) || (rd == 14) || (address & 3) || (address & 4)) {
        UNPREDICTABLE();
        return 0;
    }
    uint32_t value = arm_read_register(p, rd);
    int result = arm_write_word(p, address, value);
    if (!result) {
        value = arm_read_register(p, rd+1);
        result = arm_write_word(p, address+4, value);
    }
    return result;
}

static int ldm1(arm_core p, int16_t r_list, uint32_t s_add, uint32_t e_add) {
    uint32_t address = s_add;
    int i, result = 0;
	uint32_t value;
    
    for (i=0; i<=14 && !result; i++) {
        if (get_bit(r_list, i)) {
            result = arm_read_word(p, address, &value);
            if (result) arm_write_register(p, i, value);
            address += 4;
        }
    }
    
    if (!result && get_bit(r_list, 15)) {
        result = arm_read_word(p, address, &value);
        value &= 0xFFFFFFFE;
        arm_write_register(p, 15, value);
        uint32_t cpsr = arm_read_cpsr(p);
        cpsr &= ~(1 << 5);
        cpsr |= ((value & 1) << 5);
        arm_write_cpsr(p, cpsr);
        address += 4;
    }
    
    assert(e_add == (address - 4));
    return result;
}

static int stm1(arm_core p, int16_t r_list, uint32_t s_add, uint32_t e_add) {
    uint32_t address = s_add;
    int i, result = 0;
	uint32_t value;
    
    for (i=0; i<=15 && !result; i++) {
        if (get_bit(r_list, i)) {
            value = arm_read_register(p, i);
            result = arm_write_word(p, address, value);
            address += 4;
        }
    }
    
    assert(e_add == (address - 4));
    return result;
}

static int ldm2(arm_core p, int16_t r_list, uint32_t s_add, uint32_t e_add) {
    uint32_t address = s_add;
    int i, result = 0;
	uint32_t value;
    
    for (i=0; i<=14 && !result; i++) {
        if (get_bit(r_list, i)) {
            result = arm_read_word(p, address, &value);
            if (result) arm_write_usr_register(p, i, value);
            address += 4;
        }
    }
    
    assert(e_add == (address - 4));
    return result;
}

static int stm2(arm_core p, int16_t r_list, uint32_t s_add, uint32_t e_add) {
    uint32_t address = s_add;
    int i, result = 0;
	uint32_t value;
    
    for (i=0; i<=15 && !result; i++) {
        if (get_bit(r_list, i)) {
            value = arm_read_usr_register(p, i);
            result = arm_write_word(p, address, value);
            address += 4;
        }
    }
    
    assert(e_add == (address - 4));
    return result;
}

static int ldm3(arm_core p, int16_t r_list, uint32_t s_add, uint32_t e_add) {
    uint32_t address = s_add;
    int i, result = 0;
	uint32_t value;
    
    for (i=0; i<=14 && !result; i++) {
        if (get_bit(r_list, i)) {
            result = arm_read_word(p, address, &value);
            if (result) arm_write_register(p, i, value);
            address += 4;
        }
    }
    
    if (!result) {
        if (arm_current_mode_has_spsr(p)) {
            arm_write_cpsr(p, arm_read_spsr(p));
        } else {
            UNPREDICTABLE();
        }
        result = arm_read_word(p, address, &value);
        if (result) arm_write_register(p, 15, value);
        address += 4;
    }
    assert(e_add == (address - 4));
    return result;
}

///////////////////////////////////////////////////////////////////////////////
// Parsing of different classes
///////////////////////////////////////////////////////////////////////////////

// LDR, STR, LDRB, STRB, LDRT, STRT, LDRBT, STRBT
int arm_load_store(arm_core p, uint32_t ins) {
    debug("arm_load_store: %d\n", (int)ins);

    uint32_t address;
    int index, result;
    uint8_t rd = get_bits(ins, 15, 12);
    uint8_t rn = get_bits(ins, 19, 16);
    int val_rn = arm_read_register(p, rn);

    if (!get_bit(ins, 25)) { // Immmediate
        uint32_t offset = get_bits(ins, 11, 0);
        if (get_bit(ins, 24)) { // post_indexed
            address = val_rn;
            val_rn = (get_bit(ins, 23)) ? val_rn + offset : val_rn - offset;
            arm_write_register(p, rn, val_rn);
        } else if (get_bit(ins, 21)) { // pre_indexed
            address = (get_bit(ins, 23)) ? val_rn + offset : val_rn - offset;   
            arm_write_register(p, rn, address);           
        } else { // offset
            address = (get_bit(ins, 23)) ? val_rn + offset : val_rn - offset;
           }
    } else { // Registers
        int rm = get_bits(ins, 3, 0);
        int val_rm = arm_read_register(p, rm);
        int shift_val = get_bits(ins, 6, 5);
        int shift_imm = get_bits(ins, 11, 7);

        if (get_bit(ins, 24)) { // post_indexed
            address = val_rn;
            address = (get_bit(ins, 23)) ? val_rn + val_rm : val_rn - val_rm;
        } else if (get_bit(ins, 21)) { // pre_indexed
            index = shift(p, val_rm, shift_val, shift_imm, NULL);
            address = (get_bit(ins, 23)) ? val_rn + index : val_rn - index;
            arm_write_register(p, rn, address);
        } else { // offset and scaled offser
            index = shift(p, val_rm, shift_val, shift_imm, NULL);
            address = (get_bit(ins, 23)) ? val_rn + index : val_rn - index;   
        }
    }
    
    switch (codage3_bits(ins, 22, 21, 20)) {
        case 0 : result = str(p, rd, address);   break;
        case 1 : result = ldr(p, rd, address);   break;
        case 2 : result = strt(p, rd, address);  break;
        case 3 : result = ldrt(p, rd, address);  break;
        case 4 : result = strb(p, rd, address);  break;
        case 5 : result = ldrb(p, rd, address);  break;
        case 6 : result = strbt(p, rd, address); break;
        case 7 : result = ldrbt(p, rd, address); break;
        default: result =  0; break; // impossible
    }
    return result;
}

// LDRH, STRH, LDRSH, STRSH, LDRSB, STRSB, LDRD, STRD
int arm_load_store_miscellaneous(arm_core p, uint32_t ins) {
    debug("arm_store_miscellaneous: %d\n", (int)ins);
    
    uint32_t address;
    uint8_t rd = get_bits(ins, 15, 12);
    uint8_t rn = get_bits(ins, 19, 16);
    int val_rn = arm_read_register(p, rn);
    int offset_type = codage2_bits(ins, 24, 21);
    int offset, result;
    
    if (get_bit(ins, 22)) { // immediate
        offset = (get_bits(ins, 11, 8) << 4) | get_bits(ins, 3, 0);
    } else { // register
        uint8_t rm = get_bits(ins, 3, 0);
        offset = arm_read_register(p, rm);
    }
        
    if (offset_type == 0) { // post_indexed
        address = offset;
        address = (get_bit(ins, 23)) ? val_rn + offset : val_rn - offset;
    } else if (offset_type == 2) { // pre_indexed
        address = (get_bit(ins, 23)) ? val_rn + offset : val_rn - offset;
        arm_write_register(p, rn, address);
    } else { // offset
        address = (get_bit(ins, 23)) ? val_rn + offset : val_rn - offset;
    }
    
    switch (codage3_bits(ins, 20, 6, 5)) {
        case 1 : result = strh(p, rd, address); break;
        case 2 : result = ldrd(p, rd, address); break;
        case 3 : result = strd(p, rd, address); break;
        case 5 : result = ldrh(p, rd, address); break;
        case 6 : result = ldrsb(p, rd, address); break;
        case 7 : result = ldrsh(p, rd, address); break;
        default: result =  0; break; // impossible
    }
    return result;
}

// LDM(1), STM(1), LDM(2), STM(2), LDM(3), STM(3)
int arm_load_store_multiple(arm_core p, uint32_t ins) {
    debug("arm_load_store_multiple: %d\n", (int)ins);

    int result;
    uint8_t rn = get_bits(ins, 19, 16);
    int val_rn = arm_read_register(p, rn);
    int reg_list = get_bits(ins, 15, 0);
    int n = number_of_set_bits(reg_list);
    int32_t start_add, end_add;

    if (get_bit(ins, 23)) { // Increment
        if (get_bit(ins, 24)) { // before
            start_add = val_rn + 4;
            end_add = val_rn + (n * 4);
        } else { // after
            start_add = val_rn;
            end_add = val_rn + (n * 4) - 4;
        }
        if (get_bit(ins, 21)) {
            val_rn = val_rn + (n * 4);
            arm_write_register(p, rn, val_rn);
        }
    } else { // Decrement
        if (get_bit(ins, 24)) { // before
            start_add = val_rn - (n * 4);
            end_add = val_rn - 4;
        } else { // after
            start_add = val_rn - (n * 4) + 4;
            end_add = val_rn;
        }
        if (get_bit(ins, 21)) {
            val_rn = val_rn - (n * 4);
            arm_write_register(p, rn, val_rn);
        }
    }
    
    switch (codage3_bits(ins, 22, 20, 15)) {
        case 0 :
        case 1 : result = stm1(p, reg_list, start_add, end_add); break;
        case 2 :
        case 3 : result = ldm1(p, reg_list, start_add, end_add); break;
        case 4 : result = stm2(p, reg_list, start_add, end_add); break;
        case 6 : result = ldm2(p, reg_list, start_add, end_add); break;
        case 7 : result = ldm3(p, reg_list, start_add, end_add); break;
        default: result =  0; break; // impossible
    }
    return result;
}

// <opcode>{<cond>}{L} <coproc>,<CRd>,<addressing_mode>
int arm_coprocessor_load_store(arm_core p, uint32_t ins) {
    debug("arm_coprocessor_load_store: %d\n", (int)ins);
    debug("instruction is not implemented");
    return UNDEFINED_INSTRUCTION;
}

