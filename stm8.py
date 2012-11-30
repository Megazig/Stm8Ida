from idaapi import *
import struct

NUM_VECTORS = 32
VECTOR_INSN = 0x8200
EA_BASE = 0x9000

def accept_file(li, n):
    print 'stm8 ldr'
    if n > 0:
        return 0
    
    if li.size() < NUM_VECTORS * 4:
        return 0
    
    for vec in range(0, NUM_VECTORS):
        insn, offset = struct.unpack('>HH', li.read(4))
        if insn != VECTOR_INSN:
            return 0
        if offset - EA_BASE > li.size():
            return 0
        
    return 'STMicroelectronics/STM8 flash image'

def load_file(li, neflags, format):
    set_processor_type('stm8', SETPROC_ALL | SETPROC_FATAL)
    
    # TODO figure out how to pass off the rest of the
    # responsibilities to stm8.cfg, or reimplement it here
    
    return True
    