#include "chip8.h"

void chip8_clearScr(chip8& chip8) {
	for (int i = 0; i < 64; i++) {
		for (int j = 0; j < 32; j++) {
			chip8.display[i][j] = false;
		}
	}
}

void chip8_return(chip8& chip8) {
	chip8.pc = chip8.stack.top();
	chip8.stack.pop();
}

void chip8_jmp(chip8& chip8, unsigned char nnn) {
	chip8.pc = nnn;	
}

void chip8_call(chip8& chip8, unsigned char nnn) {
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