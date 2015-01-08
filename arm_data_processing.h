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

#ifndef __ARM_DATA_PROCESSING_H__
#define __ARM_DATA_PROCESSING_H__

#include <stdint.h>
#include "arm_core.h"

#define AND 0
#define EOR 1
#define SUB 2
#define RSB 3
#define ADD 4
#define ADC 5
#define SBC 6
#define RSC 7
#define TST 8
#define TEQ 9
#define CMP 10
#define CMN 11
#define ORR 12
#define MOV 13
#define BIC 14
#define MVN 15

int arm_data_processing_shift(arm_core p, uint32_t ins);
int arm_data_processing_immediate(arm_core p, uint32_t ins);

#endif
