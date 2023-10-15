#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>

struct CPU{
    uint8_t register_a;
    uint8_t register_x;
    uint8_t status;
    uint16_t pc;
    uint8_t memory[0xFFFF];
};

struct CPU create_cpu();
int array_size(uint8_t *program);
uint8_t mem_read(struct CPU *cpu, uint16_t addr);
void mem_write(struct CPU *cpu, uint16_t addr, uint8_t data);
void load(struct CPU *cpu, uint8_t *program);
void load_and_run(struct CPU *cpu, uint8_t *program);
void reset_cpu(struct CPU *cpu);
void update_zero_and_negative_flags(struct CPU *cpu, uint8_t result);
void lda(struct CPU *cpu, uint8_t value);
void interpret(struct CPU *cpu);
uint16_t mem_read_u16(struct CPU *cpu, uint16_t pos);

struct CPU create_cpu(){
    struct CPU cpu = {
        .register_a = 0,
        .register_x = 0,
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

void update_zero_and_negative_flags(struct CPU *cpu, uint8_t result){
    if(result == 0)
        cpu->status |= 0b00000010;
    else
        cpu->status &= 0b11111101;

    if((result & 0b10000000) != 0)
        cpu->status |= 0b10000000;
    else
        cpu->status &= 0b01111111;
}

void lda(struct CPU *cpu, uint8_t value){
    cpu->register_a = value;
    update_zero_and_negative_flags(cpu, cpu->register_a);
}

void interpret(struct CPU *cpu){

    while(1){
        uint8_t opcode = mem_read(cpu, cpu->pc);
        cpu->pc += 1;

        switch(opcode){
            case 0x00:
                return;
            case 0xAA:
                cpu->register_x = cpu->register_a;
                update_zero_and_negative_flags(cpu, cpu->register_x);

                break;
            case 0xA9:
                uint8_t param = mem_read(cpu, cpu->pc);
                cpu->pc += 1;
                lda(cpu, param);

                break;
            case 0xE8:
                cpu->register_x++;
                update_zero_and_negative_flags(cpu, cpu->register_x);

                break;
            default:
                break;
        }
    }

    return;
}

int main(){
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
}