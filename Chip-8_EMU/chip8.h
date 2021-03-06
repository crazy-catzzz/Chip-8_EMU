#pragma once

/*
* INFO:
* char	= 1B -> 8b
* short	= 2B -> 16b
*/

#include <stack>
#include <stdlib.h>
#include <SDL.h>
#include "rom.h"

#define CHIP8_SPEED 700

struct chip8 {
	std::stack<int> stack;		// stack for 16-bit addresses

	unsigned char memory[4096];	// 4KB memory
	int memSize = 4096;

	unsigned short pc;			// program counter
	unsigned short iRegister;	// 16-bit I register

	unsigned char delayTimer;	// 8-bit delay timer
	unsigned char soundTimer;	// 8-bit sound timer

	unsigned char variableRegisters[16];	// 16 8-bit variable registers

	bool display[32 * 64];		// 64x32 display

	unsigned char fontTable[16][5] = {
		{ 0xf0, 0x90, 0x90, 0x90, 0xf0 },	// 0
		{ 0x20, 0x60, 0x20, 0x20, 0x70 },	// 1
		{ 0xF0, 0x10, 0xF0, 0x80, 0xF0 },	// 2
		{ 0xF0, 0x10, 0xF0, 0x10, 0xF0 },	// 3
		{ 0x90, 0x90, 0xF0, 0x10, 0x10 },	// 4
		{ 0xF0, 0x80, 0xF0, 0x10, 0xF0 },	// 5
		{ 0xF0, 0x80, 0xF0, 0x90, 0xF0 },	// 6
		{ 0xF0, 0x10, 0x20, 0x40, 0x40 },	// 7
		{ 0xF0, 0x90, 0xF0, 0x90, 0xF0 },	// 8
		{ 0xF0, 0x90, 0xF0, 0x10, 0xF0 },	// 9
		{ 0xF0, 0x90, 0xF0, 0x90, 0x90 },	// A
		{ 0xE0, 0x90, 0xE0, 0x90, 0xE0 },	// B
		{ 0xF0, 0x80, 0x80, 0x80, 0xF0 },	// C
		{ 0xE0, 0x90, 0x90, 0x90, 0xE0 },	// D
		{ 0xF0, 0x80, 0xF0, 0x80, 0xF0 },	// E
		{ 0xF0, 0x80, 0xF0, 0x80, 0x80 },	// F
	};

	bool pad[4 * 4];

	bool on;
	bool keyPressed;

	/* BACKWARDS COMPATIBILITY */
	int oldStartMem = 512;		// To run older Chip-8 ROMs
};

void chip8_loadROM(chip8& chip8, rom& rom);
unsigned char chip8_mapKey(SDL_Keycode keycode);

// OPCODES
void chip8_clearScr(chip8& chip8);															// 00E0
void chip8_return(chip8& chip8);															// 00EE
void chip8_jmp(chip8& chip8, unsigned short nnn);											// 1NNN
void chip8_call(chip8& chip8, unsigned short nnn);											// 2NNN
void chip8_cmpRegSkip_eq(chip8& chip8, unsigned char x, unsigned char nn);
void chip8_cmpRegSkip_neq(chip8& chip8, unsigned char x, unsigned char nn);
void chip8_cmpRegSkip_eq_XY(chip8& chip8, unsigned char x, unsigned char y);
void chip8_setReg(chip8& chip8, unsigned char x, unsigned char nn);
void chip8_addToReg(chip8& chip8, unsigned char x, unsigned char nn);
void chip8_setToReg(chip8& chip8, unsigned char x, unsigned char y);
void chip8_regOr(chip8& chip8, unsigned char x, unsigned char y);
void chip8_regAnd(chip8& chip8, unsigned char x, unsigned char y);
void chip8_regXor(chip8& chip8, unsigned char x, unsigned char y);
void chip8_regSum(chip8& chip8, unsigned char x, unsigned char y);
void chip8_regDiff_XY(chip8& chip8, unsigned char x, unsigned char y);
void chip8_regDiff_YX(chip8& chip8, unsigned char x, unsigned char y);
void chip8_regShift_right(chip8& chip8, unsigned char x, unsigned char y);
void chip8_regShift_left(chip8& chip8, unsigned char x, unsigned char y);
void chip8_skipNeq(chip8& chip8, unsigned char x, unsigned char y);
void chip8_setIReg(chip8& chip8, unsigned short nnn);
void chip8_jmpV0(chip8& chip8, unsigned char x, unsigned short nnn);
void chip8_rand(chip8& chip8, unsigned char x, unsigned char nn);
void chip8_drawSprite(chip8& chip8, unsigned char x, unsigned char y, unsigned char n);
void chip8_skipKeyPressed(chip8& chip8, unsigned char x);
void chip8_skipKeyNotPressed(chip8& chip8, unsigned char x);
void chip8_setRegToDelayTimer(chip8& chip8, unsigned char x);
void chip8_setDelayTimer(chip8& chip8, unsigned char x);
void chip8_setSoundTimer(chip8& chip8, unsigned char x);
void chip8_addToIReg(chip8& chip8, unsigned char x);
void chip8_waitForKey(chip8& chip8);
void chip8_fontCharacter(chip8& chip8, unsigned char x);
void chip8_toBCD(chip8& chip8, unsigned char x);
void chip8_regToMem(chip8& chip8, unsigned char x);
void chip8_memToReg(chip8& chip8, unsigned char x);
