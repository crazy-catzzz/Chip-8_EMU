#include <iostream>
#include <stdlib.h>
#include <time.h>
#include "chip8.h"

using namespace std;

int main(int argc, char** argv) {

	srand(time(NULL));

	chip8 chip8;
	chip8.on = true;

	unsigned char currentInstruction[2];

	chip8.pc = 0x0;

	while (chip8.on) {
		// FETCH
		currentInstruction[0] = chip8.memory[chip8.pc];		// Load current instruction from memory (1st Byte)
		currentInstruction[1] = chip8.memory[chip8.pc + 1];	// Load current instruction from memory (2nd Byte)
		chip8.pc += 2;

		// x: currentInstruction[0] << 4
		// y: currentInstruction[1] >> 4
		// n: currentInstruction[1] << 4

		// nn:	currentInstruction[0] << 4 & currentInstruction[1] >> 4
		// nnn:	currentInstruction[0] << 4 & currentInstruction[1]

		// DECODE
		switch (currentInstruction[0] >> 4) { // EXECUTE
		case 0x0:
			if (currentInstruction[0] << 4 == 0x0) {
				if (currentInstruction[1] >> 4 == 0xE) {
					if (currentInstruction[1] << 4 == 0x0) {
						for (int i = 0; i < 32; i++)
							for (int j = 0; j < 64; j++)
								chip8.display[i][j] = false;
					}
					else if (currentInstruction[1] << 4 == 0xE) {
						chip8.pc = chip8.stack.top();
						chip8.stack.pop();
					}
				}
			}

			break;
		case 0x1:
			chip8.pc = currentInstruction[0] << 4 | currentInstruction[1];
			break;
		case 0x2:
			chip8.stack.push(chip8.pc);
			chip8.pc = currentInstruction[0] << 4 | currentInstruction[1];
			break;
		case 0x3:
			if (chip8.variableRegisters[currentInstruction[0] << 4] == currentInstruction[1])
				chip8.pc += 2;
			break;
		case 0x4:
			if (chip8.variableRegisters[currentInstruction[0] << 4] != currentInstruction[1])
				chip8.pc += 2;
			break;
		case 0x5:
			if (currentInstruction[1] << 4 == 0x0)
				if (chip8.variableRegisters[currentInstruction[0] << 4] == chip8.variableRegisters[currentInstruction[1] >> 4])
					chip8.pc += 2;
			break;
		case 0x6:
			chip8.variableRegisters[currentInstruction[0] << 4] = currentInstruction[1];
			break;
		case 0x7:
			chip8.variableRegisters[currentInstruction[0] << 4] += currentInstruction[1];
			break;
		case 0x8:
			switch (currentInstruction[1] << 4) {
			case 0x0:
				chip8.variableRegisters[currentInstruction[0] << 4] = chip8.variableRegisters[currentInstruction[1] >> 4];
				break;
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
		case 0xD:	// Display opcode, probably will cause a memory leak. Too bad!
			unsigned int xCoord = chip8.variableRegisters[currentInstruction[0] << 4] & 63;
			unsigned int yCoord = chip8.variableRegisters[currentInstruction[1] >> 4] & 31;
			chip8.variableRegisters[0xF] = 0x00;

			for (int i = 0; i < currentInstruction[1] << 4; i++) {
				if (yCoord >= 32) break;
				unsigned char spriteData = chip8.memory[chip8.iRegister + i];
				for (int j = 0; j < 8; j++) {
					if (xCoord >= 64) break;
					if (spriteData >> 8 - j << j == 0b1 && chip8.display[xCoord][yCoord] == 0b1) {
						chip8.display[xCoord][yCoord] = 0b0;
						chip8.variableRegisters[0xF] = 0b1;
					}
					else if (spriteData >> 8 - j << j == 0b1 && chip8.display[xCoord][yCoord] == 0b0) {
						chip8.display[xCoord][yCoord] = 0b1;
					}
					xCoord++;
				}
				yCoord++;
			}
			break;
		case 0xE:
			break;
		case 0xF:
			break;
		}
	}

	return 0;
}