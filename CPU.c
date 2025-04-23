#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include "CPU.h"
#include "opcode_pc.h"

#include <raylib.h>

#define STACK 0x0100
#define STACK_RESET 0xfd

struct CPU create_cpu(){
	struct CPU cpu = {
		.register_a = 0,
		.register_x = 0,
		.register_y = 0,
		.status = 0,
		.pc = 0,
		.sp = STACK_RESET
	};
	return cpu;
}

uint8_t stack_pop(struct CPU *cpu){
	cpu->sp += 1;
	uint8_t data = mem_read(cpu, (uint16_t)STACK | (uint16_t)cpu->sp);
	return data;
}

void stack_push(struct CPU *cpu, uint8_t data){
	uint16_t addr = (uint16_t)STACK | (uint16_t)cpu->sp;
	mem_write(cpu, addr, data);
	cpu->sp -= 1;
}

void stack_push_u16(struct CPU *cpu, uint16_t data){
	uint8_t hi = data >> 8;
	uint8_t lo = data & 0xFF;
	stack_push(cpu, hi);
	stack_push(cpu, lo);
}

uint16_t stack_pop_u16(struct CPU *cpu){
	uint16_t lo = (uint16_t)stack_pop(cpu);
	uint16_t hi = (uint16_t)stack_pop(cpu);
	uint16_t data = (hi << 8) | lo;
	return data;
}

uint8_t mem_read(struct CPU *cpu, uint16_t addr){
	return cpu->memory[addr];
}

void mem_write(struct CPU *cpu, uint16_t addr, uint8_t data){
	cpu->memory[addr] = data;
}

uint16_t mem_read_u16(struct CPU *cpu, uint16_t pos){
	//little-endian
	uint16_t lo = (uint16_t)mem_read(cpu, pos);
	uint16_t hi = (uint16_t)mem_read(cpu, pos+1);
	return (hi<<8) | (lo);
}

void mem_write_u16(struct CPU *cpu, uint16_t pos, uint16_t data){
	uint8_t hi = (uint8_t)(data>>8);
	uint8_t lo = (uint8_t)(data & 0x00ff);
	mem_write(cpu, pos, lo);
	mem_write(cpu, pos+1, hi);
}
/*
void load(struct CPU *cpu, uint8_t *program){
	for(int i=0; i < array_size(program); ++i){
		cpu->memory[0x8000+i] = program[i];
	}
	mem_write_u16(cpu, 0xfffc, 0x8000);
}
*/
void load(struct CPU *cpu, uint8_t *program, int size_of_program){
	for(int i=0; i < size_of_program; ++i){
		cpu->memory[0x0600+i] = program[i];
	}
	mem_write_u16(cpu, 0xfffc, 0x0600);
}

void load_and_run(struct CPU *cpu, uint8_t *program, int size_of_program){
	load(cpu, program, size_of_program);
	reset_cpu(cpu);
	interpret(cpu);
}

void reset_cpu(struct CPU *cpu){
	(*cpu).register_a = 0;
	(*cpu).register_x = 0;
	(*cpu).status = 0;
	(*cpu).pc = mem_read_u16(cpu, 0xfffc);
}

uint16_t get_operand_address(struct CPU *cpu, enum adressing_mode mode){
	switch(mode){
		case Immediate:
			return cpu->pc;
		case ZeroPage:
			return (uint16_t)mem_read(cpu, cpu->pc);

		case Absolute:
			return mem_read_u16(cpu, cpu->pc);

		case ZeroPage_X:
			{
				uint8_t pos = mem_read(cpu, cpu->pc);
				uint16_t addr = (pos + cpu->register_x) % 0x100;
				return addr;
			}

		case ZeroPage_Y:
			{
				uint8_t pos = mem_read(cpu, cpu->pc);
				uint16_t addr = (pos + cpu->register_y) % 0x100;
				return addr;
			}

		case Absolute_X:
			{
				uint16_t pos = mem_read_u16(cpu, cpu->pc);
				uint16_t addr = (pos + (uint16_t)cpu->register_x) % 0x10000;
				return addr;
			}

		case Absolute_Y:
			{
				uint16_t pos = mem_read_u16(cpu, cpu->pc);
				uint16_t addr = (pos + (uint16_t)cpu->register_y) % 0x10000;
				return addr;
			}

		case Indirect_X:
			{
				uint8_t pos = mem_read(cpu, cpu->pc);

				uint8_t ptr = (pos + cpu->register_x) % 0x100;
				uint8_t lo  = mem_read(cpu, (uint16_t)ptr);
				uint8_t hi  = mem_read(cpu, (uint16_t)((ptr+1) % 0x100));
				return ((uint16_t)hi << 8) | ((uint16_t)lo);
			}

		case Indirect_Y:
			{
				uint8_t pos = mem_read(cpu, cpu->pc);

				uint8_t lo = mem_read(cpu, (uint16_t)pos);
				uint8_t hi = mem_read(cpu, (uint16_t)((pos+1) % 0x100));
				uint16_t deref_pos = ((uint16_t)hi << 8) | (uint16_t)lo;
				uint16_t deref = (deref_pos + (uint16_t)cpu->register_y) % 0x10000;
				return deref;
			}

		case NoneAddressing:
			exit(1);
			break;
	}
}

