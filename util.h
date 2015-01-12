/*
Armator - simulateur de jeu d'instruction ARMv5T � but p�dagogique
Copyright (C) 2011 Guillaume Huard
Ce programme est libre, vous pouvez le redistribuer et/ou le modifier selon les
termes de la Licence Publique G�n�rale GNU publi�e par la Free Software
Foundation (version 2 ou bien toute autre version ult�rieure choisie par vous).

Ce programme est distribu� car potentiellement utile, mais SANS AUCUNE
GARANTIE, ni explicite ni implicite, y compris les garanties de
commercialisation ou d'adaptation dans un but sp�cifique. Reportez-vous � la
Licence Publique G�n�rale GNU pour plus de d�tails.

Vous devez avoir re�u une copie de la Licence Publique G�n�rale GNU en m�me
temps que ce programme ; si ce n'est pas le cas, �crivez � la Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307,
�tats-Unis.

Contact: Guillaume.Huard@imag.fr
         ENSIMAG - Laboratoire LIG
         51 avenue Jean Kuntzmann
         38330 Montbonnot Saint-Martin
*/

#ifndef __UTIL_H__
#define __UTIL_H__

#include <stdint.h>
#include "arm_core.h"

#define min(x,y) ((x)<(y)?(x):(y))
#define max(x,y) ((x)>(y)?(x):(y))

#define is_odd(x) ((rd & 1) == 1)
#define is_even(x) ((rd & 1) == 0)

#define get_bit(x, i) (((x)>>(i))&1)
#define set_bit(x, i) ((x)|(1<<(i)))
#define clr_bit(x, i) ((x)&~(1<<(i)))
#define chg_bit(x, i, v) ((x & (~(1<<i))) | ((v)?(1<<i):0))

#define get_bits(x, h, l) (((x)>>(l))&~(((~0)>>((h)-(l)+1))<<((h)-(l)+1)))
#define set_bits(x, h, l, bits) \
                 (((x)&~(((~0)>>(l))<<(l)))| \
                 ((x)&(((~0)>>((h)+1))<<((h)+1)))| \
                 ((bits)<<(l)))

#define codage2(x, y) ((x<<1) | y)
#define codage3(x, y, z) ((x<<2) | (y<<1) | z)

#define codage2_bits(v, x, y) (((((v)>>(x))&1)<<1) | (((v)>>(y))&1))
#define codage3_bits(v, x, y, z) (((((v)>>(x))&1)<<2) | \
                                 ((((v)>>(y))&1)<<1) | (((v)>>(z))&1))

#define check_mask(ins, m0, m1) (((ins) & (m0)) == 0 && ((ins) & (m1)) == (m1))

#define reverse_2(x) ((((x)&0xFF)<<8)|(((x)>>8)&0xFF))
#define reverse_4(x) ((((x)&0xFF)<<24)|((((x)>>8)&0xFF)<<16)|\
                      ((((x)>>16)&0xFF)<<8)|(((x)>>24)&0xFF))

int is_big_endian();

int number_of_set_bits(int bits_field);

uint32_t asr(uint32_t value, uint8_t shift);
uint32_t ror(uint32_t value, uint8_t rotation);
uint32_t sign_extend_24_to_30(int32_t value);

#endif

