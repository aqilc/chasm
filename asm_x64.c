/*
 * asm_x64.c v1.1.0 - Aqil Contractor @aqilc 2025
 * Dual Licenced under MIT and Public Domain.
 *
 * This file includes all of the source for the "chasm" library macros and functions.
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <malloc.h>
#include "asm_x64.h"


typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;


struct x64LookupGeneralIns {
	char* name;
	unsigned int numactualins;
	struct x64LookupActualIns {
		uint8_t mem_oper; // Index of memory operand if there is one + 1
		uint8_t reg_oper; // Index of register operand if there is one + 1
		uint8_t rel_oper; // Index of relative jump operand if there is one + 1
		uint8_t vex_oper; // Index of VEX operand if there is one + 1
    uint8_t imm_oper; // Index of immediate operand if there is one + 1
    uint8_t is4_oper; // Index of 4-byte immediate operand if there is one + 1
		uint8_t rex;
		uint8_t oplen;
		uint8_t preflen;
		uint8_t vex; // opcode_map if specified
		uint8_t vex_byte;
    uint8_t modrm;
		bool modrmreq;
		bool modrmreg;
		bool preffered;
		uint8_t base_size;
		uint8_t arglen;
		uint32_t opcode;
		uint32_t prefixes;
		x64OperandType args[4];
	}* ins;
};
typedef struct x64LookupActualIns x64LookupActualIns;
typedef struct x64LookupGeneralIns x64LookupGeneralIns;

static const x64LookupGeneralIns x64Table[] = {
	{ "adc", 21, (struct x64LookupActualIns[]) { {
		.args = { AL, IMM8 }, .arglen = 2, .imm_oper = 2,
		.opcode = 0x14, .oplen = 1,
	}, {
		.args = { AX, IMM16 }, .arglen = 2, .imm_oper = 2,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x15, .oplen = 1,
	}, {
		.args = { EAX, IMM32 }, .arglen = 2, .imm_oper = 2,
		.opcode = 0x15, .oplen = 1,
	}, {
		.rex = 0x48,
		.args = { RAX, IMM32 }, .arglen = 2, .imm_oper = 2,
		.opcode = 0x15, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x10,
		.args = { R8 | M8, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0x80, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x10,
		.args = { R16 | M16, IMM16 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x81, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x10,
		.args = { R32 | M32, IMM32 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0x81, .oplen = 1,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x10,
		.args = { R64 | M64, IMM32 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0x81, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x10,
		.args = { R16 | M16, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0x83, .oplen = 1,
		.preffered = true,
	}, {
		.modrmreq = true, .modrm = 0x10,
		.args = { R32 | M32, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0x83, .oplen = 1,
		.preffered = true,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x10,
		.args = { R64 | M64, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0x83, .oplen = 1,
		.preffered = true,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R8 | M8, R8 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x10, .oplen = 1,
		.preffered = true,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R8 | M8, RH }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x10, .oplen = 1,
		.preffered = true,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R16 | M16, R16 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x11, .oplen = 1,
		.preffered = true,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32 | M32, R32 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x11, .oplen = 1,
		.preffered = true,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64 | M64, R64 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x11, .oplen = 1,
		.preffered = true,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R8, R8 | M8 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x12, .oplen = 1,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { RH, R8 | M8 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x12, .oplen = 1,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R16, R16 | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x13, .oplen = 1,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, R32 | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x13, .oplen = 1,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, R64 | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x13, .oplen = 1,
	} } },
	{ "add", 21, (struct x64LookupActualIns[]) { {
		.args = { AL, IMM8 }, .arglen = 2, .imm_oper = 2,
		.opcode = 0x04, .oplen = 1,
	}, {
		.args = { AX, IMM16 }, .arglen = 2, .imm_oper = 2,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x05, .oplen = 1,
	}, {
		.args = { EAX, IMM32 }, .arglen = 2, .imm_oper = 2,
		.opcode = 0x05, .oplen = 1,
	}, {
		.rex = 0x48,
		.args = { RAX, IMM32 }, .arglen = 2, .imm_oper = 2,
		.opcode = 0x05, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x0,
		.args = { R8 | M8, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0x80, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x0,
		.args = { R16 | M16, IMM16 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x81, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x0,
		.args = { R32 | M32, IMM32 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0x81, .oplen = 1,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x0,
		.args = { R64 | M64, IMM32 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0x81, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x0,
		.args = { R16 | M16, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0x83, .oplen = 1,
		.preffered = true,
	}, {
		.modrmreq = true, .modrm = 0x0,
		.args = { R32 | M32, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0x83, .oplen = 1,
		.preffered = true,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x0,
		.args = { R64 | M64, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0x83, .oplen = 1,
		.preffered = true,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R8 | M8, R8 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x00, .oplen = 1,
		.preffered = true,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R8 | M8, RH }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x00, .oplen = 1,
		.preffered = true,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R16 | M16, R16 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x01, .oplen = 1,
		.preffered = true,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32 | M32, R32 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x01, .oplen = 1,
		.preffered = true,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64 | M64, R64 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x01, .oplen = 1,
		.preffered = true,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R8, R8 | M8 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x02, .oplen = 1,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { RH, R8 | M8 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x02, .oplen = 1,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R16, R16 | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x03, .oplen = 1,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, R32 | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x03, .oplen = 1,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, R64 | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x03, .oplen = 1,
	} } },
	{ "addpd", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x580F, .oplen = 2,
	} } },
	{ "vaddpd", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x58, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x58, .oplen = 1,
	} } },
	{ "addps", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x580F, .oplen = 2,
	} } },
	{ "vaddps", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x78, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x58, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7c, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x58, .oplen = 1,
	} } },
	{ "addsd", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF2, .preflen = 1, .opcode = 0x580F, .oplen = 2,
	} } },
	{ "vaddsd", 1, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x7b, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M64 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x58, .oplen = 1,
	} } },
	{ "addss", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0x580F, .oplen = 2,
	} } },
	{ "vaddss", 1, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x7a, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M32 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x58, .oplen = 1,
	} } },
	{ "addsubpd", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xD00F, .oplen = 2,
	} } },
	{ "vaddsubpd", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xD0, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xD0, .oplen = 1,
	} } },
	{ "addsubps", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF2, .preflen = 1, .opcode = 0xD00F, .oplen = 2,
	} } },
	{ "vaddsubps", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x7b, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xD0, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7f, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xD0, .oplen = 1,
	} } },
	{ "aesdec", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xDE380F, .oplen = 3,
	} } },
	{ "vaesdec", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xDE, .oplen = 1,
	} } },
	{ "aesdeclast", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xDF380F, .oplen = 3,
	} } },
	{ "vaesdeclast", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xDF, .oplen = 1,
	} } },
	{ "aesenc", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xDC380F, .oplen = 3,
	} } },
	{ "vaesenc", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xDC, .oplen = 1,
	} } },
	{ "aesenclast", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xDD380F, .oplen = 3,
	} } },
	{ "vaesenclast", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xDD, .oplen = 1,
	} } },
	{ "aesimc", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xDB380F, .oplen = 3,
	} } },
	{ "vaesimc", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xDB, .oplen = 1,
	} } },
	{ "aeskeygenassist", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xDF3A0F, .oplen = 3,
	} } },
	{ "vaeskeygenassist", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 3, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xDF, .oplen = 1,
	} } },
	{ "and", 21, (struct x64LookupActualIns[]) { {
		.args = { AL, IMM8 }, .arglen = 2, .imm_oper = 2,
		.opcode = 0x24, .oplen = 1,
	}, {
		.args = { AX, IMM16 }, .arglen = 2, .imm_oper = 2,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x25, .oplen = 1,
	}, {
		.args = { EAX, IMM32 }, .arglen = 2, .imm_oper = 2,
		.opcode = 0x25, .oplen = 1,
	}, {
		.rex = 0x48,
		.args = { RAX, IMM32 }, .arglen = 2, .imm_oper = 2,
		.opcode = 0x25, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x20,
		.args = { R8 | M8, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0x80, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x20,
		.args = { R16 | M16, IMM16 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x81, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x20,
		.args = { R32 | M32, IMM32 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0x81, .oplen = 1,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x20,
		.args = { R64 | M64, IMM32 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0x81, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x20,
		.args = { R16 | M16, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0x83, .oplen = 1,
		.preffered = true,
	}, {
		.modrmreq = true, .modrm = 0x20,
		.args = { R32 | M32, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0x83, .oplen = 1,
		.preffered = true,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x20,
		.args = { R64 | M64, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0x83, .oplen = 1,
		.preffered = true,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R8 | M8, R8 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x20, .oplen = 1,
		.preffered = true,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R8 | M8, RH }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x20, .oplen = 1,
		.preffered = true,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R16 | M16, R16 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x21, .oplen = 1,
		.preffered = true,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32 | M32, R32 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x21, .oplen = 1,
		.preffered = true,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64 | M64, R64 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x21, .oplen = 1,
		.preffered = true,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R8, R8 | M8 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x22, .oplen = 1,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { RH, R8 | M8 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x22, .oplen = 1,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R16, R16 | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x23, .oplen = 1,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, R32 | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x23, .oplen = 1,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, R64 | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x23, .oplen = 1,
	} } },
	{ "andn", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x78, .modrmreq = true, .modrmreg = true,
		.args = { R32, R32, R32 | M32 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xF2, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0xf8, .modrmreq = true, .modrmreg = true,
		.args = { R64, R64, R64 | M64 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xF2, .oplen = 1,
	} } },
	{ "andpd", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x540F, .oplen = 2,
	} } },
	{ "vandpd", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x54, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x54, .oplen = 1,
	} } },
	{ "andps", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x540F, .oplen = 2,
	} } },
	{ "vandps", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x78, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x54, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7c, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x54, .oplen = 1,
	} } },
	{ "andnpd", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x550F, .oplen = 2,
	} } },
	{ "vandnpd", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x55, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x55, .oplen = 1,
	} } },
	{ "andnps", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x550F, .oplen = 2,
	} } },
	{ "vandnps", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x78, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x55, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7c, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x55, .oplen = 1,
	} } },
	{ "blendpd", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x0D3A0F, .oplen = 3,
	} } },
	{ "vblendpd", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 3, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128, IMM8 }, .arglen = 4, .imm_oper = 4, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x0D, .oplen = 1,
	}, {
		.vex = 0x80 | 3, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256, IMM8 }, .arglen = 4, .imm_oper = 4, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x0D, .oplen = 1,
	} } },
	{ "bextr", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x78, .modrmreq = true, .modrmreg = true,
		.args = { R32, R32 | M32, R32 }, .arglen = 3, .mem_oper = 2, .reg_oper = 1, .vex_oper = 3,
		.opcode = 0xF7, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0xf8, .modrmreq = true, .modrmreg = true,
		.args = { R64, R64 | M64, R64 }, .arglen = 3, .mem_oper = 2, .reg_oper = 1, .vex_oper = 3,
		.opcode = 0xF7, .oplen = 1,
	} } },
	{ "blendps", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x0C3A0F, .oplen = 3,
	} } },
	{ "vblendps", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 3, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128, IMM8 }, .arglen = 4, .imm_oper = 4, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x0C, .oplen = 1,
	}, {
		.vex = 0x80 | 3, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256, IMM8 }, .arglen = 4, .imm_oper = 4, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x0C, .oplen = 1,
	} } },
	{ "blendvpd", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128, XMM_0 }, .arglen = 3, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x15380F, .oplen = 3,
	} } },
	{ "vblendvpd", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 3, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128, XMM }, .arglen = 4, .mem_oper = 3, .reg_oper = 1, .is4_oper = 4, .vex_oper = 2,
		.opcode = 0x4B, .oplen = 1,
	}, {
		.vex = 0x80 | 3, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256, YMM }, .arglen = 4, .mem_oper = 3, .reg_oper = 1, .is4_oper = 4, .vex_oper = 2,
		.opcode = 0x4B, .oplen = 1,
	} } },
	{ "blendvps", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128, XMM_0 }, .arglen = 3, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x14380F, .oplen = 3,
	} } },
	{ "vblendvps", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 3, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128, XMM }, .arglen = 4, .mem_oper = 3, .reg_oper = 1, .is4_oper = 4, .vex_oper = 2,
		.opcode = 0x4A, .oplen = 1,
	}, {
		.vex = 0x80 | 3, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256, YMM }, .arglen = 4, .mem_oper = 3, .reg_oper = 1, .is4_oper = 4, .vex_oper = 2,
		.opcode = 0x4A, .oplen = 1,
	} } },
	{ "blsi", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x78, .modrmreq = true, .modrm = 0x18,
		.args = { R32, R32 | M32 }, .arglen = 2, .mem_oper = 2, .vex_oper = 1,
		.opcode = 0xF3, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0xf8, .modrmreq = true, .modrm = 0x18,
		.args = { R64, R64 | M64 }, .arglen = 2, .mem_oper = 2, .vex_oper = 1,
		.opcode = 0xF3, .oplen = 1,
	} } },
	{ "blsmsk", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x78, .modrmreq = true, .modrm = 0x10,
		.args = { R32, R32 | M32 }, .arglen = 2, .mem_oper = 2, .vex_oper = 1,
		.opcode = 0xF3, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0xf8, .modrmreq = true, .modrm = 0x10,
		.args = { R64, R64 | M64 }, .arglen = 2, .mem_oper = 2, .vex_oper = 1,
		.opcode = 0xF3, .oplen = 1,
	} } },
	{ "blsr", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x78, .modrmreq = true, .modrm = 0x8,
		.args = { R32, R32 | M32 }, .arglen = 2, .mem_oper = 2, .vex_oper = 1,
		.opcode = 0xF3, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0xf8, .modrmreq = true, .modrm = 0x8,
		.args = { R64, R64 | M64 }, .arglen = 2, .mem_oper = 2, .vex_oper = 1,
		.opcode = 0xF3, .oplen = 1,
	} } },
	{ "bsf", 3, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { R16, R16 | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xBC0F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, R32 | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xBC0F, .oplen = 2,
	}, {
		.rex = 0x48,
		.args = { R64, R64 | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xBC0F, .oplen = 2,
	} } },
	{ "bsr", 3, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { R16, R16 | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xBD0F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, R32 | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xBD0F, .oplen = 2,
	}, {
		.rex = 0x48,
		.args = { R64, R64 | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xBD0F, .oplen = 2,
	} } },
	{ "bswap", 2, (struct x64LookupActualIns[]) { {
		.args = { R32 }, .arglen = 1, .reg_oper = 1,
		.opcode = 0xC80F, .oplen = 2,
	}, {
		.rex = 0x48,
		.args = { R64 }, .arglen = 1, .reg_oper = 1,
		.opcode = 0xC80F, .oplen = 2,
	} } },
	{ "bt", 6, (struct x64LookupActualIns[]) { {
		.args = { R16 | M16, R16 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xA30F, .oplen = 2,
	}, {
		.args = { R32 | M32, R32 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0xA30F, .oplen = 2,
	}, {
		.rex = 0x48,
		.args = { R64 | M64, R64 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0xA30F, .oplen = 2,
	}, {
		.modrmreq = true, .modrm = 0x20,
		.args = { R16 | M16, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0xBA0F, .oplen = 2,
	}, {
		.modrmreq = true, .modrm = 0x20,
		.args = { R32 | M32, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0xBA0F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x20,
		.args = { R64 | M64, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0xBA0F, .oplen = 2,
	} } },
	{ "btc", 6, (struct x64LookupActualIns[]) { {
		.args = { R16 | M16, R16 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xBB0F, .oplen = 2,
	}, {
		.args = { R32 | M32, R32 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0xBB0F, .oplen = 2,
	}, {
		.rex = 0x48,
		.args = { R64 | M64, R64 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0xBB0F, .oplen = 2,
	}, {
		.modrmreq = true, .modrm = 0x38,
		.args = { R16 | M16, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0xBA0F, .oplen = 2,
	}, {
		.modrmreq = true, .modrm = 0x38,
		.args = { R32 | M32, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0xBA0F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x38,
		.args = { R64 | M64, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0xBA0F, .oplen = 2,
	} } },
	{ "btr", 6, (struct x64LookupActualIns[]) { {
		.args = { R16 | M16, R16 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xB30F, .oplen = 2,
	}, {
		.args = { R32 | M32, R32 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0xB30F, .oplen = 2,
	}, {
		.rex = 0x48,
		.args = { R64 | M64, R64 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0xB30F, .oplen = 2,
	}, {
		.modrmreq = true, .modrm = 0x30,
		.args = { R16 | M16, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0xBA0F, .oplen = 2,
	}, {
		.modrmreq = true, .modrm = 0x30,
		.args = { R32 | M32, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0xBA0F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x30,
		.args = { R64 | M64, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0xBA0F, .oplen = 2,
	} } },
	{ "bts", 6, (struct x64LookupActualIns[]) { {
		.args = { R16 | M16, R16 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xAB0F, .oplen = 2,
	}, {
		.args = { R32 | M32, R32 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0xAB0F, .oplen = 2,
	}, {
		.rex = 0x48,
		.args = { R64 | M64, R64 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0xAB0F, .oplen = 2,
	}, {
		.modrmreq = true, .modrm = 0x28,
		.args = { R16 | M16, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0xBA0F, .oplen = 2,
	}, {
		.modrmreq = true, .modrm = 0x28,
		.args = { R32 | M32, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0xBA0F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x28,
		.args = { R64 | M64, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0xBA0F, .oplen = 2,
	} } },
	{ "bzhi", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x78, .modrmreq = true, .modrmreg = true,
		.args = { R32, R32 | M32, R32 }, .arglen = 3, .mem_oper = 2, .reg_oper = 1, .vex_oper = 3,
		.opcode = 0xF5, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0xf8, .modrmreq = true, .modrmreg = true,
		.args = { R64, R64 | M64, R64 }, .arglen = 3, .mem_oper = 2, .reg_oper = 1, .vex_oper = 3,
		.opcode = 0xF5, .oplen = 1,
	} } },
	{ "call", 5, (struct x64LookupActualIns[]) { {
		.args = { REL32 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0xE8, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x10,
		.args = { R64 | M64 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xFF, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x18,
		.args = { FARPTR1616 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xFF, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x18,
		.args = { FARPTR1632 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xFF, .oplen = 1,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x18,
		.args = { FARPTR1664 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xFF, .oplen = 1,
	} } },
	{ "cbw", 1, (struct x64LookupActualIns[]) { {
		.prefixes = 0x66, .preflen = 1, .opcode = 0x98, .oplen = 1,
	} } },
	{ "cwde", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0x98, .oplen = 1,
	} } },
	{ "cdqe", 1, (struct x64LookupActualIns[]) { {
		.rex = 0x48,
		.opcode = 0x98, .oplen = 1,
	} } },
	{ "clc", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xF8, .oplen = 1,
	} } },
	{ "cld", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xFC, .oplen = 1,
	} } },
	{ "clflush", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x38,
		.args = { M8 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xAE0F, .oplen = 2,
	} } },
	{ "cli", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xFA, .oplen = 1,
	} } },
	{ "clts", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0x060F, .oplen = 2,
	} } },
	{ "cmc", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xF5, .oplen = 1,
	} } },
	{ "cmova", 3, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { R16, R16 | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x470F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, R32 | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x470F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, R64 | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x470F, .oplen = 2,
	} } },
	{ "cmovae", 3, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { R16, R16 | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x430F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, R32 | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x430F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, R64 | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x430F, .oplen = 2,
	} } },
	{ "cmovb", 3, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { R16, R16 | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x420F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, R32 | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x420F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, R64 | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x420F, .oplen = 2,
	} } },
	{ "cmovbe", 3, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { R16, R16 | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x460F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, R32 | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x460F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, R64 | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x460F, .oplen = 2,
	} } },
	{ "cmovc", 3, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { R16, R16 | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x420F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, R32 | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x420F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, R64 | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x420F, .oplen = 2,
	} } },
	{ "cmove", 3, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { R16, R16 | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x440F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, R32 | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x440F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, R64 | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x440F, .oplen = 2,
	} } },
	{ "cmovg", 3, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { R16, R16 | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x4F0F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, R32 | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x4F0F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, R64 | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x4F0F, .oplen = 2,
	} } },
	{ "cmovge", 3, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { R16, R16 | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x4D0F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, R32 | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x4D0F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, R64 | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x4D0F, .oplen = 2,
	} } },
	{ "cmovl", 3, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { R16, R16 | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x4C0F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, R32 | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x4C0F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, R64 | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x4C0F, .oplen = 2,
	} } },
	{ "cmovle", 3, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { R16, R16 | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x4E0F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, R32 | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x4E0F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, R64 | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x4E0F, .oplen = 2,
	} } },
	{ "cmovna", 3, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { R16, R16 | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x460F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, R32 | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x460F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, R64 | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x460F, .oplen = 2,
	} } },
	{ "cmovnae", 3, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { R16, R16 | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x420F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, R32 | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x420F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, R64 | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x420F, .oplen = 2,
	} } },
	{ "cmovnb", 3, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { R16, R16 | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x430F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, R32 | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x430F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, R64 | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x430F, .oplen = 2,
	} } },
	{ "cmovnbe", 3, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { R16, R16 | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x470F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, R32 | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x470F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, R64 | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x470F, .oplen = 2,
	} } },
	{ "cmovnc", 3, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { R16, R16 | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x430F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, R32 | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x430F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, R64 | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x430F, .oplen = 2,
	} } },
	{ "cmovne", 3, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { R16, R16 | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x450F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, R32 | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x450F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, R64 | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x450F, .oplen = 2,
	} } },
	{ "cmovng", 3, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { R16, R16 | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x4E0F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, R32 | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x4E0F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, R64 | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x4E0F, .oplen = 2,
	} } },
	{ "cmovnge", 3, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { R16, R16 | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x4C0F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, R32 | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x4C0F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, R64 | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x4C0F, .oplen = 2,
	} } },
	{ "cmovnl", 3, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { R16, R16 | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x4D0F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, R32 | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x4D0F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, R64 | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x4D0F, .oplen = 2,
	} } },
	{ "cmovnle", 3, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { R16, R16 | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x4F0F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, R32 | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x4F0F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, R64 | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x4F0F, .oplen = 2,
	} } },
	{ "cmovno", 3, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { R16, R16 | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x410F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, R32 | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x410F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, R64 | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x410F, .oplen = 2,
	} } },
	{ "cmovnp", 3, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { R16, R16 | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x4B0F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, R32 | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x4B0F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, R64 | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x4B0F, .oplen = 2,
	} } },
	{ "cmovns", 3, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { R16, R16 | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x490F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, R32 | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x490F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, R64 | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x490F, .oplen = 2,
	} } },
	{ "cmovnz", 3, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { R16, R16 | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x450F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, R32 | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x450F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, R64 | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x450F, .oplen = 2,
	} } },
	{ "cmovo", 3, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { R16, R16 | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x400F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, R32 | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x400F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, R64 | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x400F, .oplen = 2,
	} } },
	{ "cmovp", 3, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { R16, R16 | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x4A0F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, R32 | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x4A0F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, R64 | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x4A0F, .oplen = 2,
	} } },
	{ "cmovpe", 3, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { R16, R16 | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x4A0F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, R32 | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x4A0F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, R64 | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x4A0F, .oplen = 2,
	} } },
	{ "cmovpo", 3, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { R16, R16 | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x4B0F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, R32 | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x4B0F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, R64 | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x4B0F, .oplen = 2,
	} } },
	{ "cmovs", 3, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { R16, R16 | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x480F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, R32 | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x480F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, R64 | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x480F, .oplen = 2,
	} } },
	{ "cmovz", 3, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { R16, R16 | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x440F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, R32 | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x440F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, R64 | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x440F, .oplen = 2,
	} } },
	{ "cmp", 21, (struct x64LookupActualIns[]) { {
		.args = { AL, IMM8 }, .arglen = 2, .imm_oper = 2,
		.opcode = 0x3C, .oplen = 1,
	}, {
		.args = { AX, IMM16 }, .arglen = 2, .imm_oper = 2,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x3D, .oplen = 1,
	}, {
		.args = { EAX, IMM32 }, .arglen = 2, .imm_oper = 2,
		.opcode = 0x3D, .oplen = 1,
	}, {
		.rex = 0x48,
		.args = { RAX, IMM32 }, .arglen = 2, .imm_oper = 2,
		.opcode = 0x3D, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x38,
		.args = { R8 | M8, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0x80, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x38,
		.args = { R16 | M16, IMM16 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x81, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x38,
		.args = { R32 | M32, IMM32 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0x81, .oplen = 1,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x38,
		.args = { R64 | M64, IMM32 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0x81, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x38,
		.args = { R16 | M16, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0x83, .oplen = 1,
		.preffered = true,
	}, {
		.modrmreq = true, .modrm = 0x38,
		.args = { R32 | M32, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0x83, .oplen = 1,
		.preffered = true,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x38,
		.args = { R64 | M64, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0x83, .oplen = 1,
		.preffered = true,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R8 | M8, R8 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x38, .oplen = 1,
		.preffered = true,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R8 | M8, RH }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x38, .oplen = 1,
		.preffered = true,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R16 | M16, R16 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x39, .oplen = 1,
		.preffered = true,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32 | M32, R32 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x39, .oplen = 1,
		.preffered = true,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64 | M64, R64 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x39, .oplen = 1,
		.preffered = true,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R8, R8 | M8 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x3A, .oplen = 1,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { RH, R8 | M8 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x3A, .oplen = 1,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R16, R16 | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x3B, .oplen = 1,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, R32 | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x3B, .oplen = 1,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, R64 | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x3B, .oplen = 1,
	} } },
	{ "cmppd", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xC20F, .oplen = 2,
	} } },
	{ "vcmppd", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128, IMM8 }, .arglen = 4, .imm_oper = 4, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xC2, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256, IMM8 }, .arglen = 4, .imm_oper = 4, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xC2, .oplen = 1,
	} } },
	{ "cmpps", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xC20F, .oplen = 2,
	} } },
	{ "vcmpps", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x78, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128, IMM8 }, .arglen = 4, .imm_oper = 4, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xC2, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7c, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256, IMM8 }, .arglen = 4, .imm_oper = 4, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xC2, .oplen = 1,
	} } },
	{ "cmps", 4, (struct x64LookupActualIns[]) { {
		.args = { M8, M8 }, .arglen = 2,
		.opcode = 0xA6, .oplen = 1,
	}, {
		.args = { M16, M16 }, .arglen = 2,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xA7, .oplen = 1,
	}, {
		.args = { M32, M32 }, .arglen = 2,
		.opcode = 0xA7, .oplen = 1,
	}, {
		.rex = 0x48,
		.args = { M64, M64 }, .arglen = 2,
		.opcode = 0xA7, .oplen = 1,
	} } },
	{ "cmpsb", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xA6, .oplen = 1,
	} } },
	{ "cmpsw", 1, (struct x64LookupActualIns[]) { {
		.prefixes = 0x66, .preflen = 1, .opcode = 0xA7, .oplen = 1,
	} } },
	{ "cmpsd", 2, (struct x64LookupActualIns[]) { {
		.opcode = 0xA7, .oplen = 1,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M64, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF2, .preflen = 1, .opcode = 0xC20F, .oplen = 2,
	} } },
	{ "cmpsq", 1, (struct x64LookupActualIns[]) { {
		.rex = 0x48,
		.opcode = 0xA7, .oplen = 1,
	} } },
	{ "vcmpsd", 1, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x7b, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M64, IMM8 }, .arglen = 4, .imm_oper = 4, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xC2, .oplen = 1,
	} } },
	{ "cmpss", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M32, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0xC20F, .oplen = 2,
	} } },
	{ "vcmpss", 1, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x7a, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M32, IMM8 }, .arglen = 4, .imm_oper = 4, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xC2, .oplen = 1,
	} } },
	{ "cmpxchg", 5, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { R8 | M8, R8 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0xB00F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R8 | M8, RH }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0xB00F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R16 | M16, R16 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xB10F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32 | M32, R32 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0xB10F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64 | M64, R64 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0xB10F, .oplen = 2,
	} } },
	{ "cmpxchg8b", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x8,
		.args = { M64 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xC70F, .oplen = 2,
	} } },
	{ "cmpxchg16b", 1, (struct x64LookupActualIns[]) { {
		.rex = 0x48, .modrmreq = true, .modrm = 0x8,
		.args = { M128 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xC70F, .oplen = 2,
	} } },
	{ "comisd", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x2F0F, .oplen = 2,
	} } },
	{ "vcomisd", 1, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x2F, .oplen = 1,
	} } },
	{ "comiss", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x2F0F, .oplen = 2,
	} } },
	{ "vcomiss", 1, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x78, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x2F, .oplen = 1,
	} } },
	{ "cpuid", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xA20F, .oplen = 2,
	} } },
	{ "crc32", 5, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, R8 | M8 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF2, .preflen = 1, .opcode = 0xF0380F, .oplen = 3,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, R16 | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF2, .preflen = 1, .opcode = 0xF1380F, .oplen = 3,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, R32 | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF2, .preflen = 1, .opcode = 0xF1380F, .oplen = 3,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, R8 | M8 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF2, .preflen = 1, .opcode = 0xF0380F, .oplen = 3,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, R64 | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF2, .preflen = 1, .opcode = 0xF1380F, .oplen = 3,
	} } },
	{ "cvtdq2pd", 1, (struct x64LookupActualIns[]) { {
		.args = { XMM, XMM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0xE60F, .oplen = 2,
	} } },
	{ "vcvtdq2pd", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x7a, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xE6, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7e, .modrmreq = true, .modrmreg = true,
		.args = { YMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xE6, .oplen = 1,
	} } },
	{ "cvtdq2ps", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x5B0F, .oplen = 2,
	} } },
	{ "vcvtdq2ps", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x78, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x5B, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7c, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM | M256 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x5B, .oplen = 1,
	} } },
	{ "cvtpd2dq", 1, (struct x64LookupActualIns[]) { {
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF2, .preflen = 1, .opcode = 0xE60F, .oplen = 2,
	} } },
	{ "vcvtpd2dq", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x7b, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xE6, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7f, .modrmreq = true, .modrmreg = true,
		.args = { XMM, YMM | M256 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xE6, .oplen = 1,
	} } },
	{ "cvtpd2pi", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x2D0F, .oplen = 2,
	} } },
	{ "cvtpd2ps", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x5A0F, .oplen = 2,
	} } },
	{ "vcvtpd2ps", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x5A, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { XMM, YMM | M256 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x5A, .oplen = 1,
	} } },
	{ "cvtpi2pd", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x2A0F, .oplen = 2,
	} } },
	{ "cvtpi2ps", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x2A0F, .oplen = 2,
	} } },
	{ "cvtps2dq", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x5B0F, .oplen = 2,
	} } },
	{ "vcvtps2dq", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x5B, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM | M256 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x5B, .oplen = 1,
	} } },
	{ "cvtps2pd", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x5A0F, .oplen = 2,
	} } },
	{ "vcvtps2pd", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x78, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x5A, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7c, .modrmreq = true, .modrmreg = true,
		.args = { YMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x5A, .oplen = 1,
	} } },
	{ "cvtps2pi", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, XMM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x2D0F, .oplen = 2,
	} } },
	{ "cvtsd2si", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, XMM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF2, .preflen = 1, .opcode = 0x2D0F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, XMM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF2, .preflen = 1, .opcode = 0x2D0F, .oplen = 2,
	} } },
	{ "vcvtsd2si", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x7b, .modrmreq = true, .modrmreg = true,
		.args = { R32, XMM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x2D, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0xfb, .modrmreq = true, .modrmreg = true,
		.args = { R64, XMM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x2D, .oplen = 1,
	} } },
	{ "cvtsd2ss", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF2, .preflen = 1, .opcode = 0x5A0F, .oplen = 2,
	} } },
	{ "vcvtsd2ss", 1, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x7b, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M64 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x5A, .oplen = 1,
	} } },
	{ "cvtsi2sd", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, R32 | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF2, .preflen = 1, .opcode = 0x2A0F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { XMM, R64 | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF2, .preflen = 1, .opcode = 0x2A0F, .oplen = 2,
	} } },
	{ "vcvtsi2sd", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x7b, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, R32 | M32 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x2A, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0xfb, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, R64 | M64 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x2A, .oplen = 1,
	} } },
	{ "cvtsi2ss", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, R32 | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0x2A0F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { XMM, R64 | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0x2A0F, .oplen = 2,
	} } },
	{ "vcvtsi2ss", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x7a, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, R32 | M32 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x2A, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0xfa, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, R64 | M64 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x2A, .oplen = 1,
	} } },
	{ "cvtss2sd", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0x5A0F, .oplen = 2,
	} } },
	{ "vcvtss2sd", 1, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x7a, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M32 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x5A, .oplen = 1,
	} } },
	{ "cvtss2si", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, XMM | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0x2D0F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, XMM | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0x2D0F, .oplen = 2,
	} } },
	{ "vcvtss2si", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x7a, .modrmreq = true, .modrmreg = true,
		.args = { R32, XMM | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x2D, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0xfa, .modrmreq = true, .modrmreg = true,
		.args = { R64, XMM | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x2D, .oplen = 1,
	} } },
	{ "cvttpd2dq", 1, (struct x64LookupActualIns[]) { {
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xE60F, .oplen = 2,
	} } },
	{ "vcvttpd2dq", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xE6, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { XMM, YMM | M256 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xE6, .oplen = 1,
	} } },
	{ "cvttpd2pi", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x2C0F, .oplen = 2,
	} } },
	{ "cvttps2dq", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0x5B0F, .oplen = 2,
	} } },
	{ "vcvttps2dq", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x7a, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x5B, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7e, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM | M256 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x5B, .oplen = 1,
	} } },
	{ "cvttps2pi", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, XMM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x2C0F, .oplen = 2,
	} } },
	{ "cvttsd2si", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, XMM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF2, .preflen = 1, .opcode = 0x2C0F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, XMM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF2, .preflen = 1, .opcode = 0x2C0F, .oplen = 2,
	} } },
	{ "vcvttsd2si", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x7b, .modrmreq = true, .modrmreg = true,
		.args = { R32, XMM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x2C, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0xfb, .modrmreq = true, .modrmreg = true,
		.args = { R64, XMM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x2C, .oplen = 1,
	} } },
	{ "cvttss2si", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, XMM | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0x2C0F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, XMM | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0x2C0F, .oplen = 2,
	} } },
	{ "vcvttss2si", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x7a, .modrmreq = true, .modrmreg = true,
		.args = { R32, XMM | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x2C, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0xfa, .modrmreq = true, .modrmreg = true,
		.args = { R64, XMM | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x2C, .oplen = 1,
	} } },
	{ "cwd", 1, (struct x64LookupActualIns[]) { {
		.prefixes = 0x66, .preflen = 1, .opcode = 0x99, .oplen = 1,
	} } },
	{ "cdq", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0x99, .oplen = 1,
	} } },
	{ "cqo", 1, (struct x64LookupActualIns[]) { {
		.rex = 0x48,
		.opcode = 0x99, .oplen = 1,
	} } },
	{ "dec", 4, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x8,
		.args = { R8 | M8 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xFE, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x8,
		.args = { R16 | M16 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xFF, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x8,
		.args = { R32 | M32 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xFF, .oplen = 1,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x8,
		.args = { R64 | M64 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xFF, .oplen = 1,
	} } },
	{ "div", 4, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x30,
		.args = { R8 | M8 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xF6, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x30,
		.args = { R16 | M16 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xF7, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x30,
		.args = { R32 | M32 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xF7, .oplen = 1,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x30,
		.args = { R64 | M64 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xF7, .oplen = 1,
	} } },
	{ "divpd", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x5E0F, .oplen = 2,
	} } },
	{ "vdivpd", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x5E, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x5E, .oplen = 1,
	} } },
	{ "divps", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x5E0F, .oplen = 2,
	} } },
	{ "vdivps", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x78, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x5E, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7c, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x5E, .oplen = 1,
	} } },
	{ "divsd", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF2, .preflen = 1, .opcode = 0x5E0F, .oplen = 2,
	} } },
	{ "vdivsd", 1, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x7b, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M64 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x5E, .oplen = 1,
	} } },
	{ "divss", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0x5E0F, .oplen = 2,
	} } },
	{ "vdivss", 1, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x7a, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M32 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x5E, .oplen = 1,
	} } },
	{ "dppd", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x413A0F, .oplen = 3,
	} } },
	{ "vdppd", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 3, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128, IMM8 }, .arglen = 4, .imm_oper = 4, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x41, .oplen = 1,
	} } },
	{ "dpps", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x403A0F, .oplen = 3,
	} } },
	{ "vdpps", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 3, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128, IMM8 }, .arglen = 4, .imm_oper = 4, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x40, .oplen = 1,
	}, {
		.vex = 0x80 | 3, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256, IMM8 }, .arglen = 4, .imm_oper = 4, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x40, .oplen = 1,
	} } },
	{ "emms", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0x770F, .oplen = 2,
	} } },
	{ "enter", 1, (struct x64LookupActualIns[]) { {
		.args = { IMM16, IMM8 }, .arglen = 2, .imm_oper = 1,
		.opcode = 0xC8, .oplen = 1,
	} } },
	{ "extractps", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { R64 | R32 | M32, XMM, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 1, .reg_oper = 2,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x173A0F, .oplen = 3,
	} } },
	{ "vextractps", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 3, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { R32 | M32, XMM, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x17, .oplen = 1,
	} } },
	{ "f2xm1", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xF0D9, .oplen = 2,
	} } },
	{ "fabs", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xE1D9, .oplen = 2,
	} } },
	{ "fadd", 4, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x0,
		.args = { M32 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xD8, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x0,
		.args = { M64 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xDC, .oplen = 1,
	}, {
		.args = { ST_0, ST }, .arglen = 2, .reg_oper = 2,
		.opcode = 0xC0D8, .oplen = 2,
	}, {
		.args = { ST, ST_0 }, .arglen = 2, .reg_oper = 1,
		.opcode = 0xC0DC, .oplen = 2,
	} } },
	{ "faddp", 2, (struct x64LookupActualIns[]) { {
		.args = { ST, ST_0 }, .arglen = 2, .reg_oper = 1,
		.opcode = 0xC0DE, .oplen = 2,
	}, {
		.opcode = 0xC1DE, .oplen = 2,
	} } },
	{ "fiadd", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x0,
		.args = { M32 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xDA, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x0,
		.args = { M16 }, .arglen = 1, .mem_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xDE, .oplen = 1,
	} } },
	{ "fbld", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x20,
		.args = { X64_ALLMEMMASK }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xDF, .oplen = 1,
	} } },
	{ "fbstp", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x30,
		.args = { X64_ALLMEMMASK }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xDF, .oplen = 1,
	} } },
	{ "fchs", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xE0D9, .oplen = 2,
	} } },
	{ "fclex", 1, (struct x64LookupActualIns[]) { {
		.prefixes = 0x9B, .preflen = 1, .opcode = 0xE2DB, .oplen = 2,
	} } },
	{ "fnclex", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xE2DB, .oplen = 2,
	} } },
	{ "fcmovb", 1, (struct x64LookupActualIns[]) { {
		.args = { ST_0, ST }, .arglen = 2, .reg_oper = 2,
		.opcode = 0xC0DA, .oplen = 2,
	} } },
	{ "fcmove", 1, (struct x64LookupActualIns[]) { {
		.args = { ST_0, ST }, .arglen = 2, .reg_oper = 2,
		.opcode = 0xC8DA, .oplen = 2,
	} } },
	{ "fcmovbe", 1, (struct x64LookupActualIns[]) { {
		.args = { ST_0, ST }, .arglen = 2, .reg_oper = 2,
		.opcode = 0xD0DA, .oplen = 2,
	} } },
	{ "fcmovu", 1, (struct x64LookupActualIns[]) { {
		.args = { ST_0, ST }, .arglen = 2, .reg_oper = 2,
		.opcode = 0xD8DA, .oplen = 2,
	} } },
	{ "fcmovnb", 1, (struct x64LookupActualIns[]) { {
		.args = { ST_0, ST }, .arglen = 2, .reg_oper = 2,
		.opcode = 0xC0DB, .oplen = 2,
	} } },
	{ "fcmovne", 1, (struct x64LookupActualIns[]) { {
		.args = { ST_0, ST }, .arglen = 2, .reg_oper = 2,
		.opcode = 0xC8DB, .oplen = 2,
	} } },
	{ "fcmovnbe", 1, (struct x64LookupActualIns[]) { {
		.args = { ST_0, ST }, .arglen = 2, .reg_oper = 2,
		.opcode = 0xD0DB, .oplen = 2,
	} } },
	{ "fcmovnu", 1, (struct x64LookupActualIns[]) { {
		.args = { ST_0, ST }, .arglen = 2, .reg_oper = 2,
		.opcode = 0xD8DB, .oplen = 2,
	} } },
	{ "fcom", 4, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x10,
		.args = { M32 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xD8, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x10,
		.args = { M64 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xDC, .oplen = 1,
	}, {
		.args = { ST }, .arglen = 1, .reg_oper = 1,
		.opcode = 0xD0D8, .oplen = 2,
	}, {
		.opcode = 0xD1D8, .oplen = 2,
	} } },
	{ "fcomp", 4, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x18,
		.args = { M32 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xD8, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x18,
		.args = { M64 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xDC, .oplen = 1,
	}, {
		.args = { ST }, .arglen = 1, .reg_oper = 1,
		.opcode = 0xD8D8, .oplen = 2,
	}, {
		.opcode = 0xD9D8, .oplen = 2,
	} } },
	{ "fcompp", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xD9DE, .oplen = 2,
	} } },
	{ "fcomi", 1, (struct x64LookupActualIns[]) { {
		.args = { ST, ST }, .arglen = 2, .reg_oper = 2,
		.opcode = 0xF0DB, .oplen = 2,
	} } },
	{ "fcomip", 1, (struct x64LookupActualIns[]) { {
		.args = { ST, ST }, .arglen = 2, .reg_oper = 2,
		.opcode = 0xF0DF, .oplen = 2,
	} } },
	{ "fucomi", 1, (struct x64LookupActualIns[]) { {
		.args = { ST, ST }, .arglen = 2, .reg_oper = 2,
		.opcode = 0xE8DB, .oplen = 2,
	} } },
	{ "fucomip", 1, (struct x64LookupActualIns[]) { {
		.args = { ST, ST }, .arglen = 2, .reg_oper = 2,
		.opcode = 0xE8DF, .oplen = 2,
	} } },
	{ "fcos", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xFFD9, .oplen = 2,
	} } },
	{ "fdecstp", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xF6D9, .oplen = 2,
	} } },
	{ "fdiv", 4, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x30,
		.args = { M32 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xD8, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x30,
		.args = { M64 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xDC, .oplen = 1,
	}, {
		.args = { ST_0, ST }, .arglen = 2, .reg_oper = 2,
		.opcode = 0xF0D8, .oplen = 2,
	}, {
		.args = { ST, ST_0 }, .arglen = 2, .reg_oper = 1,
		.opcode = 0xF0DC, .oplen = 2,
	} } },
	{ "fdivp", 2, (struct x64LookupActualIns[]) { {
		.args = { ST, ST_0 }, .arglen = 2, .reg_oper = 1,
		.opcode = 0xF0DE, .oplen = 2,
	}, {
		.opcode = 0xF1DE, .oplen = 2,
	} } },
	{ "fidiv", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x30,
		.args = { M32 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xDA, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x30,
		.args = { M16 }, .arglen = 1, .mem_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xDE, .oplen = 1,
	} } },
	{ "fdivr", 4, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x38,
		.args = { M32 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xD8, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x38,
		.args = { M64 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xDC, .oplen = 1,
	}, {
		.args = { ST_0, ST }, .arglen = 2, .reg_oper = 2,
		.opcode = 0xF8D8, .oplen = 2,
	}, {
		.args = { ST, ST_0 }, .arglen = 2, .reg_oper = 1,
		.opcode = 0xF8DC, .oplen = 2,
	} } },
	{ "fdivrp", 2, (struct x64LookupActualIns[]) { {
		.args = { ST, ST_0 }, .arglen = 2, .reg_oper = 1,
		.opcode = 0xF8DE, .oplen = 2,
	}, {
		.opcode = 0xF9DE, .oplen = 2,
	} } },
	{ "fidivr", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x38,
		.args = { M32 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xDA, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x38,
		.args = { M16 }, .arglen = 1, .mem_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xDE, .oplen = 1,
	} } },
	{ "ffree", 1, (struct x64LookupActualIns[]) { {
		.args = { ST }, .arglen = 1, .reg_oper = 1,
		.opcode = 0xC0DD, .oplen = 2,
	} } },
	{ "ficom", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x10,
		.args = { M16 }, .arglen = 1, .mem_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xDE, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x10,
		.args = { M32 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xDA, .oplen = 1,
	} } },
	{ "ficomp", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x18,
		.args = { M16 }, .arglen = 1, .mem_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xDE, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x18,
		.args = { M32 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xDA, .oplen = 1,
	} } },
	{ "fild", 3, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x0,
		.args = { M16 }, .arglen = 1, .mem_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xDF, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x0,
		.args = { M32 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xDB, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x28,
		.args = { M64 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xDF, .oplen = 1,
	} } },
	{ "fincstp", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xF7D9, .oplen = 2,
	} } },
	{ "finit", 1, (struct x64LookupActualIns[]) { {
		.prefixes = 0x9B, .preflen = 1, .opcode = 0xE3DB, .oplen = 2,
	} } },
	{ "fninit", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xE3DB, .oplen = 2,
	} } },
	{ "fist", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x10,
		.args = { M16 }, .arglen = 1, .mem_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xDF, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x10,
		.args = { M32 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xDB, .oplen = 1,
	} } },
	{ "fistp", 3, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x18,
		.args = { M16 }, .arglen = 1, .mem_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xDF, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x18,
		.args = { M32 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xDB, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x38,
		.args = { M64 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xDF, .oplen = 1,
	} } },
	{ "fisttp", 3, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x8,
		.args = { M16 }, .arglen = 1, .mem_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xDF, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x8,
		.args = { M32 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xDB, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x8,
		.args = { M64 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xDD, .oplen = 1,
	} } },
	{ "fld", 4, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x0,
		.args = { M32 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xD9, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x0,
		.args = { M64 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xDD, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x28,
		.args = { X64_ALLMEMMASK }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xDB, .oplen = 1,
	}, {
		.args = { ST }, .arglen = 1, .reg_oper = 1,
		.opcode = 0xC0D9, .oplen = 2,
	} } },
	{ "fld1", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xE8D9, .oplen = 2,
	} } },
	{ "fldl2t", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xE9D9, .oplen = 2,
	} } },
	{ "fldl2e", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xEAD9, .oplen = 2,
	} } },
	{ "fldpi", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xEBD9, .oplen = 2,
	} } },
	{ "fldlg2", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xECD9, .oplen = 2,
	} } },
	{ "fldln2", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xEDD9, .oplen = 2,
	} } },
	{ "fldz", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xEED9, .oplen = 2,
	} } },
	{ "fldcw", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x28,
		.args = { M32 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xD9, .oplen = 1,
	} } },
	{ "fldenv", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x20,
		.args = { M32 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xD9, .oplen = 1,
	} } },
	{ "fmul", 4, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x8,
		.args = { M32 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xD8, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x8,
		.args = { M64 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xDC, .oplen = 1,
	}, {
		.args = { ST_0, ST }, .arglen = 2, .reg_oper = 2,
		.opcode = 0xC8D8, .oplen = 2,
	}, {
		.args = { ST, ST_0 }, .arglen = 2, .reg_oper = 1,
		.opcode = 0xC8DC, .oplen = 2,
	} } },
	{ "fmulp", 2, (struct x64LookupActualIns[]) { {
		.args = { ST, ST_0 }, .arglen = 2, .reg_oper = 1,
		.opcode = 0xC8DE, .oplen = 2,
	}, {
		.opcode = 0xC9DE, .oplen = 2,
	} } },
	{ "fimul", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x8,
		.args = { M32 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xDA, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x8,
		.args = { M16 }, .arglen = 1, .mem_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xDE, .oplen = 1,
	} } },
	{ "fnop", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xD0D9, .oplen = 2,
	} } },
	{ "fpatan", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xF3D9, .oplen = 2,
	} } },
	{ "fprem", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xF8D9, .oplen = 2,
	} } },
	{ "fprem1", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xF5D9, .oplen = 2,
	} } },
	{ "fptan", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xF2D9, .oplen = 2,
	} } },
	{ "frndint", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xFCD9, .oplen = 2,
	} } },
	{ "frstor", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x20,
		.args = { X64_ALLMEMMASK }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xDD, .oplen = 1,
	} } },
	{ "fsave", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x30,
		.args = { X64_ALLMEMMASK }, .arglen = 1, .mem_oper = 1,
		.prefixes = 0x9B, .preflen = 1, .opcode = 0xDD, .oplen = 1,
	} } },
	{ "fnsave", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x30,
		.args = { X64_ALLMEMMASK }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xDD, .oplen = 1,
	} } },
	{ "fscale", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xFDD9, .oplen = 2,
	} } },
	{ "fsin", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xFED9, .oplen = 2,
	} } },
	{ "fsincos", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xFBD9, .oplen = 2,
	} } },
	{ "fsqrt", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xFAD9, .oplen = 2,
	} } },
	{ "fst", 3, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x10,
		.args = { M32 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xD9, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x10,
		.args = { M64 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xDD, .oplen = 1,
	}, {
		.args = { ST }, .arglen = 1, .reg_oper = 1,
		.opcode = 0xD0DD, .oplen = 2,
	} } },
	{ "fstp", 4, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x18,
		.args = { M32 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xD9, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x18,
		.args = { M64 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xDD, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x38,
		.args = { X64_ALLMEMMASK }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xDB, .oplen = 1,
	}, {
		.args = { ST }, .arglen = 1, .reg_oper = 1,
		.opcode = 0xD8DD, .oplen = 2,
	} } },
	{ "fstcw", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x38,
		.args = { M32 }, .arglen = 1, .mem_oper = 1,
		.prefixes = 0x9B, .preflen = 1, .opcode = 0xD9, .oplen = 1,
	} } },
	{ "fnstcw", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x38,
		.args = { M32 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xD9, .oplen = 1,
	} } },
	{ "fstenv", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x30,
		.args = { M32 }, .arglen = 1, .mem_oper = 1,
		.prefixes = 0x9B, .preflen = 1, .opcode = 0xD9, .oplen = 1,
	} } },
	{ "fnstenv", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x30,
		.args = { M32 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xD9, .oplen = 1,
	} } },
	{ "fstsw", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x38,
		.args = { M32 }, .arglen = 1, .mem_oper = 1,
		.prefixes = 0x9B, .preflen = 1, .opcode = 0xDD, .oplen = 1,
	}, {
		.args = { AX }, .arglen = 1,
		.prefixes = 0x9B, .preflen = 1, .opcode = 0xE0DF, .oplen = 2,
	} } },
	{ "fnstsw", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x38,
		.args = { M32 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xDD, .oplen = 1,
	}, {
		.args = { AX }, .arglen = 1,
		.opcode = 0xE0DF, .oplen = 2,
	} } },
	{ "fsub", 4, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x20,
		.args = { M32 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xD8, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x20,
		.args = { M64 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xDC, .oplen = 1,
	}, {
		.args = { ST_0, ST }, .arglen = 2, .reg_oper = 2,
		.opcode = 0xE0D8, .oplen = 2,
	}, {
		.args = { ST, ST_0 }, .arglen = 2, .reg_oper = 1,
		.opcode = 0xE0DC, .oplen = 2,
	} } },
	{ "fsubp", 2, (struct x64LookupActualIns[]) { {
		.args = { ST, ST_0 }, .arglen = 2, .reg_oper = 1,
		.opcode = 0xE0DE, .oplen = 2,
	}, {
		.opcode = 0xE1DE, .oplen = 2,
	} } },
	{ "fisub", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x20,
		.args = { M32 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xDA, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x20,
		.args = { M16 }, .arglen = 1, .mem_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xDE, .oplen = 1,
	} } },
	{ "fsubr", 4, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x28,
		.args = { M32 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xD8, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x28,
		.args = { M64 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xDC, .oplen = 1,
	}, {
		.args = { ST_0, ST }, .arglen = 2, .reg_oper = 2,
		.opcode = 0xE8D8, .oplen = 2,
	}, {
		.args = { ST, ST_0 }, .arglen = 2, .reg_oper = 1,
		.opcode = 0xE8DC, .oplen = 2,
	} } },
	{ "fsubrp", 2, (struct x64LookupActualIns[]) { {
		.args = { ST, ST_0 }, .arglen = 2, .reg_oper = 1,
		.opcode = 0xE8DE, .oplen = 2,
	}, {
		.opcode = 0xE9DE, .oplen = 2,
	} } },
	{ "fisubr", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x28,
		.args = { M32 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xDA, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x28,
		.args = { M16 }, .arglen = 1, .mem_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xDE, .oplen = 1,
	} } },
	{ "ftst", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xE4D9, .oplen = 2,
	} } },
	{ "fucom", 2, (struct x64LookupActualIns[]) { {
		.args = { ST }, .arglen = 1, .reg_oper = 1,
		.opcode = 0xE0DD, .oplen = 2,
	}, {
		.opcode = 0xE1DD, .oplen = 2,
	} } },
	{ "fucomp", 2, (struct x64LookupActualIns[]) { {
		.args = { ST }, .arglen = 1, .reg_oper = 1,
		.opcode = 0xE8DD, .oplen = 2,
	}, {
		.opcode = 0xE9DD, .oplen = 2,
	} } },
	{ "fucompp", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xE9DA, .oplen = 2,
	} } },
	{ "fxam", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xE5D9, .oplen = 2,
	} } },
	{ "fxch", 2, (struct x64LookupActualIns[]) { {
		.args = { ST }, .arglen = 1, .reg_oper = 1,
		.opcode = 0xC8D9, .oplen = 2,
	}, {
		.opcode = 0xC9D9, .oplen = 2,
	} } },
	{ "fxrstor", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x8,
		.args = { M512 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xAE0F, .oplen = 2,
	} } },
	{ "fxrstor64", 1, (struct x64LookupActualIns[]) { {
		.rex = 0x48, .modrmreq = true, .modrm = 0x8,
		.args = { M512 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xAE0F, .oplen = 2,
	} } },
	{ "fxsave", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x0,
		.args = { M512 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xAE0F, .oplen = 2,
	} } },
	{ "fxsave64", 1, (struct x64LookupActualIns[]) { {
		.rex = 0x48, .modrmreq = true, .modrm = 0x0,
		.args = { M512 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xAE0F, .oplen = 2,
	} } },
	{ "fxtract", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xF4D9, .oplen = 2,
	} } },
	{ "fyl2x", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xF1D9, .oplen = 2,
	} } },
	{ "fyl2xp1", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xF9D9, .oplen = 2,
	} } },
	{ "haddpd", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x7C0F, .oplen = 2,
	} } },
	{ "vhaddpd", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x7C, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x7C, .oplen = 1,
	} } },
	{ "haddps", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF2, .preflen = 1, .opcode = 0x7C0F, .oplen = 2,
	} } },
	{ "vhaddps", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x7b, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x7C, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7f, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x7C, .oplen = 1,
	} } },
	{ "hlt", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xF4, .oplen = 1,
	} } },
	{ "hsubpd", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x7D0F, .oplen = 2,
	} } },
	{ "vhsubpd", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x7D, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x7D, .oplen = 1,
	} } },
	{ "hsubps", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF2, .preflen = 1, .opcode = 0x7D0F, .oplen = 2,
	} } },
	{ "vhsubps", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x7b, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x7D, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7f, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x7D, .oplen = 1,
	} } },
	{ "idiv", 4, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x38,
		.args = { R8 | M8 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xF6, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x38,
		.args = { R16 | M16 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xF7, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x38,
		.args = { R32 | M32 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xF7, .oplen = 1,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x38,
		.args = { R64 | M64 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xF7, .oplen = 1,
	} } },
	{ "imul", 13, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x28,
		.args = { R8 | M8 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xF6, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x28,
		.args = { R16 | M16 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xF7, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x28,
		.args = { R32 | M32 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xF7, .oplen = 1,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x28,
		.args = { R64 | M64 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xF7, .oplen = 1,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R16, R16 | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xAF0F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, R32 | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xAF0F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, R64 | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xAF0F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R16, R16 | M16, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x6B, .oplen = 1,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, R32 | M32, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x6B, .oplen = 1,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, R64 | M64, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x6B, .oplen = 1,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R16, R16 | M16, IMM16 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x69, .oplen = 1,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, R32 | M32, IMM32 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x69, .oplen = 1,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, R64 | M64, IMM32 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x69, .oplen = 1,
	} } },
	{ "in", 6, (struct x64LookupActualIns[]) { {
		.args = { AL, IMM8 }, .arglen = 2, .imm_oper = 2,
		.opcode = 0xE4, .oplen = 1,
	}, {
		.args = { AX, IMM8 }, .arglen = 2, .imm_oper = 2,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xE5, .oplen = 1,
	}, {
		.args = { EAX, IMM8 }, .arglen = 2, .imm_oper = 2,
		.opcode = 0xE5, .oplen = 1,
	}, {
		.args = { AL, DX }, .arglen = 2,
		.opcode = 0xEC, .oplen = 1,
	}, {
		.args = { AX, DX }, .arglen = 2,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xED, .oplen = 1,
	}, {
		.args = { EAX, DX }, .arglen = 2,
		.opcode = 0xED, .oplen = 1,
	} } },
	{ "inc", 4, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x0,
		.args = { R8 | M8 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xFE, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x0,
		.args = { R16 | M16 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xFF, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x0,
		.args = { R32 | M32 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xFF, .oplen = 1,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x0,
		.args = { R64 | M64 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xFF, .oplen = 1,
	} } },
	{ "ins", 3, (struct x64LookupActualIns[]) { {
		.args = { M8, DX }, .arglen = 2,
		.opcode = 0x6C, .oplen = 1,
	}, {
		.args = { M16, DX }, .arglen = 2,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x6D, .oplen = 1,
	}, {
		.args = { M32, DX }, .arglen = 2,
		.opcode = 0x6D, .oplen = 1,
	} } },
	{ "insb", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0x6C, .oplen = 1,
	} } },
	{ "insw", 1, (struct x64LookupActualIns[]) { {
		.prefixes = 0x66, .preflen = 1, .opcode = 0x6D, .oplen = 1,
	} } },
	{ "insd", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0x6D, .oplen = 1,
	} } },
	{ "insertps", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M32, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x213A0F, .oplen = 3,
	} } },
	{ "vinsertps", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 3, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M32, IMM8 }, .arglen = 4, .imm_oper = 4, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x21, .oplen = 1,
	} } },
	{ "int3", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xCC, .oplen = 1,
	} } },
	{ "int1", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xF1, .oplen = 1,
	} } },
	{ "int", 1, (struct x64LookupActualIns[]) { {
		.args = { IMM8 }, .arglen = 1, .imm_oper = 1,
		.opcode = 0xCD, .oplen = 1,
	} } },
	{ "invd", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0x080F, .oplen = 2,
	} } },
	{ "invlpg", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x38,
		.args = { X64_ALLMEMMASK }, .arglen = 1,
		.opcode = 0x010F, .oplen = 2,
	} } },
	{ "invpcid", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { R64, M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x82380F, .oplen = 3,
	} } },
	{ "iret", 1, (struct x64LookupActualIns[]) { {
		.prefixes = 0x66, .preflen = 1, .opcode = 0xCF, .oplen = 1,
	} } },
	{ "iretd", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xCF, .oplen = 1,
	} } },
	{ "iretq", 1, (struct x64LookupActualIns[]) { {
		.rex = 0x48,
		.opcode = 0xCF, .oplen = 1,
	} } },
	{ "ja", 2, (struct x64LookupActualIns[]) { {
		.args = { REL8 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0x77, .oplen = 1,
	}, {
		.args = { REL32 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0x870F, .oplen = 2,
	} } },
	{ "jae", 2, (struct x64LookupActualIns[]) { {
		.args = { REL8 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0x73, .oplen = 1,
	}, {
		.args = { REL32 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0x830F, .oplen = 2,
	} } },
	{ "jb", 2, (struct x64LookupActualIns[]) { {
		.args = { REL8 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0x72, .oplen = 1,
	}, {
		.args = { REL32 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0x820F, .oplen = 2,
	} } },
	{ "jbe", 2, (struct x64LookupActualIns[]) { {
		.args = { REL8 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0x76, .oplen = 1,
	}, {
		.args = { REL32 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0x860F, .oplen = 2,
	} } },
	{ "jc", 2, (struct x64LookupActualIns[]) { {
		.args = { REL8 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0x72, .oplen = 1,
	}, {
		.args = { REL32 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0x820F, .oplen = 2,
	} } },
	{ "jecxz", 1, (struct x64LookupActualIns[]) { {
		.args = { REL8 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0xE3, .oplen = 1,
	} } },
	{ "jrcxz", 1, (struct x64LookupActualIns[]) { {
		.args = { REL8 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0xE3, .oplen = 1,
	} } },
	{ "je", 2, (struct x64LookupActualIns[]) { {
		.args = { REL8 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0x74, .oplen = 1,
	}, {
		.args = { REL32 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0x840F, .oplen = 2,
	} } },
	{ "jg", 2, (struct x64LookupActualIns[]) { {
		.args = { REL8 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0x7F, .oplen = 1,
	}, {
		.args = { REL32 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0x8F0F, .oplen = 2,
	} } },
	{ "jge", 2, (struct x64LookupActualIns[]) { {
		.args = { REL8 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0x7D, .oplen = 1,
	}, {
		.args = { REL32 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0x8D0F, .oplen = 2,
	} } },
	{ "jl", 2, (struct x64LookupActualIns[]) { {
		.args = { REL8 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0x7C, .oplen = 1,
	}, {
		.args = { REL32 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0x8C0F, .oplen = 2,
	} } },
	{ "jle", 2, (struct x64LookupActualIns[]) { {
		.args = { REL8 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0x7E, .oplen = 1,
	}, {
		.args = { REL32 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0x8E0F, .oplen = 2,
	} } },
	{ "jna", 2, (struct x64LookupActualIns[]) { {
		.args = { REL8 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0x76, .oplen = 1,
	}, {
		.args = { REL32 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0x860F, .oplen = 2,
	} } },
	{ "jnae", 2, (struct x64LookupActualIns[]) { {
		.args = { REL8 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0x72, .oplen = 1,
	}, {
		.args = { REL32 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0x820F, .oplen = 2,
	} } },
	{ "jnb", 2, (struct x64LookupActualIns[]) { {
		.args = { REL8 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0x73, .oplen = 1,
	}, {
		.args = { REL32 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0x830F, .oplen = 2,
	} } },
	{ "jnbe", 2, (struct x64LookupActualIns[]) { {
		.args = { REL8 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0x77, .oplen = 1,
	}, {
		.args = { REL32 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0x870F, .oplen = 2,
	} } },
	{ "jnc", 2, (struct x64LookupActualIns[]) { {
		.args = { REL8 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0x73, .oplen = 1,
	}, {
		.args = { REL32 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0x830F, .oplen = 2,
	} } },
	{ "jne", 2, (struct x64LookupActualIns[]) { {
		.args = { REL8 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0x75, .oplen = 1,
	}, {
		.args = { REL32 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0x850F, .oplen = 2,
	} } },
	{ "jng", 2, (struct x64LookupActualIns[]) { {
		.args = { REL8 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0x7E, .oplen = 1,
	}, {
		.args = { REL32 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0x8E0F, .oplen = 2,
	} } },
	{ "jnge", 2, (struct x64LookupActualIns[]) { {
		.args = { REL8 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0x7C, .oplen = 1,
	}, {
		.args = { REL32 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0x8C0F, .oplen = 2,
	} } },
	{ "jnl", 2, (struct x64LookupActualIns[]) { {
		.args = { REL8 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0x7D, .oplen = 1,
	}, {
		.args = { REL32 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0x8D0F, .oplen = 2,
	} } },
	{ "jnle", 2, (struct x64LookupActualIns[]) { {
		.args = { REL8 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0x7F, .oplen = 1,
	}, {
		.args = { REL32 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0x8F0F, .oplen = 2,
	} } },
	{ "jno", 2, (struct x64LookupActualIns[]) { {
		.args = { REL8 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0x71, .oplen = 1,
	}, {
		.args = { REL32 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0x810F, .oplen = 2,
	} } },
	{ "jnp", 2, (struct x64LookupActualIns[]) { {
		.args = { REL8 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0x7B, .oplen = 1,
	}, {
		.args = { REL32 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0x8B0F, .oplen = 2,
	} } },
	{ "jns", 2, (struct x64LookupActualIns[]) { {
		.args = { REL8 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0x79, .oplen = 1,
	}, {
		.args = { REL32 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0x890F, .oplen = 2,
	} } },
	{ "jnz", 2, (struct x64LookupActualIns[]) { {
		.args = { REL8 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0x75, .oplen = 1,
	}, {
		.args = { REL32 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0x850F, .oplen = 2,
	} } },
	{ "jo", 2, (struct x64LookupActualIns[]) { {
		.args = { REL8 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0x70, .oplen = 1,
	}, {
		.args = { REL32 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0x800F, .oplen = 2,
	} } },
	{ "jp", 2, (struct x64LookupActualIns[]) { {
		.args = { REL8 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0x7A, .oplen = 1,
	}, {
		.args = { REL32 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0x8A0F, .oplen = 2,
	} } },
	{ "jpe", 2, (struct x64LookupActualIns[]) { {
		.args = { REL8 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0x7A, .oplen = 1,
	}, {
		.args = { REL32 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0x8A0F, .oplen = 2,
	} } },
	{ "jpo", 2, (struct x64LookupActualIns[]) { {
		.args = { REL8 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0x7B, .oplen = 1,
	}, {
		.args = { REL32 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0x8B0F, .oplen = 2,
	} } },
	{ "js", 2, (struct x64LookupActualIns[]) { {
		.args = { REL8 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0x78, .oplen = 1,
	}, {
		.args = { REL32 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0x880F, .oplen = 2,
	} } },
	{ "jz", 2, (struct x64LookupActualIns[]) { {
		.args = { REL8 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0x74, .oplen = 1,
	}, {
		.args = { REL32 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0x840F, .oplen = 2,
	} } },
	{ "jmp", 6, (struct x64LookupActualIns[]) { {
		.args = { REL8 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0xEB, .oplen = 1,
	}, {
		.args = { REL32 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0xE9, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x20,
		.args = { R64 | M64 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xFF, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x28,
		.args = { FARPTR1616 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0xFF, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x28,
		.args = { FARPTR1632 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0xFF, .oplen = 1,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x28,
		.args = { FARPTR1664 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0xFF, .oplen = 1,
	} } },
	{ "lahf", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0x9F, .oplen = 1,
	} } },
	{ "lar", 3, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { R16, R16 | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x020F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, R32 | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x020F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, R32 | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x020F, .oplen = 2,
	} } },
	{ "lddqu", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF2, .preflen = 1, .opcode = 0xF00F, .oplen = 2,
	} } },
	{ "vlddqu", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x7b, .modrmreq = true, .modrmreg = true,
		.args = { XMM, M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xF0, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7f, .modrmreq = true, .modrmreg = true,
		.args = { YMM, M256 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xF0, .oplen = 1,
	} } },
	{ "ldmxcsr", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x10,
		.args = { M32 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xAE0F, .oplen = 2,
	} } },
	{ "vldmxcsr", 1, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x78, .modrmreq = true, .modrm = 0x10,
		.args = { M32 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xAE, .oplen = 1,
	} } },
	{ "lss", 3, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { R16, FARPTR1616 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xB20F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, FARPTR1632 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xB20F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, FARPTR1664 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xB20F, .oplen = 2,
	} } },
	{ "lfs", 3, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { R16, FARPTR1616 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xB40F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, FARPTR1632 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xB40F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, FARPTR1664 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xB40F, .oplen = 2,
	} } },
	{ "lgs", 3, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { R16, FARPTR1616 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xB50F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, FARPTR1632 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xB50F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, FARPTR1664 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xB50F, .oplen = 2,
	} } },
	{ "lea", 3, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { R16, X64_ALLMEMMASK }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x8D, .oplen = 1,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, X64_ALLMEMMASK }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x8D, .oplen = 1,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, X64_ALLMEMMASK }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x8D, .oplen = 1,
	} } },
	{ "leave", 2, (struct x64LookupActualIns[]) { {
		.args = { PREF66 }, .arglen = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xC9, .oplen = 1,
	}, {
		.opcode = 0xC9, .oplen = 1,
	} } },
	{ "lfence", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xE8AE0F, .oplen = 3,
	} } },
	{ "lgdt", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x10,
		.args = { FARPTR1664 }, .arglen = 1,
		.opcode = 0x010F, .oplen = 2,
	} } },
	{ "lidt", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x18,
		.args = { FARPTR1664 }, .arglen = 1,
		.opcode = 0x010F, .oplen = 2,
	} } },
	{ "lldt", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x10,
		.args = { R16 | M16 }, .arglen = 1,
		.opcode = 0x000F, .oplen = 2,
	} } },
	{ "lmsw", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x30,
		.args = { R16 | M16 }, .arglen = 1,
		.opcode = 0x010F, .oplen = 2,
	} } },
	{ "lock", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xF0, .oplen = 1,
	} } },
	{ "lods", 4, (struct x64LookupActualIns[]) { {
		.args = { M8 }, .arglen = 1,
		.opcode = 0xAC, .oplen = 1,
	}, {
		.args = { M16 }, .arglen = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xAD, .oplen = 1,
	}, {
		.args = { M32 }, .arglen = 1,
		.opcode = 0xAD, .oplen = 1,
	}, {
		.rex = 0x48,
		.args = { M64 }, .arglen = 1,
		.opcode = 0xAD, .oplen = 1,
	} } },
	{ "lodsb", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xAC, .oplen = 1,
	} } },
	{ "lodsw", 1, (struct x64LookupActualIns[]) { {
		.prefixes = 0x66, .preflen = 1, .opcode = 0xAD, .oplen = 1,
	} } },
	{ "lodsd", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xAD, .oplen = 1,
	} } },
	{ "lodsq", 1, (struct x64LookupActualIns[]) { {
		.rex = 0x48,
		.opcode = 0xAD, .oplen = 1,
	} } },
	{ "loop", 1, (struct x64LookupActualIns[]) { {
		.args = { REL8 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0xE2, .oplen = 1,
	} } },
	{ "loope", 1, (struct x64LookupActualIns[]) { {
		.args = { REL8 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0xE0, .oplen = 1,
	} } },
	{ "loopne", 1, (struct x64LookupActualIns[]) { {
		.args = { REL8 }, .arglen = 1, .rel_oper = 1,
		.opcode = 0xE0, .oplen = 1,
	} } },
	{ "lsl", 3, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { R16, R16 | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x030F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, R32 | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x030F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, R32 | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x030F, .oplen = 2,
	} } },
	{ "ltr", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x18,
		.args = { R16 | M16 }, .arglen = 1,
		.opcode = 0x000F, .oplen = 2,
	} } },
	{ "lzcnt", 3, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { R16, R16 | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF366, .preflen = 2, .opcode = 0xBD0F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, R32 | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0xBD0F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, R64 | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0xBD0F, .oplen = 2,
	} } },
	{ "maskmovdqu", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xF70F, .oplen = 2,
	} } },
	{ "vmaskmovdqu", 1, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xF7, .oplen = 1,
	} } },
	{ "maskmovq", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xF70F, .oplen = 2,
	} } },
	{ "maxpd", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x5F0F, .oplen = 2,
	} } },
	{ "vmaxpd", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x5F, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x5F, .oplen = 1,
	} } },
	{ "maxps", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x5F0F, .oplen = 2,
	} } },
	{ "vmaxps", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x78, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x5F, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7c, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x5F, .oplen = 1,
	} } },
	{ "maxsd", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF2, .preflen = 1, .opcode = 0x5F0F, .oplen = 2,
	} } },
	{ "vmaxsd", 1, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x7b, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M64 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x5F, .oplen = 1,
	} } },
	{ "maxss", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0x5F0F, .oplen = 2,
	} } },
	{ "vmaxss", 1, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x7a, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M32 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x5F, .oplen = 1,
	} } },
	{ "mfence", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xF0AE0F, .oplen = 3,
	} } },
	{ "minpd", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x5D0F, .oplen = 2,
	} } },
	{ "vminpd", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x5D, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x5D, .oplen = 1,
	} } },
	{ "minps", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x5D0F, .oplen = 2,
	} } },
	{ "vminps", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x78, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x5D, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7c, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x5D, .oplen = 1,
	} } },
	{ "minsd", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF2, .preflen = 1, .opcode = 0x5D0F, .oplen = 2,
	} } },
	{ "vminsd", 1, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x7b, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M64 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x5D, .oplen = 1,
	} } },
	{ "minss", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0x5D0F, .oplen = 2,
	} } },
	{ "vminss", 1, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x7a, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M32 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x5D, .oplen = 1,
	} } },
	{ "monitor", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xC8010F, .oplen = 3,
	} } },
	{ "mov", 39, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { R8 | M8, R8 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x88, .oplen = 1,
		.preffered = true,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R8 | M8, RH }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x88, .oplen = 1,
		.preffered = true,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R16 | M16, R16 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x89, .oplen = 1,
		.preffered = true,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32 | M32, R32 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x89, .oplen = 1,
		.preffered = true,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64 | M64, R64 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x89, .oplen = 1,
		.preffered = true,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R8, R8 | M8 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x8A, .oplen = 1,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { RH, R8 | M8 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x8A, .oplen = 1,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R16, R16 | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x8B, .oplen = 1,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, R32 | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x8B, .oplen = 1,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, R64 | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x8B, .oplen = 1,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R16 | M16, SREG }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x8C, .oplen = 1,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64 | M64, SREG }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x8C, .oplen = 1,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { SREG, R16 | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x8E, .oplen = 1,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { SREG, R64 | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x8E, .oplen = 1,
	}, {
		.args = { AL, MOFFS8 }, .arglen = 2, .rel_oper = 2,
		.opcode = 0xA0, .oplen = 1,
	}, {
		.rex = 0x48,
		.args = { AL, MOFFS8, PREFREX_W }, .arglen = 3, .rel_oper = 2,
		.opcode = 0xA0, .oplen = 1,
	}, {
		.args = { AX, MOFFS16 }, .arglen = 2, .rel_oper = 2,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xA1, .oplen = 1,
	}, {
		.args = { EAX, MOFFS32 }, .arglen = 2, .rel_oper = 2,
		.opcode = 0xA1, .oplen = 1,
	}, {
		.rex = 0x48,
		.args = { RAX, MOFFS64 }, .arglen = 2, .rel_oper = 2,
		.opcode = 0xA1, .oplen = 1,
	}, {
		.args = { MOFFS8, AL }, .arglen = 2, .rel_oper = 2,
		.opcode = 0xA2, .oplen = 1,
	}, {
		.rex = 0x48,
		.args = { MOFFS8, AL, PREFREX_W }, .arglen = 3, .rel_oper = 2,
		.opcode = 0xA2, .oplen = 1,
	}, {
		.args = { MOFFS16, AX }, .arglen = 2, .rel_oper = 2,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xA3, .oplen = 1,
	}, {
		.args = { MOFFS32, EAX }, .arglen = 2, .rel_oper = 2,
		.opcode = 0xA3, .oplen = 1,
	}, {
		.rex = 0x48,
		.args = { MOFFS64, RAX }, .arglen = 2, .rel_oper = 2,
		.opcode = 0xA3, .oplen = 1,
	}, {
		.args = { R8, IMM8 }, .arglen = 2, .imm_oper = 2, .reg_oper = 1,
		.opcode = 0xB0, .oplen = 1,
		.preffered = true,
	}, {
		.args = { RH, IMM8 }, .arglen = 2, .imm_oper = 2, .reg_oper = 1,
		.opcode = 0xB0, .oplen = 1,
		.preffered = true,
	}, {
		.args = { R16, IMM16 }, .arglen = 2, .imm_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xB8, .oplen = 1,
		.preffered = true,
	}, {
		.args = { R32, IMM32 }, .arglen = 2, .imm_oper = 2, .reg_oper = 1,
		.opcode = 0xB8, .oplen = 1,
		.preffered = true,
	}, {
		.rex = 0x48,
		.args = { R64, IMM64 }, .arglen = 2, .imm_oper = 2, .reg_oper = 1,
		.opcode = 0xB8, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x0,
		.args = { R8 | M8, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0xC6, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x0,
		.args = { R16 | M16, IMM16 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xC7, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x0,
		.args = { R32 | M32, IMM32 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0xC7, .oplen = 1,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x0,
		.args = { R64 | M64, IMM32 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0xC7, .oplen = 1,
		.preffered = true,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R64, CR0_7 }, .arglen = 2,
		.opcode = 0x200F, .oplen = 2,
	}, {
		.rex = 0x44,
		.args = { R64, CR8 }, .arglen = 2,
		.opcode = 0x200F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { CR0_7, R64 }, .arglen = 2,
		.opcode = 0x220F, .oplen = 2,
	}, {
		.rex = 0x44,
		.args = { CR8, R64 }, .arglen = 2,
		.opcode = 0x220F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R64, DREG }, .arglen = 2,
		.opcode = 0x210F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { DREG, R64 }, .arglen = 2,
		.opcode = 0x230F, .oplen = 2,
	} } },
	{ "movapd", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x280F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM | M128, XMM }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x290F, .oplen = 2,
		.preffered = true,
	} } },
	{ "vmovapd", 4, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x28, .oplen = 1,
		.preffered = true,
	}, {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM | M128, XMM }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x29, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM | M256 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x28, .oplen = 1,
		.preffered = true,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM | M256, YMM }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x29, .oplen = 1,
	} } },
	{ "movaps", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x280F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM | M128, XMM }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x290F, .oplen = 2,
		.preffered = true,
	} } },
	{ "vmovaps", 4, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x78, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x28, .oplen = 1,
		.preffered = true,
	}, {
		.vex = 1, .vex_byte = 0x78, .modrmreq = true, .modrmreg = true,
		.args = { XMM | M128, XMM }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x29, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7c, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM | M256 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x28, .oplen = 1,
		.preffered = true,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7c, .modrmreq = true, .modrmreg = true,
		.args = { YMM | M256, YMM }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x29, .oplen = 1,
	} } },
	{ "movbe", 6, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { R16, M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xF0380F, .oplen = 3,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xF0380F, .oplen = 3,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xF0380F, .oplen = 3,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { M16, R16 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xF1380F, .oplen = 3,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { M32, R32 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0xF1380F, .oplen = 3,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { M64, R64 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0xF1380F, .oplen = 3,
	} } },
	{ "movd", 4, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, R32 | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x6E0F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32 | M32, MM }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x7E0F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, R32 | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x6E0F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32 | M32, XMM }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x7E0F, .oplen = 2,
	} } },
	{ "movq", 8, (struct x64LookupActualIns[]) { {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { MM, R64 | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x6E0F, .oplen = 2,
		.preffered = true,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64 | M64, MM }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x7E0F, .oplen = 2,
		.preffered = true,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { XMM, R64 | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x6E0F, .oplen = 2,
		.preffered = true,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64 | M64, XMM }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x7E0F, .oplen = 2,
		.preffered = true,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x6F0F, .oplen = 2,
		.preffered = true,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { MM | M64, MM }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x7F0F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0x7E0F, .oplen = 2,
		.preffered = true,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM | M64, XMM }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xD60F, .oplen = 2,
	} } },
	{ "vmovd", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, R32 | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x6E, .oplen = 1,
	}, {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { R32 | M32, XMM }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x7E, .oplen = 1,
	} } },
	{ "vmovq", 5, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 1, .vex_byte = 0xf9, .modrmreq = true, .modrmreg = true,
		.args = { XMM, R64 | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x6E, .oplen = 1,
		.preffered = true,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0xf9, .modrmreq = true, .modrmreg = true,
		.args = { R64 | M64, XMM }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x7E, .oplen = 1,
		.preffered = true,
	}, {
		.vex = 1, .vex_byte = 0x7a, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x7E, .oplen = 1,
	}, {
		.vex = 1, .vex_byte = 0x7a, .modrmreq = true, .modrmreg = true,
		.args = { XMM, M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x7E, .oplen = 1,
	}, {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM | M64, XMM }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0xD6, .oplen = 1,
		.preffered = true,
	} } },
	{ "movddup", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF2, .preflen = 1, .opcode = 0x120F, .oplen = 2,
	} } },
	{ "vmovddup", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x7b, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x12, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7f, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM | M256 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x12, .oplen = 1,
	} } },
	{ "movdqa", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x6F0F, .oplen = 2,
		.preffered = true,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM | M128, XMM }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x7F0F, .oplen = 2,
	} } },
	{ "vmovdqa", 4, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x6F, .oplen = 1,
		.preffered = true,
	}, {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM | M128, XMM }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x7F, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM | M256 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x6F, .oplen = 1,
		.preffered = true,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM | M256, YMM }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x7F, .oplen = 1,
	} } },
	{ "movdqu", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0x6F0F, .oplen = 2,
		.preffered = true,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM | M128, XMM }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0x7F0F, .oplen = 2,
	} } },
	{ "vmovdqu", 4, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x7a, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x6F, .oplen = 1,
		.preffered = true,
	}, {
		.vex = 1, .vex_byte = 0x7a, .modrmreq = true, .modrmreg = true,
		.args = { XMM | M128, XMM }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x7F, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7e, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM | M256 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x6F, .oplen = 1,
		.preffered = true,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7e, .modrmreq = true, .modrmreg = true,
		.args = { YMM | M256, YMM }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x7F, .oplen = 1,
	} } },
	{ "movdq2q", 1, (struct x64LookupActualIns[]) { {
		.args = { MM, XMM }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF2, .preflen = 1, .opcode = 0xD60F, .oplen = 2,
	} } },
	{ "movhlps", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x120F, .oplen = 2,
	} } },
	{ "vmovhlps", 1, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x78, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x12, .oplen = 1,
	} } },
	{ "movhpd", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x160F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { M64, XMM }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x170F, .oplen = 2,
	} } },
	{ "vmovhpd", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, M64 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x16, .oplen = 1,
	}, {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { M64, XMM }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x17, .oplen = 1,
	} } },
	{ "movhps", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x160F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { M64, XMM }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x170F, .oplen = 2,
	} } },
	{ "vmovhps", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x78, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, M64 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x16, .oplen = 1,
	}, {
		.vex = 1, .vex_byte = 0x78, .modrmreq = true, .modrmreg = true,
		.args = { M64, XMM }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x17, .oplen = 1,
	} } },
	{ "movlhps", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x160F, .oplen = 2,
	} } },
	{ "vmovlhps", 1, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x78, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x16, .oplen = 1,
	} } },
	{ "movlpd", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x120F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { M64, XMM }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x130F, .oplen = 2,
	} } },
	{ "vmovlpd", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, M64 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x12, .oplen = 1,
	}, {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { M64, XMM }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x13, .oplen = 1,
	} } },
	{ "movlps", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x120F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { M64, XMM }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x130F, .oplen = 2,
	} } },
	{ "vmovlps", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x78, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, M64 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x12, .oplen = 1,
	}, {
		.vex = 1, .vex_byte = 0x78, .modrmreq = true, .modrmreg = true,
		.args = { M64, XMM }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x13, .oplen = 1,
	} } },
	{ "movmskpd", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { R64 | R32, XMM }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x500F, .oplen = 2,
	} } },
	{ "vmovmskpd", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { R64 | R32, XMM }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x50, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { R64 | R32, YMM }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x50, .oplen = 1,
	} } },
	{ "movmskps", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { R64 | R32, XMM }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x500F, .oplen = 2,
	} } },
	{ "vmovmskps", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x78, .modrmreq = true, .modrmreg = true,
		.args = { R64 | R32, XMM }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x50, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7c, .modrmreq = true, .modrmreg = true,
		.args = { R64 | R32, YMM }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x50, .oplen = 1,
	} } },
	{ "movntdqa", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x2A380F, .oplen = 3,
	} } },
	{ "vmovntdqa", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x2A, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, M256 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x2A, .oplen = 1,
	} } },
	{ "movntdq", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { M128, XMM }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xE70F, .oplen = 2,
	} } },
	{ "vmovntdq", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { M128, XMM }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0xE7, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { M256, YMM }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0xE7, .oplen = 1,
	} } },
	{ "movnti", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { M32, R32 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0xC30F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { M64, R64 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0xC30F, .oplen = 2,
	} } },
	{ "movntpd", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { M128, XMM }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x2B0F, .oplen = 2,
	} } },
	{ "vmovntpd", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { M128, XMM }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x2B, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { M256, YMM }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x2B, .oplen = 1,
	} } },
	{ "movntps", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { M128, XMM }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x2B0F, .oplen = 2,
	} } },
	{ "vmovntps", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x78, .modrmreq = true, .modrmreg = true,
		.args = { M128, XMM }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x2B, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7c, .modrmreq = true, .modrmreg = true,
		.args = { M256, YMM }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x2B, .oplen = 1,
	} } },
	{ "movntq", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { M64, MM }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0xE70F, .oplen = 2,
	} } },
	{ "movq2dq", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, MM }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0xD60F, .oplen = 2,
	} } },
	{ "movs", 4, (struct x64LookupActualIns[]) { {
		.args = { M8, M8 }, .arglen = 2,
		.opcode = 0xA4, .oplen = 1,
	}, {
		.args = { M16, M16 }, .arglen = 2,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xA5, .oplen = 1,
	}, {
		.args = { M32, M32 }, .arglen = 2,
		.opcode = 0xA5, .oplen = 1,
	}, {
		.rex = 0x48,
		.args = { M64, M64 }, .arglen = 2,
		.opcode = 0xA5, .oplen = 1,
	} } },
	{ "movsb", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xA4, .oplen = 1,
	} } },
	{ "movsw", 1, (struct x64LookupActualIns[]) { {
		.prefixes = 0x66, .preflen = 1, .opcode = 0xA5, .oplen = 1,
	} } },
	{ "movsd", 3, (struct x64LookupActualIns[]) { {
		.opcode = 0xA5, .oplen = 1,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF2, .preflen = 1, .opcode = 0x100F, .oplen = 2,
		.preffered = true,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM | M64, XMM }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.prefixes = 0xF2, .preflen = 1, .opcode = 0x110F, .oplen = 2,
	} } },
	{ "movsq", 1, (struct x64LookupActualIns[]) { {
		.rex = 0x48,
		.opcode = 0xA5, .oplen = 1,
	} } },
	{ "vmovsd", 4, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x7b, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x10, .oplen = 1,
	}, {
		.vex = 1, .vex_byte = 0x7b, .modrmreq = true, .modrmreg = true,
		.args = { XMM, M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x10, .oplen = 1,
	}, {
		.vex = 1, .vex_byte = 0x7b, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM }, .arglen = 3, .mem_oper = 1, .reg_oper = 3, .vex_oper = 2,
		.opcode = 0x11, .oplen = 1,
		.preffered = true,
	}, {
		.vex = 1, .vex_byte = 0x7b, .modrmreq = true, .modrmreg = true,
		.args = { M64, XMM }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x11, .oplen = 1,
	} } },
	{ "movshdup", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0x160F, .oplen = 2,
	} } },
	{ "vmovshdup", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x7a, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x16, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7e, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM | M256 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x16, .oplen = 1,
	} } },
	{ "movsldup", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0x120F, .oplen = 2,
	} } },
	{ "vmovsldup", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x7a, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x12, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7e, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM | M256 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x12, .oplen = 1,
	} } },
	{ "movss", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0x100F, .oplen = 2,
		.preffered = true,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM | M32, XMM }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0x110F, .oplen = 2,
	} } },
	{ "vmovss", 4, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x7a, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x10, .oplen = 1,
	}, {
		.vex = 1, .vex_byte = 0x7a, .modrmreq = true, .modrmreg = true,
		.args = { XMM, M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x10, .oplen = 1,
	}, {
		.vex = 1, .vex_byte = 0x7a, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM }, .arglen = 3, .mem_oper = 1, .reg_oper = 3, .vex_oper = 2,
		.opcode = 0x11, .oplen = 1,
		.preffered = true,
	}, {
		.vex = 1, .vex_byte = 0x7a, .modrmreq = true, .modrmreg = true,
		.args = { M32, XMM }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x11, .oplen = 1,
	} } },
	{ "movsx", 5, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { R16, R8 | M8 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xBE0F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, R8 | M8 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xBE0F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, R8 | M8 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xBE0F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, R16 | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xBF0F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, R16 | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xBF0F, .oplen = 2,
	} } },
	{ "movsxd", 1, (struct x64LookupActualIns[]) { {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, R32 | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x63, .oplen = 1,
	} } },
	{ "movupd", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x100F, .oplen = 2,
		.preffered = true,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM | M128, XMM }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x110F, .oplen = 2,
	} } },
	{ "vmovupd", 4, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x10, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM | M256 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x10, .oplen = 1,
	}, {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM | M128, XMM }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x11, .oplen = 1,
		.preffered = true,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM | M256, YMM }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x11, .oplen = 1,
		.preffered = true,
	} } },
	{ "movups", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x100F, .oplen = 2,
		.preffered = true,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM | M128, XMM }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x110F, .oplen = 2,
	} } },
	{ "vmovups", 4, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x78, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x10, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7c, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM | M256 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x10, .oplen = 1,
	}, {
		.vex = 1, .vex_byte = 0x78, .modrmreq = true, .modrmreg = true,
		.args = { XMM | M128, XMM }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x11, .oplen = 1,
		.preffered = true,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7c, .modrmreq = true, .modrmreg = true,
		.args = { YMM | M256, YMM }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x11, .oplen = 1,
		.preffered = true,
	} } },
	{ "movzx", 5, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { R16, R8 | M8 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xB60F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, R8 | M8 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xB60F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, R8 | M8 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xB60F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, R16 | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xB70F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, R16 | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xB70F, .oplen = 2,
	} } },
	{ "mpsadbw", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x423A0F, .oplen = 3,
	} } },
	{ "vmpsadbw", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 3, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128, IMM8 }, .arglen = 4, .imm_oper = 4, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x42, .oplen = 1,
	}, {
		.vex = 0x80 | 3, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256, IMM8 }, .arglen = 4, .imm_oper = 4, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x42, .oplen = 1,
	} } },
	{ "mul", 4, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x20,
		.args = { R8 | M8 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xF6, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x20,
		.args = { R16 | M16 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xF7, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x20,
		.args = { R32 | M32 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xF7, .oplen = 1,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x20,
		.args = { R64 | M64 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xF7, .oplen = 1,
	} } },
	{ "mulpd", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x590F, .oplen = 2,
	} } },
	{ "vmulpd", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x59, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x59, .oplen = 1,
	} } },
	{ "mulps", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x590F, .oplen = 2,
	} } },
	{ "vmulps", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x78, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x59, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7c, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x59, .oplen = 1,
	} } },
	{ "mulsd", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF2, .preflen = 1, .opcode = 0x590F, .oplen = 2,
	} } },
	{ "vmulsd", 1, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x7b, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M64 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x59, .oplen = 1,
	} } },
	{ "mulss", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0x590F, .oplen = 2,
	} } },
	{ "vmulss", 1, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x7a, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M32 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x59, .oplen = 1,
	} } },
	{ "mulx", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x7b, .modrmreq = true, .modrmreg = true,
		.args = { R32, R32, R32 | M32 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xF6, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0xfb, .modrmreq = true, .modrmreg = true,
		.args = { R64, R64, R64 | M64 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xF6, .oplen = 1,
	} } },
	{ "mwait", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xC9010F, .oplen = 3,
	} } },
	{ "neg", 4, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x18,
		.args = { R8 | M8 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xF6, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x18,
		.args = { R16 | M16 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xF7, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x18,
		.args = { R32 | M32 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xF7, .oplen = 1,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x18,
		.args = { R64 | M64 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xF7, .oplen = 1,
	} } },
	{ "nop", 3, (struct x64LookupActualIns[]) { {
		.opcode = 0x90, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x0,
		.args = { R16 | M16 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0x1F0F, .oplen = 2,
	}, {
		.modrmreq = true, .modrm = 0x0,
		.args = { R32 | M32 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0x1F0F, .oplen = 2,
	} } },
	{ "not", 4, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x10,
		.args = { R8 | M8 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xF6, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x10,
		.args = { R16 | M16 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xF7, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x10,
		.args = { R32 | M32 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xF7, .oplen = 1,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x10,
		.args = { R64 | M64 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xF7, .oplen = 1,
	} } },
	{ "or", 21, (struct x64LookupActualIns[]) { {
		.args = { AL, IMM8 }, .arglen = 2, .imm_oper = 2,
		.opcode = 0x0C, .oplen = 1,
	}, {
		.args = { AX, IMM16 }, .arglen = 2, .imm_oper = 2,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x0D, .oplen = 1,
	}, {
		.args = { EAX, IMM32 }, .arglen = 2, .imm_oper = 2,
		.opcode = 0x0D, .oplen = 1,
	}, {
		.rex = 0x48,
		.args = { RAX, IMM32 }, .arglen = 2, .imm_oper = 2,
		.opcode = 0x0D, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x8,
		.args = { R8 | M8, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0x80, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x8,
		.args = { R16 | M16, IMM16 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x81, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x8,
		.args = { R32 | M32, IMM32 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0x81, .oplen = 1,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x8,
		.args = { R64 | M64, IMM32 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0x81, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x8,
		.args = { R16 | M16, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0x83, .oplen = 1,
		.preffered = true,
	}, {
		.modrmreq = true, .modrm = 0x8,
		.args = { R32 | M32, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0x83, .oplen = 1,
		.preffered = true,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x8,
		.args = { R64 | M64, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0x83, .oplen = 1,
		.preffered = true,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R8 | M8, R8 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x08, .oplen = 1,
		.preffered = true,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R8 | M8, RH }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x08, .oplen = 1,
		.preffered = true,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R16 | M16, R16 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x09, .oplen = 1,
		.preffered = true,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32 | M32, R32 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x09, .oplen = 1,
		.preffered = true,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64 | M64, R64 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x09, .oplen = 1,
		.preffered = true,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R8, R8 | M8 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x0A, .oplen = 1,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { RH, R8 | M8 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x0A, .oplen = 1,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R16, R16 | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x0B, .oplen = 1,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, R32 | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x0B, .oplen = 1,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, R64 | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x0B, .oplen = 1,
	} } },
	{ "orpd", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x560F, .oplen = 2,
	} } },
	{ "vorpd", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x56, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x56, .oplen = 1,
	} } },
	{ "orps", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x560F, .oplen = 2,
	} } },
	{ "vorps", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x78, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x56, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7c, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x56, .oplen = 1,
	} } },
	{ "out", 6, (struct x64LookupActualIns[]) { {
		.args = { IMM8, AL }, .arglen = 2, .imm_oper = 1,
		.opcode = 0xE6, .oplen = 1,
	}, {
		.args = { IMM8, AX }, .arglen = 2, .imm_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xE7, .oplen = 1,
	}, {
		.args = { IMM8, EAX }, .arglen = 2, .imm_oper = 1,
		.opcode = 0xE7, .oplen = 1,
	}, {
		.args = { DX, AL }, .arglen = 2,
		.opcode = 0xEE, .oplen = 1,
	}, {
		.args = { DX, AX }, .arglen = 2,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xEF, .oplen = 1,
	}, {
		.args = { DX, EAX }, .arglen = 2,
		.opcode = 0xEF, .oplen = 1,
	} } },
	{ "outs", 3, (struct x64LookupActualIns[]) { {
		.args = { DX, M8 }, .arglen = 2,
		.opcode = 0x6E, .oplen = 1,
	}, {
		.args = { DX, M16 }, .arglen = 2,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x6F, .oplen = 1,
	}, {
		.args = { DX, M32 }, .arglen = 2,
		.opcode = 0x6F, .oplen = 1,
	} } },
	{ "outsb", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0x6E, .oplen = 1,
	} } },
	{ "outsw", 1, (struct x64LookupActualIns[]) { {
		.prefixes = 0x66, .preflen = 1, .opcode = 0x6F, .oplen = 1,
	} } },
	{ "outsd", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0x6F, .oplen = 1,
	} } },
	{ "pabsb", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x1C380F, .oplen = 3,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x1C380F, .oplen = 3,
	} } },
	{ "pabsw", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x1D380F, .oplen = 3,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x1D380F, .oplen = 3,
	} } },
	{ "pabsd", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x1E380F, .oplen = 3,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x1E380F, .oplen = 3,
	} } },
	{ "vpabsb", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x1C, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM | M256 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x1C, .oplen = 1,
	} } },
	{ "vpabsw", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x1D, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM | M256 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x1D, .oplen = 1,
	} } },
	{ "vpabsd", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x1E, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM | M256 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x1E, .oplen = 1,
	} } },
	{ "packsswb", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x630F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x630F, .oplen = 2,
	} } },
	{ "packssdw", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x6B0F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x6B0F, .oplen = 2,
	} } },
	{ "vpacksswb", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x63, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x63, .oplen = 1,
	} } },
	{ "vpackssdw", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x6B, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x6B, .oplen = 1,
	} } },
	{ "packusdw", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x2B380F, .oplen = 3,
	} } },
	{ "vpackusdw", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x2B, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x2B, .oplen = 1,
	} } },
	{ "packuswb", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x670F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x670F, .oplen = 2,
	} } },
	{ "vpackuswb", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x67, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x67, .oplen = 1,
	} } },
	{ "paddb", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xFC0F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xFC0F, .oplen = 2,
	} } },
	{ "paddw", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xFD0F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xFD0F, .oplen = 2,
	} } },
	{ "paddd", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xFE0F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xFE0F, .oplen = 2,
	} } },
	{ "vpaddb", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xFC, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xFC, .oplen = 1,
	} } },
	{ "vpaddw", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xFD, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xFD, .oplen = 1,
	} } },
	{ "vpaddd", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xFE, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xFE, .oplen = 1,
	} } },
	{ "paddq", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xD40F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xD40F, .oplen = 2,
	} } },
	{ "vpaddq", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xD4, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xD4, .oplen = 1,
	} } },
	{ "paddsb", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xEC0F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xEC0F, .oplen = 2,
	} } },
	{ "paddsw", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xED0F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xED0F, .oplen = 2,
	} } },
	{ "vpaddsb", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xEC, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xEC, .oplen = 1,
	} } },
	{ "vpaddsw", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xED, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xED, .oplen = 1,
	} } },
	{ "paddusb", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xDC0F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xDC0F, .oplen = 2,
	} } },
	{ "paddusw", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xDD0F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xDD0F, .oplen = 2,
	} } },
	{ "vpaddusb", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xDC, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xDC, .oplen = 1,
	} } },
	{ "vpaddusw", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xDD, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xDD, .oplen = 1,
	} } },
	{ "palignr", 2, (struct x64LookupActualIns[]) { {
		.args = { MM, MM | M64, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x0F3A0F, .oplen = 3,
	}, {
		.args = { XMM, XMM | M128, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x0F3A0F, .oplen = 3,
	} } },
	{ "vpalignr", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 3, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128, IMM8 }, .arglen = 4, .imm_oper = 4, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x0F, .oplen = 1,
	}, {
		.vex = 0x80 | 3, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256, IMM8 }, .arglen = 4, .imm_oper = 4, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x0F, .oplen = 1,
	} } },
	{ "pand", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xDB0F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xDB0F, .oplen = 2,
	} } },
	{ "vpand", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xDB, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xDB, .oplen = 1,
	} } },
	{ "pandn", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xDF0F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xDF0F, .oplen = 2,
	} } },
	{ "vpandn", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xDF, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xDF, .oplen = 1,
	} } },
	{ "pause", 1, (struct x64LookupActualIns[]) { {
		.prefixes = 0xF3, .preflen = 1, .opcode = 0x90, .oplen = 1,
	} } },
	{ "pavgb", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xE00F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xE00F, .oplen = 2,
	} } },
	{ "pavgw", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xE30F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xE30F, .oplen = 2,
	} } },
	{ "vpavgb", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xE0, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xE0, .oplen = 1,
	} } },
	{ "vpavgw", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xE3, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xE3, .oplen = 1,
	} } },
	{ "pblendvb", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128, XMM_0 }, .arglen = 3, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x10380F, .oplen = 3,
	} } },
	{ "vpblendvb", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 3, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128, XMM }, .arglen = 4, .mem_oper = 3, .reg_oper = 1, .is4_oper = 4, .vex_oper = 2,
		.opcode = 0x4C, .oplen = 1,
	}, {
		.vex = 0x80 | 3, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256, YMM }, .arglen = 4, .mem_oper = 3, .reg_oper = 1, .is4_oper = 4, .vex_oper = 2,
		.opcode = 0x4C, .oplen = 1,
	} } },
	{ "pblendw", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x0E3A0F, .oplen = 3,
	} } },
	{ "vpblendw", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 3, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128, IMM8 }, .arglen = 4, .imm_oper = 4, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x0E, .oplen = 1,
	}, {
		.vex = 0x80 | 3, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256, IMM8 }, .arglen = 4, .imm_oper = 4, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x0E, .oplen = 1,
	} } },
	{ "pclmulqdq", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x443A0F, .oplen = 3,
	} } },
	{ "vpclmulqdq", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 3, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128, IMM8 }, .arglen = 4, .imm_oper = 4, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x44, .oplen = 1,
	} } },
	{ "pcmpeqb", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x740F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x740F, .oplen = 2,
	} } },
	{ "pcmpeqw", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x750F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x750F, .oplen = 2,
	} } },
	{ "pcmpeqd", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x760F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x760F, .oplen = 2,
	} } },
	{ "vpcmpeqb", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x74, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x74, .oplen = 1,
	} } },
	{ "vpcmpeqw", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x75, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x75, .oplen = 1,
	} } },
	{ "vpcmpeqd", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x76, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x76, .oplen = 1,
	} } },
	{ "pcmpeqq", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x29380F, .oplen = 3,
	} } },
	{ "vpcmpeqq", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x29, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x29, .oplen = 1,
	} } },
	{ "pcmpestri", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x613A0F, .oplen = 3,
	} } },
	{ "vpcmpestri", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 3, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x61, .oplen = 1,
	} } },
	{ "pcmpestrm", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x603A0F, .oplen = 3,
	} } },
	{ "vpcmpestrm", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 3, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x60, .oplen = 1,
	} } },
	{ "pcmpgtb", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x640F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x640F, .oplen = 2,
	} } },
	{ "pcmpgtw", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x650F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x650F, .oplen = 2,
	} } },
	{ "pcmpgtd", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x660F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x660F, .oplen = 2,
	} } },
	{ "vpcmpgtb", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x64, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x64, .oplen = 1,
	} } },
	{ "vpcmpgtw", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x65, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x65, .oplen = 1,
	} } },
	{ "vpcmpgtd", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x66, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x66, .oplen = 1,
	} } },
	{ "pcmpgtq", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x37380F, .oplen = 3,
	} } },
	{ "vpcmpgtq", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x37, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x37, .oplen = 1,
	} } },
	{ "pcmpistri", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x633A0F, .oplen = 3,
	} } },
	{ "vpcmpistri", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 3, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x63, .oplen = 1,
	} } },
	{ "pcmpistrm", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x623A0F, .oplen = 3,
	} } },
	{ "vpcmpistrm", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 3, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x62, .oplen = 1,
	} } },
	{ "pdep", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x7b, .modrmreq = true, .modrmreg = true,
		.args = { R32, R32, R32 | M32 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xF5, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0xfb, .modrmreq = true, .modrmreg = true,
		.args = { R64, R64, R64 | M64 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xF5, .oplen = 1,
	} } },
	{ "pext", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x7a, .modrmreq = true, .modrmreg = true,
		.args = { R32, R32, R32 | M32 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xF5, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0xfa, .modrmreq = true, .modrmreg = true,
		.args = { R64, R64, R64 | M64 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xF5, .oplen = 1,
	} } },
	{ "pextrb", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { R64 | R32 | M8, XMM, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 1, .reg_oper = 2,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x143A0F, .oplen = 3,
	} } },
	{ "pextrd", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { R32 | M32, XMM, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 1, .reg_oper = 2,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x163A0F, .oplen = 3,
	} } },
	{ "pextrq", 1, (struct x64LookupActualIns[]) { {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64 | M64, XMM, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 1, .reg_oper = 2,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x163A0F, .oplen = 3,
	} } },
	{ "vpextrb", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 3, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { R64 | R32 | M8, XMM, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x14, .oplen = 1,
	} } },
	{ "vpextrd", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 3, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { R32 | M32, XMM, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x16, .oplen = 1,
	} } },
	{ "vpextrq", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 3, .vex_byte = 0xf9, .modrmreq = true, .modrmreg = true,
		.args = { R64 | M64, XMM, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x16, .oplen = 1,
	} } },
	{ "pextrw", 3, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { R64 | R32, MM, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xC50F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R64 | R32, XMM, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xC50F, .oplen = 2,
		.preffered = true,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R64 | R32 | M16, XMM, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 1, .reg_oper = 2,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x153A0F, .oplen = 3,
	} } },
	{ "vpextrw", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { R64 | R32, XMM, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xC5, .oplen = 1,
		.preffered = true,
	}, {
		.vex = 0x80 | 3, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { R64 | R32 | M16, XMM, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x15, .oplen = 1,
	} } },
	{ "phaddw", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x01380F, .oplen = 3,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x01380F, .oplen = 3,
	} } },
	{ "phaddd", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x02380F, .oplen = 3,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x02380F, .oplen = 3,
	} } },
	{ "vphaddw", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x01, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x01, .oplen = 1,
	} } },
	{ "vphaddd", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x02, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x02, .oplen = 1,
	} } },
	{ "phaddsw", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x03380F, .oplen = 3,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x03380F, .oplen = 3,
	} } },
	{ "vphaddsw", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x03, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x03, .oplen = 1,
	} } },
	{ "phminposuw", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x41380F, .oplen = 3,
	} } },
	{ "vphminposuw", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x41, .oplen = 1,
	} } },
	{ "phsubw", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x05380F, .oplen = 3,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x05380F, .oplen = 3,
	} } },
	{ "phsubd", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x06380F, .oplen = 3,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x06380F, .oplen = 3,
	} } },
	{ "vphsubw", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x05, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x05, .oplen = 1,
	} } },
	{ "vphsubd", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x06, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x06, .oplen = 1,
	} } },
	{ "phsubsw", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x07380F, .oplen = 3,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x07380F, .oplen = 3,
	} } },
	{ "vphsubsw", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x07, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x07, .oplen = 1,
	} } },
	{ "pinsrb", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, R32 | M8, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x203A0F, .oplen = 3,
	} } },
	{ "pinsrd", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, R32 | M32, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x223A0F, .oplen = 3,
	} } },
	{ "vpinsrb", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 3, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, R32 | M8, IMM8 }, .arglen = 4, .imm_oper = 4, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x20, .oplen = 1,
	} } },
	{ "vpinsrd", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 3, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, R32 | M32, IMM8 }, .arglen = 4, .imm_oper = 4, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x22, .oplen = 1,
	} } },
	{ "vpinsrq", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 3, .vex_byte = 0xf9, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, R64 | M64, IMM8 }, .arglen = 4, .imm_oper = 4, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x22, .oplen = 1,
	} } },
	{ "pinsrw", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, R32 | M16, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xC40F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, R32 | M16, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xC40F, .oplen = 2,
	} } },
	{ "vpinsrw", 1, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, R32 | M16, IMM8 }, .arglen = 4, .imm_oper = 4, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xC4, .oplen = 1,
	} } },
	{ "pmaddubsw", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x04380F, .oplen = 3,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x04380F, .oplen = 3,
	} } },
	{ "vpmaddubsw", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x04, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x04, .oplen = 1,
	} } },
	{ "pmaddwd", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xF50F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xF50F, .oplen = 2,
	} } },
	{ "vpmaddwd", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xF5, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xF5, .oplen = 1,
	} } },
	{ "pmaxsb", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x3C380F, .oplen = 3,
	} } },
	{ "vpmaxsb", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x3C, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x3C, .oplen = 1,
	} } },
	{ "pmaxsd", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x3D380F, .oplen = 3,
	} } },
	{ "vpmaxsd", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x3D, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x3D, .oplen = 1,
	} } },
	{ "pmaxsw", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xEE0F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xEE0F, .oplen = 2,
	} } },
	{ "vpmaxsw", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xEE, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xEE, .oplen = 1,
	} } },
	{ "pmaxub", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xDE0F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xDE0F, .oplen = 2,
	} } },
	{ "vpmaxub", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xDE, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xDE, .oplen = 1,
	} } },
	{ "pmaxud", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x3F380F, .oplen = 3,
	} } },
	{ "vpmaxud", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x3F, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x3F, .oplen = 1,
	} } },
	{ "pmaxuw", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x3E380F, .oplen = 3,
	} } },
	{ "vpmaxuw", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x3E, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x3E, .oplen = 1,
	} } },
	{ "pminsb", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x38380F, .oplen = 3,
	} } },
	{ "vpminsb", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x38, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x38, .oplen = 1,
	} } },
	{ "pminsd", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x39380F, .oplen = 3,
	} } },
	{ "vpminsd", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x39, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x39, .oplen = 1,
	} } },
	{ "pminsw", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xEA0F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xEA0F, .oplen = 2,
	} } },
	{ "vpminsw", 1, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xEA, .oplen = 1,
	} } },
	{ "pminub", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xDA0F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xDA0F, .oplen = 2,
	} } },
	{ "vpminub", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xDA, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xDA, .oplen = 1,
	} } },
	{ "pminud", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x3B380F, .oplen = 3,
	} } },
	{ "vpminud", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x3B, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x3B, .oplen = 1,
	} } },
	{ "pminuw", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x3A380F, .oplen = 3,
	} } },
	{ "vpminuw", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x3A, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x3A, .oplen = 1,
	} } },
	{ "pmovmskb", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { R64 | R32, MM }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xD70F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R64 | R32, XMM }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xD70F, .oplen = 2,
	} } },
	{ "vpmovmskb", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { R64 | R32, XMM }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xD7, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { R64 | R32, YMM }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xD7, .oplen = 1,
	} } },
	{ "pmovsxbw", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x2038, .oplen = 2,
	} } },
	{ "pmovsxbd", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x2138, .oplen = 2,
	} } },
	{ "pmovsxbq", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x2238, .oplen = 2,
	} } },
	{ "pmovsxwd", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x2338, .oplen = 2,
	} } },
	{ "pmovsxwq", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x2438, .oplen = 2,
	} } },
	{ "pmovsxdq", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x2538, .oplen = 2,
	} } },
	{ "vpmovsxbw", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x20, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x20, .oplen = 1,
	} } },
	{ "vpmovsxbd", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x21, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, XMM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x21, .oplen = 1,
	} } },
	{ "vpmovsxbq", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x22, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, XMM | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x22, .oplen = 1,
	} } },
	{ "vpmovsxwd", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x23, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x23, .oplen = 1,
	} } },
	{ "vpmovsxwq", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x24, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, XMM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x24, .oplen = 1,
	} } },
	{ "vpmovsxdq", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x25, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x25, .oplen = 1,
	} } },
	{ "pmovzxbw", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x3038, .oplen = 2,
	} } },
	{ "pmovzxbd", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x3138, .oplen = 2,
	} } },
	{ "pmovzxbq", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x3238, .oplen = 2,
	} } },
	{ "pmovzxwd", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x3338, .oplen = 2,
	} } },
	{ "pmovzxwq", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x3438, .oplen = 2,
	} } },
	{ "pmovzxdq", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x3538, .oplen = 2,
	} } },
	{ "vpmovzxbw", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x30, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x30, .oplen = 1,
	} } },
	{ "vpmovzxbd", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x31, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, XMM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x31, .oplen = 1,
	} } },
	{ "vpmovzxbq", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x32, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, XMM | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x32, .oplen = 1,
	} } },
	{ "vpmovzxwd", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x33, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x33, .oplen = 1,
	} } },
	{ "vpmovzxwq", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x34, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, XMM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x34, .oplen = 1,
	} } },
	{ "vpmovzxdq", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x35, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x35, .oplen = 1,
	} } },
	{ "pmuldq", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x28380F, .oplen = 3,
	} } },
	{ "vpmuldq", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x28, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x28, .oplen = 1,
	} } },
	{ "pmulhrsw", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x0B380F, .oplen = 3,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x0B380F, .oplen = 3,
	} } },
	{ "vpmulhrsw", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x0B, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x0B, .oplen = 1,
	} } },
	{ "pmulhuw", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xE40F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xE40F, .oplen = 2,
	} } },
	{ "vpmulhuw", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xE4, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xE4, .oplen = 1,
	} } },
	{ "pmulhw", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xE50F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xE50F, .oplen = 2,
	} } },
	{ "vpmulhw", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xE5, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xE5, .oplen = 1,
	} } },
	{ "pmulld", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x40380F, .oplen = 3,
	} } },
	{ "vpmulld", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x40, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x40, .oplen = 1,
	} } },
	{ "pmullw", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xD50F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xD50F, .oplen = 2,
	} } },
	{ "vpmullw", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xD5, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xD5, .oplen = 1,
	} } },
	{ "pmuludq", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xF40F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xF40F, .oplen = 2,
	} } },
	{ "vpmuludq", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xF4, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xF4, .oplen = 1,
	} } },
	{ "pop", 8, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x0,
		.args = { R16 | M16 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0x8F, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x0,
		.args = { R64 | M64 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0x8F, .oplen = 1,
	}, {
		.args = { R16 }, .arglen = 1, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x58, .oplen = 1,
		.preffered = true,
	}, {
		.args = { R64 }, .arglen = 1, .reg_oper = 1,
		.opcode = 0x58, .oplen = 1,
		.preffered = true,
	}, {
		.args = { FS, PREF66 }, .arglen = 2,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xA10F, .oplen = 2,
	}, {
		.args = { FS }, .arglen = 1,
		.opcode = 0xA10F, .oplen = 2,
	}, {
		.args = { GS, PREF66 }, .arglen = 2,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xA90F, .oplen = 2,
	}, {
		.args = { GS }, .arglen = 1,
		.opcode = 0xA90F, .oplen = 2,
	} } },
	{ "popcnt", 3, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { R16, R16 | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF366, .preflen = 2, .opcode = 0xB80F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, R32 | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0xB80F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, R64 | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0xB80F, .oplen = 2,
	} } },
	{ "popf", 1, (struct x64LookupActualIns[]) { {
		.prefixes = 0x66, .preflen = 1, .opcode = 0x9D, .oplen = 1,
	} } },
	{ "popfq", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0x9D, .oplen = 1,
	} } },
	{ "por", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xEB0F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xEB0F, .oplen = 2,
	} } },
	{ "vpor", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xEB, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xEB, .oplen = 1,
	} } },
	{ "prefetcht0", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x8,
		.args = { M8 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0x180F, .oplen = 2,
	} } },
	{ "prefetcht1", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x10,
		.args = { M8 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0x180F, .oplen = 2,
	} } },
	{ "prefetcht2", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x18,
		.args = { M8 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0x180F, .oplen = 2,
	} } },
	{ "prefetchnta", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x0,
		.args = { M8 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0x180F, .oplen = 2,
	} } },
	{ "psadbw", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xF60F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xF60F, .oplen = 2,
	} } },
	{ "vpsadbw", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xF6, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xF6, .oplen = 1,
	} } },
	{ "pshufb", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x00380F, .oplen = 3,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x00380F, .oplen = 3,
	} } },
	{ "vpshufb", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x00, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x00, .oplen = 1,
	} } },
	{ "pshufd", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x700F, .oplen = 2,
	} } },
	{ "vpshufd", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x70, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM | M256, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x70, .oplen = 1,
	} } },
	{ "pshufhw", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0x700F, .oplen = 2,
	} } },
	{ "vpshufhw", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x7a, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x70, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7e, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM | M256, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x70, .oplen = 1,
	} } },
	{ "pshuflw", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF2, .preflen = 1, .opcode = 0x700F, .oplen = 2,
	} } },
	{ "vpshuflw", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x7b, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x70, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7f, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM | M256, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x70, .oplen = 1,
	} } },
	{ "pshufw", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x700F, .oplen = 2,
	} } },
	{ "psignb", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x08380F, .oplen = 3,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x08380F, .oplen = 3,
	} } },
	{ "psignw", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x09380F, .oplen = 3,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x09380F, .oplen = 3,
	} } },
	{ "psignd", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x0A380F, .oplen = 3,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x0A380F, .oplen = 3,
	} } },
	{ "vpsignb", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x08, .oplen = 1,
	} } },
	{ "vpsignw", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x09, .oplen = 1,
	} } },
	{ "vpsignd", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x0A, .oplen = 1,
	} } },
	{ "pslldq", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x38,
		.args = { XMM, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x730F, .oplen = 2,
	} } },
	{ "vpslldq", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrm = 0x38,
		.args = { XMM, XMM, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .vex_oper = 1,
		.opcode = 0x73, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrm = 0x38,
		.args = { YMM, YMM, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .vex_oper = 1,
		.opcode = 0x73, .oplen = 1,
	} } },
	{ "psllw", 4, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xF10F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xF10F, .oplen = 2,
	}, {
		.modrmreq = true, .modrm = 0x30,
		.args = { MM, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0x710F, .oplen = 2,
	}, {
		.modrmreq = true, .modrm = 0x30,
		.args = { XMM, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x710F, .oplen = 2,
		.preffered = true,
	} } },
	{ "pslld", 4, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xF20F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xF20F, .oplen = 2,
	}, {
		.modrmreq = true, .modrm = 0x30,
		.args = { MM, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0x720F, .oplen = 2,
	}, {
		.modrmreq = true, .modrm = 0x30,
		.args = { XMM, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x720F, .oplen = 2,
	} } },
	{ "psllq", 4, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xF30F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xF30F, .oplen = 2,
	}, {
		.modrmreq = true, .modrm = 0x30,
		.args = { MM, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0x730F, .oplen = 2,
	}, {
		.modrmreq = true, .modrm = 0x30,
		.args = { XMM, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x730F, .oplen = 2,
	} } },
	{ "vpsllw", 4, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xF1, .oplen = 1,
	}, {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrm = 0x30,
		.args = { XMM, XMM, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .vex_oper = 1,
		.opcode = 0x71, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xF1, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrm = 0x30,
		.args = { YMM, YMM, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .vex_oper = 1,
		.opcode = 0x71, .oplen = 1,
	} } },
	{ "vpslld", 4, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xF2, .oplen = 1,
	}, {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrm = 0x30,
		.args = { XMM, XMM, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .vex_oper = 1,
		.opcode = 0x72, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xF2, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrm = 0x30,
		.args = { YMM, YMM, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .vex_oper = 1,
		.opcode = 0x72, .oplen = 1,
	} } },
	{ "vpsllq", 4, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xF3, .oplen = 1,
	}, {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrm = 0x30,
		.args = { XMM, XMM, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .vex_oper = 1,
		.opcode = 0x73, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xF3, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrm = 0x30,
		.args = { YMM, YMM, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .vex_oper = 1,
		.opcode = 0x73, .oplen = 1,
	} } },
	{ "psraw", 4, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xE10F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xE10F, .oplen = 2,
	}, {
		.modrmreq = true, .modrm = 0x20,
		.args = { MM, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0x710F, .oplen = 2,
	}, {
		.modrmreq = true, .modrm = 0x20,
		.args = { XMM, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x710F, .oplen = 2,
	} } },
	{ "psrad", 4, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xE20F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xE20F, .oplen = 2,
	}, {
		.modrmreq = true, .modrm = 0x20,
		.args = { MM, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0x720F, .oplen = 2,
	}, {
		.modrmreq = true, .modrm = 0x20,
		.args = { XMM, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x720F, .oplen = 2,
	} } },
	{ "vpsraw", 4, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xE1, .oplen = 1,
	}, {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrm = 0x20,
		.args = { XMM, XMM, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .vex_oper = 1,
		.opcode = 0x71, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xE1, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrm = 0x20,
		.args = { YMM, YMM, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .vex_oper = 1,
		.opcode = 0x71, .oplen = 1,
	} } },
	{ "vpsrad", 4, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xE2, .oplen = 1,
	}, {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrm = 0x20,
		.args = { XMM, XMM, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .vex_oper = 1,
		.opcode = 0x72, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xE2, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrm = 0x20,
		.args = { YMM, YMM, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .vex_oper = 1,
		.opcode = 0x72, .oplen = 1,
	} } },
	{ "psrldq", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x18,
		.args = { XMM, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x730F, .oplen = 2,
	} } },
	{ "vpsrldq", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrm = 0x18,
		.args = { XMM, XMM, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .vex_oper = 1,
		.opcode = 0x73, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrm = 0x18,
		.args = { YMM, YMM, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .vex_oper = 1,
		.opcode = 0x73, .oplen = 1,
	} } },
	{ "psrlw", 4, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xD10F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xD10F, .oplen = 2,
	}, {
		.modrmreq = true, .modrm = 0x10,
		.args = { MM, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0x710F, .oplen = 2,
	}, {
		.modrmreq = true, .modrm = 0x10,
		.args = { XMM, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x710F, .oplen = 2,
	} } },
	{ "psrld", 4, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xD20F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xD20F, .oplen = 2,
	}, {
		.modrmreq = true, .modrm = 0x10,
		.args = { MM, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0x720F, .oplen = 2,
	}, {
		.modrmreq = true, .modrm = 0x10,
		.args = { XMM, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x720F, .oplen = 2,
	} } },
	{ "psrlq", 4, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xD30F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xD30F, .oplen = 2,
	}, {
		.modrmreq = true, .modrm = 0x10,
		.args = { MM, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0x730F, .oplen = 2,
	}, {
		.modrmreq = true, .modrm = 0x10,
		.args = { XMM, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x730F, .oplen = 2,
	} } },
	{ "vpsrlw", 4, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xD1, .oplen = 1,
	}, {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrm = 0x10,
		.args = { XMM, XMM, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .vex_oper = 1,
		.opcode = 0x71, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xD1, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrm = 0x10,
		.args = { YMM, YMM, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .vex_oper = 1,
		.opcode = 0x71, .oplen = 1,
	} } },
	{ "vpsrld", 4, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xD2, .oplen = 1,
	}, {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrm = 0x10,
		.args = { XMM, XMM, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .vex_oper = 1,
		.opcode = 0x72, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xD2, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrm = 0x10,
		.args = { YMM, YMM, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .vex_oper = 1,
		.opcode = 0x72, .oplen = 1,
	} } },
	{ "vpsrlq", 4, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xD3, .oplen = 1,
	}, {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrm = 0x10,
		.args = { XMM, XMM, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .vex_oper = 1,
		.opcode = 0x73, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xD3, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrm = 0x10,
		.args = { YMM, YMM, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .vex_oper = 1,
		.opcode = 0x73, .oplen = 1,
	} } },
	{ "psubb", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xF80F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xF80F, .oplen = 2,
	} } },
	{ "psubw", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xF90F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xF90F, .oplen = 2,
	} } },
	{ "psubd", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xFA0F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xFA0F, .oplen = 2,
	} } },
	{ "vpsubb", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xF8, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xF8, .oplen = 1,
	} } },
	{ "vpsubw", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xF9, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xF9, .oplen = 1,
	} } },
	{ "vpsubd", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xFA, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xFA, .oplen = 1,
	} } },
	{ "psubq", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xFB0F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xFB0F, .oplen = 2,
	} } },
	{ "vpsubq", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xFB, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xFB, .oplen = 1,
	} } },
	{ "psubsb", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xE80F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xE80F, .oplen = 2,
	} } },
	{ "psubsw", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xE90F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xE90F, .oplen = 2,
	} } },
	{ "vpsubsb", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xE8, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xE8, .oplen = 1,
	} } },
	{ "vpsubsw", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xE9, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xE9, .oplen = 1,
	} } },
	{ "psubusb", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xD80F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xD80F, .oplen = 2,
	} } },
	{ "psubusw", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xD90F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xD90F, .oplen = 2,
	} } },
	{ "vpsubusb", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xD8, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xD8, .oplen = 1,
	} } },
	{ "vpsubusw", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xD9, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xD9, .oplen = 1,
	} } },
	{ "ptest", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x17380F, .oplen = 3,
	} } },
	{ "vptest", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x17, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM | M256 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x17, .oplen = 1,
	} } },
	{ "punpckhbw", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x680F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x680F, .oplen = 2,
	} } },
	{ "punpckhwd", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x690F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x690F, .oplen = 2,
	} } },
	{ "punpckhdq", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x6A0F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x6A0F, .oplen = 2,
	} } },
	{ "punpckhqdq", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x6D0F, .oplen = 2,
	} } },
	{ "vpunpckhbw", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x68, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x68, .oplen = 1,
	} } },
	{ "vpunpckhwd", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x69, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x69, .oplen = 1,
	} } },
	{ "vpunpckhdq", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x6A, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x6A, .oplen = 1,
	} } },
	{ "vpunpckhqdq", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x6D, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x6D, .oplen = 1,
	} } },
	{ "punpcklbw", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x600F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x600F, .oplen = 2,
	} } },
	{ "punpcklwd", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x610F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x610F, .oplen = 2,
	} } },
	{ "punpckldq", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x620F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x620F, .oplen = 2,
	} } },
	{ "punpcklqdq", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x6C0F, .oplen = 2,
	} } },
	{ "vpunpcklbw", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x60, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x60, .oplen = 1,
	} } },
	{ "vpunpcklwd", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x61, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x61, .oplen = 1,
	} } },
	{ "vpunpckldq", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x62, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x62, .oplen = 1,
	} } },
	{ "vpunpcklqdq", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x6C, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x6C, .oplen = 1,
	} } },
	{ "push", 6, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x30,
		.args = { R16 | M16 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xFF, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x30,
		.args = { R64 | M64 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xFF, .oplen = 1,
	}, {
		.args = { R16 }, .arglen = 1, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x50, .oplen = 1,
		.preffered = true,
	}, {
		.args = { R64 }, .arglen = 1, .reg_oper = 1,
		.opcode = 0x50, .oplen = 1,
		.preffered = true,
	}, {
		.args = { FS }, .arglen = 1,
		.opcode = 0xA00F, .oplen = 2,
	}, {
		.args = { GS }, .arglen = 1,
		.opcode = 0xA80F, .oplen = 2,
	} } },
	{ "pushq", 3, (struct x64LookupActualIns[]) { {
		.args = { IMM8 }, .arglen = 1, .imm_oper = 1,
		.opcode = 0x6A, .oplen = 1,
	}, {
		.args = { IMM16 }, .arglen = 1, .imm_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x68, .oplen = 1,
	}, {
		.args = { IMM32 }, .arglen = 1, .imm_oper = 1,
		.opcode = 0x68, .oplen = 1,
	} } },
	{ "pushw", 2, (struct x64LookupActualIns[]) { {
		.args = { IMM8 }, .arglen = 1, .imm_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x6A, .oplen = 1,
	}, {
		.args = { IMM16 }, .arglen = 1, .imm_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x68, .oplen = 1,
	} } },
	{ "pushf", 1, (struct x64LookupActualIns[]) { {
		.prefixes = 0x66, .preflen = 1, .opcode = 0x9C, .oplen = 1,
	} } },
	{ "pushfq", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0x9C, .oplen = 1,
	} } },
	{ "pxor", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { MM, MM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xEF0F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xEF0F, .oplen = 2,
	} } },
	{ "vpxor", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xEF, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xEF, .oplen = 1,
	} } },
	{ "rcl", 12, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x10,
		.args = { R8 | M8, ONE }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD0, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x10,
		.args = { R8 | M8, CL }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD2, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x10,
		.args = { R8 | M8, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0xC0, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x10,
		.args = { R16 | M16, ONE }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD1, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x10,
		.args = { R16 | M16, CL }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD3, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x10,
		.args = { R16 | M16, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0xC1, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x10,
		.args = { R32 | M32, ONE }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD1, .oplen = 1,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x10,
		.args = { R64 | M64, ONE }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD1, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x10,
		.args = { R32 | M32, CL }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD3, .oplen = 1,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x10,
		.args = { R64 | M64, CL }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD3, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x10,
		.args = { R32 | M32, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0xC1, .oplen = 1,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x10,
		.args = { R64 | M64, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0xC1, .oplen = 1,
	} } },
	{ "rcr", 12, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x18,
		.args = { R8 | M8, ONE }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD0, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x18,
		.args = { R8 | M8, CL }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD2, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x18,
		.args = { R8 | M8, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0xC0, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x18,
		.args = { R16 | M16, ONE }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD1, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x18,
		.args = { R16 | M16, CL }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD3, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x18,
		.args = { R16 | M16, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0xC1, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x18,
		.args = { R32 | M32, ONE }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD1, .oplen = 1,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x18,
		.args = { R64 | M64, ONE }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD1, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x18,
		.args = { R32 | M32, CL }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD3, .oplen = 1,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x18,
		.args = { R64 | M64, CL }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD3, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x18,
		.args = { R32 | M32, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0xC1, .oplen = 1,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x18,
		.args = { R64 | M64, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0xC1, .oplen = 1,
	} } },
	{ "rol", 12, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x0,
		.args = { R8 | M8, ONE }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD0, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x0,
		.args = { R8 | M8, CL }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD2, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x0,
		.args = { R8 | M8, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0xC0, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x0,
		.args = { R16 | M16, ONE }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD1, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x0,
		.args = { R16 | M16, CL }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD3, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x0,
		.args = { R16 | M16, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0xC1, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x0,
		.args = { R32 | M32, ONE }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD1, .oplen = 1,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x0,
		.args = { R64 | M64, ONE }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD1, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x0,
		.args = { R32 | M32, CL }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD3, .oplen = 1,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x0,
		.args = { R64 | M64, CL }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD3, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x0,
		.args = { R32 | M32, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0xC1, .oplen = 1,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x0,
		.args = { R64 | M64, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0xC1, .oplen = 1,
	} } },
	{ "ror", 12, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x8,
		.args = { R8 | M8, ONE }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD0, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x8,
		.args = { R8 | M8, CL }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD2, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x8,
		.args = { R8 | M8, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0xC0, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x8,
		.args = { R16 | M16, ONE }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD1, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x8,
		.args = { R16 | M16, CL }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD3, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x8,
		.args = { R16 | M16, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0xC1, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x8,
		.args = { R32 | M32, ONE }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD1, .oplen = 1,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x8,
		.args = { R64 | M64, ONE }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD1, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x8,
		.args = { R32 | M32, CL }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD3, .oplen = 1,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x8,
		.args = { R64 | M64, CL }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD3, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x8,
		.args = { R32 | M32, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0xC1, .oplen = 1,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x8,
		.args = { R64 | M64, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0xC1, .oplen = 1,
	} } },
	{ "rcpps", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x530F, .oplen = 2,
	} } },
	{ "vrcpps", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x78, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x53, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7c, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM | M256 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x53, .oplen = 1,
	} } },
	{ "rcpss", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0x530F, .oplen = 2,
	} } },
	{ "vrcpss", 1, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x7a, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M32 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x53, .oplen = 1,
	} } },
	{ "rdfsbase", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x0,
		.args = { R32 }, .arglen = 1, .mem_oper = 1,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0xAE0F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x0,
		.args = { R64 }, .arglen = 1, .mem_oper = 1,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0xAE0F, .oplen = 2,
	} } },
	{ "rdgsbase", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x8,
		.args = { R32 }, .arglen = 1, .mem_oper = 1,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0xAE0F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x8,
		.args = { R64 }, .arglen = 1, .mem_oper = 1,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0xAE0F, .oplen = 2,
	} } },
	{ "rdmsr", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0x320F, .oplen = 2,
	} } },
	{ "rdpmc", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0x330F, .oplen = 2,
	} } },
	{ "rdrand", 3, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x30,
		.args = { R16 }, .arglen = 1, .mem_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xC70F, .oplen = 2,
	}, {
		.modrmreq = true, .modrm = 0x30,
		.args = { R32 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xC70F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x30,
		.args = { R64 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xC70F, .oplen = 2,
	} } },
	{ "rdtsc", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0x310F, .oplen = 2,
	} } },
	{ "rdtscp", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xF9010F, .oplen = 3,
	} } },
	{ "rep ins", 5, (struct x64LookupActualIns[]) { {
		.args = { M8, DX }, .arglen = 2,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0x6C, .oplen = 1,
		.preffered = true,
	}, {
		.rex = 0x48,
		.args = { M8, DX }, .arglen = 2,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0x6C, .oplen = 1,
	}, {
		.args = { M16, DX }, .arglen = 2,
		.prefixes = 0xF366, .preflen = 2, .opcode = 0x6D, .oplen = 1,
		.preffered = true,
	}, {
		.args = { M32, DX }, .arglen = 2,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0x6D, .oplen = 1,
		.preffered = true,
	}, {
		.rex = 0x48,
		.args = { M64, DX }, .arglen = 2,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0x6D, .oplen = 1,
	} } },
	{ "rep movs", 5, (struct x64LookupActualIns[]) { {
		.args = { M8, M8 }, .arglen = 2,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0xA4, .oplen = 1,
		.preffered = true,
	}, {
		.rex = 0x48,
		.args = { M8, M8 }, .arglen = 2,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0xA4, .oplen = 1,
	}, {
		.args = { M16, M16 }, .arglen = 2,
		.prefixes = 0xF366, .preflen = 2, .opcode = 0xA5, .oplen = 1,
		.preffered = true,
	}, {
		.args = { M32, M32 }, .arglen = 2,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0xA5, .oplen = 1,
		.preffered = true,
	}, {
		.rex = 0x48,
		.args = { M64, M64 }, .arglen = 2,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0xA5, .oplen = 1,
	} } },
	{ "rep outs", 5, (struct x64LookupActualIns[]) { {
		.args = { DX, M8 }, .arglen = 2,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0x6E, .oplen = 1,
		.preffered = true,
	}, {
		.rex = 0x48,
		.args = { DX, M8 }, .arglen = 2,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0x6E, .oplen = 1,
	}, {
		.args = { DX, M16 }, .arglen = 2,
		.prefixes = 0xF366, .preflen = 2, .opcode = 0x6F, .oplen = 1,
		.preffered = true,
	}, {
		.args = { DX, M32 }, .arglen = 2,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0x6F, .oplen = 1,
		.preffered = true,
	}, {
		.rex = 0x48,
		.args = { DX, M64 }, .arglen = 2,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0x6F, .oplen = 1,
	} } },
	{ "rep lods", 5, (struct x64LookupActualIns[]) { {
		.args = { AL }, .arglen = 1,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0xAC, .oplen = 1,
		.preffered = true,
	}, {
		.rex = 0x48,
		.args = { AL }, .arglen = 1,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0xAC, .oplen = 1,
	}, {
		.args = { AX }, .arglen = 1,
		.prefixes = 0xF366, .preflen = 2, .opcode = 0xAD, .oplen = 1,
		.preffered = true,
	}, {
		.args = { EAX }, .arglen = 1,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0xAD, .oplen = 1,
		.preffered = true,
	}, {
		.rex = 0x48,
		.args = { RAX }, .arglen = 1,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0xAD, .oplen = 1,
	} } },
	{ "rep stos", 5, (struct x64LookupActualIns[]) { {
		.args = { M8 }, .arglen = 1,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0xAA, .oplen = 1,
		.preffered = true,
	}, {
		.rex = 0x48,
		.args = { M8 }, .arglen = 1,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0xAA, .oplen = 1,
	}, {
		.args = { M16 }, .arglen = 1,
		.prefixes = 0xF366, .preflen = 2, .opcode = 0xAB, .oplen = 1,
		.preffered = true,
	}, {
		.args = { M32 }, .arglen = 1,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0xAB, .oplen = 1,
		.preffered = true,
	}, {
		.rex = 0x48,
		.args = { M64 }, .arglen = 1,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0xAB, .oplen = 1,
	} } },
	{ "repe cmps", 5, (struct x64LookupActualIns[]) { {
		.args = { M8, M8 }, .arglen = 2,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0xA6, .oplen = 1,
		.preffered = true,
	}, {
		.rex = 0x48,
		.args = { M8, M8 }, .arglen = 2,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0xA6, .oplen = 1,
	}, {
		.args = { M16, M16 }, .arglen = 2,
		.prefixes = 0xF366, .preflen = 2, .opcode = 0xA7, .oplen = 1,
		.preffered = true,
	}, {
		.args = { M32, M32 }, .arglen = 2,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0xA7, .oplen = 1,
		.preffered = true,
	}, {
		.rex = 0x48,
		.args = { M64, M64 }, .arglen = 2,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0xA7, .oplen = 1,
	} } },
	{ "repe scas", 5, (struct x64LookupActualIns[]) { {
		.args = { M8 }, .arglen = 1,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0xAE, .oplen = 1,
		.preffered = true,
	}, {
		.rex = 0x48,
		.args = { M8 }, .arglen = 1,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0xAE, .oplen = 1,
	}, {
		.args = { M16 }, .arglen = 1,
		.prefixes = 0xF366, .preflen = 2, .opcode = 0xAF, .oplen = 1,
		.preffered = true,
	}, {
		.args = { M32 }, .arglen = 1,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0xAF, .oplen = 1,
		.preffered = true,
	}, {
		.rex = 0x48,
		.args = { M64 }, .arglen = 1,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0xAF, .oplen = 1,
	} } },
	{ "repne cmps", 5, (struct x64LookupActualIns[]) { {
		.args = { M8, M8 }, .arglen = 2,
		.prefixes = 0xF2, .preflen = 1, .opcode = 0xA6, .oplen = 1,
		.preffered = true,
	}, {
		.rex = 0x48,
		.args = { M8, M8 }, .arglen = 2,
		.prefixes = 0xF2, .preflen = 1, .opcode = 0xA6, .oplen = 1,
	}, {
		.args = { M16, M16 }, .arglen = 2,
		.prefixes = 0xF266, .preflen = 2, .opcode = 0xA7, .oplen = 1,
		.preffered = true,
	}, {
		.args = { M32, M32 }, .arglen = 2,
		.prefixes = 0xF2, .preflen = 1, .opcode = 0xA7, .oplen = 1,
		.preffered = true,
	}, {
		.rex = 0x48,
		.args = { M64, M64 }, .arglen = 2,
		.prefixes = 0xF2, .preflen = 1, .opcode = 0xA7, .oplen = 1,
	} } },
	{ "repne scas", 5, (struct x64LookupActualIns[]) { {
		.args = { M8 }, .arglen = 1,
		.prefixes = 0xF2, .preflen = 1, .opcode = 0xAE, .oplen = 1,
		.preffered = true,
	}, {
		.rex = 0x48,
		.args = { M8 }, .arglen = 1,
		.prefixes = 0xF2, .preflen = 1, .opcode = 0xAE, .oplen = 1,
	}, {
		.args = { M16 }, .arglen = 1,
		.prefixes = 0xF266, .preflen = 2, .opcode = 0xAF, .oplen = 1,
		.preffered = true,
	}, {
		.args = { M32 }, .arglen = 1,
		.prefixes = 0xF2, .preflen = 1, .opcode = 0xAF, .oplen = 1,
		.preffered = true,
	}, {
		.rex = 0x48,
		.args = { M64 }, .arglen = 1,
		.prefixes = 0xF2, .preflen = 1, .opcode = 0xAF, .oplen = 1,
	} } },
	{ "ret", 4, (struct x64LookupActualIns[]) { {
		.opcode = 0xC3, .oplen = 1,
	}, {
		.args = { FAR }, .arglen = 1,
		.opcode = 0xCB, .oplen = 1,
	}, {
		.args = { IMM16 }, .arglen = 1, .imm_oper = 1,
		.opcode = 0xC2, .oplen = 1,
	}, {
		.args = { IMM16, FAR }, .arglen = 2, .imm_oper = 1,
		.opcode = 0xCA, .oplen = 1,
	} } },
	{ "rorx", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 3, .vex_byte = 0x7b, .modrmreq = true, .modrmreg = true,
		.args = { R32, R32 | M32, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xF0, .oplen = 1,
	}, {
		.vex = 0x80 | 3, .vex_byte = 0xfb, .modrmreq = true, .modrmreg = true,
		.args = { R64, R64 | M64, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xF0, .oplen = 1,
	} } },
	{ "roundpd", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x093A0F, .oplen = 3,
	} } },
	{ "vroundpd", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 3, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x09, .oplen = 1,
	}, {
		.vex = 0x80 | 3, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM | M256, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x09, .oplen = 1,
	} } },
	{ "roundps", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x083A0F, .oplen = 3,
	} } },
	{ "vroundps", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 3, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x08, .oplen = 1,
	}, {
		.vex = 0x80 | 3, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM | M256, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x08, .oplen = 1,
	} } },
	{ "roundsd", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M64, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x0B3A0F, .oplen = 3,
	} } },
	{ "vroundsd", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 3, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M64, IMM8 }, .arglen = 4, .imm_oper = 4, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x0B, .oplen = 1,
	} } },
	{ "roundss", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M32, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x0A3A0F, .oplen = 3,
	} } },
	{ "vroundss", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 3, .vex_byte = 0x79,
		.args = { XMM, XMM, XMM | M32, IMM8 }, .arglen = 4, .imm_oper = 4, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x0A, .oplen = 1,
	} } },
	{ "rsqrtps", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x520F, .oplen = 2,
	} } },
	{ "vrsqrtps", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x78, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x52, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7c, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM | M256 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x52, .oplen = 1,
	} } },
	{ "rsqrtss", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0x520F, .oplen = 2,
	} } },
	{ "vrsqrtss", 1, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x7a, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M32 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x52, .oplen = 1,
	} } },
	{ "sahf", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0x9E, .oplen = 1,
	} } },
	{ "sal", 12, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x20,
		.args = { R8 | M8, ONE }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD0, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x20,
		.args = { R8 | M8, CL }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD2, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x20,
		.args = { R8 | M8, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0xC0, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x20,
		.args = { R16 | M16, ONE }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD1, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x20,
		.args = { R16 | M16, CL }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD3, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x20,
		.args = { R16 | M16, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0xC1, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x20,
		.args = { R32 | M32, ONE }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD1, .oplen = 1,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x20,
		.args = { R64 | M64, ONE }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD1, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x20,
		.args = { R32 | M32, CL }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD3, .oplen = 1,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x20,
		.args = { R64 | M64, CL }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD3, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x20,
		.args = { R32 | M32, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0xC1, .oplen = 1,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x20,
		.args = { R64 | M64, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0xC1, .oplen = 1,
	} } },
	{ "sar", 12, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x38,
		.args = { R8 | M8, ONE }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD0, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x38,
		.args = { R8 | M8, CL }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD2, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x38,
		.args = { R8 | M8, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0xC0, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x38,
		.args = { R16 | M16, ONE }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD1, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x38,
		.args = { R16 | M16, CL }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD3, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x38,
		.args = { R16 | M16, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0xC1, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x38,
		.args = { R32 | M32, ONE }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD1, .oplen = 1,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x38,
		.args = { R64 | M64, ONE }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD1, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x38,
		.args = { R32 | M32, CL }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD3, .oplen = 1,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x38,
		.args = { R64 | M64, CL }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD3, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x38,
		.args = { R32 | M32, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0xC1, .oplen = 1,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x38,
		.args = { R64 | M64, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0xC1, .oplen = 1,
	} } },
	{ "shl", 12, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x20,
		.args = { R8 | M8, ONE }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD0, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x20,
		.args = { R8 | M8, CL }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD2, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x20,
		.args = { R8 | M8, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0xC0, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x20,
		.args = { R16 | M16, ONE }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD1, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x20,
		.args = { R16 | M16, CL }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD3, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x20,
		.args = { R16 | M16, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0xC1, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x20,
		.args = { R32 | M32, ONE }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD1, .oplen = 1,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x20,
		.args = { R64 | M64, ONE }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD1, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x20,
		.args = { R32 | M32, CL }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD3, .oplen = 1,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x20,
		.args = { R64 | M64, CL }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD3, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x20,
		.args = { R32 | M32, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0xC1, .oplen = 1,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x20,
		.args = { R64 | M64, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0xC1, .oplen = 1,
	} } },
	{ "shr", 12, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x28,
		.args = { R8 | M8, ONE }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD0, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x28,
		.args = { R8 | M8, CL }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD2, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x28,
		.args = { R8 | M8, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0xC0, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x28,
		.args = { R16 | M16, ONE }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD1, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x28,
		.args = { R16 | M16, CL }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD3, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x28,
		.args = { R16 | M16, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0xC1, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x28,
		.args = { R32 | M32, ONE }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD1, .oplen = 1,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x28,
		.args = { R64 | M64, ONE }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD1, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x28,
		.args = { R32 | M32, CL }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD3, .oplen = 1,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x28,
		.args = { R64 | M64, CL }, .arglen = 2, .mem_oper = 1,
		.opcode = 0xD3, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x28,
		.args = { R32 | M32, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0xC1, .oplen = 1,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x28,
		.args = { R64 | M64, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0xC1, .oplen = 1,
	} } },
	{ "sarx", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x7a, .modrmreq = true, .modrmreg = true,
		.args = { R32, R32 | M32, R32 }, .arglen = 3, .mem_oper = 2, .reg_oper = 1, .vex_oper = 3,
		.opcode = 0xF7, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0xfa, .modrmreq = true, .modrmreg = true,
		.args = { R64, R64 | M64, R64 }, .arglen = 3, .mem_oper = 2, .reg_oper = 1, .vex_oper = 3,
		.opcode = 0xF7, .oplen = 1,
	} } },
	{ "shlx", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { R32, R32 | M32, R32 }, .arglen = 3, .mem_oper = 2, .reg_oper = 1, .vex_oper = 3,
		.opcode = 0xF7, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0xf9, .modrmreq = true, .modrmreg = true,
		.args = { R64, R64 | M64, R64 }, .arglen = 3, .mem_oper = 2, .reg_oper = 1, .vex_oper = 3,
		.opcode = 0xF7, .oplen = 1,
	} } },
	{ "shrx", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x7b, .modrmreq = true, .modrmreg = true,
		.args = { R32, R32 | M32, R32 }, .arglen = 3, .mem_oper = 2, .reg_oper = 1, .vex_oper = 3,
		.opcode = 0xF7, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0xfb, .modrmreq = true, .modrmreg = true,
		.args = { R64, R64 | M64, R64 }, .arglen = 3, .mem_oper = 2, .reg_oper = 1, .vex_oper = 3,
		.opcode = 0xF7, .oplen = 1,
	} } },
	{ "sbb", 21, (struct x64LookupActualIns[]) { {
		.args = { AL, IMM8 }, .arglen = 2, .imm_oper = 2,
		.opcode = 0x1C, .oplen = 1,
	}, {
		.args = { AX, IMM16 }, .arglen = 2, .imm_oper = 2,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x1D, .oplen = 1,
	}, {
		.args = { EAX, IMM32 }, .arglen = 2, .imm_oper = 2,
		.opcode = 0x1D, .oplen = 1,
	}, {
		.rex = 0x48,
		.args = { RAX, IMM32 }, .arglen = 2, .imm_oper = 2,
		.opcode = 0x1D, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x18,
		.args = { R8 | M8, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0x80, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x18,
		.args = { R16 | M16, IMM16 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x81, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x18,
		.args = { R32 | M32, IMM32 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0x81, .oplen = 1,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x18,
		.args = { R64 | M64, IMM32 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0x81, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x18,
		.args = { R16 | M16, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0x83, .oplen = 1,
		.preffered = true,
	}, {
		.modrmreq = true, .modrm = 0x18,
		.args = { R32 | M32, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0x83, .oplen = 1,
		.preffered = true,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x18,
		.args = { R64 | M64, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0x83, .oplen = 1,
		.preffered = true,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R8 | M8, R8 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x18, .oplen = 1,
		.preffered = true,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R8 | M8, RH }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x18, .oplen = 1,
		.preffered = true,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R16 | M16, R16 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x19, .oplen = 1,
		.preffered = true,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32 | M32, R32 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x19, .oplen = 1,
		.preffered = true,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64 | M64, R64 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x19, .oplen = 1,
		.preffered = true,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R8, R8 | M8 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x1A, .oplen = 1,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { RH, R8 | M8 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x1A, .oplen = 1,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R16, R16 | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x1B, .oplen = 1,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, R32 | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x1B, .oplen = 1,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, R64 | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x1B, .oplen = 1,
	} } },
	{ "scas", 4, (struct x64LookupActualIns[]) { {
		.args = { M8 }, .arglen = 1,
		.opcode = 0xAE, .oplen = 1,
	}, {
		.args = { M16 }, .arglen = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xAF, .oplen = 1,
	}, {
		.args = { M32 }, .arglen = 1,
		.opcode = 0xAF, .oplen = 1,
	}, {
		.rex = 0x48,
		.args = { M64 }, .arglen = 1,
		.opcode = 0xAF, .oplen = 1,
	} } },
	{ "scasb", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xAE, .oplen = 1,
	} } },
	{ "scasw", 1, (struct x64LookupActualIns[]) { {
		.prefixes = 0x66, .preflen = 1, .opcode = 0xAF, .oplen = 1,
	} } },
	{ "scasd", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xAF, .oplen = 1,
	} } },
	{ "scasq", 1, (struct x64LookupActualIns[]) { {
		.rex = 0x48,
		.opcode = 0xAF, .oplen = 1,
	} } },
	{ "seta", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x0,
		.args = { R8 | M8 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0x970F, .oplen = 2,
	} } },
	{ "setae", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x0,
		.args = { R8 | M8 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0x930F, .oplen = 2,
	} } },
	{ "setb", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x0,
		.args = { R8 | M8 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0x920F, .oplen = 2,
	} } },
	{ "setbe", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x0,
		.args = { R8 | M8 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0x960F, .oplen = 2,
	} } },
	{ "setc", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x0,
		.args = { R8 | M8 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0x920F, .oplen = 2,
	} } },
	{ "sete", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x0,
		.args = { R8 | M8 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0x940F, .oplen = 2,
	} } },
	{ "setg", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x0,
		.args = { R8 | M8 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0x9F0F, .oplen = 2,
	} } },
	{ "setge", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x0,
		.args = { R8 | M8 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0x9D0F, .oplen = 2,
	} } },
	{ "setl", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x0,
		.args = { R8 | M8 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0x9C0F, .oplen = 2,
	} } },
	{ "setle", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x0,
		.args = { R8 | M8 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0x9E0F, .oplen = 2,
	} } },
	{ "setna", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x0,
		.args = { R8 | M8 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0x960F, .oplen = 2,
	} } },
	{ "setnae", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x0,
		.args = { R8 | M8 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0x920F, .oplen = 2,
	} } },
	{ "setnb", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x0,
		.args = { R8 | M8 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0x930F, .oplen = 2,
	} } },
	{ "setnbe", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x0,
		.args = { R8 | M8 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0x970F, .oplen = 2,
	} } },
	{ "setnc", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x0,
		.args = { R8 | M8 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0x930F, .oplen = 2,
	} } },
	{ "setne", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x0,
		.args = { R8 | M8 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0x950F, .oplen = 2,
	} } },
	{ "setng", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x0,
		.args = { R8 | M8 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0x9E0F, .oplen = 2,
	} } },
	{ "setnge", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x0,
		.args = { R8 | M8 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0x9C0F, .oplen = 2,
	} } },
	{ "setnl", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x0,
		.args = { R8 | M8 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0x9D0F, .oplen = 2,
	} } },
	{ "setnle", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x0,
		.args = { R8 | M8 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0x9F0F, .oplen = 2,
	} } },
	{ "setno", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x0,
		.args = { R8 | M8 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0x910F, .oplen = 2,
	} } },
	{ "setnp", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x0,
		.args = { R8 | M8 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0x9B0F, .oplen = 2,
	} } },
	{ "setns", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x0,
		.args = { R8 | M8 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0x990F, .oplen = 2,
	} } },
	{ "setnz", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x0,
		.args = { R8 | M8 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0x950F, .oplen = 2,
	} } },
	{ "seto", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x0,
		.args = { R8 | M8 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0x900F, .oplen = 2,
	} } },
	{ "setp", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x0,
		.args = { R8 | M8 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0x9A0F, .oplen = 2,
	} } },
	{ "setpe", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x0,
		.args = { R8 | M8 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0x9A0F, .oplen = 2,
	} } },
	{ "setpo", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x0,
		.args = { R8 | M8 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0x9B0F, .oplen = 2,
	} } },
	{ "sets", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x0,
		.args = { R8 | M8 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0x980F, .oplen = 2,
	} } },
	{ "setz", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x0,
		.args = { R8 | M8 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0x940F, .oplen = 2,
	} } },
	{ "sfence", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xF8AE0F, .oplen = 3,
	} } },
	{ "sgdt", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x0,
		.args = { X64_ALLMEMMASK }, .arglen = 1, .mem_oper = 1,
		.opcode = 0x010F, .oplen = 2,
	} } },
	{ "shld", 6, (struct x64LookupActualIns[]) { {
		.args = { R16 | M16, R16, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 1, .reg_oper = 2,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xA40F, .oplen = 2,
	}, {
		.args = { R16 | M16, R16, CL }, .arglen = 3, .mem_oper = 1, .reg_oper = 2,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xA50F, .oplen = 2,
	}, {
		.args = { R32 | M32, R32, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0xA40F, .oplen = 2,
	}, {
		.rex = 0x48,
		.args = { R64 | M64, R64, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0xA40F, .oplen = 2,
	}, {
		.args = { R32 | M32, R32, CL }, .arglen = 3, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0xA50F, .oplen = 2,
	}, {
		.rex = 0x48,
		.args = { R64 | M64, R64, CL }, .arglen = 3, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0xA50F, .oplen = 2,
	} } },
	{ "shrd", 6, (struct x64LookupActualIns[]) { {
		.args = { R16 | M16, R16, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 1, .reg_oper = 2,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xAC0F, .oplen = 2,
	}, {
		.args = { R16 | M16, R16, CL }, .arglen = 3, .mem_oper = 1, .reg_oper = 2,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xAD0F, .oplen = 2,
	}, {
		.args = { R32 | M32, R32, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0xAC0F, .oplen = 2,
	}, {
		.rex = 0x48,
		.args = { R64 | M64, R64, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0xAC0F, .oplen = 2,
	}, {
		.args = { R32 | M32, R32, CL }, .arglen = 3, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0xAD0F, .oplen = 2,
	}, {
		.rex = 0x48,
		.args = { R64 | M64, R64, CL }, .arglen = 3, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0xAD0F, .oplen = 2,
	} } },
	{ "shufpd", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xC60F, .oplen = 2,
	} } },
	{ "vshufpd", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128, IMM8 }, .arglen = 4, .imm_oper = 4, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xC6, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256, IMM8 }, .arglen = 4, .imm_oper = 4, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xC6, .oplen = 1,
	} } },
	{ "shufps", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0xC60F, .oplen = 2,
	} } },
	{ "vshufps", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x78, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128, IMM8 }, .arglen = 4, .imm_oper = 4, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xC6, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7c, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256, IMM8 }, .arglen = 4, .imm_oper = 4, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xC6, .oplen = 1,
	} } },
	{ "sidt", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x8,
		.args = { X64_ALLMEMMASK }, .arglen = 1, .mem_oper = 1,
		.opcode = 0x010F, .oplen = 2,
	} } },
	{ "sldt", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x0,
		.args = { R16 | M16 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0x000F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x0,
		.args = { R64 | M16 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0x000F, .oplen = 2,
	} } },
	{ "smsw", 3, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x20,
		.args = { R16 | M16 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0x010F, .oplen = 2,
	}, {
		.modrmreq = true, .modrm = 0x20,
		.args = { R32 | M16 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0x010F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x20,
		.args = { R64 | M16 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0x010F, .oplen = 2,
	} } },
	{ "sqrtpd", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x510F, .oplen = 2,
	} } },
	{ "vsqrtpd", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x51, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM | M256 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x51, .oplen = 1,
	} } },
	{ "sqrtps", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x510F, .oplen = 2,
	} } },
	{ "vsqrtps", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x78, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x51, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7c, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM | M256 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x51, .oplen = 1,
	} } },
	{ "sqrtsd", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF2, .preflen = 1, .opcode = 0x510F, .oplen = 2,
	} } },
	{ "vsqrtsd", 1, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x7b, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M64 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x51, .oplen = 1,
	} } },
	{ "sqrtss", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0x510F, .oplen = 2,
	} } },
	{ "vsqrtss", 1, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x7a,
		.args = { XMM, XMM, XMM | M32 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x51, .oplen = 1,
	} } },
	{ "stc", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xF9, .oplen = 1,
	} } },
	{ "std", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xFD, .oplen = 1,
	} } },
	{ "sti", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xFB, .oplen = 1,
	} } },
	{ "stmxcsr", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x18,
		.args = { M32 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xAE0F, .oplen = 2,
	} } },
	{ "vstmxcsr", 1, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x78, .modrmreq = true, .modrm = 0x18,
		.args = { M32 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xAE, .oplen = 1,
	} } },
	{ "stos", 4, (struct x64LookupActualIns[]) { {
		.args = { M8 }, .arglen = 1,
		.opcode = 0xAA, .oplen = 1,
	}, {
		.args = { M16 }, .arglen = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xAB, .oplen = 1,
	}, {
		.args = { M32 }, .arglen = 1,
		.opcode = 0xAB, .oplen = 1,
	}, {
		.rex = 0x48,
		.args = { M64 }, .arglen = 1,
		.opcode = 0xAB, .oplen = 1,
	} } },
	{ "stosb", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xAA, .oplen = 1,
	} } },
	{ "stosw", 1, (struct x64LookupActualIns[]) { {
		.prefixes = 0x66, .preflen = 1, .opcode = 0xAB, .oplen = 1,
	} } },
	{ "stosd", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xAB, .oplen = 1,
	} } },
	{ "stosq", 1, (struct x64LookupActualIns[]) { {
		.rex = 0x48,
		.opcode = 0xAB, .oplen = 1,
	} } },
	{ "str", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x8,
		.args = { R16 | M16 }, .arglen = 1,
		.opcode = 0x000F, .oplen = 2,
	} } },
	{ "sub", 21, (struct x64LookupActualIns[]) { {
		.args = { AL, IMM8 }, .arglen = 2, .imm_oper = 2,
		.opcode = 0x2C, .oplen = 1,
	}, {
		.args = { AX, IMM16 }, .arglen = 2, .imm_oper = 2,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x2D, .oplen = 1,
	}, {
		.args = { EAX, IMM32 }, .arglen = 2, .imm_oper = 2,
		.opcode = 0x2D, .oplen = 1,
	}, {
		.rex = 0x48,
		.args = { RAX, IMM32 }, .arglen = 2, .imm_oper = 2,
		.opcode = 0x2D, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x28,
		.args = { R8 | M8, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0x80, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x28,
		.args = { R16 | M16, IMM16 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x81, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x28,
		.args = { R32 | M32, IMM32 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0x81, .oplen = 1,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x28,
		.args = { R64 | M64, IMM32 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0x81, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x28,
		.args = { R16 | M16, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0x83, .oplen = 1,
		.preffered = true,
	}, {
		.modrmreq = true, .modrm = 0x28,
		.args = { R32 | M32, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0x83, .oplen = 1,
		.preffered = true,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x28,
		.args = { R64 | M64, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0x83, .oplen = 1,
		.preffered = true,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R8 | M8, R8 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x28, .oplen = 1,
		.preffered = true,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R8 | M8, RH }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x28, .oplen = 1,
		.preffered = true,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R16 | M16, R16 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x29, .oplen = 1,
		.preffered = true,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32 | M32, R32 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x29, .oplen = 1,
		.preffered = true,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64 | M64, R64 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x29, .oplen = 1,
		.preffered = true,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R8, R8 | M8 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x2A, .oplen = 1,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { RH, R8 | M8 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x2A, .oplen = 1,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R16, R16 | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x2B, .oplen = 1,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, R32 | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x2B, .oplen = 1,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, R64 | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x2B, .oplen = 1,
	} } },
	{ "subpd", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x5C0F, .oplen = 2,
	} } },
	{ "vsubpd", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x5C, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x5C, .oplen = 1,
	} } },
	{ "subps", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x5C0F, .oplen = 2,
	} } },
	{ "vsubps", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x78, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x5C, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7c, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x5C, .oplen = 1,
	} } },
	{ "subsd", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF2, .preflen = 1, .opcode = 0x5C0F, .oplen = 2,
	} } },
	{ "vsubsd", 1, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x7b, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M64 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x5C, .oplen = 1,
	} } },
	{ "subss", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0x5C0F, .oplen = 2,
	} } },
	{ "vsubss", 1, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x7a, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M32 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x5C, .oplen = 1,
	} } },
	{ "swapgs", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xF8010F, .oplen = 3,
	} } },
	{ "syscall", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0x050F, .oplen = 2,
	} } },
	{ "sysenter", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0x340F, .oplen = 2,
	} } },
	{ "sysexit", 2, (struct x64LookupActualIns[]) { {
		.opcode = 0x350F, .oplen = 2,
	}, {
		.rex = 0x48,
		.args = { PREFREX_W }, .arglen = 1,
		.opcode = 0x350F, .oplen = 2,
	} } },
	{ "sysret", 2, (struct x64LookupActualIns[]) { {
		.opcode = 0x070F, .oplen = 2,
	}, {
		.rex = 0x48,
		.args = { PREFREX_W }, .arglen = 1,
		.opcode = 0x070F, .oplen = 2,
	} } },
	{ "test", 13, (struct x64LookupActualIns[]) { {
		.args = { AL, IMM8 }, .arglen = 2, .imm_oper = 2,
		.opcode = 0xA8, .oplen = 1,
	}, {
		.args = { AX, IMM16 }, .arglen = 2, .imm_oper = 2,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xA9, .oplen = 1,
	}, {
		.args = { EAX, IMM32 }, .arglen = 2, .imm_oper = 2,
		.opcode = 0xA9, .oplen = 1,
	}, {
		.rex = 0x48,
		.args = { RAX, IMM32 }, .arglen = 2, .imm_oper = 2,
		.opcode = 0xA9, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x0,
		.args = { R8 | M8, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0xF6, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x0,
		.args = { R16 | M16, IMM16 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xF7, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x0,
		.args = { R32 | M32, IMM32 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0xF7, .oplen = 1,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x0,
		.args = { R64 | M64, IMM32 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0xF7, .oplen = 1,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R8 | M8, R8 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x84, .oplen = 1,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R8 | M8, RH }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x84, .oplen = 1,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R16 | M16, R16 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x85, .oplen = 1,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32 | M32, R32 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x85, .oplen = 1,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64 | M64, R64 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x85, .oplen = 1,
	} } },
	{ "tzcnt", 3, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { R16, R16 | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF366, .preflen = 2, .opcode = 0xBC0F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, R32 | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0xBC0F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, R64 | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0xBC0F, .oplen = 2,
	} } },
	{ "ucomisd", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x2E0F, .oplen = 2,
	} } },
	{ "vucomisd", 1, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x2E, .oplen = 1,
	} } },
	{ "ucomiss", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x2E0F, .oplen = 2,
	} } },
	{ "vucomiss", 1, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x78, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x2E, .oplen = 1,
	} } },
	{ "ud2", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0x0B0F, .oplen = 2,
	} } },
	{ "unpckhpd", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x150F, .oplen = 2,
	} } },
	{ "vunpckhpd", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x15, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x15, .oplen = 1,
	} } },
	{ "unpckhps", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x150F, .oplen = 2,
	} } },
	{ "vunpckhps", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x78, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x15, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7c, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x15, .oplen = 1,
	} } },
	{ "unpcklpd", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x140F, .oplen = 2,
	} } },
	{ "vunpcklpd", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x14, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x14, .oplen = 1,
	} } },
	{ "unpcklps", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x140F, .oplen = 2,
	} } },
	{ "vunpcklps", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x78, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x14, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7c, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x14, .oplen = 1,
	} } },
	{ "vbroadcastss", 4, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x18, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x18, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x18, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, XMM }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x18, .oplen = 1,
	} } },
	{ "vbroadcastsd", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x19, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, XMM }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x19, .oplen = 1,
	} } },
	{ "vbroadcastf128", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x1A, .oplen = 1,
	} } },
	{ "vcvtph2ps", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x13, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x13, .oplen = 1,
	} } },
	{ "vcvtps2ph", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 3, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { XMM | M128, YMM, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x1D, .oplen = 1,
	}, {
		.vex = 0x80 | 3, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM | M64, XMM, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x1D, .oplen = 1,
	} } },
	{ "verr", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x20,
		.args = { R16 | M16 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0x000F, .oplen = 2,
	} } },
	{ "verw", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x28,
		.args = { R16 | M16 }, .arglen = 1, .mem_oper = 1,
		.opcode = 0x000F, .oplen = 2,
	} } },
	{ "vextractf128", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 3, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { XMM | M128, YMM, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x19, .oplen = 1,
	} } },
	{ "vextracti128", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 3, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { XMM | M128, YMM, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x39, .oplen = 1,
	} } },
	{ "vfmadd132pd", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0xf9, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x98, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0xfd, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x98, .oplen = 1,
	} } },
	{ "vfmadd213pd", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0xf9, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xA8, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0xfd, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xA8, .oplen = 1,
	} } },
	{ "vfmadd231pd", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0xf9, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xB8, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0xfd, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xB8, .oplen = 1,
	} } },
	{ "vfmadd132ps", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x98, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x98, .oplen = 1,
	} } },
	{ "vfmadd213ps", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xA8, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xA8, .oplen = 1,
	} } },
	{ "vfmadd231ps", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xB8, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xB8, .oplen = 1,
	} } },
	{ "vfmadd132sd", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0xf9, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M64 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x99, .oplen = 1,
	} } },
	{ "vfmadd213sd", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0xf9, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M64 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xA9, .oplen = 1,
	} } },
	{ "vfmadd231sd", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0xf9, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M64 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xB9, .oplen = 1,
	} } },
	{ "vfmadd132ss", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M32 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x99, .oplen = 1,
	} } },
	{ "vfmadd213ss", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M32 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xA9, .oplen = 1,
	} } },
	{ "vfmadd231ss", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M32 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xB9, .oplen = 1,
	} } },
	{ "vfmaddsub132pd", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0xf9, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x96, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0xfd, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x96, .oplen = 1,
	} } },
	{ "vfmaddsub213pd", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0xf9, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xA6, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0xfd, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xA6, .oplen = 1,
	} } },
	{ "vfmaddsub231pd", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0xf9, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xB6, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0xfd, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xB6, .oplen = 1,
	} } },
	{ "vfmaddsub132ps", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x96, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x96, .oplen = 1,
	} } },
	{ "vfmaddsub213ps", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xA6, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xA6, .oplen = 1,
	} } },
	{ "vfmaddsub231ps", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xB6, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xB6, .oplen = 1,
	} } },
	{ "vfmsubadd132pd", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0xf9, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x97, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0xfd, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x97, .oplen = 1,
	} } },
	{ "vfmsubadd213pd", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0xf9, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xA7, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0xfd, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xA7, .oplen = 1,
	} } },
	{ "vfmsubadd231pd", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0xf9, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xB7, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0xfd, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xB7, .oplen = 1,
	} } },
	{ "vfmsubadd132ps", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x97, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x97, .oplen = 1,
	} } },
	{ "vfmsubadd213ps", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xA7, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xA7, .oplen = 1,
	} } },
	{ "vfmsubadd231ps", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xB7, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xB7, .oplen = 1,
	} } },
	{ "vfmsub132pd", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0xf9, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x9A, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0xfd, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x9A, .oplen = 1,
	} } },
	{ "vfmsub213pd", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0xf9, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xAA, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0xfd, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xAA, .oplen = 1,
	} } },
	{ "vfmsub231pd", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0xf9, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xBA, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0xfd, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xBA, .oplen = 1,
	} } },
	{ "vfmsub132ps", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x9A, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x9A, .oplen = 1,
	} } },
	{ "vfmsub213ps", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xAA, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xAA, .oplen = 1,
	} } },
	{ "vfmsub231ps", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xBA, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xBA, .oplen = 1,
	} } },
	{ "vfmsub132sd", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0xf9, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M64 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x9B, .oplen = 1,
	} } },
	{ "vfmsub213sd", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0xf9, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M64 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xAB, .oplen = 1,
	} } },
	{ "vfmsub231sd", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0xf9, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M64 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xBB, .oplen = 1,
	} } },
	{ "vfmsub132ss", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M32 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x9B, .oplen = 1,
	} } },
	{ "vfmsub213ss", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M32 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xAB, .oplen = 1,
	} } },
	{ "vfmsub231ss", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M32 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xBB, .oplen = 1,
	} } },
	{ "vfnmadd132pd", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0xf9, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x9C, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0xfd, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x9C, .oplen = 1,
	} } },
	{ "vfnmadd213pd", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0xf9, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xAC, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0xfd, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xAC, .oplen = 1,
	} } },
	{ "vfnmadd231pd", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0xf9, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xBC, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0xfd, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xBC, .oplen = 1,
	} } },
	{ "vfnmadd132ps", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x9C, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x9C, .oplen = 1,
	} } },
	{ "vfnmadd213ps", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xAC, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xAC, .oplen = 1,
	} } },
	{ "vfnmadd231ps", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xBC, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xBC, .oplen = 1,
	} } },
	{ "vfnmadd132sd", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0xf9, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M64 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x9D, .oplen = 1,
	} } },
	{ "vfnmadd213sd", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0xf9, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M64 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xAD, .oplen = 1,
	} } },
	{ "vfnmadd231sd", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0xf9, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M64 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xBD, .oplen = 1,
	} } },
	{ "vfnmadd132ss", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M32 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x9D, .oplen = 1,
	} } },
	{ "vfnmadd213ss", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M32 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xAD, .oplen = 1,
	} } },
	{ "vfnmadd231ss", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M32 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xBD, .oplen = 1,
	} } },
	{ "vfnmsub132pd", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0xf9, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x9E, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0xfd, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x9E, .oplen = 1,
	} } },
	{ "vfnmsub213pd", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0xf9, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xAE, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0xfd, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xAE, .oplen = 1,
	} } },
	{ "vfnmsub231pd", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0xf9, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xBE, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0xfd, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xBE, .oplen = 1,
	} } },
	{ "vfnmsub132ps", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x9E, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x9E, .oplen = 1,
	} } },
	{ "vfnmsub213ps", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xAE, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xAE, .oplen = 1,
	} } },
	{ "vfnmsub231ps", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xBE, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xBE, .oplen = 1,
	} } },
	{ "vfnmsub132sd", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0xf9, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M64 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x9F, .oplen = 1,
	} } },
	{ "vfnmsub213sd", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0xf9, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M64 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xAF, .oplen = 1,
	} } },
	{ "vfnmsub231sd", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0xf9, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M64 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xBF, .oplen = 1,
	} } },
	{ "vfnmsub132ss", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M32 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x9F, .oplen = 1,
	} } },
	{ "vfnmsub213ss", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M32 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xAF, .oplen = 1,
	} } },
	{ "vfnmsub231ss", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M32 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0xBF, .oplen = 1,
	} } },
	{ "vgatherdpd", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0xf9, .modrmreq = true, .modrmreg = true,
		.args = { XMM, M32, XMM }, .arglen = 3, .mem_oper = 2, .reg_oper = 1, .vex_oper = 3,
		.opcode = 0x92, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0xfd, .modrmreq = true, .modrmreg = true,
		.args = { YMM, M32, YMM }, .arglen = 3, .mem_oper = 2, .reg_oper = 1, .vex_oper = 3,
		.opcode = 0x92, .oplen = 1,
	} } },
	{ "vgatherqpd", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0xf9, .modrmreq = true, .modrmreg = true,
		.args = { XMM, M64, XMM }, .arglen = 3, .mem_oper = 2, .reg_oper = 1, .vex_oper = 3,
		.opcode = 0x93, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0xfd, .modrmreq = true, .modrmreg = true,
		.args = { YMM, M64, YMM }, .arglen = 3, .mem_oper = 2, .reg_oper = 1, .vex_oper = 3,
		.opcode = 0x93, .oplen = 1,
	} } },
	{ "vgatherdps", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, M32, XMM }, .arglen = 3, .mem_oper = 2, .reg_oper = 1, .vex_oper = 3,
		.opcode = 0x92, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, M32, YMM }, .arglen = 3, .mem_oper = 2, .reg_oper = 1, .vex_oper = 3,
		.opcode = 0x92, .oplen = 1,
	} } },
	{ "vgatherqps", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, M64, XMM }, .arglen = 3, .mem_oper = 2, .reg_oper = 1, .vex_oper = 3,
		.opcode = 0x93, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { XMM, M64, XMM }, .arglen = 3, .mem_oper = 2, .reg_oper = 1, .vex_oper = 3,
		.opcode = 0x93, .oplen = 1,
	} } },
	{ "vpgatherdd", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, M32, XMM }, .arglen = 3, .mem_oper = 2, .reg_oper = 1, .vex_oper = 3,
		.opcode = 0x90, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, M32, YMM }, .arglen = 3, .mem_oper = 2, .reg_oper = 1, .vex_oper = 3,
		.opcode = 0x90, .oplen = 1,
	} } },
	{ "vpgatherqd", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, M64, XMM }, .arglen = 3, .mem_oper = 2, .reg_oper = 1, .vex_oper = 3,
		.opcode = 0x91, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { XMM, M64, XMM }, .arglen = 3, .mem_oper = 2, .reg_oper = 1, .vex_oper = 3,
		.opcode = 0x91, .oplen = 1,
	} } },
	{ "vpgatherdq", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0xf9, .modrmreq = true, .modrmreg = true,
		.args = { XMM, M32, XMM }, .arglen = 3, .mem_oper = 2, .reg_oper = 1, .vex_oper = 3,
		.opcode = 0x90, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0xfd, .modrmreq = true, .modrmreg = true,
		.args = { YMM, M32, YMM }, .arglen = 3, .mem_oper = 2, .reg_oper = 1, .vex_oper = 3,
		.opcode = 0x90, .oplen = 1,
	} } },
	{ "vpgatherqq", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0xf9, .modrmreq = true, .modrmreg = true,
		.args = { XMM, M64, XMM }, .arglen = 3, .mem_oper = 2, .reg_oper = 1, .vex_oper = 3,
		.opcode = 0x91, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0xfd, .modrmreq = true, .modrmreg = true,
		.args = { YMM, M64, YMM }, .arglen = 3, .mem_oper = 2, .reg_oper = 1, .vex_oper = 3,
		.opcode = 0x91, .oplen = 1,
	} } },
	{ "vinsertf128", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 3, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, XMM | M128, IMM8 }, .arglen = 4, .imm_oper = 4, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x18, .oplen = 1,
	} } },
	{ "vinserti128", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 3, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, XMM | M128, IMM8 }, .arglen = 4, .imm_oper = 4, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x38, .oplen = 1,
	} } },
	{ "vmaskmovps", 4, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x2C, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x2C, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { M128, XMM, XMM }, .arglen = 3, .mem_oper = 1, .reg_oper = 3, .vex_oper = 2,
		.opcode = 0x2E, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { M256, YMM, YMM }, .arglen = 3, .mem_oper = 1, .reg_oper = 3, .vex_oper = 2,
		.opcode = 0x2E, .oplen = 1,
	} } },
	{ "vmaskmovpd", 4, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x2D, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x2D, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { M128, XMM, XMM }, .arglen = 3, .mem_oper = 1, .reg_oper = 3, .vex_oper = 2,
		.opcode = 0x2F, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { M256, YMM, YMM }, .arglen = 3, .mem_oper = 1, .reg_oper = 3, .vex_oper = 2,
		.opcode = 0x2F, .oplen = 1,
	} } },
	{ "vpblendd", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 3, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128, IMM8 }, .arglen = 4, .imm_oper = 4, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x02, .oplen = 1,
	}, {
		.vex = 0x80 | 3, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256, IMM8 }, .arglen = 4, .imm_oper = 4, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x02, .oplen = 1,
	} } },
	{ "vpbroadcastb", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M8 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x78, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, XMM | M8 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x78, .oplen = 1,
	} } },
	{ "vpbroadcastw", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x79, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, XMM | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x79, .oplen = 1,
	} } },
	{ "vpbroadcastd", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x58, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, XMM | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x58, .oplen = 1,
	} } },
	{ "vpbroadcastq", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x59, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, XMM | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x59, .oplen = 1,
	} } },
	{ "vbroadcasti128", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x5A, .oplen = 1,
	} } },
	{ "vpermd", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x36, .oplen = 1,
	} } },
	{ "vpermpd", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 3, .vex_byte = 0xfd, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM | M256, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x01, .oplen = 1,
	} } },
	{ "vpermps", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x16, .oplen = 1,
	} } },
	{ "vpermq", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 3, .vex_byte = 0xfd, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM | M256, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x00, .oplen = 1,
	} } },
	{ "vperm2i128", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 3, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256, IMM8 }, .arglen = 4, .imm_oper = 4, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x46, .oplen = 1,
	} } },
	{ "vpermilpd", 4, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x0D, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x0D, .oplen = 1,
	}, {
		.vex = 0x80 | 3, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x05, .oplen = 1,
	}, {
		.vex = 0x80 | 3, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM | M256, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x05, .oplen = 1,
	} } },
	{ "vpermilps", 4, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x0C, .oplen = 1,
	}, {
		.vex = 0x80 | 3, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x04, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x0C, .oplen = 1,
	}, {
		.vex = 0x80 | 3, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM | M256, IMM8 }, .arglen = 3, .imm_oper = 3, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x04, .oplen = 1,
	} } },
	{ "vperm2f128", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 3, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256, IMM8 }, .arglen = 4, .imm_oper = 4, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x06, .oplen = 1,
	} } },
	{ "vpmaskmovd", 4, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x8C, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x8C, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { M128, XMM, XMM }, .arglen = 3, .mem_oper = 1, .reg_oper = 3, .vex_oper = 2,
		.opcode = 0x8E, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { M256, YMM, YMM }, .arglen = 3, .mem_oper = 1, .reg_oper = 3, .vex_oper = 2,
		.opcode = 0x8E, .oplen = 1,
	} } },
	{ "vpmaskmovq", 4, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0xf9, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x8C, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0xfd, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x8C, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0xf9, .modrmreq = true, .modrmreg = true,
		.args = { M128, XMM, XMM }, .arglen = 3, .mem_oper = 1, .reg_oper = 3, .vex_oper = 2,
		.opcode = 0x8E, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0xfd, .modrmreq = true, .modrmreg = true,
		.args = { M256, YMM, YMM }, .arglen = 3, .mem_oper = 1, .reg_oper = 3, .vex_oper = 2,
		.opcode = 0x8E, .oplen = 1,
	} } },
	{ "vpsllvd", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x47, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x47, .oplen = 1,
	} } },
	{ "vpsllvq", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0xf9, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x47, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0xfd, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x47, .oplen = 1,
	} } },
	{ "vpsravd", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x46, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x46, .oplen = 1,
	} } },
	{ "vpsrlvd", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x45, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x45, .oplen = 1,
	} } },
	{ "vpsrlvq", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0xf9, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x45, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0xfd, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x45, .oplen = 1,
	} } },
	{ "vtestps", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x0E, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM | M256 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x0E, .oplen = 1,
	} } },
	{ "vtestpd", 2, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 2, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x0F, .oplen = 1,
	}, {
		.vex = 0x80 | 2, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM | M256 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x0F, .oplen = 1,
	} } },
	{ "vzeroall", 1, (struct x64LookupActualIns[]) { {
		.vex = 0x80 | 1, .vex_byte = 0x7c,
		.opcode = 0x77, .oplen = 1,
	} } },
	{ "vzeroupper", 1, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x78,
		.opcode = 0x77, .oplen = 1,
	} } },
	{ "wait", 1, (struct x64LookupActualIns[]) { {
		.prefixes = 0x9B, .preflen = 1, .opcode = 0x0, .oplen = 0,
	} } },
	{ "fwait", 1, (struct x64LookupActualIns[]) { {
		.prefixes = 0x9B, .preflen = 1, .opcode = 0x0, .oplen = 0,
	} } },
	{ "wbinvd", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0x090F, .oplen = 2,
	} } },
	{ "wrfsbase", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x10,
		.args = { R32 }, .arglen = 1, .mem_oper = 1,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0xAE0F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x10,
		.args = { R64 }, .arglen = 1, .mem_oper = 1,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0xAE0F, .oplen = 2,
	} } },
	{ "wrgsbase", 2, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x18,
		.args = { R32 }, .arglen = 1, .mem_oper = 1,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0xAE0F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x18,
		.args = { R64 }, .arglen = 1, .mem_oper = 1,
		.prefixes = 0xF3, .preflen = 1, .opcode = 0xAE0F, .oplen = 2,
	} } },
	{ "wrmsr", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0x300F, .oplen = 2,
	} } },
	{ "xacquire", 1, (struct x64LookupActualIns[]) { {
		.prefixes = 0xF2, .preflen = 1, .opcode = 0x0, .oplen = 0,
	} } },
	{ "xrelease", 1, (struct x64LookupActualIns[]) { {
		.prefixes = 0xF3, .preflen = 1, .opcode = 0x0, .oplen = 0,
	} } },
	{ "xabort", 1, (struct x64LookupActualIns[]) { {
		.args = { IMM8 }, .arglen = 1, .imm_oper = 1,
		.opcode = 0xF8C6, .oplen = 2,
	} } },
	{ "xadd", 5, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { R8 | M8, R8 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0xC00F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R8 | M8, RH }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0xC00F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R16 | M16, R16 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.prefixes = 0x66, .preflen = 1, .opcode = 0xC10F, .oplen = 2,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32 | M32, R32 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0xC10F, .oplen = 2,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64 | M64, R64 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0xC10F, .oplen = 2,
	} } },
	{ "xbegin", 1, (struct x64LookupActualIns[]) { {
		.args = { REL32 }, .arglen = 1,
		.opcode = 0xF8C7, .oplen = 2,
	} } },
	{ "xchg", 16, (struct x64LookupActualIns[]) { {
		.args = { AX, R16 }, .arglen = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x90, .oplen = 1,
	}, {
		.args = { R16, AX }, .arglen = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x90, .oplen = 1,
	}, {
		.args = { EAX, R32 }, .arglen = 2, .reg_oper = 1,
		.opcode = 0x90, .oplen = 1,
	}, {
		.rex = 0x48,
		.args = { RAX, R64 }, .arglen = 2, .reg_oper = 1,
		.opcode = 0x90, .oplen = 1,
	}, {
		.args = { R32, EAX }, .arglen = 2, .reg_oper = 1,
		.opcode = 0x90, .oplen = 1,
	}, {
		.rex = 0x48,
		.args = { R64, RAX }, .arglen = 2, .reg_oper = 1,
		.opcode = 0x90, .oplen = 1,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R8 | M8, R8 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x86, .oplen = 1,
		.preffered = true,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R8 | M8, RH }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x86, .oplen = 1,
		.preffered = true,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R8, R8 | M8 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x86, .oplen = 1,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { RH, R8 | M8 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x86, .oplen = 1,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R16 | M16, R16 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x87, .oplen = 1,
		.preffered = true,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R16, R16 | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x87, .oplen = 1,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32 | M32, R32 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x87, .oplen = 1,
		.preffered = true,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64 | M64, R64 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x87, .oplen = 1,
		.preffered = true,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, R32 | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x87, .oplen = 1,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, R64 | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x87, .oplen = 1,
	} } },
	{ "xend", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xD5010F, .oplen = 3,
	} } },
	{ "xgetbv", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xD0010F, .oplen = 3,
	} } },
	{ "xlat", 1, (struct x64LookupActualIns[]) { {
		.args = { M8 }, .arglen = 1,
		.opcode = 0xD7, .oplen = 1,
	} } },
	{ "xlatb", 1, (struct x64LookupActualIns[]) { {
		.rex = 0x48,
		.opcode = 0xD7, .oplen = 1,
	} } },
	{ "xor", 21, (struct x64LookupActualIns[]) { {
		.args = { AL, IMM8 }, .arglen = 2, .imm_oper = 2,
		.opcode = 0x34, .oplen = 1,
	}, {
		.args = { AX, IMM16 }, .arglen = 2, .imm_oper = 2,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x35, .oplen = 1,
	}, {
		.args = { EAX, IMM32 }, .arglen = 2, .imm_oper = 2,
		.opcode = 0x35, .oplen = 1,
	}, {
		.rex = 0x48,
		.args = { RAX, IMM32 }, .arglen = 2, .imm_oper = 2,
		.opcode = 0x35, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x30,
		.args = { R8 | M8, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0x80, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x30,
		.args = { R16 | M16, IMM16 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x81, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x30,
		.args = { R32 | M32, IMM32 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0x81, .oplen = 1,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x30,
		.args = { R64 | M64, IMM32 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0x81, .oplen = 1,
	}, {
		.modrmreq = true, .modrm = 0x30,
		.args = { R16 | M16, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0x83, .oplen = 1,
		.preffered = true,
	}, {
		.modrmreq = true, .modrm = 0x30,
		.args = { R32 | M32, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0x83, .oplen = 1,
		.preffered = true,
	}, {
		.rex = 0x48, .modrmreq = true, .modrm = 0x30,
		.args = { R64 | M64, IMM8 }, .arglen = 2, .imm_oper = 2, .mem_oper = 1,
		.opcode = 0x83, .oplen = 1,
		.preffered = true,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R8 | M8, R8 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x30, .oplen = 1,
		.preffered = true,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R8 | M8, RH }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x30, .oplen = 1,
		.preffered = true,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R16 | M16, R16 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x31, .oplen = 1,
		.preffered = true,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32 | M32, R32 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x31, .oplen = 1,
		.preffered = true,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64 | M64, R64 }, .arglen = 2, .mem_oper = 1, .reg_oper = 2,
		.opcode = 0x31, .oplen = 1,
		.preffered = true,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R8, R8 | M8 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x32, .oplen = 1,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { RH, R8 | M8 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x32, .oplen = 1,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R16, R16 | M16 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x33, .oplen = 1,
	}, {
		.modrmreq = true, .modrmreg = true,
		.args = { R32, R32 | M32 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x33, .oplen = 1,
	}, {
		.rex = 0x48, .modrmreq = true, .modrmreg = true,
		.args = { R64, R64 | M64 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x33, .oplen = 1,
	} } },
	{ "xorpd", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.prefixes = 0x66, .preflen = 1, .opcode = 0x570F, .oplen = 2,
	} } },
	{ "vxorpd", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x79, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x57, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7d, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x57, .oplen = 1,
	} } },
	{ "xorps", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM | M128 }, .arglen = 2, .mem_oper = 2, .reg_oper = 1,
		.opcode = 0x570F, .oplen = 2,
	} } },
	{ "vxorps", 2, (struct x64LookupActualIns[]) { {
		.vex = 1, .vex_byte = 0x78, .modrmreq = true, .modrmreg = true,
		.args = { XMM, XMM, XMM | M128 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x57, .oplen = 1,
	}, {
		.vex = 0x80 | 1, .vex_byte = 0x7c, .modrmreq = true, .modrmreg = true,
		.args = { YMM, YMM, YMM | M256 }, .arglen = 3, .mem_oper = 3, .reg_oper = 1, .vex_oper = 2,
		.opcode = 0x57, .oplen = 1,
	} } },
	{ "xrstor", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x28,
		.args = { X64_ALLMEMMASK }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xAE0F, .oplen = 2,
	} } },
	{ "xrstor64", 1, (struct x64LookupActualIns[]) { {
		.rex = 0x48, .modrmreq = true, .modrm = 0x28,
		.args = { X64_ALLMEMMASK }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xAE0F, .oplen = 2,
	} } },
	{ "xsave", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x20,
		.args = { X64_ALLMEMMASK }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xAE0F, .oplen = 2,
	} } },
	{ "xsave64", 1, (struct x64LookupActualIns[]) { {
		.rex = 0x48, .modrmreq = true, .modrm = 0x20,
		.args = { X64_ALLMEMMASK }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xAE0F, .oplen = 2,
	} } },
	{ "xsaveopt", 1, (struct x64LookupActualIns[]) { {
		.modrmreq = true, .modrm = 0x30,
		.args = { X64_ALLMEMMASK }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xAE0F, .oplen = 2,
	} } },
	{ "xsaveopt64", 1, (struct x64LookupActualIns[]) { {
		.rex = 0x48, .modrmreq = true, .modrm = 0x30,
		.args = { X64_ALLMEMMASK }, .arglen = 1, .mem_oper = 1,
		.opcode = 0xAE0F, .oplen = 2,
	} } },
	{ "xsetbv", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xD1010F, .oplen = 3,
	} } },
	{ "xtest", 1, (struct x64LookupActualIns[]) { {
		.opcode = 0xD6010F, .oplen = 3,
	} } }
};


/**
 * TODO: Error messages for using a 32 bit register to address on a mem64 operand.
 * TODO: 32 Bit support?
 */

