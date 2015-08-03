#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <endian.h>
#include <assert.h>
#include "system.h"
#include "errors.h"
#include "op.h"

system_t chip8 = {0};
uint16_t* rom_bin = NULL;
size_t rom_size;

#define BYTES_PER_PC   2

size_t file_getSize(FILE* f)
{
	fseek(f, 0, SEEK_END);
	size_t fsize = ftell(f);
	fseek(f, 0, SEEK_SET);
	return fsize;
}

int file_load(FILE* f, void* buf, size_t size)
{
	fread(buf, 1, size, f);
}

void system_init(void)
{
	memset(&chip8, 0, sizeof(chip8));
	chip8.SP = SYSTEM_BASE_SP;
	chip8.PC = SYSTEM_BASE_PC;
}

int system_load(const char* rom)
{
	FILE* file = fopen(rom, "rb");
	if (!file) {
		printf("ERROR: Couldn't open %s\n", rom);
		return ERR_GENERIC;
	}
	
	size_t file_size = file_getSize(file);
	
	rom_bin = malloc(file_size);
	file_load(file, rom_bin, file_size);
	fclose(file);
	
	rom_size = file_size / BYTES_PER_PC;
	
	int i;
	for (i=0; i<rom_size; i++)
		rom_bin[i] = be16toh(rom_bin[i]);
	
	memcpy(chip8.ram.program, rom_bin, file_size);
	
	for (i=0; i<rom_size; i++)
		assert(*(uint16_t*)&chip8.ram.bytes[0x200 + i*2] == rom_bin[i]);
}

size_t system_getSize(void)
{
	return rom_size;
}

uint16_t* system_getRom(void)
{
	return rom_bin;
}

void system_incPC(void)
{
	chip8.PC += SYSTEM_INST_SIZE;
}

void system_decPC(void)
{
	chip8.PC -= SYSTEM_INST_SIZE;
}

void system_start(bool debug)
{
	uint16_t* rom = system_getRom();
	size_t size   = system_getSize();
	
	int status;
	do {
		word_t op = *(word_t*)&chip8.ram.bytes[chip8.PC];
		dasm_op(chip8.PC, op);
		printf("\n");
		status = op_do(op);
	} while (status == SUCCESS);
}

int system_destroy(void)
{
	free(rom_bin);
	rom_bin = NULL;
}
