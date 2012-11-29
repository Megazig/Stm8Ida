#include "stm8.hpp"

#define F_NONDX		0

#define F_REGA		0
#define F_REGX		1
#define F_REGY		2
#define F_REGCC		3
#define F_REGSP		4
#define F_REGLX		5
#define F_REGHX		6
#define F_REGLY		7
#define F_REGHY		8

#define F_BYTE			10
#define F_WORD			11
#define F_SHORTMEM		12
#define F_LONGMEM		13
#define F_PARX			14
#define F_SHORTOFFX		15
#define F_LONGOFFX		16
#define F_PARY			17
#define F_SHORTOFFY		18
#define F_LONGOFFY		19
#define F_SHORTOFFSP	110
#define F_SHORTPTRW_	111
#define F_LONGPTRW_		112
#define F_SHORTPTRWX	113
#define F_LONGPTRWX		114
#define F_SHORTPTRWY	115
#define F_EXTMEM		116
#define F_LONGPTRE_		117
#define F_LONGPTREX		118
#define F_LONGPTREY		119
#define F_EXTOFFX		120
#define F_EXTOFFY		121
#define F_SHORTJUMP		122
#define F_NPOS0			300
#define F_NPOS1			301
#define F_NPOS2			302
#define F_NPOS3			303
#define F_NPOS4			304
#define F_NPOS5			305
#define F_NPOS6			306
#define F_NPOS7			307
#define F_NONE			999


struct opcode_t
{
	uchar itype;
	uchar opcode;
	ushort dst;
	ushort src;
	uchar len;
	bool extended;
	uchar iform;
};
/* XXX - TYPE 3 ALWAYS HAVE IMPLIED F_SHORTJUMP AND USE PC_LEN */