// https://github.com/StanfordPL/x64asm

/**
 * Cool resources:
 * - https://stackoverflow.com/questions/31853189/x86-64-assembly-why-displacement-not-64-bits
 * - https://nixhacker.com/segmentation-in-intel-64-bit/
 * - https://stackoverflow.com/questions/58182616/assembler-use-of-segment-register
 * - https://stackoverflow.com/questions/41573502/why-doesnt-gcc-use-partial-registers
 * - https://stackoverflow.com/questions/52522544/rbp-not-allowed-as-sib-base
 * - https://blog.yossarian.net/2020/11/30/How-many-registers-does-an-x86-64-cpu-have
 * - https://stackoverflow.com/a/54895495/10013227   # x86 instruction prefix decoding
 * - https://www.cs.virginia.edu/~evans/cs216/guides/x86.html # instruction guide, handy
 * - https://www.cs.put.poznan.pl/tzok/public/cawllp-04-asm.html#:~:text=default%20rel%20instructs%20to%20use,16%2D%20or%2032%2Dbit%20number # Various examples
 * - https://www.felixcloutier.com/x86/ # x86 instruction set reference
 * - https://learn.microsoft.com/en-us/cpp/cpp/argument-passing-and-naming-conventions?view=msvc-170
 * - https://wiki.osdev.org/Calling_Conventions
 * - https://chromium.googlesource.com/v8/v8.git/+/refs/heads/main/src/codegen/x64/assembler-x64.h # V8's x64 assembler
 * - https://zhu45.org/posts/2017/Jul/30/understanding-how-function-call-works/#google_vignette
 * - https://www.computerenhance.com/p/how-does-queryperformancecounter
 * - https://github.com/llvm/llvm-project/blob/main/llvm/lib/Target/X86/Disassembler/X86Disassembler.cpp#L335 # LLVM's x86 disassembler
 *
 * - What each operand name means (Note: Reg = DST, Mem = Src in op/en):
 *   - https://stackoverflow.com/questions/15017659/how-to-read-the-intel-opcode-notation/41616657#41616657:~:text=%E2%80%A2-,m16%2632%2C%20m16%2616%2C%20m32%2632%2C%20m16%2664%20%E2%80%94%20A%20memory,load%20the%20base%20field%20of%20the%20corresponding%20GDTR%20and%20IDTR%20registers.,-%E2%80%A2%20moffs8%2C%20moffs16%2C%20moffs32
 *
 * - https://gist.github.com/mikesmullin/6259449 # Mike's x86 assembly notes
 *   - https://gist.github.com/mikesmullin/6259449#appendix-reverse-engineering--malware-analysis
 *   - https://gist.github.com/mikesmullin/6259449#appendix-writing-a-compiler
 *
 * - Register Allocator docs: https://cfallin.org/blog/2021/03/15/cranelift-isel-3/
 */