void update_zero_and_negative_flags(struct CPU *cpu, uint8_t result){
	if(result == 0)
		enable_flag(cpu, ZERO);//cpu->status |= 0b00000010;
	else
		disable_flag(cpu, ZERO);//cpu->status &= 0b11111101;

	if((result & 0b10000000) != 0)
		cpu->status |= 0b10000000;
	else
		cpu->status &= 0b01111111;
}

void lda(struct CPU *cpu, enum adressing_mode mode/*uint8_t value*/){
	uint16_t addr = get_operand_address(cpu, mode);
	uint8_t value = mem_read(cpu, addr);

	cpu->register_a = value;
	update_zero_and_negative_flags(cpu, cpu->register_a);
	// cpu->register_a = value;
	// update_zero_and_negative_flags(cpu, cpu->register_a);
}

void ldx(struct CPU *cpu, enum adressing_mode mode){
	uint16_t addr = get_operand_address(cpu, mode);
	uint8_t value = mem_read(cpu, addr);

	cpu->register_x = value;
	update_zero_and_negative_flags(cpu, cpu->register_x);
}

void ldy(struct CPU *cpu, enum adressing_mode mode){
	uint16_t addr = get_operand_address(cpu, mode);
	uint8_t value = mem_read(cpu, addr);

	cpu->register_y = value;
	update_zero_and_negative_flags(cpu, cpu->register_y);
}

void dec(struct CPU *cpu, enum adressing_mode mode){
	uint16_t addr = get_operand_address(cpu, mode);
	uint8_t value = mem_read(cpu, addr);

	uint8_t data = value - 1;
	mem_write(cpu, addr, data);
	update_zero_and_negative_flags(cpu, data);
}

void eor(struct CPU *cpu, enum adressing_mode mode){
	uint16_t addr = get_operand_address(cpu, mode);
	uint8_t value = mem_read(cpu, addr);

	cpu->register_a ^= value;
	update_zero_and_negative_flags(cpu, cpu->register_a);
}

void inc(struct CPU *cpu, enum adressing_mode mode){
	uint16_t addr = get_operand_address(cpu, mode);
	uint8_t value = mem_read(cpu, addr);

	uint8_t data = value + 1;
	mem_write(cpu, addr, data);
	update_zero_and_negative_flags(cpu, data);
}

void sta(struct CPU *cpu, enum adressing_mode mode){
	uint16_t addr = get_operand_address(cpu, mode);
	mem_write(cpu, addr, cpu->register_a);
}

void stx(struct CPU *cpu, enum adressing_mode mode){
	uint16_t addr = get_operand_address(cpu, mode);
	mem_write(cpu, addr, cpu->register_x);
}

void sty(struct CPU *cpu, enum adressing_mode mode){
	uint16_t addr = get_operand_address(cpu, mode);
	mem_write(cpu, addr, cpu->register_y);
}

void and(struct CPU *cpu, enum adressing_mode mode){
	uint16_t addr = get_operand_address(cpu, mode);
	uint8_t value = mem_read(cpu, addr);

	cpu->register_a &= value;
	update_zero_and_negative_flags(cpu, cpu->register_a);
}

void enable_flag(struct CPU *cpu, uint8_t flag){
	cpu->status |= flag;
}