static const opcode_t primary[256] =
{
	{STM8_neg,0x00,F_SHORTOFFSP,F_NONE,2,0,1},
	{STM8_rrwa,0x01,F_REGX,F_REGA,1,0,1},
	{STM8_rlwa,0x02,F_REGX,F_NONE,1,0,1},
	{STM8_cpl,0x03,F_SHORTOFFSP,F_NONE,2,0,1},
	{STM8_srl,0x04,F_SHORTOFFSP,F_NONE,2,0,1},
	{STM8_null,0x05,F_NONE,F_NONE,1,0,1},
	{STM8_rrc,0x06,F_SHORTOFFSP,F_NONE,2,0,1},
	{STM8_sra,0x07,F_SHORTOFFSP,F_NONE,2,0,1},
	{STM8_sll,0x08,F_SHORTOFFSP,F_NONE,2,0,1},
	{STM8_rlc,0x09,F_SHORTOFFSP,F_NONE,2,0,1},
	{STM8_dec,0x0A,F_SHORTOFFSP,F_NONE,2,0,1},
	{STM8_null,0x0B,F_NONE,F_NONE,1,0,1},
	{STM8_inc,0x0C,F_SHORTOFFSP,F_NONE,2,0,1},
	{STM8_tnz,0x0D,F_SHORTOFFSP,F_NONE,2,0,1},
	{STM8_swap,0x0E,F_SHORTOFFSP,F_NONE,2,0,1},
	{STM8_clr,0x0F,F_SHORTOFFSP,F_NONE,2,0,1},
	{STM8_sub,0x10,F_REGA,F_SHORTOFFSP,2,0,1},
	{STM8_cp,0x11,F_REGA,F_SHORTOFFSP,2,0,1},
	{STM8_sbc,0x12,F_REGA,F_SHORTOFFSP,2,0,1},
	{STM8_cpw,0x13,F_REGX,F_SHORTOFFSP,2,0,1},
	{STM8_and,0x14,F_REGA,F_SHORTOFFSP,2,0,1},
	{STM8_bcp,0x15,F_REGA,F_SHORTOFFSP,2,0,1},
	{STM8_ldw,0x16,F_REGY,F_SHORTOFFSP,2,0,1},
	{STM8_ldw,0x17,F_SHORTOFFSP,F_REGY,2,0,1},
	{STM8_xor,0x18,F_REGA,F_SHORTOFFSP,2,0,1},
	{STM8_adc,0x19,F_REGA,F_SHORTOFFSP,2,0,1},
	{STM8_or,0x1A,F_REGA,F_SHORTOFFSP,2,0,1},
	{STM8_add,0x1B,F_REGA,F_SHORTOFFSP,2,0,1},
	{STM8_addw,0x1C,F_REGX,F_WORD,3,0,1},
	{STM8_subw,0x1D,F_REGX,F_WORD,3,0,1},
	{STM8_ldw,0x1E,F_REGX,F_SHORTOFFSP,2,0,1},
	{STM8_ldw,0x1F,F_SHORTOFFSP,F_REGX,2,0,1},
	{STM8_jra,0x20,F_SHORTJUMP,F_NONE,2,0,1},
	{STM8_jrf,0x21,F_SHORTJUMP,F_NONE,2,0,1},
	{STM8_jrugt,0x22,F_SHORTJUMP,F_NONE,2,0,1},
	{STM8_jrule,0x23,F_SHORTJUMP,F_NONE,2,0,1},
	{STM8_jrnc,0x24,F_SHORTJUMP,F_NONE,2,0,1},
	{STM8_jrc,0x25,F_SHORTJUMP,F_NONE,2,0,1},
	{STM8_jrne,0x26,F_SHORTJUMP,F_NONE,2,0,1},
	{STM8_jreq,0x27,F_SHORTJUMP,F_NONE,2,0,1},
	{STM8_jrnv,0x28,F_SHORTJUMP,F_NONE,2,0,1},
	{STM8_jrv,0x29,F_SHORTJUMP,F_NONE,2,0,1},
	{STM8_jrpl,0x2A,F_SHORTJUMP,F_NONE,2,0,1},
	{STM8_jrmi,0x2B,F_SHORTJUMP,F_NONE,2,0,1},
	{STM8_jrsgt,0x2C,F_SHORTJUMP,F_NONE,2,0,1},
	{STM8_jrsle,0x2D,F_SHORTJUMP,F_NONE,2,0,1},
	{STM8_jrsge,0x2E,F_SHORTJUMP,F_NONE,2,0,1},
	{STM8_jrslt,0x2F,F_SHORTJUMP,F_NONE,2,0,1},
	{STM8_neg,0x30,F_SHORTMEM,F_NONE,2,0,1},
	{STM8_exg,0x31,F_REGA,F_LONGMEM,3,0,1},
	{STM8_pop,0x32,F_LONGMEM,F_NONE,3,0,1},
	{STM8_cpl,0x33,F_SHORTMEM,F_NONE,2,0,1},
	{STM8_srl,0x34,F_SHORTMEM,F_NONE,2,0,1},
	{STM8_mov,0x35,F_LONGMEM,F_BYTE,4,0,2},
	{STM8_rrc,0x36,F_SHORTMEM,F_NONE,2,0,1},
	{STM8_sra,0x37,F_SHORTMEM,F_NONE,2,0,1},
	{STM8_sll,0x38,F_SHORTMEM,F_NONE,2,0,1},
	{STM8_rlc,0x39,F_SHORTMEM,F_NONE,2,0,1},
	{STM8_dec,0x3A,F_SHORTMEM,F_NONE,2,0,1},
	{STM8_push,0x3B,F_LONGMEM,F_NONE,3,0,1},
	{STM8_inc,0x3C,F_SHORTMEM,F_NONE,2,0,1},
	{STM8_tnz,0x3D,F_SHORTMEM,F_NONE,2,0,1},
	{STM8_swap,0x3E,F_SHORTMEM,F_NONE,2,0,1},
	{STM8_clr,0x3F,F_SHORTMEM,F_NONE,2,0,1},
	{STM8_neg,0x40,F_REGA,F_NONE,1,0,1},
	{STM8_exg,0x41,F_REGA,F_REGLX,1,0,1},
	{STM8_mul,0x42,F_REGX,F_REGA,1,0,1},
	{STM8_cpl,0x43,F_REGA,F_NONE,1,0,1},
	{STM8_srl,0x44,F_REGA,F_NONE,1,0,1},
	{STM8_mov,0x45,F_SHORTMEM,F_SHORTMEM,3,0,2},
	{STM8_rrc,0x46,F_REGA,F_NONE,1,0,1},
	{STM8_sra,0x47,F_REGA,F_NONE,1,0,1},
	{STM8_sll,0x48,F_REGA,F_NONE,1,0,1},
	{STM8_rlc,0x49,F_REGA,F_NONE,1,0,1},
	{STM8_dec,0x4A,F_REGA,F_NONE,1,0,1},
	{STM8_push,0x4B,F_BYTE,F_NONE,2,0,1},
	{STM8_inc,0x4C,F_REGA,F_NONE,1,0,1},
	{STM8_tnz,0x4D,F_REGA,F_NONE,1,0,1},
	{STM8_swap,0x4E,F_REGA,F_NONE,1,0,1},
	{STM8_clr,0x4F,F_REGA,F_NONE,1,0,1},
	{STM8_negw,0x50,F_REGX,F_NONE,1,0,1},
	{STM8_exgw,0x51,F_REGX,F_REGY,1,0,1},
	{STM8_sub,0x52,F_REGSP,F_BYTE,2,0,1},
	{STM8_cplw,0x53,F_REGX,F_NONE,1,0,1},
	{STM8_srlw,0x54,F_REGX,F_NONE,1,0,1},
	{STM8_mov,0x55,F_LONGMEM,F_LONGMEM,5,0,2},
	{STM8_rrcw,0x56,F_REGX,F_NONE,1,0,1},
	{STM8_sraw,0x57,F_REGX,F_NONE,1,0,1},
	{STM8_sllw,0x58,F_REGX,F_NONE,1,0,1},
	{STM8_rlcw,0x59,F_REGX,F_NONE,1,0,1},
	{STM8_decw,0x5A,F_REGX,F_NONE,1,0,1},
	{STM8_addw,0x5B,F_REGSP,F_BYTE,2,0,1},
	{STM8_incw,0x5C,F_REGX,F_NONE,1,0,1},
	{STM8_tnzw,0x5D,F_REGX,F_NONE,1,0,1},
	{STM8_swapw,0x5E,F_REGX,F_NONE,1,0,1},
	{STM8_clrw,0x5F,F_REGX,F_NONE,1,0,1},
	{STM8_neg,0x60,F_SHORTOFFX,F_NONE,2,0,1},
	{STM8_exg,0x61,F_REGA,F_REGLY,1,0,1},
	{STM8_div,0x62,F_REGX,F_NONE,1,0,1},
	{STM8_cpl,0x63,F_SHORTOFFX,F_NONE,2,0,1},
	{STM8_srl,0x64,F_SHORTOFFX,F_NONE,2,0,1},
	{STM8_divw,0x65,F_REGX,F_REGY,1,0,1},
	{STM8_rrc,0x66,F_SHORTOFFX,F_NONE,2,0,1},
	{STM8_sra,0x67,F_SHORTOFFX,F_NONE,2,0,1},
	{STM8_sll,0x68,F_SHORTOFFX,F_NONE,2,0,1},
	{STM8_rlc,0x69,F_SHORTOFFX,F_NONE,2,0,1},
	{STM8_dec,0x6A,F_SHORTOFFX,F_NONE,2,0,1},
	{STM8_ld,0x6B,F_SHORTOFFSP,F_REGA,2,0,1},
	{STM8_inc,0x6C,F_SHORTOFFX,F_NONE,2,0,1},
	{STM8_tnz,0x6D,F_SHORTOFFX,F_NONE,2,0,1},
	{STM8_swap,0x6E,F_SHORTOFFX,F_NONE,2,0,1},
	{STM8_clr,0x6F,F_SHORTOFFX,F_NONE,2,0,1},
	{STM8_neg,0x70,F_PARX,F_NONE,1,0,1},
	{STM8_unknown,0x71,F_NONE,F_NONE,1,0,1},
	{STM8_null,0x72,F_NONE,F_NONE,1,0,1},
	{STM8_cpl,0x73,F_PARX,F_NONE,1,0,1},
	{STM8_srl,0x74,F_PARX,F_NONE,1,0,1},
	{STM8_null,0x75,F_NONE,F_NONE,1,0,1},
	{STM8_rrc,0x76,F_PARX,F_NONE,1,0,1},
	{STM8_sra,0x77,F_PARX,F_NONE,1,0,1},
	{STM8_sll,0x78,F_PARX,F_NONE,1,0,1},
	{STM8_rlc,0x79,F_PARX,F_NONE,1,0,1},
	{STM8_dec,0x7A,F_PARX,F_NONE,1,0,1},
	{STM8_ld,0x7B,F_REGA,F_SHORTOFFSP,2,0,1},
	{STM8_inc,0x7C,F_PARX,F_NONE,1,0,1},
	{STM8_tnz,0x7D,F_PARX,F_NONE,1,0,1},
	{STM8_swap,0x7E,F_PARX,F_NONE,1,0,1},
	{STM8_clr,0x7F,F_PARX,F_NONE,1,0,1},
	{STM8_iret,0x80,F_NONE,F_NONE,1,0,1},
	{STM8_ret,0x81,F_NONE,F_NONE,1,0,1},
	{STM8_int,0x82,F_EXTMEM,F_NONE,4,0,1},
	{STM8_trap,0x83,F_NONE,F_NONE,1,0,1},
	{STM8_pop,0x84,F_REGA,F_NONE,1,0,1},
	{STM8_popw,0x85,F_REGX,F_NONE,1,0,1},
	{STM8_pop,0x86,F_REGCC,F_NONE,1,0,1},
	{STM8_retf,0x87,F_NONE,F_NONE,1,0,1},
	{STM8_push,0x88,F_REGA,F_NONE,1,0,1},
	{STM8_pushw,0x89,F_REGX,F_NONE,1,0,1},
	{STM8_push,0x8A,F_REGCC,F_NONE,1,0,1},
	{STM8_break,0x8B,F_NONE,F_NONE,1,0,1},
	{STM8_ccf,0x8C,F_NONE,F_NONE,1,0,1},
	{STM8_callf,0x8D,F_EXTMEM,F_NONE,4,0,1},
	{STM8_halt,0x8E,F_NONE,F_NONE,1,0,1},
	{STM8_wfi,0x8F,F_NONE,F_NONE,1,0,1},
	{STM8_null,0x90,F_NONE,F_NONE,1,0,1},
	{STM8_null,0x91,F_NONE,F_NONE,1,0,1},
	{STM8_null,0x92,F_NONE,F_NONE,1,0,1},
	{STM8_ldw,0x93,F_REGX,F_REGY,1,0,1},
	{STM8_ldw,0x94,F_REGSP,F_REGX,1,0,1},
	{STM8_ld,0x95,F_REGHX,F_REGA,1,0,1},
	{STM8_ldw,0x96,F_REGX,F_REGSP,1,0,1},
	{STM8_ld,0x97,F_REGLX,F_REGA,1,0,1},
	{STM8_rcf,0x98,F_NONE,F_NONE,1,0,1},
	{STM8_scf,0x99,F_NONE,F_NONE,1,0,1},
	{STM8_rim,0x9A,F_NONE,F_NONE,1,0,1},
	{STM8_sim,0x9B,F_NONE,F_NONE,1,0,1},
	{STM8_rvf,0x9C,F_NONE,F_NONE,1,0,1},
	{STM8_nop,0x9D,F_NONE,F_NONE,1,0,1},
	{STM8_ld,0x9E,F_REGA,F_REGHX,1,0,1},
	{STM8_ld,0x9F,F_REGA,F_REGLX,1,0,1},
	{STM8_sub,0xA0,F_REGA,F_BYTE,2,0,1},
	{STM8_cp,0xA1,F_REGA,F_BYTE,2,0,1},
	{STM8_sbc,0xA2,F_REGA,F_BYTE,2,0,1},
	{STM8_cpw,0xA3,F_REGX,F_WORD,3,0,1},
	{STM8_and,0xA4,F_REGA,F_BYTE,2,0,1},
	{STM8_bcp,0xA5,F_REGA,F_BYTE,2,0,1},
	{STM8_ld,0xA6,F_REGA,F_BYTE,2,0,1},
	{STM8_ldf,0xA7,F_EXTOFFX,F_REGA,4,0,1},
	{STM8_xor,0xA8,F_REGA,F_BYTE,2,0,1},
	{STM8_adc,0xA9,F_REGA,F_BYTE,2,0,1},
	{STM8_or,0xAA,F_REGA,F_BYTE,2,0,1},
	{STM8_add,0xAB,F_REGA,F_BYTE,2,0,1},
	{STM8_jpf,0xAC,F_EXTMEM,F_NONE,4,0,1},
	{STM8_callr,0xAD,F_SHORTMEM,F_NONE,2,0,1},
	{STM8_ldw,0xAE,F_REGX,F_WORD,3,0,1},
	{STM8_ldf,0xAF,F_REGA,F_EXTOFFX,4,0,1},
	{STM8_sub,0xB0,F_REGA,F_SHORTMEM,2,0,1},
	{STM8_cp,0xB1,F_REGA,F_SHORTMEM,2,0,1},
	{STM8_sbc,0xB2,F_REGA,F_SHORTMEM,2,0,1},
	{STM8_cpw,0xB3,F_REGX,F_SHORTMEM,2,0,1},
	{STM8_and,0xB4,F_REGA,F_SHORTMEM,2,0,1},
	{STM8_bcp,0xB5,F_REGA,F_SHORTMEM,2,0,1},
	{STM8_ld,0xB6,F_REGA,F_SHORTMEM,2,0,1},
	{STM8_ld,0xB7,F_SHORTMEM,F_REGA,2,0,1},
	{STM8_xor,0xB8,F_REGA,F_SHORTMEM,2,0,1},
	{STM8_adc,0xB9,F_REGA,F_SHORTMEM,2,0,1},
	{STM8_or,0xBA,F_REGA,F_SHORTMEM,2,0,1},
	{STM8_add,0xBB,F_REGA,F_SHORTMEM,2,0,1},
	{STM8_ldf,0xBC,F_REGA,F_EXTMEM,4,0,1},
	{STM8_ldf,0xBD,F_EXTMEM,F_REGA,4,0,1},
	{STM8_ldw,0xBE,F_REGX,F_SHORTMEM,2,0,1},
	{STM8_ldw,0xBF,F_SHORTMEM,F_REGX,2,0,1},
	{STM8_sub,0xC0,F_REGA,F_LONGMEM,3,0,1},
	{STM8_cp,0xC1,F_REGA,F_LONGMEM,3,0,1},
	{STM8_sbc,0xC2,F_REGA,F_LONGMEM,3,0,1},
	{STM8_cpw,0xC3,F_REGX,F_LONGMEM,3,0,1},
	{STM8_and,0xC4,F_REGA,F_LONGMEM,3,0,1},
	{STM8_bcp,0xC5,F_REGA,F_LONGMEM,3,0,1},
	{STM8_ld,0xC6,F_REGA,F_LONGMEM,3,0,1},
	{STM8_ld,0xC7,F_LONGMEM,F_REGA,3,0,1},
	{STM8_xor,0xC8,F_REGA,F_LONGMEM,3,0,1},
	{STM8_adc,0xC9,F_REGA,F_LONGMEM,3,0,1},
	{STM8_or,0xCA,F_REGA,F_LONGMEM,3,0,1},
	{STM8_add,0xCB,F_REGA,F_LONGMEM,3,0,1},
	{STM8_jp,0xCC,F_LONGMEM,F_NONE,3,0,1},
	{STM8_call,0xCD,F_LONGMEM,F_NONE,3,0,1},
	{STM8_ldw,0xCE,F_REGX,F_LONGMEM,3,0,1},
	{STM8_ldw,0xCF,F_LONGMEM,F_REGX,3,0,1},
	{STM8_sub,0xD0,F_REGA,F_LONGOFFX,3,0,1},
	{STM8_cp,0xD1,F_REGA,F_LONGOFFX,3,0,1},
	{STM8_sbc,0xD2,F_REGA,F_LONGOFFX,3,0,1},
	{STM8_cpw,0xD3,F_REGX,F_LONGOFFX,3,0,1},
	{STM8_and,0xD4,F_REGA,F_LONGOFFX,3,0,1},
	{STM8_bcp,0xD5,F_REGA,F_LONGOFFX,3,0,1},
	{STM8_ld,0xD6,F_REGA,F_LONGOFFX,3,0,1},
	{STM8_ld,0xD7,F_LONGOFFX,F_REGA,3,0,1},
	{STM8_xor,0xD8,F_REGA,F_LONGOFFX,3,0,1},
	{STM8_adc,0xD9,F_REGA,F_LONGOFFX,3,0,1},
	{STM8_or,0xDA,F_REGA,F_LONGOFFX,3,0,1},
	{STM8_add,0xDB,F_REGA,F_LONGOFFX,3,0,1},
	{STM8_jp,0xDC,F_LONGOFFX,F_NONE,3,0,1},
	{STM8_call,0xDD,F_LONGOFFX,F_NONE,3,0,1},
	{STM8_ldw,0xDE,F_REGX,F_LONGOFFX,3,0,1},
	{STM8_ldw,0xDF,F_LONGOFFX,F_REGY,3,0,1},
	{STM8_sub,0xE0,F_REGA,F_SHORTOFFX,2,0,1},
	{STM8_cp,0xE1,F_REGA,F_SHORTOFFX,2,0,1},
	{STM8_sbc,0xE2,F_REGA,F_SHORTOFFX,2,0,1},
	{STM8_cpw,0xE3,F_REGX,F_SHORTOFFX,2,0,1},
	{STM8_and,0xE4,F_REGA,F_SHORTOFFX,2,0,1},
	{STM8_bcp,0xE5,F_REGA,F_SHORTOFFX,2,0,1},
	{STM8_ld,0xE6,F_REGA,F_SHORTOFFX,2,0,1},
	{STM8_ld,0xE7,F_SHORTOFFX,F_REGA,2,0,1},
	{STM8_xor,0xE8,F_REGA,F_SHORTOFFX,2,0,1},
	{STM8_adc,0xE9,F_REGA,F_SHORTOFFX,2,0,1},
	{STM8_or,0xEA,F_REGA,F_SHORTOFFX,2,0,1},
	{STM8_add,0xEB,F_REGA,F_SHORTOFFX,2,0,1},
	{STM8_jp,0xEC,F_SHORTOFFX,F_NONE,2,0,1},
	{STM8_call,0xED,F_SHORTOFFX,F_NONE,2,0,1},
	{STM8_ldw,0xEE,F_REGX,F_SHORTOFFX,2,0,1},
	{STM8_ldw,0xEF,F_SHORTOFFX,F_REGY,2,0,1},
	{STM8_sub,0xF0,F_REGA,F_PARX,1,0,1},
	{STM8_cp,0xF1,F_REGA,F_PARX,1,0,1},
	{STM8_sbc,0xF2,F_REGA,F_PARX,1,0,1},
	{STM8_cpw,0xF3,F_REGX,F_PARX,1,0,1},
	{STM8_and,0xF4,F_REGA,F_PARX,1,0,1},
	{STM8_bcp,0xF5,F_REGA,F_PARX,1,0,1},
	{STM8_ld,0xF6,F_REGA,F_PARX,1,0,1},
	{STM8_ld,0xF7,F_PARX,F_REGA,1,0,1},
	{STM8_xor,0xF8,F_REGA,F_PARX,1,0,1},
	{STM8_adc,0xF9,F_REGA,F_PARX,1,0,1},
	{STM8_or,0xFA,F_REGA,F_PARX,1,0,1},
	{STM8_add,0xFB,F_REGA,F_PARX,1,0,1},
	{STM8_jp,0xFC,F_PARX,F_NONE,1,0,1},
	{STM8_call,0xFD,F_PARX,F_NONE,1,0,1},
	{STM8_ldw,0xFE,F_REGX,F_PARX,1,0,1},
	{STM8_ldw,0xFF,F_PARX,F_REGY,1,0,1}
};