// https://l-m.dev/cs/jitcalc/#:~:text=make%20it%20executable%3F-,C%20Territory,-V%20does%20not

static u32 encode(const x64Ins* ins, x64LookupActualIns* res, u8* opcode_dest);

#define ismem(x) (x & X64_ALLMEMMASK)
#define membase(x)  ((x >> 32) & 0x1f)
#define memindex(x) ((x >> 40) & 0x1f)
#define memscale(x) ((x >> 48) & 0x3)

_Thread_local struct AssemblyError {
  bool error;
  char buf[100];
  enum AssemblyErrorType {
    ASMERR_INVALID_INS,
    ASMERR_INVALID_REG_TYPE,
    ASMERR_INS_ARGUMENT_MISMATCH,
    ASMERR_ESPRSP_USED_AS_INDEX,
    ASMERR_REL_OUT_OF_RANGE,
    ASMERR_LABEL_NOT_FOUND
    // More to come
  } error_type;
} cur_error;

static inline u32 error(enum AssemblyErrorType type, char* fmt, ...) {
  cur_error.error = true;
  cur_error.error_type = type;
  va_list args;
  va_start(args, fmt);
  vsnprintf(cur_error.buf, 100, fmt, args);
  va_end(args);
  return 0;
}

char* x64error(int* errcode) {
  if(cur_error.error) {
    cur_error.error = false;
    if(errcode) *errcode = cur_error.error_type;
    return cur_error.buf;
  }
  return NULL;
}

