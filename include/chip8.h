#ifndef chip8h
#define chip8h

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define MEM 4096


extern unsigned char sprites[64*32];

typedef u_int8_t u8;
typedef u_int16_t u16;

struct chip8 {
  u16 I;
  u16 pc;
  unsigned short opcode;
  u16 stack[16];
  u16 sp;
  unsigned char key[16];
  unsigned char memory[MEM];
  unsigned char V[16];
  u8 gfx[64*32];
  u8 delay_timer;
  u8 sound_timer;
  bool drawFlag;
};

void chip8_init(struct chip8 *cpu);
void chip8_loadProgram(struct chip8 *cpu, unsigned char *buf, int bufsize);
void chip8_input(struct chip8 *cpu);
int chip8_cycle(struct chip8 *cpu);
#endif