static const opcode_t pre72[] =
{
	{STM8_btjt,0x00,F_LONGMEM,F_NPOS0,5,1,3},
	{STM8_btjf,0x01,F_LONGMEM,F_NPOS0,5,1,3},
	{STM8_btjt,0x02,F_LONGMEM,F_NPOS1,5,1,3},
	{STM8_btjf,0x03,F_LONGMEM,F_NPOS1,5,1,3},
	{STM8_btjt,0x04,F_LONGMEM,F_NPOS2,5,1,3},
	{STM8_btjf,0x05,F_LONGMEM,F_NPOS2,5,1,3},
	{STM8_btjt,0x06,F_LONGMEM,F_NPOS3,5,1,3},
	{STM8_btjf,0x07,F_LONGMEM,F_NPOS3,5,1,3},
	{STM8_btjt,0x08,F_LONGMEM,F_NPOS4,5,1,3},
	{STM8_btjf,0x09,F_LONGMEM,F_NPOS4,5,1,3},
	{STM8_btjt,0x0A,F_LONGMEM,F_NPOS5,5,1,3},
	{STM8_btjf,0x0B,F_LONGMEM,F_NPOS5,5,1,3},
	{STM8_btjt,0x0C,F_LONGMEM,F_NPOS6,5,1,3},
	{STM8_btjf,0x0D,F_LONGMEM,F_NPOS6,5,1,3},
	{STM8_btjt,0x0E,F_LONGMEM,F_NPOS7,5,1,3},
	{STM8_btjf,0x0F,F_LONGMEM,F_NPOS7,5,1,3},
	{STM8_bset,0x10,F_LONGMEM,F_NPOS0,4,1,1},
	{STM8_bres,0x11,F_LONGMEM,F_NPOS0,4,1,1},
	{STM8_bset,0x12,F_LONGMEM,F_NPOS1,4,1,1},
	{STM8_bres,0x13,F_LONGMEM,F_NPOS1,4,1,1},
	{STM8_bset,0x14,F_LONGMEM,F_NPOS2,4,1,1},
	{STM8_bres,0x15,F_LONGMEM,F_NPOS2,4,1,1},
	{STM8_bset,0x16,F_LONGMEM,F_NPOS3,4,1,1},
	{STM8_bres,0x17,F_LONGMEM,F_NPOS3,4,1,1},
	{STM8_bset,0x18,F_LONGMEM,F_NPOS4,4,1,1},
	{STM8_bres,0x19,F_LONGMEM,F_NPOS4,4,1,1},
	{STM8_bset,0x1A,F_LONGMEM,F_NPOS5,4,1,1},
	{STM8_bres,0x1B,F_LONGMEM,F_NPOS5,4,1,1},
	{STM8_bset,0x1C,F_LONGMEM,F_NPOS6,4,1,1},
	{STM8_bres,0x1D,F_LONGMEM,F_NPOS6,4,1,1},
	{STM8_bset,0x1E,F_LONGMEM,F_NPOS7,4,1,1},
	{STM8_bres,0x1F,F_LONGMEM,F_NPOS7,4,1,1},
	{STM8_neg,0x30,F_LONGPTRW_,F_NONE,4,1,1},
	{STM8_cpl,0x33,F_LONGPTRW_,F_NONE,4,1,1},
	{STM8_srl,0x34,F_LONGPTRW_,F_NONE,4,1,1},
	{STM8_rrc,0x36,F_LONGPTRW_,F_NONE,4,1,1},
	{STM8_sra,0x37,F_LONGPTRW_,F_NONE,4,1,1},
	{STM8_sll,0x38,F_LONGPTRW_,F_NONE,4,1,1},
	{STM8_rlc,0x39,F_LONGPTRW_,F_NONE,4,1,1},
	{STM8_dec,0x3A,F_LONGPTRW_,F_NONE,4,1,1},
	{STM8_inc,0x3C,F_LONGPTRW_,F_NONE,4,1,1},
	{STM8_tnz,0x3D,F_LONGPTRW_,F_NONE,4,1,1},
	{STM8_swap,0x3E,F_LONGPTRW_,F_NONE,4,1,1},
	{STM8_clr,0x3F,F_LONGPTRW_,F_NONE,4,1,1},
	{STM8_neg,0x40,F_LONGOFFX,F_NONE,4,1,1},
	{STM8_cpl,0x43,F_LONGOFFX,F_NONE,4,1,1},
	{STM8_srl,0x44,F_LONGOFFX,F_NONE,4,1,1},
	{STM8_rrc,0x46,F_LONGOFFX,F_NONE,4,1,1},
	{STM8_sra,0x47,F_LONGOFFX,F_NONE,4,1,1},
	{STM8_sll,0x48,F_LONGOFFX,F_NONE,4,1,1},
	{STM8_rlc,0x49,F_LONGOFFX,F_NONE,4,1,1},
	{STM8_dec,0x4A,F_LONGOFFX,F_NONE,4,1,1},
	{STM8_inc,0x4C,F_LONGOFFX,F_NONE,4,1,1},
	{STM8_tnz,0x4D,F_LONGOFFX,F_NONE,4,1,1},
	{STM8_swap,0x4E,F_LONGOFFX,F_NONE,4,1,1},
	{STM8_clr,0x4F,F_LONGOFFX,F_NONE,4,1,1},
	{STM8_neg,0x50,F_LONGMEM,F_NONE,4,1,1},
	{STM8_cpl,0x53,F_LONGMEM,F_NONE,4,1,1},
	{STM8_srl,0x54,F_LONGMEM,F_NONE,4,1,1},
	{STM8_rrc,0x56,F_LONGMEM,F_NONE,4,1,1},
	{STM8_sra,0x57,F_LONGMEM,F_NONE,4,1,1},
	{STM8_sll,0x58,F_LONGMEM,F_NONE,4,1,1},
	{STM8_rlc,0x59,F_LONGMEM,F_NONE,4,1,1},
	{STM8_dec,0x5A,F_LONGMEM,F_NONE,4,1,1},
	{STM8_inc,0x5C,F_LONGMEM,F_NONE,4,1,1},
	{STM8_tnz,0x5D,F_LONGMEM,F_NONE,4,1,1},
	{STM8_swap,0x5E,F_LONGMEM,F_NONE,4,1,1},
	{STM8_clr,0x5F,F_LONGMEM,F_NONE,4,1,1},
	{STM8_neg,0x60,F_LONGPTRWX,F_NONE,4,1,1},
	{STM8_cpl,0x63,F_LONGPTRWX,F_NONE,4,1,1},
	{STM8_srl,0x64,F_LONGPTRWX,F_NONE,4,1,1},
	{STM8_rrc,0x66,F_LONGPTRWX,F_NONE,4,1,1},
	{STM8_sra,0x67,F_LONGPTRWX,F_NONE,4,1,1},
	{STM8_sll,0x68,F_LONGPTRWX,F_NONE,4,1,1},
	{STM8_rlc,0x69,F_LONGPTRWX,F_NONE,4,1,1},
	{STM8_dec,0x6A,F_LONGPTRWX,F_NONE,4,1,1},
	{STM8_inc,0x6C,F_LONGPTRWX,F_NONE,4,1,1},
	{STM8_tnz,0x6D,F_LONGPTRWX,F_NONE,4,1,1},
	{STM8_swap,0x6E,F_LONGPTRWX,F_NONE,4,1,1},
	{STM8_clr,0x6F,F_LONGPTRWX,F_NONE,4,1,1},
	{STM8_wfe,0x8F,F_NONE,F_NONE,2,1,1},
	{STM8_subw,0xA2,F_REGY,F_WORD,4,1,1},
	{STM8_addw,0xA9,F_REGY,F_WORD,4,1,1},
	{STM8_subw,0xB0,F_REGX,F_LONGMEM,4,1,1},
	{STM8_subw,0xB2,F_REGY,F_LONGMEM,4,1,1},
	{STM8_addw,0xB9,F_REGY,F_LONGMEM,4,1,1},
	{STM8_addw,0xBB,F_REGX,F_LONGMEM,4,1,1},
	{STM8_sub,0xC0,F_REGA,F_LONGPTRW_,4,1,1},
	{STM8_cp,0xC1,F_REGA,F_LONGPTRW_,4,1,1},
	{STM8_sbc,0xC2,F_REGA,F_LONGPTRW_,4,1,1},
	{STM8_cpw,0xC3,F_REGX,F_LONGPTRW_,4,1,1},
	{STM8_and,0xC4,F_REGA,F_LONGPTRW_,4,1,1},
	{STM8_bcp,0xC5,F_REGA,F_LONGPTRW_,4,1,1},
	{STM8_ld,0xC6,F_REGA,F_LONGPTRW_,4,1,1},
	{STM8_ld,0xC7,F_LONGPTRW_,F_REGA,4,1,1},
	{STM8_xor,0xC8,F_REGA,F_LONGPTRW_,4,1,1},
	{STM8_adc,0xC9,F_REGA,F_LONGPTRW_,4,1,1},
	{STM8_or,0xCA,F_REGA,F_LONGPTRW_,4,1,1},
	{STM8_add,0xCB,F_REGA,F_LONGPTRW_,4,1,1},
	{STM8_jp,0xCC,F_LONGPTRW_,F_NONE,4,1,1},
	{STM8_call,0xCD,F_LONGPTRW_,F_NONE,4,1,1},
	{STM8_ldw,0xCE,F_REGX,F_LONGPTRW_,4,1,1},
	{STM8_ldw,0xCF,F_LONGPTRW_,F_REGY,4,1,1},
	{STM8_sub,0xD0,F_REGA,F_LONGPTRWX,4,1,1},
	{STM8_cp,0xD1,F_REGA,F_LONGPTRWX,4,1,1},
	{STM8_sbc,0xD2,F_REGA,F_LONGPTRWX,4,1,1},
	{STM8_cpw,0xD3,F_REGY,F_LONGPTRWX,4,1,1},
	{STM8_and,0xD4,F_REGA,F_LONGPTRWX,4,1,1},
	{STM8_bcp,0xD5,F_REGA,F_LONGPTRWX,4,1,1},
	{STM8_ld,0xD6,F_REGA,F_LONGPTRWX,4,1,1},
	{STM8_ld,0xD7,F_LONGPTRWX,F_REGA,4,1,1},
	{STM8_xor,0xD8,F_REGA,F_LONGPTRWX,4,1,1},
	{STM8_adc,0xD9,F_REGA,F_LONGPTRWX,4,1,1},
	{STM8_or,0xDA,F_REGA,F_LONGPTRWX,4,1,1},
	{STM8_add,0xDB,F_REGA,F_LONGPTRWX,4,1,1},
	{STM8_jp,0xDC,F_LONGPTRWX,F_NONE,4,1,1},
	{STM8_call,0xDD,F_LONGPTRWX,F_NONE,4,1,1},
	{STM8_ldw,0xDE,F_REGX,F_LONGPTRWX,4,1,1},
	{STM8_ldw,0xDF,F_LONGPTRWX,F_REGY,4,1,1},
	{STM8_subw,0xF0,F_REGX,F_SHORTOFFSP,3,1,1},
	{STM8_subw,0xF2,F_REGY,F_SHORTOFFSP,3,1,1},
	{STM8_addw,0xF9,F_REGY,F_SHORTOFFSP,3,1,1},
	{STM8_addw,0xFB,F_REGX,F_SHORTOFFSP,3,1,1}
};

