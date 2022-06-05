#include "chip8.h"

#include <stdlib.h>
#include <iostream>
#include "rom.h"

using namespace std;

void chip8_clearScr(chip8& chip8) {
	for (int i = 0; i < 64; i++) {
		for (int j = 0; j < 32; j++) {
			chip8.display[j * 64 + i] = false;
		}
		
	}
}

void chip8_return(chip8& chip8) {
	chip8.pc = chip8.stack.top();
	chip8.stack.pop();
}

void chip8_jmp(chip8& chip8, unsigned short nnn) {
	chip8.pc = nnn;	
}

void chip8_call(chip8& chip8, unsigned short nnn) {
	chip8.stack.push(chip8.pc);
	chip8.pc = nnn;
}

void chip8_cmpRegSkip_eq(chip8& chip8, unsigned char x, unsigned char nn) {
	if (chip8.variableRegisters[x] == nn) chip8.pc += 2;
}

void chip8_cmpRegSkip_neq(chip8& chip8, unsigned char x, unsigned char nn) {
	if (chip8.variableRegisters[x] != nn) chip8.pc += 2;
}

void chip8_cmpRegSkip_eq_XY(chip8& chip8, unsigned char x, unsigned char y) {
	if (chip8.variableRegisters[x] == chip8.variableRegisters[y]) chip8.pc += 2;
}

void chip8_setReg(chip8& chip8, unsigned char x, unsigned char nn) {
	chip8.variableRegisters[x] = nn;
}

void chip8_addToReg(chip8& chip8, unsigned char x, unsigned char nn) {
	chip8.variableRegisters[x] += nn;
}

void chip8_setToReg(chip8& chip8, unsigned char x, unsigned char y) {
	chip8.variableRegisters[x] = chip8.variableRegisters[y];
}

void chip8_regOr(chip8& chip8, unsigned char x, unsigned char y) {
	chip8.variableRegisters[x] |= chip8.variableRegisters[y];
}

void chip8_regAnd(chip8& chip8, unsigned char x, unsigned char y) {
	chip8.variableRegisters[x] &= chip8.variableRegisters[y];
}

void chip8_regXor(chip8& chip8, unsigned char x, unsigned char y) {
	chip8.variableRegisters[x] ^= chip8.variableRegisters[y];
}

void chip8_regSum(chip8& chip8, unsigned char x, unsigned char y) {
	chip8.variableRegisters[x] += chip8.variableRegisters[y];
}

void chip8_regDiff_XY(chip8& chip8, unsigned char x, unsigned char y) {
	if (chip8.variableRegisters[x] > chip8.variableRegisters[y]) chip8.variableRegisters[0xF] = 0b1;
	else chip8.variableRegisters[0xF] = 0b0;
	chip8.variableRegisters[x] -= chip8.variableRegisters[y];
}

void chip8_regDiff_YX(chip8& chip8, unsigned char x, unsigned char y) {
	if (chip8.variableRegisters[x] > chip8.variableRegisters[y]) chip8.variableRegisters[0xF] = 0b1;
	else chip8.variableRegisters[0xF] = 0b0;
	chip8.variableRegisters[x] = chip8.variableRegisters[x] - chip8.variableRegisters[y];
}

void chip8_regShift_right(chip8& chip8, unsigned char x, unsigned char y) {
	//chip8.variableRegisters[x] = chip8.variableRegisters[y];
	chip8.variableRegisters[0xF] = (chip8.variableRegisters[x] & 0b00000001);
	chip8.variableRegisters[x] >>= 1;
}

void chip8_regShift_left(chip8& chip8, unsigned char x, unsigned char y) {
	//chip8.variableRegisters[x] = chip8.variableRegisters[y];
	chip8.variableRegisters[0xF] = (chip8.variableRegisters[x] & 0b10000000) >> 7;
	chip8.variableRegisters[x] <<= 1;
}

void chip8_skipNeq(chip8& chip8, unsigned char x, unsigned char y) {
	if (chip8.variableRegisters[x] != chip8.variableRegisters[y]) chip8.pc += 2;
}

void chip8_setIReg(chip8& chip8, unsigned short nnn) {
	chip8.iRegister = nnn;
}

void chip8_jmpV0(chip8& chip8, unsigned char x, unsigned short nnn) {
	chip8.pc = nnn + chip8.variableRegisters[x];
}

void chip8_rand(chip8& chip8, unsigned char x, unsigned char nn) {
	chip8.variableRegisters[x] = (rand() % 255) & nn;
}