void disable_flag(struct CPU *cpu, uint8_t flag){
	cpu->status &= ~flag;
}

int test_flag(struct CPU *cpu, uint8_t flag){
	return (flag & cpu->status) > 0;
}

void branch(struct CPU *cpu, bool cond){
	if(cond){
		int8_t jump = mem_read(cpu, cpu->pc);
		cpu->pc = (cpu->pc + jump + 1) % 0x10000;
	}
}

void asl(struct CPU *cpu, enum adressing_mode mode){
	uint16_t addr = get_operand_address(cpu, mode);
	uint8_t value = mem_read(cpu, addr);

	if(value >> 7 == 1) enable_flag(cpu, CARRY);
	else disable_flag(cpu, CARRY);

	value = value << 1;
	mem_write(cpu, addr, value);
	update_zero_and_negative_flags(cpu, value);
}

void compare(struct CPU *cpu, enum adressing_mode mode, uint8_t comp_val){
	uint16_t addr = get_operand_address(cpu, mode);
	uint8_t value = mem_read(cpu, addr);


	if(comp_val >= value){
		enable_flag(cpu, CARRY);
	}else{
		disable_flag(cpu, CARRY);
	}

	update_zero_and_negative_flags(cpu, (comp_val - value));
}

void lsr(struct CPU *cpu, enum adressing_mode mode){
	uint16_t addr = get_operand_address(cpu, mode);
	uint8_t data = mem_read(cpu, addr);
	if(data & 1){
		enable_flag(cpu, CARRY);
	}else{
		disable_flag(cpu, CARRY);
	}
	data = data >> 1;
	mem_write(cpu, addr, data);	
	update_zero_and_negative_flags(cpu, data);
}

void ora(struct CPU *cpu, enum adressing_mode mode){
	uint16_t addr = get_operand_address(cpu, mode);
	uint8_t value = mem_read(cpu, addr);

	cpu->register_a |= value;
	update_zero_and_negative_flags(cpu, cpu->register_a);
}

void rol(struct CPU *cpu, enum adressing_mode mode){
	if(mode == NoneAddressing){
		int c = test_flag(cpu, CARRY);
		uint8_t value = cpu->register_a;
		if((value >> 7) == 1){
			enable_flag(cpu, CARRY);
		}else{
			disable_flag(cpu, CARRY);
		}

		value = value << 1;
		if(c == 1){
			value |= 1;
		}else{
		}
		cpu->register_a = value;
		update_zero_and_negative_flags(cpu, cpu->register_a);
	}else{
		int c = test_flag(cpu, CARRY);
		uint16_t addr = get_operand_address(cpu, mode);
		uint8_t value = mem_read(cpu, addr);
		if((value >> 7) == 1){
			enable_flag(cpu, CARRY);
		}else{
			disable_flag(cpu, CARRY);
		}
		
		value = value << 1;
		if(c == 1){
			value |= 1;
		}else{
		}
		mem_write(cpu, addr, value);
		update_zero_and_negative_flags(cpu, value);
	}
}

void ror(struct CPU *cpu, enum adressing_mode mode){
	if(mode == NoneAddressing){
		int c = test_flag(cpu, CARRY);
		uint8_t data = cpu->register_a;
		if((data & 1) == 1){
			enable_flag(cpu, CARRY);
		}else{
			disable_flag(cpu, CARRY);
		}

		data = data >> 1;
		if(c == 1){
			data |= 0b10000000;
		}else{
		}

		cpu->register_a = data;
		update_zero_and_negative_flags(cpu, cpu->register_a);
	}else{
		int c = test_flag(cpu, CARRY);
		uint16_t addr = get_operand_address(cpu, mode);
		uint8_t data = mem_read(cpu, addr);
		if((data & 1) == 1){
			enable_flag(cpu, CARRY);
		}else{
			disable_flag(cpu, CARRY);
		}

		data = data >> 1;
		if(c == 1){
			data |= 0b10000000;
		}else{
		}

		mem_write(cpu, addr, data);
		update_zero_and_negative_flags(cpu, data);
	}
}

