#include "stm8.hpp"
//#include <frame.hpp>

static int flow;
//------------------------------------------------------------------------
static void process_immediate_number(int n)
{
  doImmd(cmd.ea);
  if ( isDefArg(uFlag,n) ) return;
  switch ( cmd.itype )
  {
    case STM8_bres:
    case STM8_bset:
    case STM8_btjf:
    case STM8_btjt:
      op_dec(cmd.ea, n);
      break;
  }
}

//----------------------------------------------------------------------
ea_t calc_mem(ea_t ea)
{
  return toEA(cmd.cs, ea);
}

//----------------------------------------------------------------------
static void process_operand(op_t &x, bool isload)
{
  ea_t ea;
  switch ( x.type )
  {
    case o_reg:
    case o_phrase:
      break;

    case o_imm:
      if ( !isload ) interr("emu");
      process_immediate_number(x.n);
      if ( isOff(uFlag, x.n) )
        ua_add_off_drefs2(x, dr_O, 0);
      break;

    case o_mem:
      if ( !is_forced_operand(cmd.ea, x.n) )
      {
        ea = calc_mem(x.addr);
        ua_dodata2(x.offb, ea, x.dtyp);
        dref_t dref = isload || (cmd.auxpref & aux_indir) ? dr_R : dr_W;
        ua_add_dref(x.offb, ea, dref);
      }
      break;

    case o_near:
      {
        cref_t ftype = fl_JN;
        ea = calc_mem(x.addr);
        if ( InstrIsSet(cmd.itype, CF_CALL) )
        {
          if ( !func_does_return(ea) )
            flow = false;
          ftype = fl_CN;
        }
        ua_add_cref(x.offb, ea, ftype);
      }
      break;

    case o_displ:
      process_immediate_number(x.n);
      if ( isOff(uFlag, x.n) && !is_forced_operand(cmd.ea, x.n) )
        ua_add_off_drefs2(x, isload ? dr_R : dr_W, OOF_ADDR);
      break;

    default:
      interr("emu");
  }
}


//----------------------------------------------------------------------
int idaapi emu(void)
{
  uint32_t Feature = cmd.get_canon_feature();

  flow = ((Feature & CF_STOP) == 0);

  if ( Feature & CF_USE1 ) process_operand(cmd.Op1, true);
  if ( Feature & CF_USE2 ) process_operand(cmd.Op2, true);
  if ( Feature & CF_USE3 ) process_operand(cmd.Op3, true);
  if ( Feature & CF_CHG1 ) process_operand(cmd.Op1, false);
  if ( Feature & CF_CHG2 ) process_operand(cmd.Op2, false);
  if ( Feature & CF_CHG3 ) process_operand(cmd.Op3, false);

//
//      Determine if the next instruction should be executed
//
  if ( segtype(cmd.ea) == SEG_XTRN ) flow = 0;
  if ( flow ) ua_add_cref(0,cmd.ea+cmd.size,fl_F);

  return 1;
}

//----------------------------------------------------------------------
int is_jump_func(const func_t * /*pfn*/, ea_t *jump_target)
{
  *jump_target = BADADDR;
  return 1; // means "no"
}

//----------------------------------------------------------------------
int may_be_func(void)           // can a function start here?
                                // arg: none, the instruction is in 'cmd'
                                // returns: probability 0..100
                                // 'cmd' structure is filled upon the entrace
                                // the idp module is allowed to modify 'cmd'
{
//  if ( cmd.itype == H8_push && isbp(cmd.Op1.reg) ) return 100;  // push.l er6
  return 0;
}

//----------------------------------------------------------------------
int is_sane_insn(int /*nocrefs*/)
{
  if ( cmd.itype == STM8_nop )
  {
    for ( int i=0; i < 8; i++ )
      if ( get_word(cmd.ea-i*2) != 0 ) return 1;
    return 0; // too many nops in a row
  }
  return 1;
}

//----------------------------------------------------------------------
// Known STM8 switch idioms:
//  Switch statements are straightforward:
//     cp      a, #$E        ; Arithmetic Compare
//     jrnc    out_of_bounds ; Jump if C = 0
//     clrw    x             ; x = 0
//     ld      xl, a         ; x |= a & 0xff
//     sllw    x             ; index = x * sizeof(u16)
//     ldw     x, ($F693,x)  ; x = *(0xf693 + index)
//     jp      (x)           ; Absolute Jump
//   out_of_bounds:
//     jra     return        ; Jump relative always
//  Indirect calls are less code but harder to resolve,
//  as they lookup the target address from memory that is
//  modified during runtime.
//     ld      a, byte_1E0   ; a = index
//     clrw    y             ; y = 0
//     ld      yl, a         ; y |= a & 0xff
//     sllw    y             ; y *= sizeof(u16)
//     ldw     y, (7,y)      ; t = *(7 + y) // data at 7 is changed at runtime
//     call    (y)           ; Call subroutine
#include <jptcmn.cpp>

class stm8_jump_pattern_t : public jump_pattern_t
{
public:
	stm8_jump_pattern_t(switch_info_ex_t &si)
		: jump_pattern_t(roots, depends, si)
		, indirect_register(-1)
		, index_register(-1)
	{
	}

	static char const roots[];
	static char const depends[][2];

	int indirect_register;
	int index_register;

	bool jpi0()
	{
		bool valid = cmd.itype == STM8_jp;
		if (valid)
			indirect_register = cmd.Op1.reg;
		return valid;
	}
	bool jpi1()
	{
		bool valid = cmd.itype == STM8_ldw && cmd.Op1.reg == indirect_register;
		if (valid)
			si.jumps = cmd.Op2.addr;
		return valid;
	}
	bool jpi2()
	{
		return cmd.itype == STM8_sllw && cmd.Op1.reg == indirect_register;
	}
	bool jpi3()
	{
		uint16 dst_reg = cmd.Op1.reg;
		if ((indirect_register == regnum_t::X) && (dst_reg != regnum_t::XL))
			return false;
		else if ((indirect_register == regnum_t::Y) && (dst_reg != regnum_t::YL))
			return false;

		index_register = cmd.Op2.reg;

		return cmd.itype == STM8_ld;
	}
	bool jpi4()
	{
		return cmd.itype == STM8_clrw && cmd.Op1.reg == indirect_register;
	}
	bool jpi6()
	{
		bool valid = cmd.itype == STM8_cp && cmd.Op1.reg == index_register;
		if (valid)
			si.ncases = cmd.Op2.value;
		return valid;
	}
};

char const stm8_jump_pattern_t::roots[] = { 1 };
char const stm8_jump_pattern_t::depends[][2] =
{
	{ 1 },    // jp      (x)
	{ 2 },    // ldw     x, ($F693,x)
	{ 3 },    // sllw    x
	{ 4, 6 }, // ld      xl, a
	{ 0 },    // clrw    x
	{ 0 },    // jrnc    out_of_bounds
	{ 0 }     // cp      a, #$E
};

bool idaapi is_switch(switch_info_ex_t *si)
{
	return stm8_jump_pattern_t(*si).match(cmd.ea);
}

//----------------------------------------------------------------------
int idaapi is_align_insn(ea_t ea)
{
  if ( !decode_insn(ea) ) return 0;
  switch ( cmd.itype )
  {
    case STM8_nop:
      break;
    default:
      return 0;
  }
  return cmd.size;
}

