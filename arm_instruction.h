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

#ifndef __ARM_INSTRUCTION_H__
#define __ARM_INSTRUCTION_H__

#include "arm_core.h"
         
// Execution of instructions
int arm_step(arm_core p);

// Condition of instructions
int instruction_check_condition(arm_core p, uint32_t ins);

// Handlers
typedef int(* instruction_handler_t)(arm_core, uint32_t);
instruction_handler_t instruction_get_handler(uint32_t ins);


uint32_t get_immediate(arm_core p, uint32_t ins, uint8_t* shift_C);
uint32_t get_shifted(arm_core p, uint32_t ins, uint8_t* shift_C);

#endif