static const opcode_t pre90[] =
{
	{STM8_rrwa,0x01,F_REGY,F_REGA,2,1,1},
	{STM8_rlwa,0x02,F_REGY,F_NONE,2,1,1},
	{STM8_bcpl,0x10,F_LONGMEM,F_NPOS0,4,1,1},
	{STM8_bccm,0x11,F_LONGMEM,F_NPOS0,4,1,1},
	{STM8_bcpl,0x12,F_LONGMEM,F_NPOS1,4,1,1},
	{STM8_bccm,0x13,F_LONGMEM,F_NPOS1,4,1,1},
	{STM8_bcpl,0x14,F_LONGMEM,F_NPOS2,4,1,1},
	{STM8_bccm,0x15,F_LONGMEM,F_NPOS2,4,1,1},
	{STM8_bcpl,0x16,F_LONGMEM,F_NPOS3,4,1,1},
	{STM8_bccm,0x17,F_LONGMEM,F_NPOS3,4,1,1},
	{STM8_bcpl,0x18,F_LONGMEM,F_NPOS4,4,1,1},
	{STM8_bccm,0x19,F_LONGMEM,F_NPOS4,4,1,1},
	{STM8_bcpl,0x1A,F_LONGMEM,F_NPOS5,4,1,1},
	{STM8_bccm,0x1B,F_LONGMEM,F_NPOS5,4,1,1},
	{STM8_bcpl,0x1C,F_LONGMEM,F_NPOS6,4,1,1},
	{STM8_bccm,0x1D,F_LONGMEM,F_NPOS6,4,1,1},
	{STM8_bcpl,0x1E,F_LONGMEM,F_NPOS7,4,1,1},
	{STM8_bccm,0x1F,F_LONGMEM,F_NPOS7,4,1,1},
	{STM8_jrnh,0x28,F_SHORTMEM,F_NONE,3,1,1},
	{STM8_jrh,0x29,F_SHORTMEM,F_NONE,3,1,1},
	{STM8_jrnm,0x2C,F_SHORTMEM,F_NONE,3,1,1},
	{STM8_jrm,0x2D,F_SHORTMEM,F_NONE,3,1,1},
	{STM8_jril,0x2E,F_SHORTMEM,F_NONE,3,1,1},
	{STM8_jrih,0x2F,F_SHORTMEM,F_NONE,3,1,1},
	{STM8_neg,0x40,F_LONGOFFY,F_NONE,4,1,1},
	{STM8_mul,0x42,F_REGY,F_REGA,2,1,1},
	{STM8_cpl,0x43,F_LONGOFFY,F_NONE,4,1,1},
	{STM8_srl,0x44,F_LONGOFFY,F_NONE,4,1,1},
	{STM8_rrc,0x46,F_LONGOFFY,F_NONE,4,1,1},
	{STM8_sra,0x47,F_LONGOFFY,F_NONE,4,1,1},
	{STM8_sll,0x48,F_LONGOFFY,F_NONE,4,1,1},
	{STM8_rlc,0x49,F_LONGOFFY,F_NONE,4,1,1},
	{STM8_dec,0x4A,F_LONGOFFY,F_NONE,4,1,1},
	{STM8_inc,0x4C,F_LONGOFFY,F_NONE,4,1,1},
	{STM8_tnz,0x4D,F_LONGOFFY,F_NONE,4,1,1},
	{STM8_swap,0x4E,F_LONGOFFY,F_NONE,4,1,1},
	{STM8_clr,0x4F,F_LONGOFFY,F_NONE,4,1,1},
	{STM8_negw,0x50,F_REGY,F_NONE,2,1,1},
	{STM8_cplw,0x53,F_REGY,F_NONE,2,1,1},
	{STM8_srlw,0x54,F_REGY,F_NONE,2,1,1},
	{STM8_rrcw,0x56,F_REGY,F_NONE,2,1,1},
	{STM8_sraw,0x57,F_REGY,F_NONE,2,1,1},
	{STM8_sllw,0x58,F_REGY,F_NONE,2,1,1},
	{STM8_rlcw,0x59,F_REGY,F_NONE,2,1,1},
	{STM8_decw,0x5A,F_REGY,F_NONE,2,1,1},
	{STM8_incw,0x5C,F_REGY,F_NONE,2,1,1},
	{STM8_tnzw,0x5D,F_REGY,F_NONE,2,1,1},
	{STM8_swapw,0x5E,F_REGY,F_NONE,2,1,1},
	{STM8_clrw,0x5F,F_REGY,F_NONE,2,1,1},
	{STM8_neg,0x60,F_SHORTOFFY,F_NONE,3,1,1},
	{STM8_div,0x62,F_REGY,F_NONE,2,1,1},
	{STM8_cpl,0x63,F_SHORTOFFY,F_NONE,3,1,1},
	{STM8_srl,0x64,F_SHORTOFFY,F_NONE,3,1,1},
	{STM8_rrc,0x66,F_SHORTOFFY,F_NONE,3,1,1},
	{STM8_sra,0x67,F_SHORTOFFY,F_NONE,3,1,1},
	{STM8_sll,0x68,F_SHORTOFFY,F_NONE,3,1,1},
	{STM8_rlc,0x69,F_SHORTOFFY,F_NONE,3,1,1},
	{STM8_dec,0x6A,F_SHORTOFFY,F_NONE,3,1,1},
	{STM8_inc,0x6C,F_SHORTOFFY,F_NONE,4,1,1},
	{STM8_tnz,0x6D,F_SHORTOFFY,F_NONE,3,1,1},
	{STM8_swap,0x6E,F_SHORTOFFY,F_NONE,3,1,1},
	{STM8_clr,0x6F,F_SHORTOFFY,F_NONE,3,1,1},
	{STM8_neg,0x70,F_PARY,F_NONE,2,1,1},
	{STM8_cpl,0x73,F_PARY,F_NONE,2,1,1},
	{STM8_srl,0x74,F_PARY,F_NONE,2,1,1},
	{STM8_rrc,0x76,F_PARY,F_NONE,2,1,1},
	{STM8_sra,0x77,F_PARY,F_NONE,2,1,1},
	{STM8_sll,0x78,F_PARY,F_NONE,2,1,1},
	{STM8_rlc,0x79,F_PARY,F_NONE,2,1,1},
	{STM8_dec,0x7A,F_PARY,F_NONE,2,1,1},
	{STM8_inc,0x7C,F_PARY,F_NONE,2,1,1},
	{STM8_tnz,0x7D,F_PARY,F_NONE,2,1,1},
	{STM8_swap,0x7E,F_PARY,F_NONE,2,1,1},
	{STM8_clr,0x7F,F_PARY,F_NONE,2,1,1},
	{STM8_popw,0x85,F_REGY,F_NONE,2,1,1},
	{STM8_pushw,0x89,F_REGY,F_NONE,2,1,1},
	{STM8_ldw,0x93,F_REGY,F_REGX,2,1,1},
	{STM8_ldw,0x94,F_REGSP,F_REGY,2,1,1},
	{STM8_ld,0x95,F_REGHY,F_REGA,2,1,1},
	{STM8_ldw,0x96,F_REGY,F_REGSP,2,1,1},
	{STM8_ld,0x97,F_REGLY,F_REGA,2,1,1},
	{STM8_ld,0x9E,F_REGA,F_REGHY,2,1,1},
	{STM8_ld,0x9F,F_REGA,F_REGLY,2,1,1},
	{STM8_cpw,0xA3,F_REGY,F_WORD,4,1,1},
	{STM8_ldf,0xA7,F_EXTOFFY,F_REGA,5,1,1},
	{STM8_ldw,0xAE,F_REGY,F_WORD,4,1,1},
	{STM8_ldf,0xAF,F_REGA,F_EXTOFFY,5,1,1},
	{STM8_cpw,0xB3,F_REGY,F_SHORTMEM,3,1,1},
	{STM8_ldw,0xBE,F_REGY,F_SHORTMEM,3,1,1},
	{STM8_ldw,0xBF,F_SHORTMEM,F_REGY,3,1,1},
	{STM8_cpw,0xC3,F_REGY,F_LONGMEM,4,1,1},
	{STM8_ldw,0xCE,F_REGY,F_LONGMEM,4,1,1},
	{STM8_ldw,0xCF,F_LONGMEM,F_REGY,4,1,1},
	{STM8_sub,0xD0,F_REGA,F_LONGOFFY,4,1,1},
	{STM8_cp,0xD1,F_REGA,F_LONGOFFY,4,1,1},
	{STM8_sbc,0xD2,F_REGA,F_LONGOFFY,4,1,1},
	{STM8_cpw,0xD3,F_REGX,F_LONGOFFY,4,1,1},
	{STM8_and,0xD4,F_REGA,F_LONGOFFX,4,1,1},
	{STM8_bcp,0xD5,F_REGA,F_LONGOFFY,4,1,1},
	{STM8_ld,0xD6,F_REGA,F_LONGOFFY,4,1,1},
	{STM8_ld,0xD7,F_LONGOFFY,F_REGA,4,1,1},
	{STM8_xor,0xD8,F_REGA,F_LONGOFFY,4,1,1},
	{STM8_adc,0xD9,F_REGA,F_LONGOFFY,4,1,1},
	{STM8_or,0xDA,F_REGA,F_LONGOFFY,4,1,1},
	{STM8_add,0xDB,F_REGA,F_LONGOFFY,4,1,1},
	{STM8_jp,0xDC,F_LONGOFFY,F_NONE,4,1,1},
	{STM8_call,0xDD,F_LONGOFFY,F_NONE,4,1,1},
	{STM8_ldw,0xDE,F_REGY,F_LONGOFFY,4,1,1},
	{STM8_ldw,0xDF,F_LONGOFFY,F_REGX,4,1,1},
	{STM8_sub,0xE0,F_REGA,F_SHORTOFFY,3,1,1},
	{STM8_cp,0xE1,F_REGA,F_SHORTOFFY,3,1,1},
	{STM8_sbc,0xE2,F_REGA,F_SHORTOFFY,3,1,1},
	{STM8_cpw,0xE3,F_REGX,F_SHORTOFFY,3,1,1},
	{STM8_and,0xE4,F_REGA,F_SHORTOFFX,3,1,1},
	{STM8_bcp,0xE5,F_REGA,F_SHORTOFFY,3,1,1},
	{STM8_ld,0xE6,F_REGA,F_SHORTOFFY,3,1,1},
	{STM8_ld,0xE7,F_SHORTOFFY,F_REGA,3,1,1},
	{STM8_xor,0xE8,F_REGA,F_SHORTOFFY,3,1,1},
	{STM8_adc,0xE9,F_REGA,F_SHORTOFFY,3,1,1},
	{STM8_or,0xEA,F_REGA,F_SHORTOFFY,3,1,1},
	{STM8_add,0xEB,F_REGA,F_SHORTOFFY,3,1,1},
	{STM8_jp,0xEC,F_SHORTOFFY,F_NONE,3,1,1},
	{STM8_call,0xED,F_SHORTOFFY,F_NONE,3,1,1},
	{STM8_ldw,0xEE,F_REGY,F_SHORTOFFY,3,1,1},
	{STM8_ldw,0xEF,F_SHORTOFFY,F_REGX,3,1,1},
	{STM8_sub,0xF0,F_REGA,F_PARY,2,1,1},
	{STM8_cp,0xF1,F_REGA,F_PARY,2,1,1},
	{STM8_sbc,0xF2,F_REGA,F_PARY,2,1,1},
	{STM8_cpw,0xF3,F_REGX,F_PARY,2,1,1},
	{STM8_and,0xF4,F_REGA,F_PARX,2,1,1},
	{STM8_bcp,0xF5,F_REGA,F_PARY,2,1,1},
	{STM8_ld,0xF6,F_REGA,F_PARY,2,1,1},
	{STM8_ld,0xF7,F_PARY,F_REGA,2,1,1},
	{STM8_xor,0xF8,F_REGA,F_PARY,2,1,1},
	{STM8_adc,0xF9,F_REGA,F_PARY,2,1,1},
	{STM8_or,0xFA,F_REGA,F_PARY,2,1,1},
	{STM8_add,0xFB,F_REGA,F_PARY,2,1,1},
	{STM8_jp,0xFC,F_PARY,F_NONE,2,1,1},
	{STM8_call,0xFD,F_PARY,F_NONE,2,1,1},
	{STM8_ldw,0xFE,F_REGY,F_PARY,2,1,1},
	{STM8_ldw,0xFF,F_PARY,F_REGX,2,1,1}
};

