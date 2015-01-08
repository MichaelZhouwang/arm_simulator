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

#include "util.h"
#include "arm_core.h"

// We implement asr because shifting a signed is non portable in ANSI C
uint32_t asr(uint32_t value, uint8_t shift) {
    return (value >> shift) | (get_bit(value, 31) ? ~0<<(32-shift) : 0);
}

uint32_t ror(uint32_t value, uint8_t rotation) {
    return (value >> rotation) | (value << (32-rotation));
}

int shift(arm_core p, int op, int code, int value, uint8_t *shift_C) {
    uint8_t c;
	switch(code) {
		case 0: // LSL
			if (value == 0) {
			    c = arm_read_c(p);
			} else {
				if(value < 32) {
					c = get_bit(op, 32-value);
					op <<= value;
				} else {
					c = (value == 32) ? get_bit(op, 0) : 0;
					op = 0;
				}				
			}
			break;
			
		case 1: // LSR
			if (value == 0) {
			    c = arm_read_c(p);
			} else {
				if(value < 32) {
					c = get_bit(op, value-1);
					op >>= value;
				} else {
					c = (value == 32) ? get_bit(op, 31) : 0;
					op = 0;
				}				
			}
			break;
			
		case 2: // ASR
			if (value == 0) {
			    c = arm_read_c(p);
			} else {
				if(value < 32) {
					c = get_bit(op, value-1);
					op = asr(op, value);
				} else {
					c = get_bit(op,31);
					op = (get_bit(op,31)) ? : 0xFFFFFFFF;
				}				
			}			
			break;
			
		case 3: 
			if (!value) { // RRX
				c = get_bit(op,0);
				op = (arm_read_c(p) << 31) | (op >> 1);
			} else { // ROR
				if (!get_bit(value,4,0)) {
				    c = get_bit(op, 31);
				} else {
					c = get_bit(op, get_bit(value,4,0) - 1);
					op = ror(op, value);
				}
			}
			break;
	}
	if (shift_C) *shift_C = c;
	return op;
}

int number_of_set_bits(int bits_field) {
	int i;
	int n = 0;
	for(i=0; i<16; i++){
		if(get_bit(bits_field, i)) {
			n++;
		}
	}
	return n;
}

int is_big_endian() {
    static uint32_t one = 1;
    return ((* (uint8_t *) &one) == 0);
}

