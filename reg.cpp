#include "stm8.hpp"
#include <fpro.h>
#include <diskio.hpp>
#include <entry.hpp>

//--------------------------------------------------------------------------
static const char *register_names[] =
{
  "a", "x", "y", "cc", "sp",
  "xl", "xh", "yl", "yh",
  "ds", "cs"
};

//--------------------------------------------------------------------------
static uchar retcode0[] = { 0x80 }; // iret  80
static uchar retcode1[] = { 0x81 }; // ret   81
static bytes_t retcodes[] =
{
 { sizeof(retcode0), retcode0 },
 { sizeof(retcode1), retcode1 },
 { 0, NULL }
};

//-----------------------------------------------------------------------
//      STMicroelectronics - Assembler - rel. 4.10
//      We support Motorola format
//-----------------------------------------------------------------------
static const char *stm8_header[] =
{
  "stm8/",
  "",
  NULL
};

static asm_t stasm =
{
  ASH_HEXF4|    // $1234
  ASD_DECF0|    // 1234
  ASB_BINF2|    // %1010
  ASO_OCTF6|    // ~1234
  AS_NOXRF|     // Disable xrefs during the output file generation
  AS_ONEDUP,    // one array definition per line
  0,
  "STMicroelectronics - Assembler8",
  0,
  stm8_header,   // header lines
  NULL,         // no bad instructions
  "org",        // org
  "end",        // end

  ";",          // comment string
  '\"',         // string delimiter
  '\'',         // char delimiter
  "'\"",        // special symbols in char and string constants

  "dc.b",       // ascii string directive
  "dc.b",       // byte directive
  "dc.w",       // word directive
  "dc.l",       // double words
  NULL,         // qwords
  NULL,         // oword  (16 bytes)
  NULL,         // float  (4 bytes)
  NULL,         // double (8 bytes)
  NULL,         // tbyte  (10/12 bytes)
  NULL,         // packed decimal real
  "skip#s( )#d, #v", // arrays (#h,#d,#v,#s(...)  ONLY BYTE ARRAYS!!!
  "ds.b %s",    // uninited arrays
  "equ",        // equ
  NULL,         // 'seg' prefix (example: push seg seg001)
  NULL,         // Pointer to checkarg_preline() function.
  NULL,         // char *(*checkarg_atomprefix)(char *operand,void *res); // if !NULL, is called before each atom
  NULL,         // const char **checkarg_operations;
  NULL,         // translation to use in character and string constants.
  "*",          // current IP (instruction pointer)
  NULL,         // func_header
  NULL,         // func_footer
  "public",     // "public" name keyword
  NULL,         // "weak"   name keyword
  "extern",     // "extrn"  name keyword
                // .extern directive requires an explicit object size
  NULL,         // "comm" (communal variable)
  NULL,         // get_type_name
  NULL,         // "align" keyword
  '{', '}',     // lbrace, rbrace
  NULL,         // mod
  "and",        // and
  "or",         // or
  "xor",        // xor
  NULL,         // not
  "shl",        // shl
  "shr",        // shr
  NULL,         // sizeof
  AS2_BRACE,
};

static asm_t *asms[] = { &stasm, NULL };

//--------------------------------------------------------------------------
ea_t memstart;
//static char device[MAXSTR] = "";
//static const char cfgname[] = "stm8.cfg";

char device[MAXSTR] = "";
static size_t numports;
static ioport_t *ports = NULL;

#include <iocommon.cpp>

//
//static void load_symbols(void)
//{
//  free_ioports(ports, numports);
//  ports = read_ioports(&numports, cfgname, device, NULL);
//}
//
//----------------------------------------------------------------------
const ioport_t *find_sym(ea_t address)
{
  const ioport_t *port = find_ioport(ports, numports, address);
  return port;
}

//----------------------------------------------------------------------
static void create_words(void)
{
  for ( size_t i=0; i < numports; i++ )
  {
    ea_t ea = ports[i].address;
    if ( isTail(get_flags_novalue(ea)) )
      do_unknown(ea, DOUNK_SIMPLE);
    doWord(ea, 2);
  }
}

//--------------------------------------------------------------------------
const char * idaapi set_idp_options(const char *keyword,int /*value_type*/,const void * /*value*/)
{
  if ( keyword != NULL ) return IDPOPT_BADKEY;
  char cfgfile[QMAXFILE];
  get_cfg_filename(cfgfile, sizeof(cfgfile));
  if ( choose_ioport_device(cfgfile, device, sizeof(device), NULL) )
    set_device_name(device, IORESP_PORT|IORESP_INT);
  return IDPOPT_OK;
}

//--------------------------------------------------------------------------
netnode helper;

