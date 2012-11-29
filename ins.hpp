#ifndef __INSTRS_HPP
#define __INSTRS_HPP

extern instruc_t Instructions[];

enum nameNum
{
STM8_null = 0,           // Unknown Operation

STM8_adc,       // Add with Carry
STM8_add,       // Addition
STM8_addw,       // Addition Word
STM8_and,       // Logical And
STM8_bccm,      // Copy carry in memory bit
STM8_bcp,       // Bit compare
STM8_bcpl,       // Bit complement
STM8_break,       // Software breakpoint
STM8_bres,       // Bit Reset
STM8_bset,       // Bit Set
STM8_btjf,       // Jump if bit is false
STM8_btjt,       // Jump if bit is true
STM8_call,       // Call subroutine
STM8_callf,       // Call subroutine far
STM8_callr,       // Call subroutine relative
STM8_ccf,       // Complement Carry Flag
STM8_clr,       // Clear
STM8_clrw,       // Clear destination index register
STM8_cp,       // Arithmetic Compare
STM8_cpw,       // Arithmetic Compare Word
STM8_cpl,       // One Complement
STM8_cplw,       // One Complement Word
STM8_dec,       // Decrement
STM8_decw,       // Decrement Word
STM8_div,       // Division
STM8_divw,       // Division Word
STM8_exg,       // Data byte exchange
STM8_exgw,       // Data word exchange
STM8_halt,       // Halt
STM8_inc,       // Increment
STM8_incw,       // Increment Word
STM8_int,       // Interrupt
STM8_iret,       // Interrupt routine return
STM8_jp,       // Absolute Jump
STM8_jpf,       // Absolute Jump far
STM8_jra,       // Jump relative always
//STM8_jrt,       // Jump relative
STM8_jrf,       // Never jump
STM8_jrih,       // Jump if Port INT pin = 1
STM8_jril,       // Jump if Port INT pin = 0
STM8_jrh,       // Jump if H = 1
STM8_jrnh,       // Jump if H = 0
STM8_jrm,       // Jump if I = 1
STM8_jrnm,       // Jump if I = 0
STM8_jrmi,       // Jump if N = 1 (minus)
STM8_jrpl,       // Jump if N = 0 (plus)
STM8_jreq,       // Jump if Z = 1 (equal)
STM8_jrne,       // Jump if Z = 0 (not equal)
STM8_jrc,       // Jump if C = 1
STM8_jrnc,       // Jump if C = 0
STM8_jrnv,        // Jump if V = 0
STM8_jrsge,
STM8_jrsgt,
STM8_jrsle,
STM8_jrslt,
STM8_jrult,       // Jump if C = 1
STM8_jruge,       // Jump if C = 0
STM8_jrugt,       // Jump if (C + Z = 0)
STM8_jrule,       // Jump if (C + Z = 1)
STM8_jrv,        // Jump if V = 1
STM8_ld,       // Load
STM8_ldf,       // Load far
STM8_ldw,       // Load word
STM8_mov,       // Move
STM8_mul,       // Multiply
STM8_neg,       // Negate
STM8_negw,       // Negate word
STM8_nop,       // No Operation
STM8_or,       // OR Operation
STM8_pop,       // Pop from the Stack
STM8_popw,       // Pop word from the Stack
STM8_push,       // Push onto the Stack
STM8_pushw,       // Push word onto the Stack
STM8_rcf,       // Reset carry flag
STM8_ret,       // Subroutine Return
STM8_retf,       // Subroutine Return far
STM8_rim,       // Enable Interrupts
STM8_rlc,       // Rotate left true
STM8_rlcw,       // Rotate left true word
STM8_rlwa,       // Rotate left through accumulator
STM8_rrc,       // Rotate right true
STM8_rrcw,       // Rotate right true word
STM8_rrwa,       // Rotate right through accumulator
STM8_rvf,       // Reset overflow flag
STM8_sbc,       // Subtract with Carry
STM8_scf,       // Set carry flag
STM8_sim,       // Disable Interrupts
STM8_sla,       // Shift left Arithmetic
STM8_slaw,       // Shift left Arithmetic word
STM8_sll,       // Shift left Logic
STM8_sllw,       // Shift left Logic word
STM8_sra,       // Shift right Arithmetic
STM8_sraw,       // Shift right Arithmetic word
STM8_srl,       // Shift right Logic
STM8_srlw,       // Shift right Logic word
STM8_sub,       // Substraction
STM8_subw,       // Substraction word
STM8_swap,       // SWAP nibbles
STM8_swapw,       // SWAP bytes
STM8_tnz,       // Test for Neg & Zero
STM8_tnzw,       // Test for Neg & Zero word
STM8_trap,       // S/W trap
STM8_unknown,   // UNKNOWN 0x71 Instruction
STM8_wfe,       // Wait for Event
STM8_wfi,       // Wait for Interrupt
STM8_xor,       // Exclusive OR

STM8_last,

    };

#endif