static const opcode_t pre91[] =
{
	{STM8_neg,0x60,F_SHORTPTRWY,F_NONE,3,1,1},
	{STM8_cpl,0x63,F_SHORTPTRWY,F_NONE,3,1,1},
	{STM8_srl,0x64,F_SHORTPTRWY,F_NONE,3,1,1},
	{STM8_rrc,0x66,F_SHORTPTRWY,F_NONE,3,1,1},
	{STM8_sra,0x67,F_SHORTPTRWY,F_NONE,3,1,1},
	{STM8_sll,0x68,F_SHORTPTRWY,F_NONE,3,1,1},
	{STM8_rlc,0x69,F_SHORTPTRWY,F_NONE,3,1,1},
	{STM8_dec,0x6A,F_SHORTPTRWY,F_NONE,3,1,1},
	{STM8_inc,0x6C,F_SHORTPTRWY,F_NONE,3,1,1},
	{STM8_tnz,0x6D,F_SHORTPTRWY,F_NONE,3,1,1},
	{STM8_swap,0x6E,F_SHORTPTRWY,F_NONE,3,1,1},
	{STM8_clr,0x6F,F_SHORTPTRWY,F_NONE,3,1,1},
	{STM8_ldf,0xA7,F_LONGPTREY,F_REGA,4,1,1},
	{STM8_ldf,0xAF,F_REGA,F_LONGPTREY,4,1,1},
	{STM8_cpw,0xC3,F_REGY,F_SHORTPTRW_,3,1,1},
	{STM8_ldw,0xCE,F_REGY,F_SHORTPTRW_,3,1,1},
	{STM8_ldw,0xCF,F_SHORTPTRW_,F_REGY,3,1,1},
	{STM8_sub,0xD0,F_REGA,F_SHORTPTRWY,3,1,1},
	{STM8_cp,0xD1,F_REGA,F_SHORTPTRWY,3,1,1},
	{STM8_sbc,0xD2,F_REGA,F_SHORTPTRWY,3,1,1},
	{STM8_cpw,0xD3,F_REGX,F_SHORTPTRWY,3,1,1},
	{STM8_and,0xD4,F_REGA,F_SHORTPTRWY,3,1,1},
	{STM8_bcp,0xD5,F_REGA,F_SHORTPTRWY,3,1,1},
	{STM8_ld,0xD6,F_REGA,F_SHORTPTRWY,3,1,1},
	{STM8_ld,0xD7,F_SHORTPTRWY,F_REGA,3,1,1},
	{STM8_xor,0xD8,F_REGA,F_SHORTPTRWY,3,1,1},
	{STM8_adc,0xD9,F_REGA,F_SHORTPTRWY,3,1,1},
	{STM8_or,0xDA,F_REGA,F_SHORTPTRWY,3,1,1},
	{STM8_add,0xDB,F_REGA,F_SHORTPTRWY,3,1,1},
	{STM8_jp,0xDC,F_SHORTPTRWY,F_NONE,3,1,1},
	{STM8_call,0xDD,F_SHORTPTRWY,F_NONE,3,1,1},
	{STM8_ldw,0xDE,F_REGY,F_SHORTPTRWY,3,1,1},
	{STM8_ldw,0xDF,F_SHORTPTRWY,F_REGX,3,1,1}
};