static int notify(processor_t::idp_notify msgid, ...)
{
	int retcode = 1;
	va_list va;
	va_start(va, msgid);

	// A well behaving processor module should call invoke_callbacks()
	// in his notify() function. If this function returns 0, then
	// the processor module should process the notification itself
	// Otherwise the code should be returned to the caller:

	int code = invoke_callbacks(HT_IDP, msgid, va);
	if ( code ) return code;

	switch(msgid)
	{
	case processor_t::init:
		helper.create("$ stm8");
		inf.mf = 1;
		break;

	case processor_t::newfile:  // new file loaded
		{
			char cfgfile[QMAXFILE];
			get_cfg_filename(cfgfile, sizeof(cfgfile));
			if ( choose_ioport_device(cfgfile, device, sizeof(device), NULL) )
				set_device_name(device, IORESP_ALL);
			create_words();
		}
		break;

	case processor_t::oldfile:  // old file loaded
		{
			char buf[MAXSTR];
			if ( helper.supval(-1, buf, sizeof(buf)) > 0 )
				set_device_name(buf, IORESP_NONE);
		}
		break;

	case processor_t::is_jump_func:
		{
			const func_t *pfn = va_arg(va, const func_t *);
			ea_t *jump_target = va_arg(va, ea_t *);
			retcode = is_jump_func(pfn, jump_target);
		}
		break;

	case processor_t::is_sane_insn:
		retcode = is_sane_insn(va_arg(va, int));
		break;

	case processor_t::may_be_func:
		// can a function start here?
		// arg: none, the instruction is in 'cmd'
		// returns: probability 0..100
		// 'cmd' structure is filled upon the entrace
		// the idp module is allowed to modify 'cmd'
		retcode = may_be_func();
		break;

	case processor_t::get_autocmt:
		{
			char *buf = va_arg(va, char *);
			size_t bufsize = va_arg(va, size_t);
			if ( qsnprintf(buf, bufsize, "%s", insn_auto_cmts[cmd.itype]) )
				retcode = 2;
		}
		break;
	}

	va_end(va);
	return retcode;
}

//-----------------------------------------------------------------------
static char const *shnames[] = { "stm8", NULL };
static char const *lnames[] = {
  "SGS-Thomson STM8",
  NULL
};

//-----------------------------------------------------------------------
//      Processor Definition
//-----------------------------------------------------------------------
processor_t LPH =
{
  IDP_INTERFACE_VERSION,        // version
  //PLFM_STM8,                    // id
  PLFM_TMS320C1X,               // id
  PRN_HEX|PR_RNAMESOK,
  8,                            // 8 bits in a byte for code segments
  8,                            // 8 bits in a byte for other segments

  (char const **)shnames,
  (char const **)lnames,

  asms,

  notify,

  header,
  footer,

  segstart,
  segend,

  NULL,                 // generate "assume" directives

  ana,                  // analyze instruction
  emu,                  // emulate instruction

  out,                  // generate text representation of instruction
  outop,                // generate ...                    operand
  intel_data,           // generate ...                    data directive
  NULL,                 // compare operands
  NULL,                 // can have type

  qnumber(register_names), // Number of registers
  (char const **)register_names,       // Register names
  NULL,                 // get abstract register

  0,                    // Number of register files
  NULL,                 // Register file names
  NULL,                 // Register descriptions
  NULL,                 // Pointer to CPU registers

  ds,                   // first
  cs,                   // last
  2,                    // size of a segment register
  cs, ds,

  NULL,                 // No known code start sequences
  retcodes,

  STM8_null,
  STM8_last,
  Instructions,

  NULL,                 // int  (*is_far_jump)(int icode);
  NULL,                 // Translation function for offsets
  0,                    // int tbyte_size;  -- doesn't exist
  NULL,                 // int (*realcvt)(void *m, ushort *e, ushort swt);
  { 0, 7, 15, 0 },      // char real_width[4];
                        // number of symbols after decimal point
                        // 2byte float (0-does not exist)
                        // normal float
                        // normal double
                        // long double
  NULL,                 // int (*is_switch)(switch_info_t *si);
  NULL,                 // long (*gen_map_file)(FILE *fp);
  NULL,                 // ulong (*extract_address)(ulong ea,const char *string,int x);
  NULL,                 // int (*is_sp_based)(op_t &x);
  NULL,                 // int (*create_func_frame)(func_t *pfn);
  NULL,                 // int (*get_frame_retsize(func_t *pfn)
  NULL,                 // void (*gen_stkvar_def)(char *buf,const member_t *mptr,long v);
  gen_spcdef,           // Generate text representation of an item in a special segment
  STM8_ret,             // Icode of return instruction. It is ok to give any of possible return instructions
  set_idp_options,      // const char *(*set_idp_options)(const char *keyword,int value_type,const void *value);
  is_align_insn,        // int (*is_align_insn)(ulong ea);
  NULL,                 // mvm_t *mvm;
};
