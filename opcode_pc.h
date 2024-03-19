#ifndef _OPCODEPC
#define _OPCODEPC
#include "CPU.h"
#include <stdint.h>

enum _INSTRUCTION{
    ADC = 1,
    LDA,
    TAX,
    BRK,
    STA,
    INX,
    AND,
    ASL,
    BCC,
    BCS,
    BEQ,
    BMI,
    BNE,
    BPL,
    BVC,
    BVS,
	CLC,
    CLD,
    CLI,
    CLV,
    CMP,
    CPX,
    CPY,
};

struct _OPCODE{
    enum _INSTRUCTION instruction; 
    enum adressing_mode mode;
    uint8_t bytes;
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
    /*AND*/
    OPCODE[0x29] = (struct _OPCODE){AND, Immediate, 2};
    OPCODE[0x25] = (struct _OPCODE){AND, ZeroPage, 2};
    OPCODE[0x35] = (struct _OPCODE){AND, ZeroPage_X, 2};
    OPCODE[0x2D] = (struct _OPCODE){AND, Absolute, 3};
    OPCODE[0x3D] = (struct _OPCODE){AND, Absolute_X, 3};
    OPCODE[0x39] = (struct _OPCODE){AND, Absolute_Y, 3};
    OPCODE[0x21] = (struct _OPCODE){AND, Indirect_X, 2};
    OPCODE[0x31] = (struct _OPCODE){AND, Indirect_Y, 2};    
    /*ASL*/
    OPCODE[0x0A] = (struct _OPCODE){ASL, NoneAddressing, 1};
    OPCODE[0x06] = (struct _OPCODE){ASL, ZeroPage, 2};
    OPCODE[0x16] = (struct _OPCODE){ASL, ZeroPage_X, 2};
    OPCODE[0x0E] = (struct _OPCODE){ASL, Absolute, 3};
    OPCODE[0x1E] = (struct _OPCODE){ASL, Absolute_X, 3};   
    /*BCC*/
    OPCODE[0x90] = (struct _OPCODE){BCC, NoneAddressing, 2}; 
    /*BCS*/
    OPCODE[0xB0] = (struct _OPCODE){BCS, NoneAddressing, 2};
    /*BEQ*/
    OPCODE[0xF0] = (struct _OPCODE){BEQ, NoneAddressing, 2};
    /*BMI*/
    OPCODE[0x30] = (struct _OPCODE){BMI, NoneAddressing, 2};
    /*BNE*/
    OPCODE[0xD0] = (struct _OPCODE){BNE, NoneAddressing, 2};
    /*BPL*/
    OPCODE[0x10] = (struct _OPCODE){BPL, NoneAddressing, 2};
    /*BVC*/
    OPCODE[0x50] = (struct _OPCODE){BVC, NoneAddressing, 2};
    /*BVS*/
    OPCODE[0x70] = (struct _OPCODE){BVS, NoneAddressing, 2};
    /*CLC*/
	OPCODE[0x18] = (struct _OPCODE){CLC, NoneAddressing, 2};
    /*CLD*/
    OPCODE[0xD8] = (struct _OPCODE){CLD, NoneAddressing, 2};
    /*CLI*/
    OPCODE[0x58] = (struct _OPCODE){CLI, NoneAddressing, 2};
    /*CLV*/
    OPCODE[0xB8] = (struct _OPCODE){CLV, NoneAddressing, 2};
    /*CMP*/
    OPCODE[0xC9] = (struct _OPCODE){CMP, Immediate, 2};
    OPCODE[0xC5] = (struct _OPCODE){CMP, ZeroPage, 2};
    OPCODE[0xD5] = (struct _OPCODE){CMP, ZeroPage_X, 2};
    OPCODE[0xCD] = (struct _OPCODE){CMP, Absolute, 3};
    OPCODE[0xDD] = (struct _OPCODE){CMP, Absolute_X, 3};
    OPCODE[0xD9] = (struct _OPCODE){CMP, Absolute_Y, 3};
    OPCODE[0xC1] = (struct _OPCODE){CMP, Indirect_X, 2};
    OPCODE[0xD1] = (struct _OPCODE){CMP, Indirect_Y, 2};    
    /*CPX*/
    OPCODE[0xE0] = (struct _OPCODE){CPX, Immediate, 2};    
    OPCODE[0xE4] = (struct _OPCODE){CPX, ZeroPage, 2};
    OPCODE[0xEC] = (struct _OPCODE){CPX, Absolute, 3};    
    /*CPY*/
    OPCODE[0xC0] = (struct _OPCODE){CPY, Immediate, 2};
    OPCODE[0xC4] = (struct _OPCODE){CPY, ZeroPage, 2};
    OPCODE[0xCC] = (struct _OPCODE){CPY, Absolute, 3};   
}

#endif
