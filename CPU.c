#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include "CPU.h"
#include "opcode_pc.h"

struct CPU create_cpu(){
    struct CPU cpu = {
        .register_a = 0,
        .register_x = 0,
        .register_y = 0,
        .status = 0,
        .pc = 0
        };
    return cpu;
}

int array_size(uint8_t *program){
    return sizeof(program) / sizeof(program[0]);
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

void load(struct CPU *cpu, uint8_t *program){
    for(int i=0; i < array_size(program); ++i){
        cpu->memory[0x8000+i] = program[i];
    }
    mem_write_u16(cpu, 0xfffc, 0x8000);
}

void load_and_run(struct CPU *cpu, uint8_t *program){
    load(cpu, program);
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

void sta(struct CPU *cpu, enum adressing_mode mode){
    uint16_t addr = get_operand_address(cpu, mode);
    mem_write(cpu, addr, cpu->register_a);
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

void interpret(struct CPU *cpu){

    while(1){
        uint8_t opcode = mem_read(cpu, cpu->pc);
        cpu->pc += 1;

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
            default:
                break;
        }
    }

    return;
}

int main(){
    init_opcode_pc();

    printf("test_0xa9_lda_immediate_load_data: ");
    struct CPU cpu_1 = create_cpu();
    uint8_t program_1[] = {0xa9, 0x05, 0x00};
    load_and_run(&cpu_1, program_1);
    assert(cpu_1.register_a == 0x05);
    assert((cpu_1.status & 0b00000010) == 0b00);
    assert((cpu_1.status & 0b10000000) == 0);
    printf("PASSED\n");

    printf("test_0xa9_lda_zero_flag: ");
    struct CPU cpu_2 = create_cpu();
    uint8_t program_2[] = {0xa9, 0x00, 0x00};
    load_and_run(&cpu_2, program_2);
    assert((cpu_2.status & 0b00000010) == 0b00000010);
    printf("PASSED\n");

    printf("text_0xaa_tax_move_a_to_x: ");
    struct CPU cpu_3 = create_cpu();
    uint8_t program_3[] = {0xa9, 10, 0xaa, 0x00};
    load_and_run(&cpu_3, program_3);
    assert(cpu_3.register_x == 10);
    printf("PASSED\n");

    printf("test_5_ops_working_together: ");
    struct CPU cpu_4 = create_cpu();
    uint8_t program_4[] = {0xa9, 0xc0, 0xaa, 0xe8, 0x00};
    load_and_run(&cpu_4, program_4);
    assert(cpu_4.register_x == 0xC1);
    printf("PASSED\n");

    printf("test_inx_overflow: ");
    struct CPU cpu_5 = create_cpu();
    uint8_t program_5[] = {0xa9, 0xff, 0xaa, 0xe8, 0xe8, 0x00};
    load_and_run(&cpu_5, program_5);
    assert(cpu_5.register_x == 1);
    printf("PASSED\n");

    printf("test_lda_from_memory: ");
    struct CPU cpu_6 = create_cpu();
    mem_write(&cpu_6, 0x10, 0x55);
    uint8_t program_6[] = {0xa5, 0x10, 0x00};
    load_and_run(&cpu_6, program_6);
    assert(cpu_6.register_a == 0x55);
    printf("PASSED\n");
}