static inline x64LookupActualIns* identify(const x64Ins* ins) {
  if (ins->op > sizeof(x64Table) / sizeof(x64LookupGeneralIns) || ins->op < 1) {
    error(ASMERR_INVALID_INS, "Invalid instruction: %d.", ins->op);
    return NULL;
  }

  const x64LookupGeneralIns* unresins = x64Table + (ins->op - 1);
  x64LookupActualIns* resolved = NULL;
  bool preferred = false;

  u64 insoperands[] = { ins->params[0].type, ins->params[1].type, ins->params[2].type, ins->params[3].type };

  u32 operandnum = 0;
  while(insoperands[operandnum] && operandnum < 4)
    operandnum ++;

  // Specifies bigger, more specific sizes for MOV based on the immediate value. Basically, picks the right instruction when there's an ambiguous immediate value.
  if(ins->op == MOV) {
    u8 immplace = 0;
    if(insoperands[1] & (IMM8 | IMM16 | IMM32 | IMM64)) immplace = 2;
    else immplace = 1;
    
    // If there's no ambiguity in the immediate value, and it does not go into the RAX-R15 registers, there's no point
    if ((insoperands[immplace - 1] & IMM64) == 0 || (insoperands[immplace - 1] & IMM32) == 0 ||
        (insoperands[2 - immplace] & R64) == 0) goto next;
    
    if(ins->params[immplace - 1].value > 0x100000000) insoperands[immplace - 1] = IMM64;
    else if(ins->params[immplace - 1].value > 0x10000) insoperands[immplace - 1] = IMM32 | IMM8 | IMM16;
  }
  // Adds more specificity to the ambiguous rel() macro's REL32 | REL8
  else if(insoperands[0] & REL8) {
    // 8 * 15 = 120, which is the maximum value for a REL8. This is suboptimal but fast enough and simple for now.
    if((i32) ins->params[0].value > 8 || (i32) ins->params[0].value < -8) insoperands[0] = REL32;
    else insoperands[0] = REL8;
  }
next:
  
  // ---------------------- Instruction resolution and Validation ---------------------- //

  for(u32 i = 0; i < unresins->numactualins; i ++) {
    if(unresins->ins[i].arglen != operandnum) continue;
    x64LookupActualIns* currentins = unresins->ins + i;

    // bool morespecific = false;
    for(u32 j = 0; j < operandnum; j ++) {
      if(!(currentins->args[j] & insoperands[j])) goto cont;
      // else if(resolved && (
          // resolved->args[j] < (currentins->args[j] & insoperands[j]) || // Generally, more specific arguments have a higher set bit than less specific ones.
          // (resolved->modrmreq && !currentins->modrmreq))) morespecific = true;
    }
    
    if(preferred && !currentins->preffered) continue;
    else if(currentins->preffered) preferred = true; // this order is necessary, since the first if validates there's no preference, and the last if only works if there's no preference
    else if(resolved) continue;
    resolved = currentins;
cont:
    continue;
  }

  if(!resolved) {
    error(ASMERR_INS_ARGUMENT_MISMATCH, "Argument mismatch for %s.", x64stringify(ins, 1));
    return NULL;
  }

  return resolved;
}

