#ifndef _interface_shared_GlobalEventNumber_h_
#define _interface_shared_GlobalEventNumber_h_


#include <stddef.h>
#include "interface/shared/fed_header.h"

namespace evtn{
    const unsigned int SLINK_WORD_SIZE = 8;
    const unsigned int SLINK_HALFWORD_SIZE = 4;
    
    const unsigned int DAQ_TOTTRG_OFFSET = 2; //offset in 32-bit words
    const unsigned int DAQ_BOARDID_OFFSET = 1;
    const unsigned int DAQ_BOARDID_MASK = 0xffff0000;
    const unsigned int DAQ_BOARDID_SHIFT = 24;
    const unsigned int DAQ_BOARDID_VALUE = 0x11;
    
    enum EvmRecordScheme {
	BST32_3BX = 34 * SLINK_WORD_SIZE, 
	BST32_5BX = 48 * SLINK_WORD_SIZE,
	BST52_3BX = 37 * SLINK_WORD_SIZE,
	BST52_5BX = 51 * SLINK_WORD_SIZE
    }; // EvmRecordsize
    
    const unsigned int EVM_BOARDID_OFFSET = 1;
    const unsigned int EVM_BOARDID_MASK = 0xffff0000;
    const unsigned int EVM_BOARDID_SHIFT = 24;
    const unsigned int EVM_BOARDID_VALUE = 0x11;

    const unsigned int EVM_GTFE_SETUPVERSION_OFFSET = 0; //offset in 32-bit words
    const unsigned int EVM_GTFE_SETUPVERSION_MASK = 0x00000001;
    const unsigned int EVM_GTFE_FDLMODE_OFFSET = 3; //offset in 32-bit words
    const unsigned int EVM_GTFE_FDLMODE_MASK = 0x00000002;
    
    const unsigned int EVM_GTFE_BLOCK_V0000 = 6; //size in 64-bit words
    const unsigned int EVM_GTFE_BLOCK_V0011 = 9; //size in 64-bit words, new format
    const unsigned int EVM_GTFE_BSTGPS_OFFSET = 4; //offset in 32-bit words
    
    const unsigned int EVM_TCS_BLOCK = 5; //size in 64-bit words
    const unsigned int EVM_TCS_BOARDID_OFFSET = 1; //offset in 32-bit words
    const unsigned int EVM_TCS_BOARDID_MASK   = 0xffff0000; // 16 MSB
    const unsigned int EVM_TCS_BOARDID_SHIFT  = 16;
    const unsigned int EVM_TCS_BOARDID_VALUE  = 0xCC07;
    const unsigned int EVM_TCS_TRIGNR_OFFSET  = 5; //offset in 32-bit words
    const unsigned int EVM_TCS_LSBLNR_OFFSET  = 0; //offset in 32-bit words
    const unsigned int EVM_TCS_ORBTNR_OFFSET  = 6; //offset in 32-bit words
    const unsigned int EVM_TCS_LSBLNR_MASK    = 0x0000ffff; // 16 LSB
    const unsigned int EVM_TCS_EVNTYP_MASK    = 0x00f00000; // 4 bits
    const unsigned int EVM_TCS_EVNTYP_SHIFT   = 20; 
    const unsigned int EVM_TCS_BCNRIN_MASK    = 0x00000fff; // 12 LSB
    
    const unsigned int EVM_FDL_BLOCK = 7; //size in 64-bit words
    const unsigned int EVM_FDL_BOARDID_OFFSET = 1; //offset in 32-bit words
    const unsigned int EVM_FDL_BOARDID_MASK = 0xffff0000; // 16 MSB
    const unsigned int EVM_FDL_BOARDID_SHIFT = 16;
    const unsigned int EVM_FDL_BOARDID_VALUE = 0xFD0A;
    const unsigned int EVM_FDL_BCNRINEVT_MASK = 0xf000;
    const unsigned int EVM_FDL_BCNRINEVT_SHIFT = 12;
    const unsigned int EVM_FDL_BCNRIN_OFFSET  = 1; //offset in 32-bit words
    const unsigned int EVM_FDL_TECTRG_OFFSET  = 2; //offset in 32-bit words
    const unsigned int EVM_FDL_ALGOB1_OFFSET  = 4; //offset in 32-bit words
    const unsigned int EVM_FDL_ALGOB2_OFFSET  = 6; //offset in 32-bit words
    const unsigned int EVM_FDL_PSCVSN_OFFSET  = 11; //offset in 32-bit words
    
    extern unsigned int EVM_GTFE_BLOCK;    
    extern unsigned int EVM_FDL_NOBX;
        
    const unsigned int GTPE_BOARDID_OFFSET = 16;
    const unsigned int GTPE_BOARDID_MASK = 0x000000ff;
    const unsigned int GTPE_BOARDID_SHIFT = 0;
    const unsigned int GTPE_BOARDID_VALUE = 0x1;
    const unsigned int GTPE_TRIGNR_OFFSET  = 14; //offset in 32-bit words
    const unsigned int GTPE_ORBTNR_OFFSET  = 18; //offset in 32-bit words
    const unsigned int GTPE_BCNRIN_OFFSET  = 3; //offset in 32-bit words
    const unsigned int GTPE_BCNRIN_MASK    = 0x00000fff; // 12 LSB
    
    unsigned int offset(bool);
    inline bool daq_board_sense(const unsigned char *p)
    {
        return (*(unsigned int*)(p + sizeof(fedh_t) + DAQ_BOARDID_OFFSET * SLINK_WORD_SIZE / 2) >> DAQ_BOARDID_SHIFT) == DAQ_BOARDID_VALUE;
    }
    bool evm_board_sense(const unsigned char *p, size_t size);
    void evm_board_setformat(size_t size);
    bool set_evm_board_sense(const unsigned char *);
    
    inline bool gtpe_board_sense(const unsigned char *p)
    {
        return (*(unsigned int*)(p + GTPE_BOARDID_OFFSET * SLINK_WORD_SIZE / 2) >> GTPE_BOARDID_SHIFT) != 0;
    }
    bool has_evm_tcs(const unsigned char *);
    bool has_evm_fdl(const unsigned char *);

    unsigned int get(const unsigned char *, bool);
    unsigned int gtpe_get(const unsigned char *);
    unsigned int getlbn(const unsigned char *);
    unsigned int gtpe_getlbn(const unsigned char *);
    unsigned int getgpslow(const unsigned char *);
    unsigned int getgpshigh(const unsigned char *);
    unsigned int getorbit(const unsigned char *);
    unsigned int getevtyp(const unsigned char *);
    unsigned int gtpe_getorbit(const unsigned char *);
    unsigned int getfdlbx(const unsigned char *);
    unsigned int getfdlbxevt(const unsigned char *);
    unsigned int gtpe_getbx(const unsigned char *);
    unsigned int getfdlpsc(const unsigned char *);
    unsigned long long getfdlttr(const unsigned char *);
    unsigned long long getfdlta1(const unsigned char *);
    unsigned long long getfdlta2(const unsigned char *);
}

#endif //_interface_shared_GlobalEventNumber_h_
