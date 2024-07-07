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
	DEC,
	DEX,
	DEY,
	EOR,
	INC,
	INY,
	JMP,
	JSR,
	LDX,
	LDY,
	LSR,
	NOP,
	ORA,
	PHA,
	PHP,
	PLA,
	PLP,
	ROL,
	ROR,
	RTI,
	RTS,
	SBC,
	SEC,
	SED,
	SEI,
	STX,
	STY,
	TAY,
	TSX,
	TXA,
	TXS,
	TYA,
	BIT,
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
	OPCODE[0x18] = (struct _OPCODE){CLC, NoneAddressing, 1};
	/*CLD*/
	OPCODE[0xD8] = (struct _OPCODE){CLD, NoneAddressing, 1};
	/*CLI*/
	OPCODE[0x58] = (struct _OPCODE){CLI, NoneAddressing, 1};
	/*CLV*/
	OPCODE[0xB8] = (struct _OPCODE){CLV, NoneAddressing, 1};
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
	/*DEC*/
	OPCODE[0xC6] = (struct _OPCODE){DEC, ZeroPage, 2};
	OPCODE[0xD6] = (struct _OPCODE){DEC, ZeroPage_X, 2};
	OPCODE[0xCE] = (struct _OPCODE){DEC, Absolute, 3};
	OPCODE[0xDE] = (struct _OPCODE){DEC, Absolute_X, 3};
	/*DEX*/
	OPCODE[0xCA] = (struct _OPCODE){DEX, NoneAddressing, 1};
	/*DEY*/
	OPCODE[0x88] = (struct _OPCODE){DEY, NoneAddressing, 1};
	/*EOR*/
	OPCODE[0x49] = (struct _OPCODE){EOR, Immediate, 2};
	OPCODE[0x45] = (struct _OPCODE){EOR, ZeroPage, 2};
	OPCODE[0x55] = (struct _OPCODE){EOR, ZeroPage_X, 2};
	OPCODE[0x4D] = (struct _OPCODE){EOR, Absolute, 3};
	OPCODE[0x5D] = (struct _OPCODE){EOR, Absolute_X, 3};
	OPCODE[0x59] = (struct _OPCODE){EOR, Absolute_Y, 3};
	OPCODE[0x41] = (struct _OPCODE){EOR, Indirect_X, 2};
	OPCODE[0x51] = (struct _OPCODE){EOR, Indirect_Y, 2};
	/*INC*/
	OPCODE[0xE6] = (struct _OPCODE){INC, ZeroPage, 2};
	OPCODE[0xF6] = (struct _OPCODE){INC, ZeroPage_X, 2};
	OPCODE[0xEE] = (struct _OPCODE){INC, Absolute, 3};
	OPCODE[0xFE] = (struct _OPCODE){INC, Absolute_X, 3};
	/*INY*/
	OPCODE[0xC8] = (struct _OPCODE){INY, NoneAddressing, 1};
	/*JMP*/
	OPCODE[0x4C] = (struct _OPCODE){JMP, Absolute, 3};
	OPCODE[0x6C] = (struct _OPCODE){JMP, NoneAddressing, 3};
	/*JSR*/
	OPCODE[0x20] = (struct _OPCODE){JSR, Absolute, 3};
	/*LDX*/
	OPCODE[0xA2] = (struct _OPCODE){LDX, Immediate, 2};
	OPCODE[0xA6] = (struct _OPCODE){LDX, ZeroPage, 2};
	OPCODE[0xB6] = (struct _OPCODE){LDX, ZeroPage_Y, 2};
	OPCODE[0xAE] = (struct _OPCODE){LDX, Absolute, 3};
	OPCODE[0xBE] = (struct _OPCODE){LDX, Absolute_Y, 3};
	/*LDY*/
	OPCODE[0xA0] = (struct _OPCODE){LDY, Immediate, 2};
	OPCODE[0xA4] = (struct _OPCODE){LDY, ZeroPage, 2};
	OPCODE[0xB4] = (struct _OPCODE){LDY, ZeroPage_X, 2};
	OPCODE[0xAC] = (struct _OPCODE){LDY, Absolute, 3};
	OPCODE[0xBC] = (struct _OPCODE){LDY, Absolute_X, 3};
	/*LSR*/
	OPCODE[0x4A] = (struct _OPCODE){LSR, NoneAddressing, 1};
	OPCODE[0x46] = (struct _OPCODE){LSR, ZeroPage, 2};
	OPCODE[0x56] = (struct _OPCODE){LSR, ZeroPage_X, 2};
	OPCODE[0x4E] = (struct _OPCODE){LSR, Absolute, 3};
	OPCODE[0x5E] = (struct _OPCODE){LSR, Absolute_X, 3};
	/*NOP*/
	OPCODE[0xEA] = (struct _OPCODE){NOP, NoneAddressing, 1};
	/*ORA*/
	OPCODE[0x09] = (struct _OPCODE){ORA, Immediate, 2};
	OPCODE[0x05] = (struct _OPCODE){ORA, ZeroPage, 2};
	OPCODE[0x15] = (struct _OPCODE){ORA, ZeroPage_X, 2};
	OPCODE[0x0D] = (struct _OPCODE){ORA, Absolute, 3};
	OPCODE[0x1D] = (struct _OPCODE){ORA, Absolute_X, 3};
	OPCODE[0x19] = (struct _OPCODE){ORA, Absolute_Y, 3};
	OPCODE[0x01] = (struct _OPCODE){ORA, Indirect_X, 2};
	OPCODE[0x11] = (struct _OPCODE){ORA, Indirect_Y, 2};
	/*PHA*/
	OPCODE[0x48] = (struct _OPCODE){PHA, NoneAddressing, 1};
	/*PHP*/
	OPCODE[0x08] = (struct _OPCODE){PHP, NoneAddressing, 1};
	/*PLA*/
	OPCODE[0x68] = (struct _OPCODE){PLA, NoneAddressing, 1};
	/*PLP*/
	OPCODE[0x28] = (struct _OPCODE){PLP, NoneAddressing, 1};
	/*ROL*/
	OPCODE[0x2A] = (struct _OPCODE){ROL, NoneAddressing, 1};
	OPCODE[0x26] = (struct _OPCODE){ROL, ZeroPage, 2};
	OPCODE[0x36] = (struct _OPCODE){ROL, ZeroPage_X, 2};
	OPCODE[0x2E] = (struct _OPCODE){ROL, Absolute, 3};
	OPCODE[0x3E] = (struct _OPCODE){ROL, Absolute_X, 3};
	/*ROR*/
	OPCODE[0x6A] = (struct _OPCODE){ROR, NoneAddressing, 1 };
	OPCODE[0x66] = (struct _OPCODE){ROR, ZeroPage, 2 };
	OPCODE[0x76] = (struct _OPCODE){ROR, ZeroPage_X, 2 };
	OPCODE[0x6E] = (struct _OPCODE){ROR, Absolute, 3 };
	OPCODE[0x7E] = (struct _OPCODE){ROR, Absolute_X, 3 };
	/* ADC */
	OPCODE[0x69] = (struct _OPCODE){ADC, Immediate, 2};
	OPCODE[0x65] = (struct _OPCODE){ADC, ZeroPage, 2};
	OPCODE[0x75] = (struct _OPCODE){ADC, ZeroPage_X, 2};
	OPCODE[0x6D] = (struct _OPCODE){ADC, Absolute, 3}; 
	OPCODE[0x7D] = (struct _OPCODE){ADC, Absolute_X, 3};
	OPCODE[0x79] = (struct _OPCODE){ADC, Absolute_Y, 3};
	OPCODE[0x61] = (struct _OPCODE){ADC, Indirect_X, 2};
	OPCODE[0x71] = (struct _OPCODE){ADC, Indirect_Y, 2};
	/* RTI */
	OPCODE[0x40] = (struct _OPCODE){RTI, NoneAddressing, 1};
	/* RTS */
	OPCODE[0x60] = (struct _OPCODE){RTS, NoneAddressing, 1};
	/* SBC */
	OPCODE[0xE9] = (struct _OPCODE){SBC, Immediate, 2};
	OPCODE[0xE5] = (struct _OPCODE){SBC, ZeroPage, 2};
	OPCODE[0xF5] = (struct _OPCODE){SBC, ZeroPage_X, 2};
	OPCODE[0xED] = (struct _OPCODE){SBC, Absolute, 3};
	OPCODE[0xFD] = (struct _OPCODE){SBC, Absolute_X, 3};
	OPCODE[0xF9] = (struct _OPCODE){SBC, Absolute_Y, 3};
	OPCODE[0xE1] = (struct _OPCODE){SBC, Indirect_X, 2};
	OPCODE[0xF1] = (struct _OPCODE){SBC, Indirect_Y, 2};
	/* SEC */
	OPCODE[0x38] = (struct _OPCODE){SEC, NoneAddressing, 1};
	/* SED */
	OPCODE[0xF8] = (struct _OPCODE){SED, NoneAddressing, 1};
	/* SEI */
	OPCODE[0x78] = (struct _OPCODE){SEI, NoneAddressing, 1};
	/* STX */
	OPCODE[0x86] = (struct _OPCODE){STX, ZeroPage, 2};
	OPCODE[0x96] = (struct _OPCODE){STX, ZeroPage_Y, 2};
	OPCODE[0x8E] = (struct _OPCODE){STX, Absolute, 3};
	/* STY */
	OPCODE[0x84] = (struct _OPCODE){STY, ZeroPage, 2};
	OPCODE[0x94] = (struct _OPCODE){STY, ZeroPage_X, 2};
	OPCODE[0x8C] = (struct _OPCODE){STY, Absolute, 3};
	/* TAY */
	OPCODE[0xA8] = (struct _OPCODE){TAY, NoneAddressing, 1};
	/* TSX */
	OPCODE[0xBA] = (struct _OPCODE){TSX, NoneAddressing, 1};
	/* TXA */
	OPCODE[0x8A] = (struct _OPCODE){TXA, NoneAddressing, 1};
	/* TXS */
	OPCODE[0x9A] = (struct _OPCODE){TXS, NoneAddressing, 1};
	/* TYA */
	OPCODE[0x98] = (struct _OPCODE){TYA, NoneAddressing, 1};
	/* BIT */
	OPCODE[0x24] = (struct _OPCODE){BIT, ZeroPage, 2};
	OPCODE[0x2C] = (struct _OPCODE){BIT, Absolute, 3};
}

#endif
