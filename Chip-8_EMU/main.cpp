#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <fstream>
#include <SDL.h>
#include "chip8.h"

using namespace std;

int main(int argc, char** argv) {

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

		//system("cls");

		while (SDL_PollEvent(&e)) {
			switch (e.type) {
			case SDL_QUIT:
				chip8.on = false;
				break;
			};
		}

		// FETCH
		currentInstruction[0] = chip8.memory[chip8.pc];		// Load current instruction from memory (1st Byte)
		currentInstruction[1] = chip8.memory[chip8.pc + 1];	// Load current instruction from memory (2nd Byte)
		
		unsigned short opcode = currentInstruction[0] << 8 | currentInstruction[1];

		cout << "PC: " << chip8.pc << endl;
		
		chip8.pc += 2;

		cout << "Current opcode: ";
		cout << (void *)opcode;
		cout << endl;

		unsigned char x = (opcode & 0x0F00) >> 8;
		unsigned char y = (opcode & 0x00F0) >> 4;
		unsigned char n = (opcode & 0x000F);
		unsigned char nn = (opcode & 0x00FF);
		unsigned short nnn = (opcode & 0x0FFF);

		cout << "X: " << (void *)x << endl;
		cout << "Y: " << (void *)y << endl;
		cout << "N: " << (void *)n << endl;
		cout << "NN: " << (void *)nn << endl;
		cout << "NNN: " << (void *)nnn << endl;

		// DECODE
		switch ((opcode & 0xF000) >> 12) { // EXECUTE
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
			case 0x3:
				chip8_regXor(chip8, x, y);
				break;
			case 0x4:
				chip8_regSum(chip8, x, y);
				break;
			case 0x5:
				chip8_regDiff_XY(chip8, x, y);
				break;
			case 0x6:
				chip8_regShift_right(chip8, x, y);
				break;
			case 0x7:
				chip8_regDiff_YX(chip8, x, y);
			case 0xE:
				chip8_regShift_left(chip8, x, y);
				break;
			}
		case 0x9:
			if (n == 0x0) chip8_skipNeq(chip8, x, y);
			break;
		case 0xA:
			chip8_setIReg(chip8, nnn);
			break;
		case 0xB:
			chip8_jmpV0(chip8, x, nnn);
			break;
		case 0xC:
			chip8_rand(chip8, x, nn);
			break;
		case 0xD:
			chip8_display(chip8, x, y, n);
		case 0xE:
			switch (nn) {
			case 0x9E:
				chip8_skipKeyPressed(chip8, x);
				break;
			case 0xA1:
				chip8_skipKeyNotPressed(chip8, x);
				break;
			}
		case 0xF:
			switch (nn) {
			case 0x07:
				chip8_setRegToDelayTimer(chip8, x);
				break;
			case 0x15:
				chip8_setDelayTimer(chip8, x);
				break;
			case 0x18:
				chip8_setSoundTimer(chip8, x);
				break;
			case 0x1E:
				chip8_addToIReg(chip8, x);
				break;
			case 0x0A:
				chip8_waitForKey(chip8);
				break;
			case 0x29:
				chip8_fontCharacter(chip8, x);
				break;
			case 0x33:
				chip8_toBCD(chip8, x);
				break;
			case 0x55:
				chip8_regToMem(chip8, x);
				break;
			case 0x65:
				chip8_memToReg(chip8, x);
				break;
			}
		}

		// INPUT
		// TODO: Use SDL to get input

		// RENDERING
		// TODO: Use SDL to render stuff
		/*for (int i = 0; i < 64; i++) {
			for (int j = 0; j < 32; j++) {
				if (chip8.display[i][j] == true) cout << '0';
				else if (chip8.display[i][j] == false) cout << 'e';
			}
			cout << endl;
		}*/
	}

	SDL_Quit();

	return 0;
}