// static inline u32 ins_size(x64LookupActualIns* res, x64Ins* ins) {
//   bool rex = res->rex || (res->reg_oper && ins->params[res->reg_oper - 1].value & 0x8);
//   int modrm = res->modrmreq;
//   if(modrm) {
//     x64Operand* rm = ins->params + res->mem_oper - 1;
    
//     if(ismem(rm->type)) {
//       if(rm->value & 0x2000000000000000) modrm = 5;
//       else if(membase(rm->value) & 0x10) modrm = 6; // THIS IS WRONG, IT CAN HAVE SIZE 2 BASED ON HAVING A DISPLACEMENT OR NOT
//       else {
//         if((membase(rm->value) | memindex(rm->value)) & 0x8) rex = true;
//         if (rm->value) {
//           modrm += ((i32) rm->value > 127 || (i32) rm->value < -128) * 3 + 1;
//           if(memindex(rm->value) != 0x10) modrm += 1;
//         }
//       }
//     }
//   }
//   return rex + modrm + res->base_size;
// }

// Instructions to keep note of: MOVS mem, mem on line 1203, page 844 in the manual.
static u32 encode(const x64Ins* ins, x64LookupActualIns* res, u8* opcode_dest) {
  if(!res) return 0;

  // ------------------------------ Special Instructions ------------------------------ //

  // Enter is the weirdest instruction ever :'(
  if(ins->op == ENTER) {
    *(u32*) opcode_dest = (u32) 0xC8 | (u32) ((u16) ins->params[0].value) << 8 | (u32) ((u8) ins->params[1].value) << 24;
    return 4;
  }

  // ----------------------------- Instruction encoding ----------------------------- //

  u8 *const opcode_dest_start = opcode_dest;

  if(res->vex) {
    u8 vex_map = res->vex & 0xf;
    
    u8 vex_byte = res->vex_byte;
    if(res->vex_oper) vex_byte &= ~(ins->params[res->vex_oper - 1].value << 3);
    
    u8 vex_r = 0x80, vex_x = 0x40, vex_b = 0x20;
    if(res->reg_oper && ins->params[res->reg_oper - 1].value & 0x8) res->modrmreg ? (vex_r = 0) : (vex_b = 0);
    if(res->mem_oper) {
      if(ismem(ins->params[res->mem_oper - 1].type)) {
        if(membase(ins->params[res->mem_oper - 1].value) & 0x8) vex_b = 0;
        if(memindex(ins->params[res->mem_oper - 1].value) & 0x8) vex_x = 0;
      } else if(ins->params[res->mem_oper - 1].value & 0x8) vex_b = 0;
    }
    
    // VEX 3 byte form
    //     7                           0       7                           0
    // +---+---+---+---+---+---+---+---+   +---+---+---+---+---+---+---+---+
    // |~R |~X |~B | map_select        |   |W/E|    ~vvvv      | L |   pp  |
    // +---+---+---+---+---+---+---+---+   +---+---+---+---+---+---+---+---+
    if((res->vex >> 4) || !vex_x || !vex_b) {
      *(u32*) opcode_dest = (u32) 0xC4 | ((vex_map | vex_r | vex_x | vex_b) << 8) | (vex_byte << 16);
      opcode_dest += 3;
    }
    // VEX 2 byte form
    //     7                           0
    // +---+---+---+---+---+---+---+---+
    // |~R |     ~vvvv     | L |   pp  |
    // +---+---+---+---+---+---+---+---+
    else {
      *(u16*) opcode_dest = (u16) 0xC5 | ((vex_byte | vex_r) << 8);
      opcode_dest += 2;
    }
  }
  
  if(res->mem_oper) {
    
    // Segment Register for memory operands - Prefix group 2 (GCC Ordering)
    if(ins->params[res->mem_oper - 1].type & ((u64) 0x7 << 56))
      *opcode_dest = ((u8[]) { 0x26, 0x2e, 0x36, 0x3e, 0x64, 0x65 })[((ins->params[res->mem_oper - 1].type >> 56) & 0x7) - 1], opcode_dest ++;
  
    // 67H prefix - Prefix group 4 (GCC Ordering)
    if(ins->params[res->mem_oper - 1].value & ((u64) 0x1 << 60))
      *opcode_dest = 0x67, opcode_dest ++;
  }

  // Only for Normal **NON** VEX and EVEX instructions
  if(!res->vex) {
    
    // 66H prefix - Prefix group 3 (GCC Ordering) + FWAIT and Prefix Group 1
    if(res->prefixes) {
      *(u32*) opcode_dest = res->prefixes;
      opcode_dest += res->preflen;
    }

    // REX prefix
    //     7                           0
    // +---+---+---+---+---+---+---+---+
    // | 0   1   0   0 | W | R | X | B |
    // +---+---+---+---+---+---+---+---+
    
    // Intel SDM 2.2.1.2: REX prefix: The REX prefix is used to access additional registers (R8-R15) and to extend most instructions to 64 bits.
    // W: 1 = 64 Bit Operand Size
    // R: Extension of the ModR/M reg field. REX.R modifies the ModR/M reg field when that field encodes a GPR, SSE, control or debug register.
    // X: Extension of the SIB index field.
    // B: Extension of the ModR/M r/m field, SIB base field, or Opcode reg field.
    // AMD APM Vol 3: 1.8.2 Figure 1-6: Encoding Examples Using REX R, X, and B Bits
    u8 rex = res->rex;
    if(res->reg_oper && ins->params[res->reg_oper - 1].value & 0x8) rex |= res->modrmreg ? 0x44 : 0x41;
    if(res->mem_oper) {
      if(ismem(ins->params[res->mem_oper - 1].type)) {
        if(membase(ins->params[res->mem_oper - 1].value) & 0x8) rex |= 0x41;
        if(memindex(ins->params[res->mem_oper - 1].value) & 0x8) rex |= 0x42;
      } else if(ins->params[res->mem_oper - 1].value & 0x8) rex |= 0x41;
    }
    if(rex) *opcode_dest = rex, opcode_dest ++;
  }

  // opcode
  *(u32*) opcode_dest = res->opcode;
  opcode_dest += res->oplen;

  // ModR/M | MOD = XX, REG = XXX, RM = XXX | https://wiki.osdev.org/X86-64_Instruction_Encoding#:~:text=r/m-,32/64%2Dbit%20addressing,-These%20are%20the
  if(res->modrmreq) {
    u8 modrm = res->modrm;
    const x64Operand* rm = ins->params + res->mem_oper - 1;

    // Need to watch out for VSIB
    // https://en.wikipedia.org/wiki/ModR/M#:~:text=of%20these%20exceptions.-,Special%20SIB%20byte%20addressing%20modes,-%5Bedit%5D

    if(res->modrmreg)
      modrm |= (ins->params[res->reg_oper - 1].value & 0x7) << 3;

    if(ismem(rm->type)) {
      if(rm->value & 0x2000000000000000) // RIP-relative addressing
        *opcode_dest = modrm | 0x05 /* MOD = 00, REG = XXX, RM = 101 (RIP) */, *(i32*)(opcode_dest + 1) = rm->value, opcode_dest += 5;

      else {
        // NOTE: RBP CANNOT BE A BASE REGISTER WHEN USING SIB! Use it to index instead! https://stackoverflow.com/a/52522744/10013227
        u16 base = membase(rm->value);
        u16 index = memindex(rm->value);
        const i32 value = (i32) rm->value;

        if(base & 0x10) { // No base register => SIB byte without base.
          u8 sib = (index | 0x10) ? 0x25/* Scale = 00, Index = 100, Base = 101, both null*/ : memscale(rm->value) << 6 | (index & 0x7) << 3 | 0x5;
          *opcode_dest = modrm | 0x04 /* MOD = 00, REG = XXX, RM = 100 */, *(opcode_dest + 1) = sib;
          *(i32*) (opcode_dest + 2) = value; // I don't know why, but when the base is null, there's always a 32 bit displacement encoded with the ModR/M + SIB byte.
          opcode_dest += 6;
          goto end;
        }

        base &= 0x7; // It doesn't matter if it's an r8-r13 register now, since we already took care of that in the REX prefix byte.        
        if(value || index != 0x10) {
          if(index & 0x10 && base != $esp) {// No SIB byte required!
            if(value < 128 && value >= -128) // 1 byte/8 bit displacement
              *opcode_dest = modrm | 0x40 | base /* MOD = 01, REG = XXX, RM = XXX */, *(opcode_dest + 1) = (i8) value, opcode_dest += 2;

            // 4 byte/32 bit displacement
            else *opcode_dest = modrm | 0x80 | base /* MOD = 10, REG = XXX, RM = XXX */, *(i32*) (opcode_dest + 1) = value, opcode_dest += 5;

          } else { // SIB required.
            if(index == $esp)
              return error(ASMERR_ESPRSP_USED_AS_INDEX, "ESP/RSP cannot be used as an index register for memory addressing! "
                                                        "If not using scale, switch the base and index(esp/rsp), making esp/rsp the base.");
            if(index & 0x10) index = 0x4; // Happens in the case of base being ESP/RSP, in which case we set the index to 0b100(ESP), which is no register for index in SIB.
            *(opcode_dest + 1) = memscale(rm->value) << 6 | index << 3 | base;

            // If using an index without a displacement
            if(!value) *opcode_dest = modrm | 0x04 /* MOD = 00, REG = XXX, RM = 100 */, opcode_dest += 2;

            else if(value < 128 && value >= -128) // 1 byte/8 bit displacement
              *opcode_dest = modrm | 0x44 /* MOD = 01, REG = XXX, RM = 100 */, *(opcode_dest + 2) = (i8) value, opcode_dest += 3;

            // 4 byte/32 bit displacement
            else *opcode_dest = modrm | 0x84 /* MOD = 10, REG = XXX, RM = 100 */, *(i32*) (opcode_dest + 2) = value, opcode_dest += 6;
          }
        }
        else if(base == $esp) // Special case for ESP/RSP without displacement since ESP as base for the RM means that SIB is used.
          *opcode_dest = modrm | 0x04 /* MOD = 00, REG = XXX, RM = 100 */, *(opcode_dest + 1) = 0x24 /* Scale = 00, Index = 100, Base = 100 */, opcode_dest += 2;
        else if(base == $ebp) // Special case for register EBP/R13 without displacement because with mod = 00, r13 = rip
          *opcode_dest = 0x45 /* MOD = 01, REG = XXX, RM = 101 */, *(opcode_dest + 1) = 0 /* 1 Byte/8 Bit Disp */, opcode_dest ++;
        else *opcode_dest = modrm | base, opcode_dest ++;
      }
    } else *opcode_dest = modrm | 0xC0 | (rm->value & 0x7) /* MOD = 11, REG = XXX, RM = XXX */, opcode_dest ++; // Is not a memory operand, so RM is the register number.
  }

  // For instructions that have +rw, +rd etc
  else if(res->reg_oper)
    *(opcode_dest - 1) |= (ins->params[res->reg_oper - 1].value & 0x7);

end:
  if(res->imm_oper) {
    u32 size = res->args[res->imm_oper - 1] >> 1;
    switch(size) {
    case 1: *       opcode_dest = ins->params[res->imm_oper - 1].value; break;
    case 2: *(i16*) opcode_dest = ins->params[res->imm_oper - 1].value; break;
    case 4: *(i32*) opcode_dest = ins->params[res->imm_oper - 1].value; break;
    case 8: *(i64*) opcode_dest = ins->params[res->imm_oper - 1].value; break;
    }
    opcode_dest += size;
  }

  // Relative displacement for instructions like JMP
  else if(res->rel_oper) {
    if(res->args[res->rel_oper - 1] == REL8) {
      *opcode_dest = ins->params[res->rel_oper - 1].value;
      opcode_dest ++;
    } else { // HAS to be Rel32 for x64.
      *(i32*) opcode_dest = ins->params[res->rel_oper - 1].value;
      opcode_dest += 4;
    }
  }

  else if(res->is4_oper)
    *opcode_dest = (u8) ins->params[res->is4_oper - 1].value << 4, opcode_dest ++;

  return opcode_dest - opcode_dest_start;
}