static const opcode_t pre92[] =
{
	{STM8_neg,0x30,F_SHORTPTRW_,F_NONE,3,1,1},
	{STM8_cpl,0x33,F_SHORTPTRW_,F_NONE,3,1,1},
	{STM8_srl,0x34,F_SHORTPTRW_,F_NONE,3,1,1},
	{STM8_rrc,0x36,F_SHORTPTRW_,F_NONE,3,1,1},
	{STM8_sra,0x37,F_SHORTPTRW_,F_NONE,3,1,1},
	{STM8_sll,0x38,F_SHORTPTRW_,F_NONE,3,1,1},
	{STM8_rlc,0x39,F_SHORTPTRW_,F_NONE,3,1,1},
	{STM8_dec,0x3A,F_SHORTPTRW_,F_NONE,3,1,1},
	{STM8_inc,0x3C,F_SHORTPTRW_,F_NONE,3,1,1},
	{STM8_tnz,0x3D,F_SHORTPTRW_,F_NONE,3,1,1},
	{STM8_swap,0x3E,F_SHORTPTRW_,F_NONE,3,1,1},
	{STM8_clr,0x3F,F_SHORTPTRW_,F_NONE,3,1,1},
	{STM8_neg,0x60,F_SHORTPTRWX,F_NONE,3,1,1},
	{STM8_cpl,0x63,F_SHORTPTRWX,F_NONE,3,1,1},
	{STM8_srl,0x64,F_SHORTPTRWX,F_NONE,3,1,1},
	{STM8_rrc,0x66,F_SHORTPTRWX,F_NONE,3,1,1},
	{STM8_sra,0x67,F_SHORTPTRWX,F_NONE,3,1,1},
	{STM8_sll,0x68,F_SHORTPTRWX,F_NONE,3,1,1},
	{STM8_rlc,0x69,F_SHORTPTRWX,F_NONE,3,1,1},
	{STM8_dec,0x6A,F_SHORTPTRWX,F_NONE,3,1,1},
	{STM8_inc,0x6C,F_SHORTPTRWX,F_NONE,3,1,1},
	{STM8_tnz,0x6D,F_SHORTPTRWX,F_NONE,3,1,1},
	{STM8_swap,0x6E,F_SHORTPTRWX,F_NONE,3,1,1},
	{STM8_clr,0x6F,F_SHORTPTRWX,F_NONE,3,1,1},
	{STM8_callf,0x8D,F_LONGPTRE_,F_NONE,4,1,1},
	{STM8_ldf,0xA7,F_LONGPTREX,F_REGA,4,1,1},
	{STM8_jpf,0xAC,F_LONGPTRE_,F_NONE,4,1,1},
	{STM8_ldf,0xAF,F_REGA,F_LONGPTREX,4,1,1},
	{STM8_ldf,0xBC,F_REGA,F_LONGPTRE_,4,1,1},
	{STM8_ldf,0xBD,F_LONGPTRE_,F_REGA,4,1,1},
	{STM8_sub,0xC0,F_REGA,F_SHORTPTRW_,3,1,1},
	{STM8_cp,0xC1,F_REGA,F_SHORTPTRW_,3,1,1},
	{STM8_sbc,0xC2,F_REGA,F_SHORTPTRW_,3,1,1},
	{STM8_cpw,0xC3,F_REGX,F_SHORTPTRW_,3,1,1},
	{STM8_and,0xC4,F_REGA,F_SHORTPTRW_,3,1,1},
	{STM8_bcp,0xC5,F_REGA,F_SHORTPTRW_,3,1,1},
	{STM8_ld,0xC6,F_REGA,F_SHORTPTRW_,3,1,1},
	{STM8_ld,0xC7,F_SHORTPTRW_,F_REGA,3,1,1},
	{STM8_xor,0xC8,F_REGA,F_SHORTPTRW_,3,1,1},
	{STM8_adc,0xC9,F_REGA,F_SHORTPTRW_,3,1,1},
	{STM8_or,0xCA,F_REGA,F_SHORTPTRW_,3,1,1},
	{STM8_add,0xCB,F_REGA,F_SHORTPTRW_,3,1,1},
	{STM8_jp,0xCC,F_SHORTPTRW_,F_NONE,3,1,1},
	{STM8_call,0xCD,F_SHORTPTRW_,F_NONE,3,1,1},
	{STM8_ldw,0xCE,F_REGX,F_SHORTPTRW_,3,1,1},
	{STM8_ldw,0xCF,F_SHORTPTRW_,F_REGY,3,1,1},
	{STM8_sub,0xD0,F_REGA,F_SHORTPTRWX,3,1,1},
	{STM8_cp,0xD1,F_REGA,F_SHORTPTRWX,3,1,1},
	{STM8_sbc,0xD2,F_REGA,F_SHORTPTRWX,3,1,1},
	{STM8_cpw,0xD3,F_REGY,F_SHORTPTRWX,3,1,1},
	{STM8_and,0xD4,F_REGA,F_SHORTPTRWX,3,1,1},
	{STM8_bcp,0xD5,F_REGA,F_SHORTPTRWX,3,1,1},
	{STM8_ld,0xD6,F_REGA,F_SHORTPTRWX,3,1,1},
	{STM8_ld,0xD7,F_SHORTPTRWX,F_REGA,3,1,1},
	{STM8_xor,0xD8,F_REGA,F_SHORTPTRWX,3,1,1},
	{STM8_adc,0xD9,F_REGA,F_SHORTPTRWX,3,1,1},
	{STM8_or,0xDA,F_REGA,F_SHORTPTRWX,3,1,1},
	{STM8_add,0xDB,F_REGA,F_SHORTPTRWX,3,1,1},
	{STM8_jp,0xDC,F_SHORTPTRWX,F_NONE,3,1,1},
	{STM8_call,0xDD,F_SHORTPTRWX,F_NONE,3,1,1},
	{STM8_ldw,0xDE,F_REGX,F_SHORTPTRWX,3,1,1},
	{STM8_ldw,0xDF,F_SHORTPTRWX,F_REGY,3,1,1}
};