void adc(struct CPU *cpu, enum adressing_mode mode){
	uint16_t addr = get_operand_address(cpu, mode);
	uint8_t value = mem_read(cpu, addr);
	uint8_t ra = cpu->register_a;
	//sum = value + ra + CARRY;
	uint16_t sum;
	sum = (uint16_t)value + (uint16_t)ra;
	if(test_flag(cpu, CARRY) == 1){
		sum += 1;
	}else{
	}

	bool new_carry = (sum > 0xFF); // will set carry flag in overflow too
	if(new_carry == 1) enable_flag(cpu, CARRY);
	else disable_flag(cpu, CARRY);

	sum &= 0xFF; // sum & 0b11111111
	uint8_t res = (uint8_t)sum;
	// ver se os sinais mudaram
	// ra, value, res
	// ra ^ res -> bit 7 flipou?
	// value ^ res -> bit 7 fliplou?
	// 1 1 0 - 1
	// 0 0 1 - 1
	// 0 0 0 - 0
	// 1 1 1 - 0
	if( (value ^ res) & (ra ^ res) & 0x80 ){
		enable_flag(cpu, OVERFLOW);
	}else{
		disable_flag(cpu, OVERFLOW);
	}

	cpu->register_a = res;
	update_zero_and_negative_flags(cpu, cpu->register_a);
}

void sbc(struct CPU *cpu, enum adressing_mode mode){
	uint16_t addr = get_operand_address(cpu, mode);
	uint8_t value = mem_read(cpu, addr);
	value = ~value + 1;
	uint8_t ra = cpu->register_a;
	//sum = value + ra + CARRY;
	uint16_t sum;
	sum = (uint16_t)value + (uint16_t)ra;
	if(test_flag(cpu, CARRY) == 1){
		sum += !1;
	}else{
	}

	bool new_carry = (sum > 0xFF); // will set carry flag in overflow too
	if(new_carry == 1) enable_flag(cpu, CARRY);
	else disable_flag(cpu, CARRY);

	sum &= 0xFF; // sum & 0b11111111
	uint8_t res = (uint8_t)sum;
	// ver se os sinais mudaram
	// ra, value, res
	// ra ^ res -> bit 7 flipou?
	// value ^ res -> bit 7 fliplou?
	// 1 1 0 - 1
	// 0 0 1 - 1
	// 0 0 0 - 0
	// 1 1 1 - 0
	if( (value ^ res) & (ra ^ res) & 0x80 ){
		enable_flag(cpu, OVERFLOW);
	}else{
		disable_flag(cpu, OVERFLOW);
	}

	cpu->register_a = res;
	update_zero_and_negative_flags(cpu, cpu->register_a);

}
void bit(struct CPU *cpu, enum adressing_mode mode){
	uint16_t addr = get_operand_address(cpu, mode);
	uint8_t value = mem_read(cpu, addr);

	value &= cpu->register_a;
	if(value == 0){
		enable_flag(cpu, ZERO);
	}else{
		disable_flag(cpu, ZERO);
	}

	if((value & 0b01000000) > 0){
		enable_flag(cpu, OVERFLOW);
	}else{
		disable_flag(cpu, OVERFLOW);
	}

	if((value & 0b10000000) > 0){
		enable_flag(cpu, NEGATIV);
	}else{
		disable_flag(cpu, NEGATIV);
	}
}

