#ifndef _CPU
#define _CPU

struct CPU{
    uint8_t register_a;
    uint8_t register_x;
    uint8_t register_y;
    uint8_t status;
    uint16_t pc;
    uint8_t memory[0xFFFF];
};

enum adressing_mode{
    Immediate = 1,
    ZeroPage,
    ZeroPage_X,
    ZeroPage_Y,
    Absolute,
    Absolute_X,
    Absolute_Y,
    Indirect_X,
    Indirect_Y,
    NoneAddressing,
};



struct CPU create_cpu();
int array_size(uint8_t *program);
uint8_t mem_read(struct CPU *cpu, uint16_t addr);
void mem_write(struct CPU *cpu, uint16_t addr, uint8_t data);
void load(struct CPU *cpu, uint8_t *program);
void load_and_run(struct CPU *cpu, uint8_t *program);
void reset_cpu(struct CPU *cpu);
void update_zero_and_negative_flags(struct CPU *cpu, uint8_t result);
void lda(struct CPU *cpu, enum adressing_mode mode);
void interpret(struct CPU *cpu);
uint16_t mem_read_u16(struct CPU *cpu, uint16_t pos);
uint16_t get_operand_address(struct CPU *cpu, enum adressing_mode mode);

#endif