//--------------------------------------------------------------------------
uint ReadU24(void)
{
	ushort value = ua_next_byte() << 16;
	value |= ua_next_byte() << 8;
	value |= ua_next_byte();
	return value;
}
ushort ReadU16(void)
{
	ushort value = ua_next_byte() << 8;
	value |= ua_next_byte();
	return value;
}
uchar ReadU8(void)
{
	return ua_next_byte();
}
char ReadS8(void)
{
	uchar value = ua_next_byte();
	return (char)value;
}

//--------------------------------------------------------------------------
static int NT_CDECL cmp_opcodes(const void *a, const void *b)
{
  const opcode_t *x = (const opcode_t *)a;
  const opcode_t *y = (const opcode_t *)b;
  return x->opcode - y->opcode;
}

/*
static uchar find_opcode(uchar code, const opcode_t *table, size_t size)
{
  opcode_t key;
  key.opcode = code;
  opcode_t *op = (opcode_t *)bsearch(&key, table, size, sizeof(opcode_t), cmp_opcodes);
  return op ? op->itype : STM8_null;
}
*/
static opcode_t* find_opcode(uchar code, const opcode_t *table, size_t size)
{
  opcode_t key;
  key.opcode = code;
  opcode_t *op = (opcode_t *)bsearch(&key, table, size, sizeof(opcode_t), cmp_opcodes);
  return op;
  //return op ? op->itype : STM8_null;
}

//--------------------------------------------------------------------------
inline void opextaddr(op_t &x, char dtyp)
{
  x.type = o_near;
  x.dtyp = dtyp;
  x.addr = ReadU24();
}
inline void opextmem(op_t &x, char dtyp)
{
  x.type = o_mem;
  x.dtyp = dtyp;
  x.offb = cmd.size;
  u_int32_t value = ua_next_byte() << 16;
  value |= ua_next_byte() << 8;
  value |= ua_next_byte();
  x.addr = value;
}

inline void opmem(op_t &x, char dtyp)
{
  x.type = o_mem;
  x.dtyp = dtyp;
  x.offb = cmd.size;
  x.addr = ua_next_byte();
}


inline void opaddr(op_t &x, char dtyp)
{

  ulong value = ReadU16();
  x.type = o_near;
  x.dtyp = dtyp;
  x.addr = value;
}
inline void opimm(op_t &x, ulong value, char dtyp)
{
  x.type = o_imm;
  x.dtyp = dtyp;
  x.value = value;
}

inline void oprel(op_t &x)
{
  x.type = o_near;
  x.dtyp = dt_code;
  x.offb = cmd.size;
  long disp = char(ua_next_byte());
  x.addr = cmd.ip + cmd.size + disp;
}

inline void opreg(op_t &x, int reg)
{
  x.type = o_reg;
  x.dtyp = dt_byte;
  x.reg  = reg;
}

inline void opdsp(op_t &x, int reg, char dtyp)
{
  x.type = o_displ;
  x.dtyp = dtyp;
  x.reg  = reg;
  x.offb = cmd.size;
  if (dtyp == dt_byte)
	  x.addr = ua_next_byte();
  else if (dtyp == dt_word)
	  x.addr = ReadU16();
  else if (dtyp == dt_dword)
	  x.addr = ReadU24();
  else
  {
	  interr("opdsp");
	  x.addr = ua_next_byte();
  }
}