void interpret(struct CPU *cpu){

	//while(1){
	{
		uint8_t opcode = mem_read(cpu, cpu->pc);
	//	printf("opcode %x\n", opcode);
	//	printf("pc %x\n", cpu->pc);
		cpu->pc += 1;
		uint16_t save_pc = cpu->pc;

		switch(OPCODE[opcode].instruction){
			case BRK:
				return;
			case TAX:
				cpu->register_x = cpu->register_a;
				update_zero_and_negative_flags(cpu, cpu->register_x);
				break;
			case LDA:
				lda(cpu, OPCODE[opcode].mode);
				cpu->pc += OPCODE[opcode].bytes - 1;
				break;
			case STA:
				sta(cpu, OPCODE[opcode].mode);
				cpu->pc += OPCODE[opcode].bytes - 1;
				break;
			case INX:
				cpu->register_x++;
				update_zero_and_negative_flags(cpu, cpu->register_x);
				break;
			case AND:
				and(cpu, OPCODE[opcode].mode);
				cpu->pc += OPCODE[opcode].bytes - 1;
				break;
			case ASL:
				if(OPCODE[opcode].mode == NoneAddressing){
					uint8_t value = cpu->register_a;
					if(value >> 7 == 1) enable_flag(cpu, CARRY);
					else disable_flag(cpu, CARRY);
					cpu->register_a = value << 1;
					update_zero_and_negative_flags(cpu, cpu->register_a);
				}else{
					asl(cpu, OPCODE[opcode].mode);
				}
				cpu->pc += OPCODE[opcode].bytes - 1;
				break;
			case BCC:
				branch(cpu, !test_flag(cpu, CARRY));
				break;
			case BCS:
				branch(cpu, test_flag(cpu, CARRY));
				break;
			case BEQ:
				branch(cpu, test_flag(cpu, ZERO));
				break;
			case BMI:
				branch(cpu, test_flag(cpu, NEGATIV));
				break;
			case BNE:
				branch(cpu, !test_flag(cpu, ZERO));
				break;
			case BPL:
				branch(cpu, !test_flag(cpu, NEGATIV));
				break;
			case BVC:
				branch(cpu, !test_flag(cpu, OVERFLOW));
				break;
			case BVS:
				branch(cpu, test_flag(cpu, OVERFLOW));
				break;
			case CLC:
				disable_flag(cpu, CARRY);
				break;
			case CLD:
				disable_flag(cpu, DECIMAL_MODE);
				break;
			case CLI:
				disable_flag(cpu, INTERRUPT_DISABLE);
				break;
			case CLV:
				disable_flag(cpu, OVERFLOW);
				break;
			case CMP:
				compare(cpu, OPCODE[opcode].mode, cpu->register_a);
				break;
			case CPX:
				compare(cpu, OPCODE[opcode].mode, cpu->register_x);
				break;
			case CPY:
				compare(cpu, OPCODE[opcode].mode, cpu->register_y);
				break;
			case DEC:
				dec(cpu, OPCODE[opcode].mode);
				break;
			case DEX:
				cpu->register_x -= 1;
				update_zero_and_negative_flags(cpu, cpu->register_x);
				break;
			case DEY:
				cpu->register_y -= 1;
				update_zero_and_negative_flags(cpu, cpu->register_y);
			case EOR:
				eor(cpu, OPCODE[opcode].mode);
				break;
			case INC:
				inc(cpu, OPCODE[opcode].mode);
				break;
			case INY:
				cpu->register_y++;
				update_zero_and_negative_flags(cpu, cpu->register_y);
				break;
			case JMP:
				if(OPCODE[opcode].mode == Absolute){
					uint16_t addr = get_operand_address(cpu, Absolute);
					cpu->pc = addr;
				}else{
					/*Indirect -> JMP is the only 6502 instruction to support indirection.*/
					uint16_t addr = mem_read_u16(cpu, cpu->pc);

					uint16_t indirect_ref;
					if((addr & 0x00FF) == 0x00FF){
						uint8_t lo = mem_read(cpu, addr);
						uint8_t hi = mem_read(cpu, addr & 0x00FF);
						indirect_ref = ((uint16_t)hi << 8) | ((uint16_t)lo);
					}else{
						indirect_ref = mem_read_u16(cpu, addr);
					}

					cpu->pc = indirect_ref;
				}
				break;
			case JSR: // + 2 -> JSR uses 3 bytes, but 1 is already added
				stack_push_u16(cpu, cpu->pc + 2 - 1);
				uint16_t new_addr = mem_read_u16(cpu, cpu->pc);
				cpu->pc = new_addr;

				break;
			case LDX:
				ldx(cpu, OPCODE[opcode].mode);
				break;
			case LDY:
				ldy(cpu, OPCODE[opcode].mode);
				break;
			case LSR:
				if(OPCODE[opcode].mode == NoneAddressing){
					uint8_t data = cpu->register_a;
					if(data & 1){
						enable_flag(cpu, CARRY);
					}else{
						disable_flag(cpu, CARRY);
					}
					data = data >> 1;
					cpu->register_a = data;
					update_zero_and_negative_flags(cpu, cpu->register_a);
				}else{
					lsr(cpu, OPCODE[opcode].mode);
				}
				break;
			case NOP:
				break;
			case ORA:
				ora(cpu, OPCODE[opcode].mode);
				break;
			case PHA:
				stack_push(cpu, cpu->register_a);
				break;
			case PHP:
				stack_push(cpu, cpu->status | BREAK | BREAK2);
				break;
			case PLA:
				cpu->register_a = stack_pop(cpu);
				update_zero_and_negative_flags(cpu, cpu->register_a);
				break;
			case PLP:
				{
				uint8_t flags_data = stack_pop(cpu);
				printf("flags pulled: %x\n", flags_data);
				flags_data &= (~BREAK);
				flags_data |= BREAK2;
				printf("flags saved in status: %x\n", flags_data);
				cpu->status = flags_data;
				}
				break;
			case ROL:
				rol(cpu, OPCODE[opcode].mode);
				break;
			case ROR:
				ror(cpu, OPCODE[opcode].mode);
				break;
			case ADC:
				adc(cpu, OPCODE[opcode].mode);
				break;
			case RTI:
				cpu->status = stack_pop(cpu);
				cpu->status &= ~BREAK;
				cpu->status |= BREAK2;
				cpu->pc = stack_pop_u16(cpu);
				break;
			case RTS:
				cpu->pc = stack_pop_u16(cpu) + 1;
				break;
			case SBC:
				sbc(cpu, OPCODE[opcode].mode);
				break;
			case SEC:
				enable_flag(cpu, CARRY);
				break;
			case SED:
				enable_flag(cpu, DECIMAL_MODE);
				break;
			case SEI:
				enable_flag(cpu, INTERRUPT_DISABLE);
				break;
			case TAY:
				cpu->register_y = cpu->register_a;
				update_zero_and_negative_flags(cpu, cpu->register_y);
				break;
			case TSX:
				cpu->register_x = cpu->sp;
				update_zero_and_negative_flags(cpu, cpu->register_x);
				break;
			case TXA:
				cpu->register_a = cpu->register_x;
				update_zero_and_negative_flags(cpu, cpu->register_a);
				break;
			case TXS:
				cpu->sp = cpu->register_x;
				break;
			case TYA:
				cpu->register_a = cpu->register_y;
				update_zero_and_negative_flags(cpu, cpu->register_a);
				break;
			case BIT:
				bit(cpu, OPCODE[opcode].mode);
				break;
			default:
				break;
		}

		if(save_pc == cpu->pc){
			cpu->pc += OPCODE[opcode].bytes - 1;
		}
	}

	return;
}

