#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <fstream>
#include <SDL.h>
#include "chip8.h"

using namespace std;

int main(int argc, char** argv) {

	unsigned int xCoord;
	unsigned int yCoord;
	unsigned char spriteData;
	unsigned char character;
	unsigned char number;
	unsigned char regNum;

	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Window* win;
	SDL_CreateWindow("Chip-8 Emu", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 320, 160, SDL_WINDOW_OPENGL);

	SDL_Event e;

	srand(time(NULL));

	chip8 chip8;
	chip8.on = true;

	unsigned char currentInstruction[2];

	chip8.pc = 0x0;

	// LOAD FONTS
	int k = 0;
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 5; j++) {
			chip8.memory[k] = chip8.fontTable[i][j];
			k++;
		}
	}

	// LOAD ROM
	ifstream rom;
	rom.open("C:\\Users\\Davide\\Downloads\\IBM Logo.ch8", ofstream::binary);
	rom.seekg(0, rom.end);
	unsigned short rLen = (unsigned short)rom.tellg();
	rom.seekg(0, rom.beg);
	char* content;
	content = new char[rLen];
	rom.read(content, rLen);

	memcpy(&chip8.memory[chip8.oldStartMem], content, rLen);
	cout << "Loaded!";

	chip8.pc = chip8.oldStartMem;

	while (chip8.on == true) {

		switch (SDL_PollEvent(&e)) {
		case SDL_QUIT:
			chip8.on = false;
			break;
		};

		// FETCH
		currentInstruction[0] = chip8.memory[chip8.pc];		// Load current instruction from memory (1st Byte)
		currentInstruction[1] = chip8.memory[chip8.pc + 1];	// Load current instruction from memory (2nd Byte)
		
		unsigned short opcode = currentInstruction[0] << 8 | currentInstruction[1];
		
		chip8.pc += 2;

		cout << "PC: " << chip8.pc << endl;

		cout << "Current opcode: ";
		cout << (void *)((opcode & 0xF000) >> 12);
		cout << endl;

		unsigned char x = (opcode & 0x0F00) >> 8;
		unsigned char y = (opcode & 0x00F0) >> 4;
		unsigned char n = (opcode & 0x000F);
		unsigned char nn = (opcode & 0x00FF);
		unsigned char nnn = (opcode & 0x0FFF);

		switch ((opcode & 0xF000) >> 12) {
		case 0x0:
			switch (n) {
			case 0x0:
				chip8_clearScr(chip8);
				break;
			case 0xE:
				chip8_return(chip8);
				break;
			}
			break;
		case 0x1:
			chip8_jmp(chip8, nnn);
			break;
		case 0x2:
			chip8_call(chip8, nnn);
			break;
		case 0x3:
			chip8_cmpRegSkip_eq(chip8, x, nn);
			break;
		case 0x4:
			chip8_cmpRegSkip_neq(chip8, x, nn);
			break;
		case 0x5:
			if (n == 0x0) chip8_cmpRegSkip_eq_XY(chip8, x, y);
			break;
		case 0x6:
			chip8_setReg(chip8, x, nn);
			break;
		case 0x7:
			chip8_addToReg(chip8, x, nn);
			break;
		case 0x8:
			switch (n) {
			case 0x0:
				chip8_setToReg(chip8, x, y);
				break;
			case 0x1:
				chip8_regOr(chip8, x, y);
				break;
			case 0x2:
				chip8_regAnd(chip8, x, y);
				break;
			}
		case 0x9:
		case 0xA:
		case 0xB:
		case 0xC:
		case 0xD:
		case 0xE:
		case 0xF:
		}

		// DECODE
		switch (currentInstruction[0] >> 4) { // EXECUTE
		case 0x8:
			switch (currentInstruction[1] << 4) {
			case 0x1:
				chip8.variableRegisters[currentInstruction[0] << 4] |= chip8.variableRegisters[currentInstruction[1] >> 4];
				break;
			case 0x2:
				chip8.variableRegisters[currentInstruction[0] << 4] &= chip8.variableRegisters[currentInstruction[1] >> 4];
				break;
			case 0x3:
				chip8.variableRegisters[currentInstruction[0] << 4] ^= chip8.variableRegisters[currentInstruction[1] >> 4];
				break;
			case 0x4:
				chip8.variableRegisters[currentInstruction[0] << 4] += chip8.variableRegisters[currentInstruction[1] >> 4];
				break;
			case 0x5:
				chip8.variableRegisters[currentInstruction[0] << 4] -= chip8.variableRegisters[currentInstruction[1] >> 4];
				break;
			case 0x6:
				chip8.variableRegisters[currentInstruction[0] << 4] = chip8.variableRegisters[currentInstruction[1] >> 4];
				chip8.variableRegisters[0xF] = chip8.variableRegisters[currentInstruction[0] << 4] >> 7;
				chip8.variableRegisters[currentInstruction[0] << 4] <<= 1;
				break;
			case 0x7:
				chip8.variableRegisters[currentInstruction[1] << 4] = chip8.variableRegisters[currentInstruction[0] >> 4] - chip8.variableRegisters[currentInstruction[1] << 4];
				break;
			case 0xE:
				chip8.variableRegisters[currentInstruction[0] << 4] = chip8.variableRegisters[currentInstruction[1] >> 4];
				chip8.variableRegisters[0xF] = chip8.variableRegisters[currentInstruction[0] << 4] << 7;
				chip8.variableRegisters[currentInstruction[0] << 4] >>= 1;
				break;
			}
		case 0x9:
			if (currentInstruction[1] << 4 == 0x0)
				if (chip8.variableRegisters[currentInstruction[0] << 4] != chip8.variableRegisters[currentInstruction[1] >> 4])
					chip8.pc += 2;
			break;
		case 0xA:
			chip8.iRegister = currentInstruction[0] << 4 | currentInstruction[1];
			break;
		case 0xB:
			chip8.pc = (currentInstruction[0] << 4 | currentInstruction[1]) + chip8.variableRegisters[0x0];
			break;
		case 0xC:
			chip8.variableRegisters[currentInstruction[0] << 4] = (unsigned char)(rand() % 255) & currentInstruction[1];
			break;
		case 0xD:
			xCoord = chip8.variableRegisters[currentInstruction[0] << 4] & 63;
			yCoord = chip8.variableRegisters[currentInstruction[1] >> 4] & 31;
			chip8.variableRegisters[0xF] = 0x00;

			for (int i = 0; i < currentInstruction[1] << 4; i++) {
				if (yCoord >= 32) break;
				spriteData = chip8.memory[chip8.iRegister + i];
				for (int j = 0; j < 8; j++) {
					if (xCoord >= 64) break;
					if (spriteData >> 8 - j << j == true && chip8.display[xCoord][yCoord] == true) {
						chip8.display[xCoord][yCoord] = false;
						chip8.variableRegisters[0xF] = true;
					}
					else if (spriteData >> 8 - j << j == true && chip8.display[xCoord][yCoord] == false) {
						chip8.display[xCoord][yCoord] = true;
					}
					xCoord++;
				}
				yCoord++;
			}
			break;
		case 0xE:
			switch (currentInstruction[1]) {
			case 0x9E:
				if (chip8.currentKey == chip8.variableRegisters[currentInstruction[0] << 4]) chip8.pc += 2;
				break;
			case 0xA1:
				if (chip8.currentKey != chip8.variableRegisters[currentInstruction[0] << 4]) chip8.pc += 2;
				break;
			}
			break;
		case 0xF:
			switch (currentInstruction[1]) {
			case 0x07:
				chip8.variableRegisters[currentInstruction[0] << 4] = chip8.delayTimer;
				break;
			case 0x15:
				chip8.delayTimer = chip8.variableRegisters[currentInstruction[0] << 4];
				break;
			case 0x18:
				chip8.soundTimer = chip8.variableRegisters[currentInstruction[0] << 4];
				break;
			case 0x1E:
				chip8.iRegister += chip8.variableRegisters[currentInstruction[0] << 4];
				break;
			case 0x0A:
				chip8.pc -= 2;
				if (chip8.keyPressed == true) chip8.pc += 2;
				break;
			case 0x29:
				character = chip8.variableRegisters[currentInstruction[0] << 4] << 4;
				for (int i = 0; i < 0xF; i++) {
					if (character == i) chip8.iRegister = 5 * i;
				}
				break;
			case 0x33:
				number = chip8.variableRegisters[currentInstruction[0] << 4];
				for (int i = 0; i < 3; i++) {
					chip8.memory[chip8.iRegister + i] = (number / (unsigned char)pow(10, 2 - i)) % 10;
				}
				break;
			case 0x55:
				regNum = currentInstruction[0] << 4;
				for (int i = 0; i <= regNum; i++) {
					chip8.memory[chip8.iRegister + i] = chip8.variableRegisters[i];
				}
				break;
			case 0x65:
				regNum = currentInstruction[0] << 4;
				for (int i = 0; i <= regNum; i++) {
					chip8.variableRegisters[i] = chip8.memory[chip8.iRegister + i];
				}
				break;
			}
			break;
		}

		// INPUT
		// TODO: Use SDL to get input

		// RENDERING
		// TODO: Use SDL to render stuff
		

		//chip8.on = false;
	}

	SDL_Quit();

	return 0;
}