u32 x64emit(const x64Ins* ins, u8* opcode_dest) {
  x64LookupActualIns* res = identify(ins);
  return encode(ins, res, opcode_dest);
}


static const char* reg_stringify(const x64Operand* reg) {
  if(reg->type & R8) return ((const char*[]){ "al", "cl", "dl", "bl", "sil", "dil", "bpl", "spl", "r8b", "r9b", "r10b", "r11b", "r12b", "r13b", "r14b", "r15b" })[reg->value & 0xF];
  if(reg->type & RH) return ((const char*[]){ "ah", "ch", "dh", "bh" })[reg->value & 0x3];
  if(reg->type & R16) return ((const char*[]){ "ax", "cx", "dx", "bx", "sp", "bp", "si", "di", "bp", "sp", "r8w", "r9w", "r10w", "r11w", "r12w", "r13w", "r14w", "r15w" })[reg->value & 0xF];
  if(reg->type & R32) return ((const char*[]){ "eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi", "r8d", "r9d", "r10d", "r11d", "r12d", "r13d", "r14d", "r15d" })[reg->value & 0xF];
  if(reg->type & R64) return ((const char*[]){ "rax", "rcx", "rdx", "rbx", "rsp", "rbp", "rsi", "rdi", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15" })[reg->value & 0xF];
  if(reg->type & XMM) return ((const char*[]){ "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7", "xmm8", "xmm9", "xmm10", "xmm11", "xmm12", "xmm13", "xmm14", "xmm15", "xmm16", "xmm17", "xmm18", "xmm19", "xmm20", "xmm21", "xmm22", "xmm23", "xmm24", "xmm25", "xmm26", "xmm27", "xmm28", "xmm29", "xmm30", "xmm31" })[reg->value & 0xF];
  if(reg->type & YMM) return ((const char*[]){ "ymm0", "ymm1", "ymm2", "ymm3", "ymm4", "ymm5", "ymm6", "ymm7", "ymm8", "ymm9", "ymm10", "ymm11", "ymm12", "ymm13", "ymm14", "ymm15", "ymm16", "ymm17", "ymm18", "ymm19", "ymm20", "ymm21", "ymm22", "ymm23", "ymm24", "ymm25", "ymm26", "ymm27", "ymm28", "ymm29", "ymm30", "ymm31" })[reg->value & 0xF];
  if(reg->type & ZMM) return ((const char*[]){ "zmm0", "zmm1", "zmm2", "zmm3", "zmm4", "zmm5", "zmm6", "zmm7", "zmm8", "zmm9", "zmm10", "zmm11", "zmm12", "zmm13", "zmm14", "zmm15", "zmm16", "zmm17", "zmm18", "zmm19", "zmm20", "zmm21", "zmm22", "zmm23", "zmm24", "zmm25", "zmm26", "zmm27", "zmm28", "zmm29", "zmm30", "zmm31" })[reg->value & 0xF];
  if(reg->type & MM) return ((const char*[]){ "mm0", "mm1", "mm2", "mm3", "mm4", "mm5", "mm6", "mm7" })[reg->value & 0x7];
  if(reg->type & SREG) return ((const char*[]){ "es", "cs", "ss", "ds", "fs", "gs" })[reg->value & 0x7];
  if(reg->type & CR0_7) return ((const char*[]){ "cr0", "cr1", "cr2", "cr3", "cr4", "cr5", "cr6", "cr7" })[reg->value & 0x7];
  if(reg->type & CR8) return "cr8";
  if(reg->type & DREG) return ((const char*[]){ "dr0", "dr1", "dr2", "dr3", "dr4", "dr5", "dr6", "dr7" })[reg->value & 0x7];
  if(reg->type & ST_0) return "st";
  else if(reg->type & ST) return ((const char*[]){ "st(0)", "st(1)", "st(2)", "st(3)", "st(4)", "st(5)", "st(6)", "st(7)" })[reg->value & 0x7];
  return NULL;
}

static const char* reg_ref_stringify(int reg) {
  if(reg == $none || reg > 40) return NULL;
  return ((const char*[]) { "eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi", "r8d", "r9d", "r10d", "r11d", "r12d", "r13d", "r14d", "r15d", "rax", "rcx", "rdx", "rbx", "rsp", "rbp", "rsi", "rdi", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15", "rip", "es", "cs", "ss", "ds", "fs", "gs" })[reg];
}

enum: u64 {
  allregmask = R8 | RH | R16 | R32 | R64 | MM | XMM | YMM | ZMM | SREG | CR0_7 | DREG | CR8,
  allfarmask = FARPTR1616 | FARPTR1632 | FARPTR1664
};

char* x64stringify(const x64 p, u32 num) {
  if(!num) return "";
  
  u32 mallocsize = num * 20 + 50;
  char* code = malloc(mallocsize);
  u32 cursize = 0;
  u32 curins = 0;
  u8 tab;

  if(num == 1) tab = ' ';
  else {
    tab = code[cursize] = '\t';
    cursize ++;
  }

  while (num --) {
    if (p[curins].op > sizeof(x64Table) / sizeof(x64LookupGeneralIns) || p[curins].op < 1) {
      error(ASMERR_INVALID_INS, "Invalid instruction: %d.", p[curins].op);
      return NULL;
    }
    const x64LookupGeneralIns* ins = x64Table + (p[curins].op - 1);

    u32 inslen = strlen(ins->name);
    memcpy(code + cursize, ins->name, inslen);
    cursize += inslen;

    if(p[curins].params[0].type) {
      code[cursize] = tab;
      cursize ++;
    }

    for(u32 i = 0; i < 4; i ++) {
      if(!p[curins].params[i].type) break;
      if(i) {
        code[cursize] = ',';
        code[cursize + 1] = ' ';
        cursize += 2;
      }
      if(p[curins].params[i].type & allregmask) {
        const char* reg = reg_stringify(p[curins].params + i);
        if(!reg) {
          error(ASMERR_INVALID_REG_TYPE, "Invalid register type: %llX", p[curins].params[i].type);
          free(code);
          return NULL;
        }
        u32 reglen = strlen(reg);
        memcpy(code + cursize, reg, reglen);
        cursize += reglen;
      }
      else if(p[curins].params[i].type & (IMM8 | IMM16 | IMM32 | IMM64))
        cursize += sprintf(code + cursize, "0x%llX", p[curins].params[i].value);

      else if(p[curins].params[i].type & (REL8 | REL32))
        cursize += sprintf(code + cursize, "$%+d", (u32) p[curins].params[i].value);

      // Rip relative memory operand detection
      else if(p[curins].params[i].type & (X64_ALLMEMMASK | allfarmask) && p[curins].params[i].value & ((u64)1 << 62))
        cursize += sprintf(code + cursize, "[$%+d]", (u32) p[curins].params[i].value);

      // else if(p[curins].params[i].type == X64_LABEL_REF)
      //   cursize += sprintf(code + cursize, "%s", p[curins].label_name);

      else if(p[curins].params[i].type & (X64_ALLMEMMASK | allfarmask)) {

        if(p[curins].params[i].type != X64_ALLMEMMASK) {
          const int type = p[curins].params[i].type;

          if(type & allfarmask) memcpy(code + cursize, "far ", 4), cursize += 4;
          else {
            const int ptrstrlen = ((u32 const[]) { 9, 9, 10, 10, 12, 12, 12 })[- (__builtin_clz(type) - 25)];
            memcpy(code + cursize, ((char const *const[]) { "byte ptr ", "word ptr ", "dword ptr ", "qword ptr ", "xmmword ptr ",
              "ymmword ptr ", "zmmword ptr " })[- (__builtin_clz(type) /* f(type) = 32 - log_2 type */ - 25 /* f(M8) */)], ptrstrlen);
            cursize += ptrstrlen;
          }
        }
        

        if(p[curins].params[i].value & 0x0700000000000000)
          cursize += sprintf(code + cursize, "%s:", reg_ref_stringify(((p[curins].params[i].value >> 48) & 0x7) - 1 + $es));

        code[cursize] = '[';
        cursize += 1;

        if(p[curins].params[i].value & 0x2000000000000000) {
          if(p[curins].params[i].value & 0x2000000000000000)
            cursize += sprintf(code + cursize, "rip + 0x%X", (u32) p[curins].params[i].value);

          // RIP-relative taking instructions into account
          else cursize += sprintf(code + cursize, "$%+d", (u32) p[curins].params[i].value);
          
        } else {
          u32 base = membase(p[curins].params[i].value);
          u32 index = memindex(p[curins].params[i].value);
          u32 scale = memscale(p[curins].params[i].value);
          u32 disp = (u32) p[curins].params[i].value;

          if(!(base & 0x10))
            cursize += sprintf(code + cursize, "%s", reg_ref_stringify(base + (p[curins].params[i].value & 0x1000000000000000 ? 0 : $rax)));

          if(disp) cursize += sprintf(code + cursize, " + 0x%X", disp);

          if(!(index & 0x10)) {
            const char* str;
            if(p[curins].params[i].value & 0x8000000000000000)
              str = reg_stringify(&(x64Operand) { p[curins].params[0].type, index });
            else str = reg_ref_stringify(index + (p[curins].params[i].value & 0x1000000000000000 ? 0 : $rax));
            cursize += sprintf(code + cursize, " + %s", str);
          }

          if(scale) cursize += sprintf(code + cursize, " * %d", 1 << scale);
        }
        code[cursize] = ']';
        cursize ++;
      }
    }

    // If there is a next instruction, add a newline.
    if(num > 0)
      *((u16*) code + cursize) = (u16) ('\n' << 8) | '\t', cursize += 2;

    if(cursize + 50 >= mallocsize) {
      mallocsize += 100 + num * 20;
      code = realloc(code, mallocsize);
    }
    curins ++;
  }

  code[cursize] = 0;
  return code;
}


struct x64_relative {
  u32 ins; bool relref; // regular relative or riprel
  u8 size; u8 param;
  x64LookupActualIns* res;
};

// static inline u32 fnv1a(const char* data) {
//   u32 hash = 0x811c9dc5;
//   while (*data) {
//     hash ^= *data++;
//     hash += (hash<<1) + (hash<<4) + (hash<<7) + (hash<<8) + (hash<<24);
//   }
//   return hash;
// }

// static inline u32 checksum(const char* data) {
//   u32 hash = 0;
//   while (*data) hash += *data++;
//   return hash;
// }


/**
Current Problems with label-based linking:
  - Need to completely change the original instruction and re-identify the instruction when trying to reduce the size of JMP / JCC instructions, since labels don't have a size.
  - Need to keep re-calculating all of the instruction distances when a jump instruction changes size, as there can be jumps inside the area jumped over.
  - Need to have a format that supports bringing different code together with different labels to allow them to link together

union x64MCode {
  u8 bytes[16];
  struct {
    u8 len;
    u8 data[15];
  };
};
typedef union x64MCode x64MCode;

struct x64AssemblyRes {
  x64MCode* code;
  u32 code_len;
  struct x64Relocatable {
    x64Op op;
    
  }* reloc;
};

So I just want to release this tbh, I will hold off on label based linking and string storage.
*/

u8* x64as(const x64 p, u32 num, u32* len) {
  u32 code_size = num * 15;// 15 is the maximum size of 1 instruction. Example: lwpval rax, cs:[rax+rbx*8+0x23829382], 100000000
  u32 indexes_size = num * sizeof(u16);
  u32 relref_size = num * sizeof(struct x64_relative);

  u8 *const encoding_arena = calloc(code_size + indexes_size + relref_size, 1);
  u8 *const code = encoding_arena;
  u16 *const indexes = (u16*) (encoding_arena + code_size);
  struct x64_relative *const relrefidxes = (struct x64_relative*) (encoding_arena + code_size + indexes_size);
  // 2048 * 15 + 2048 * 2 + 2048 * 16 = 67584, about 33x more memory than instructions :skull:

  *len = 0;
  u32 codelen = 0;
  u32 index = 0;
  u32 relreflen = 0;
  
  while (num --) {
    x64LookupActualIns* res = identify(p + index);
    int curlen = encode(p + index, res, code + codelen);
    if(!curlen) goto error;

    if(res->rel_oper) {
      i32 insns = p[index].params[res->rel_oper - 1].value;

      // we don't need relrefs if the value was negative
      if(insns <= 1) {
        if(insns + index < 0) {
          error(ASMERR_REL_OUT_OF_RANGE, "Relative reference out of range on ins '%s'", x64stringify(p + index, 1));
          goto error;
        }

        i32 offset;

        if(insns == 1) offset = 0;
        else if (insns == 0) offset = -curlen;
        else offset = indexes[index + insns] - curlen;

        // JCC size is either 2, 3, 5 or 6 with 0f prefixes
        if(curlen > 4) { // DOWNSIZE IF INSTRUCTION IS TOO BIG
          if(offset >= -125) {
            curlen -= 3;
            if(offset != 0) offset -= 3;
            goto downsize;
          }
upsize:
          *(int*) (code + codelen + curlen - 4) = offset;
        }
        else if (curlen < 4) {
          if(offset <= -128) { // UPSIZE IF INSTRUCTION IS TOO SMALL
            curlen += 3;
            if(offset != 0) offset += 3;
            goto upsize;
          }
downsize:
          code[codelen + curlen - 1] = (i8) offset;
        }
      } else {
        relrefidxes[relreflen].ins = index;
        relrefidxes[relreflen].res = res;
        relrefidxes[relreflen].param = res->rel_oper - 1;
        relrefidxes[relreflen].size = res->args[res->rel_oper - 1] == REL32 ? 4 : 1;
        relreflen ++;
      }
    }

    // Identify riprels
    else if(res->mem_oper && p[index].params[res->mem_oper - 1].value & 0x4000000000000000) {
      i32 insns = p[index].params[res->mem_oper - 1].value;

      // If resolving offsets for instructions that were already resolved, including the current instruction, take this fast path before adding work to the other for loop
      if(insns <= 1) {
        if(insns + index < 0) {
          error(ASMERR_REL_OUT_OF_RANGE, "RIP Relative out of range on ins '%s'", x64stringify(p + index, 1));
          goto error;
        }

        i32 offset;

        if(insns == 1) offset = 0;
        else if (insns == 0) offset = -curlen;
        else offset = indexes[index + insns] - codelen - curlen; // i don't like repeating code but this is just simpler.
        
        // Currently no better way other than to directly reencode.
        x64Ins ins = p[index];
        ins.params[res->mem_oper - 1].value &= ~((u64) 0xffffffff);
        ins.params[res->mem_oper - 1].value |= ((u64) offset) & 0xffffffff;
        encode(&ins, res, code + codelen); // THIS WILL ALWAYS BE THE SAME SIZE SO NO NEED TO MANIPULATE CURLEN, RIP SIB HAS A CONSTANT SIZE.
      } else {
        relrefidxes[relreflen].ins = index;
        relrefidxes[relreflen].res = res;
        relrefidxes[relreflen].param = res->mem_oper - 1;
        relrefidxes[relreflen].relref = true;
        relreflen ++;
      }
    }

    indexes[index] = codelen;
    codelen += curlen;

    index ++;
  }

  for(u32 i = 0; i < relreflen; i ++) {

    // Index of current instruction in `indexes`
    u32 relidx = relrefidxes[i].ins;
    const i64* insoffset = &p[relidx].params[relrefidxes[i].param].value;
    i32 offset = indexes[relidx + /* The current instruction that is being resolved */ *(i32*) insoffset /* The offset of the relative */]
                 - indexes[relidx + 1]; /* Added 1 because the offset is added to a rip pointing to the next instruction */
    
    if(relrefidxes[i].relref) {
      // Currently no better way other than to directly reencode.
      x64Ins ins = p[relidx];
      ins.params[relrefidxes[i].param].value &= ~((u64) 0xffffffff);
      ins.params[relrefidxes[i].param].value |= ((u64) offset) & 0xffffffff;
      encode(&ins, relrefidxes[i].res, code + indexes[relidx]);
    } else {
      if(relrefidxes[i].size == 4) {
        *(int*) (code + indexes[relidx + 1] - 4) = offset;
      } else code[indexes[relidx + 1] - 1] = (i8) offset;
    }
  }

  *len = codelen;
  return encoding_arena;
error:
  free(encoding_arena);
  *len = 0;
  return NULL;
}

#if defined _WIN32 || defined __CYGWIN__

// https://learn.microsoft.com/en-us/windows/win32/memory/memory-protection-constants
#define PAGE_EXECUTE_READ 0x20
#define PAGE_READWRITE 0x4
// https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-virtualalloc
#define MEM_COMMIT 0x00001000
// https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-virtualfree
#define MEM_RELEASE 0x8000

__attribute((dllimport)) void* __attribute((stdcall)) VirtualAlloc(void* lpAddress, size_t dwSize, u32 flAllocationType, u32 flProtect);
__attribute((dllimport)) int __attribute((stdcall)) VirtualProtect(void* lpAddress, size_t dwSize, u32 flNewProtect, u32* lpflOldProtect);
__attribute((dllimport)) int __attribute((stdcall)) VirtualFree(void* lpAddress, size_t dwSize, u32 dwFreeType);

// Aligns to the next multiple of a, where a is a power of 2
static inline u32 align(u32 n, u32 a) { return (n + a - 1) & ~(a - 1); }
void (*x64exec(void* mem, u32 size))() {
	u32 pagesize = 4096;
	u32 alignedsize = align(size, pagesize);

	void* buf = VirtualAlloc(NULL, alignedsize, MEM_COMMIT, PAGE_READWRITE);
	memcpy(buf, mem, size);

	u32 old;
	VirtualProtect(buf, size, PAGE_EXECUTE_READ, &old);
	return buf;
}

void x64exec_free(void* buf, u32 size) {
  VirtualFree(buf, 0, MEM_RELEASE);
  (void)size;
}

#else
#include <sys/mman.h>
#include <unistd.h>

void (*x64exec(void* mem, u32 size))() {\
	void* buf = mmap(NULL, size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANON, -1, 0);
	memcpy(buf, mem, size);

	mprotect(buf, size, PROT_READ | PROT_EXEC);
	return buf;
}

void x64exec_free(void* buf, u32 size) {
  munmap(buf, size);
}

#endif