int test_operands(){
	init_opcode_pc();

	printf("test_0xa9_lda_immediate_load_data: ");
	struct CPU cpu_1 = create_cpu();
	uint8_t program_1[] = {0xa9, 0x05, 0x00};
	load_and_run(&cpu_1, program_1, sizeof(program_1)/sizeof(program_1[0]));
	assert(cpu_1.register_a == 0x05);
	assert((cpu_1.status & 0b00000010) == 0b00);
	assert((cpu_1.status & 0b10000000) == 0);
	printf("PASSED\n");

	printf("test_0xa9_lda_zero_flag: ");
	struct CPU cpu_2 = create_cpu();
	uint8_t program_2[] = {0xa9, 0x00, 0x00};
	load_and_run(&cpu_2, program_2, sizeof(program_2)/sizeof(program_2[0]));
	assert((cpu_2.status & 0b00000010) == 0b00000010);
	printf("PASSED\n");

	printf("text_0xaa_tax_move_a_to_x: ");
	struct CPU cpu_3 = create_cpu();
	uint8_t program_3[] = {0xa9, 10, 0xaa, 0x00};
	load_and_run(&cpu_3, program_3, sizeof(program_3)/sizeof(program_3[0]));
	assert(cpu_3.register_x == 10);
	printf("PASSED\n");

	printf("test_5_ops_working_together: ");
	struct CPU cpu_4 = create_cpu();
	uint8_t program_4[] = {0xa9, 0xc0, 0xaa, 0xe8, 0x00};
	load_and_run(&cpu_4, program_4, sizeof(program_4)/sizeof(program_4[0]));
	assert(cpu_4.register_x == 0xC1);
	printf("PASSED\n");

	printf("test_inx_overflow: ");
	struct CPU cpu_5 = create_cpu();
	uint8_t program_5[] = {0xa9, 0xff, 0xaa, 0xe8, 0xe8, 0x00};
	load_and_run(&cpu_5, program_5, sizeof(program_5)/sizeof(program_5[0]));
	assert(cpu_5.register_x == 1);
	printf("PASSED\n");

	printf("test_lda_from_memory: ");
	struct CPU cpu_6 = create_cpu();
	mem_write(&cpu_6, 0x10, 0x55);
	uint8_t program_6[] = {0xa5, 0x10, 0x00};
	load_and_run(&cpu_6, program_6, sizeof(program_6)/sizeof(program_6[0]));
	assert(cpu_6.register_a == 0x55);
	printf("PASSED\n");

	printf("test_adc: ");
	struct CPU cpu_7 = create_cpu();
	uint8_t program_7[] = {0xa9, 64, 0x69, 64, 0x00};
	load_and_run(&cpu_7, program_7, sizeof(program_7)/sizeof(program_7[0]));
	assert(cpu_7.register_a == 128);
	assert(test_flag(&cpu_7, OVERFLOW) == 1);
	printf("PASSED\n");
	
	printf("test_sbc: ");
	struct CPU cpu_8 = create_cpu();
	uint8_t program_8[] = {0xa9, 0b10000000, 0xe9, 31, 0x00};
	load_and_run(&cpu_8, program_8, sizeof(program_8)/sizeof(program_8[0]));
	assert(cpu_8.register_a == 128-31);
	assert(test_flag(&cpu_8, OVERFLOW) == 1);
	printf("PASSED\n");

	printf("test_sec: ");
	struct CPU cpu_9 = create_cpu();
	uint8_t program_9[] = {0xa9, 10, 0x38, 0xa9, 11, 0xa9, 12, 0x00};
	load_and_run(&cpu_9, program_9, sizeof(program_9)/sizeof(program_9[0]));
	assert(test_flag(&cpu_9, CARRY) == 1);
	assert(cpu_9.register_a == 12);
	printf("PASSED\n");
	
	return 0;
}