inline void oplng(op_t &x)
{
  opmem(x, dt_word);
  if ( (cmd.auxpref & aux_indir) == 0 )
  {
    cmd.auxpref |= aux_16;
    x.dtyp = dt_byte;
    x.addr <<= 8;
    x.addr |= ua_next_byte();
  }
  else
  {
    cmd.auxpref |= aux_long;
  }
}

enum ndx_t { ndx_short, ndx_long, ndx_none };

static void opndx(op_t &x, int type, int index)
{
  switch ( type )
  {
    case ndx_short:
      opmem(x, dt_byte);
      break;
    case ndx_long:
      oplng(x);
      break;
    case ndx_none:
      x.type = o_phrase;
      x.dtyp = dt_byte;
      break;
  }
  if ( index )
  {
    if ( type != ndx_none && (cmd.auxpref & aux_indir) == 0 )
      x.type = o_displ;
    cmd.auxpref |= aux_index;
    x.reg = index;
  }
}

//--------------------------------------------------------------------------
void HandleOp(opcode_t* op, ushort format, uchar which_op)
{
	uchar off = 0;
	ushort uimm = 0;
	char jmp = 0;

	switch(format)
	{
		case F_REGA:
		case F_REGX:
		case F_REGY:
		case F_REGSP:
		case F_REGHX:
		case F_REGHY:
		case F_REGLX:
		case F_REGLY:
		case F_REGCC:
		{
			opreg(cmd.Operands[which_op], format);
		} break;
		case F_BYTE:
		{
			off = ReadU8();
			opimm(cmd.Operands[which_op], off, dt_byte);
		} break;
		case F_WORD:
		{
			uimm = ReadU16();
			opimm(cmd.Operands[which_op], uimm, dt_word);
		} break;
		case F_SHORTMEM:
		{
			if (cmd.itype == STM8_callr)
				oprel(cmd.Operands[which_op]);
			else
				opmem(cmd.Operands[which_op], dt_byte);
		} break;
		case F_LONGMEM:
		{
			if (cmd.itype == STM8_call)
				opaddr(cmd.Operands[which_op], dt_word);
			else
				oplng(cmd.Operands[which_op]);
		} break;
		case F_PARX:
		{
			cmd.Operands[which_op].type = o_phrase;
			cmd.Operands[which_op].dtyp = dt_byte;
			cmd.Operands[which_op].reg = F_REGX;
		} break;
		case F_SHORTOFFX:
		{
			opdsp(cmd.Operands[which_op], F_REGX, dt_byte);
		} break;
		case F_LONGOFFX:
		{
			opdsp(cmd.Operands[which_op], F_REGX, dt_word);
		} break;
		case F_PARY:
		{
			cmd.Operands[which_op].type = o_phrase;
			cmd.Operands[which_op].dtyp = dt_byte;
			cmd.Operands[which_op].reg = F_REGY;
		} break;
		case F_SHORTOFFY:
		{
			opdsp(cmd.Operands[which_op], F_REGY, dt_byte);
		} break;
		case F_LONGOFFY:
		{
			opdsp(cmd.Operands[which_op], F_REGY, dt_word);
		} break;
		case F_SHORTOFFSP:
		{
			opdsp(cmd.Operands[which_op], F_REGSP, dt_byte);
		} break;
		case F_SHORTPTRW_:
		{
			cmd.auxpref |= aux_indir; cmd.auxpref |= aux_long;
			int ndx = ndx_long; int index = F_NONDX;
			opndx(cmd.Operands[which_op], ndx, index);
		} break;
		case F_LONGPTRW_:
		{
			cmd.auxpref |= aux_indir; cmd.auxpref |= aux_long;
			int ndx = ndx_long; int index = F_NONDX;
			opndx(cmd.Operands[which_op], ndx, index);
		} break;
		case F_SHORTPTRWX:
		{
			cmd.auxpref |= aux_indir; cmd.auxpref |= aux_long;
			int ndx = ndx_long; int index = F_REGX;
			opndx(cmd.Operands[which_op], ndx, index);
		} break;
		case F_LONGPTRWX:
		{
			cmd.auxpref |= aux_indir; cmd.auxpref |= aux_long;
			int ndx = ndx_long; int index = F_REGX;
			opndx(cmd.Operands[which_op], ndx, index);
		} break;
		case F_SHORTPTRWY:
		{
			cmd.auxpref |= aux_indir; cmd.auxpref |= aux_long;
			int ndx = ndx_long; int index = F_REGY;
			opndx(cmd.Operands[which_op], ndx, index);
		} break;
		case F_EXTMEM:
		{
			if ((cmd.itype == STM8_int) || (cmd.itype == STM8_callf))
				opextaddr(cmd.Operands[which_op], dt_dword);
			else
				opextmem(cmd.Operands[which_op], dt_dword);
		} break;
		case F_LONGPTRE_:
		{
			cmd.auxpref |= aux_indir; cmd.auxpref |= aux_extd;
			int ndx = ndx_long; int index = F_NONDX;
			opndx(cmd.Operands[which_op], ndx, index);
		} break;
		case F_LONGPTREX:
		{
			cmd.auxpref |= aux_indir; cmd.auxpref |= aux_extd;
			int ndx = ndx_long; int index = F_REGX;
			opndx(cmd.Operands[which_op], ndx, index);
		} break;
		case F_LONGPTREY:
		{
			cmd.auxpref |= aux_indir; cmd.auxpref |= aux_extd;
			int ndx = ndx_long; int index = F_REGY;
			opndx(cmd.Operands[which_op], ndx, index);
		} break;
		case F_EXTOFFX:
		{
			opdsp(cmd.Operands[which_op], F_REGX, dt_dword);
		} break;
		case F_EXTOFFY:
		{
			opdsp(cmd.Operands[which_op], F_REGY, dt_dword);
		} break;
		case F_SHORTJUMP:
		{
			oprel(cmd.Operands[which_op]);
		} break;
		case F_NPOS0:
		case F_NPOS1:
		case F_NPOS2:
		case F_NPOS3:
		case F_NPOS4:
		case F_NPOS5:
		case F_NPOS6:
		case F_NPOS7:
		{
			opimm(cmd.Operands[which_op], format-F_NPOS0, dt_byte);
		} break;
		case F_NONE:
		{
		} break;
		default:
		{
			interr("HandleOp");
		} break;
	}
}

//--------------------------------------------------------------------------
void Insn1(opcode_t* op)
{
	HandleOp(op, op->dst, 0);
	HandleOp(op, op->src, 1);
}
void Insn2(opcode_t* op)
{
	HandleOp(op, op->src, 1);
	HandleOp(op, op->dst, 0);
}
void Insn3(opcode_t* op)
{
	HandleOp(op, op->dst, 0);
	HandleOp(op, op->src, 1);
	HandleOp(op, F_SHORTJUMP, 2);
}

//--------------------------------------------------------------------------
int ana(void)
{
	opcode_t* op = NULL;
	uchar code = ua_next_byte();
	switch ( code )
	{
		case 0x90:
			code = ua_next_byte();
			op = find_opcode(code,pre90, qnumber(pre90));
			cmd.itype = op->itype;
			break;
		case 0x91:
			cmd.auxpref |= aux_indir;
			code = ua_next_byte();
			op = find_opcode(code,pre91, qnumber(pre91));
			cmd.itype = op->itype;
			break;
		case 0x92:
			cmd.auxpref |= aux_indir;
			code = ua_next_byte();
			op = find_opcode(code,pre92, qnumber(pre92));
			cmd.itype = op->itype;
			break;
		case 0x72:
			code = ua_next_byte();
			op = find_opcode(code,pre72, qnumber(pre72));
			cmd.itype = op->itype;
			break;
		default:
			op = (opcode_t*)&primary[code];
			cmd.itype = op->itype;
			break;
	}
	if ( cmd.itype == STM8_null ) return 0;

	switch (op->iform)
	{
		case 1:
		{
			Insn1(op);
		} break;
		case 2:
		{
			Insn2(op);
		} break;
		case 3:
		{
			Insn3(op);
		} break;
		default:
		{
			interr("ana");
		} break;
	}

	if (cmd.size != op->len)
		interr("ana");

	return cmd.size;
}

//--------------------------------------------------------------------------
void interr(const char *module)
{
	const char *name = NULL;
	if ( cmd.itype < ph.instruc_end )
		name = Instructions[cmd.itype].name;
	else
		cmd.itype = ph.instruc_start;
	warning("%a(%s): internal error in %s", cmd.ea, name, module);
}

