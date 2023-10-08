#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>

struct CPU{
    uint8_t register_a;
    uint8_t status;
    uint16_t pc;
};

struct CPU create_cpu(){
    struct CPU cpu = {
        .register_a = 0,
        .status = 0,
        .pc = 0
        };
    return cpu;
}

void interpret(struct CPU *cpu, uint8_t *program){
    cpu->pc = 0;

    while(1){
        uint8_t opcode = program[cpu->pc];
        cpu->pc += 1;

        switch(opcode){
            case 0x00:
                return;
            case 0xA9:
                uint8_t param = program[cpu->pc];
                cpu->pc += 1;
                cpu->register_a = param;

                if(cpu->register_a == 0)
                    cpu->status |= 0b00000010;
                else
                    cpu->status &= 0b11111101;

                if((cpu->register_a & 0b10000000) != 0)
                    cpu->status |= 0b10000000;
                else
                    cpu->status &= 0b01111111;
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
    interpret(&cpu_1, program_1);
    assert(cpu_1.register_a == 0x05);
    assert((cpu_1.status & 0b00000010) == 0b00);
    assert((cpu_1.status & 0b10000000) == 0);
    printf("PASSED\n");

    printf("test_0xa9_lda_zero_flag: ");
    struct CPU cpu_2 = create_cpu();
    uint8_t program_2[] = {0xa9, 0x00, 0x00};
    interpret(&cpu_2, program_2);
    assert((cpu_2.status & 0b00000010) == 0b00000010);
    printf("PASSED\n");
}