int main(){
	//test_operands();
	init_opcode_pc();
	
	struct CPU cpu = create_cpu();
	uint8_t program[] = {
		0x20, 0x06, 0x06, 0x20, 0x38, 0x06, 0x20, 0x0d, 0x06, 0x20, 0x2a, 0x06, 0x60, 0xa9, 0x02, 0x85,
		0x02, 0xa9, 0x04, 0x85, 0x03, 0xa9, 0x11, 0x85, 0x10, 0xa9, 0x10, 0x85, 0x12, 0xa9, 0x0f, 0x85,
		0x14, 0xa9, 0x04, 0x85, 0x11, 0x85, 0x13, 0x85, 0x15, 0x60, 0xa5, 0xfe, 0x85, 0x00, 0xa5, 0xfe,
		0x29, 0x03, 0x18, 0x69, 0x02, 0x85, 0x01, 0x60, 0x20, 0x4d, 0x06, 0x20, 0x8d, 0x06, 0x20, 0xc3,
		0x06, 0x20, 0x19, 0x07, 0x20, 0x20, 0x07, 0x20, 0x2d, 0x07, 0x4c, 0x38, 0x06, 0xa5, 0xff, 0xc9,
		0x77, 0xf0, 0x0d, 0xc9, 0x64, 0xf0, 0x14, 0xc9, 0x73, 0xf0, 0x1b, 0xc9, 0x61, 0xf0, 0x22, 0x60,
		0xa9, 0x04, 0x24, 0x02, 0xd0, 0x26, 0xa9, 0x01, 0x85, 0x02, 0x60, 0xa9, 0x08, 0x24, 0x02, 0xd0,
		0x1b, 0xa9, 0x02, 0x85, 0x02, 0x60, 0xa9, 0x01, 0x24, 0x02, 0xd0, 0x10, 0xa9, 0x04, 0x85, 0x02,
		0x60, 0xa9, 0x02, 0x24, 0x02, 0xd0, 0x05, 0xa9, 0x08, 0x85, 0x02, 0x60, 0x60, 0x20, 0x94, 0x06,
		0x20, 0xa8, 0x06, 0x60, 0xa5, 0x00, 0xc5, 0x10, 0xd0, 0x0d, 0xa5, 0x01, 0xc5, 0x11, 0xd0, 0x07,
		0xe6, 0x03, 0xe6, 0x03, 0x20, 0x2a, 0x06, 0x60, 0xa2, 0x02, 0xb5, 0x10, 0xc5, 0x10, 0xd0, 0x06,
		0xb5, 0x11, 0xc5, 0x11, 0xf0, 0x09, 0xe8, 0xe8, 0xe4, 0x03, 0xf0, 0x06, 0x4c, 0xaa, 0x06, 0x4c,
		0x35, 0x07, 0x60, 0xa6, 0x03, 0xca, 0x8a, 0xb5, 0x10, 0x95, 0x12, 0xca, 0x10, 0xf9, 0xa5, 0x02,
		0x4a, 0xb0, 0x09, 0x4a, 0xb0, 0x19, 0x4a, 0xb0, 0x1f, 0x4a, 0xb0, 0x2f, 0xa5, 0x10, 0x38, 0xe9,
		0x20, 0x85, 0x10, 0x90, 0x01, 0x60, 0xc6, 0x11, 0xa9, 0x01, 0xc5, 0x11, 0xf0, 0x28, 0x60, 0xe6,
		0x10, 0xa9, 0x1f, 0x24, 0x10, 0xf0, 0x1f, 0x60, 0xa5, 0x10, 0x18, 0x69, 0x20, 0x85, 0x10, 0xb0,
		0x01, 0x60, 0xe6, 0x11, 0xa9, 0x06, 0xc5, 0x11, 0xf0, 0x0c, 0x60, 0xc6, 0x10, 0xa5, 0x10, 0x29,
		0x1f, 0xc9, 0x1f, 0xf0, 0x01, 0x60, 0x4c, 0x35, 0x07, 0xa0, 0x00, 0xa5, 0xfe, 0x91, 0x00, 0x60,
		0xa6, 0x03, 0xa9, 0x00, 0x81, 0x10, 0xa2, 0x00, 0xa9, 0x01, 0x81, 0x10, 0x60, 0xa2, 0x00, 0xea,
		0xea, 0xca, 0xd0, 0xfb, 0x60
	};

	int sW, sH;
	sW = sH = 320;
	InitWindow(sW, sH, "snake");
	load(&cpu, program, sizeof(program)/sizeof(program[0]));
	reset_cpu(&cpu);

	mem_write(&cpu, 0xff, 0x77);
	//while(1){
	//	mem_write(&cpu, 0xfe, rand()%15 + 1);
	//	interpret(&cpu);
	//	printf("%d", OPCODE[cpu.memory[cpu.pc]].instruction);
	//	printf("%x\n", cpu.pc);
	//	for(int i=0x0200; i < 0x0600; ++i){
	//		printf("%d ", cpu.memory[i]);
	//		if(i%32 == 0) printf("\n");
	//	}
	//	printf("\n");
	//}
	
	SetTargetFPS(5000);
	while(!WindowShouldClose()){
		if(IsKeyDown(KEY_W)){
			mem_write(&cpu, 0xff, 0x77);
		}
		if(IsKeyDown(KEY_S)){
			mem_write(&cpu, 0xff, 0x73);
		}
		if(IsKeyDown(KEY_A)){
			mem_write(&cpu, 0xff, 0x61);
		}
		if(IsKeyDown(KEY_D)){
			mem_write(&cpu, 0xff, 0x64);
		}
		mem_write(&cpu, 0xfe, rand()%15 + 1);

		interpret(&cpu);

		BeginDrawing();
		{
			int block = 0;
			for(int i=0x0200; i < 0x0600; ++i){
				uint8_t x = cpu.memory[i];
				Color c;
				if(x == 0) c = BLACK;
				if(x == 1) c = WHITE;
				if(x == 2 || x == 9) c = GRAY;
				if(x == 3 || x == 10) c = RED;
				if(x == 4 || x == 11) c = GREEN;
				if(x == 5 || x == 12) c = BLUE;
				if(x == 6 || x == 13) c = MAGENTA;
				if(x == 7 || x == 14) c = YELLOW;
		
				int px, py;
				py = block/32;
				px = block%32;
				DrawRectangle(px * 10, py * 10, 10, 10, c);
				block++;
			}
		}
		EndDrawing();
	}
	return 0;
}