void chip8_drawSprite(chip8& chip8, unsigned char x, unsigned char y, unsigned char n) {
	unsigned char xCoord = chip8.variableRegisters[x] % 64;
	unsigned char yCoord = chip8.variableRegisters[y] % 32;

	chip8.variableRegisters[0xF] = 0x00;

	for (int i = 0; i < n; i++) {
		unsigned char spriteData = chip8.memory[chip8.iRegister + i];
		if (yCoord < 32) {
			xCoord = chip8.variableRegisters[x] % 64;
			for (unsigned int j = 1 << 7; j != 0; j >>= 1) {
				if (xCoord < 64) {
					if (spriteData & j && chip8.display[yCoord * 64 + xCoord] == true) {
						chip8.display[yCoord * 64 + xCoord] = 0b0;
						chip8.variableRegisters[0xF] = true;
					}
					else if (spriteData & j && chip8.display[yCoord * 64 + xCoord] == false) {
						chip8.display[yCoord * 64 + xCoord] = 0b1;
					}
					xCoord++;
				}
			}
			yCoord++;
		}
	}
}

void chip8_skipKeyPressed(chip8& chip8, unsigned char x) {
	if (chip8.pad[chip8.variableRegisters[x]] == true) chip8.pc += 2;
}

void chip8_skipKeyNotPressed(chip8& chip8, unsigned char x) {
	if (chip8.pad[chip8.variableRegisters[x]] == false) chip8.pc += 2;
}

void chip8_setRegToDelayTimer(chip8& chip8, unsigned char x) {
	chip8.variableRegisters[x] = chip8.delayTimer;
}

void chip8_setDelayTimer(chip8& chip8, unsigned char x) {
	chip8.delayTimer = chip8.variableRegisters[x];
}

void chip8_setSoundTimer(chip8& chip8, unsigned char x) {
	chip8.soundTimer = chip8.variableRegisters[x];
}

void chip8_addToIReg(chip8& chip8, unsigned char x) {
	chip8.iRegister += chip8.variableRegisters[x];
}

void chip8_waitForKey(chip8& chip8) {
	chip8.pc -= 2;
	if (chip8.keyPressed == true) chip8.pc += 2;
}

void chip8_fontCharacter(chip8& chip8, unsigned char x) {
	for (int i = 0; i < 0xF; i++) {
		if ((chip8.variableRegisters[x] & 0x0F) == i) chip8.iRegister = 5 * i;
	}
}

void chip8_toBCD(chip8& chip8, unsigned char x) {
	unsigned char number = chip8.variableRegisters[x];
	for (int i = 0; i < 3; i++) {
		chip8.memory[chip8.iRegister + i] = (number / (unsigned char)pow(10, 2 - i)) % 10;
	}
}

void chip8_regToMem(chip8& chip8, unsigned char x) {
	unsigned char regNum = x;
	for (int i = 0; i <= regNum; i++) {
		chip8.memory[chip8.iRegister + i] = chip8.variableRegisters[i];
	}
}

void chip8_memToReg(chip8& chip8, unsigned char x) {
	unsigned char regNum = x;
	for (int i = 0; i <= regNum; i++) {
		chip8.variableRegisters[i] = chip8.memory[chip8.iRegister + i];
	}
}

void chip8_loadROM(chip8& chip8, rom& rom) {
	rom.filePath.seekg(0, rom.filePath.end);
	unsigned short rLen = (unsigned short)rom.filePath.tellg();
	rom.filePath.seekg(0, rom.filePath.beg);
	char* content;
	content = new char[rLen];
	rom.filePath.read(content, rLen);

	memcpy(&chip8.memory[chip8.oldStartMem], content, rLen);
}

unsigned char chip8_mapKey(SDL_Keycode keycode) {
	if (keycode == SDLK_1) return 0x1;
	if (keycode == SDLK_2) return 0x2;
	if (keycode == SDLK_3) return 0x3;
	if (keycode == SDLK_4) return 0xC;
	if (keycode == SDLK_q) return 0x4;
	if (keycode == SDLK_w) return 0x5;
	if (keycode == SDLK_e) return 0x6;
	if (keycode == SDLK_r) return 0xD;
	if (keycode == SDLK_a) return 0x7;
	if (keycode == SDLK_s) return 0x8;
	if (keycode == SDLK_d) return 0x9;
	if (keycode == SDLK_f) return 0xE;
	if (keycode == SDLK_z) return 0xA;
	if (keycode == SDLK_x) return 0x0;
	if (keycode == SDLK_c) return 0xB;
	if (keycode == SDLK_v) return 0xF;
	else return -1;
}