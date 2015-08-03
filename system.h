#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include <stdint.h>

#define SYSTEM_BASE_SP    0xFA0
#define SYSTEM_BASE_PC    0x200

#define SYSTEM_INST_SIZE  sizeof(word_t)

#define REG_VF   regs.V[0xF]  // The VF register doubles as a carry flag

typedef uint8_t reg_t;

// CHIP-8 has 4096 (0x1000) memory locations, each of which is 8 bits (a byte)
// Bytes 0x000-0x1FF: originally reserved for interpreter; now used for font data
// Bytes 0x200-0xE9F: program space
// Bytes 0xEA0-0xEFF: reserved for call stack, internal use, and other variables
// Bytes 0xF00-0xFFF: reserved for display refresh
typedef union ram {
	uint8_t bytes[0x1000];
	struct {
		uint8_t font   [0x200];
		uint8_t program[0xCA0];
		uint8_t stack  [0x60];
		uint8_t display[0x100];
	};
} ram_t;

typedef struct system {
	reg_t     V[0x10];   // CHIP-8 has 16 8-bit data registers named from V0 to VF
	uint16_t  I;         // The address register is used for memory operations
	uint16_t  SP;
	uint16_t  PC;
	uint8_t   delay_timer;
	uint8_t   sound_timer;
	ram_t     ram;
} system_t;

void      system_init    (void);
size_t    system_getSize (void);
uint16_t* system_getRom  (void);
uint16_t  system_getPC   (void);
void      system_setPC   (uint16_t pc);
void      system_setSP   (uint16_t sp);
uint16_t  system_getSP   (void);
reg_t*    system_getReg  (uint8_t reg);
ram_t*    system_ram     (void);

#endif /* __SYSTEM_H__ */