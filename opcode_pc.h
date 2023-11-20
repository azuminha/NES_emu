#ifndef _OPCODEPC
#define _OPCODEPC
#include "CPU.h"

enum _INSTRUCTION{
    ADC = 1,
    LDA,
    TAX,
    BRK,
    STA,
    INX,
};

struct _OPCODE{
    enum _INSTRUCTION instruction; 
    enum adressing_mode mode;
    int bytes;
}OPCODE[0xFF];

void init_opcode_pc(){
    /*LDA*/
    OPCODE[0xA9] = (struct _OPCODE){LDA, Immediate, 2};
    OPCODE[0xA5] = (struct _OPCODE){LDA, ZeroPage, 2};
    OPCODE[0xB5] = (struct _OPCODE){LDA, ZeroPage_X, 2};
    OPCODE[0XAD] = (struct _OPCODE){LDA, Absolute, 3};
    OPCODE[0xBD] = (struct _OPCODE){LDA, Absolute_X, 3};
    OPCODE[0xB9] = (struct _OPCODE){LDA, Absolute_Y, 3};
    OPCODE[0xA1] = (struct _OPCODE){LDA, Indirect_X, 2};
    OPCODE[0xB1] = (struct _OPCODE){LDA, Indirect_Y, 2};
    /*BRK*/
    OPCODE[0x00] = (struct _OPCODE){BRK, NoneAddressing, 1};
    /*TAX*/
    OPCODE[0xAA] = (struct _OPCODE){TAX, NoneAddressing, 1};
    /*STA*/
    OPCODE[0x85] = (struct _OPCODE){STA, ZeroPage, 2}; 
    OPCODE[0x95] = (struct _OPCODE){STA, ZeroPage_X, 2};
    OPCODE[0x8D] = (struct _OPCODE){STA, Absolute, 3};
    OPCODE[0x9D] = (struct _OPCODE){STA, Absolute_X, 3};
    OPCODE[0x99] = (struct _OPCODE){STA, Absolute_Y, 3};
    OPCODE[0x81] = (struct _OPCODE){STA, Indirect_X, 2};
    OPCODE[0x91] = (struct _OPCODE){STA, Indirect_Y, 2};
    /*INX*/   
    OPCODE[0xE8] = (struct _OPCODE){INX, NoneAddressing, 1}; 
}

#endif