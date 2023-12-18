#ifndef _CPU
#define _CPU

#define CARRY              0b00000001
#define ZERO               0b00000010
#define INTERRUPT_DISABLE  0b00000100
#define DECIMAL_MODE       0b00001000
#define BREAK              0b00010000
#define BREAK2             0b00100000
#define OVERFLOW           0b01000000
#define NEGATIV            0b10000000

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
void and(struct CPU *cpu, enum adressing_mode mode);
void enable_flag(struct CPU *cpu, uint8_t flag);
void disable_flag(struct CPU *cpu, uint8_t flag);
int test_flag(struct CPU *cpu, uint8_t flag);
void asl(struct CPU *cpu, enum adressing_mode mode);
void branch(struct CPU *cpu, bool cond);


#endif