#include "chip8.h"

unsigned char chip8_fontset[80] =
{ 
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

unsigned char sprites[64*32];

void chip8_init(struct chip8 *cpu) {
  for(int i = 0; i < 64*32; i++) {
    sprites[i] = 0;
  }
  cpu->pc = 0x200;
  cpu->I = 0;
  cpu->sp = 0;
  cpu->opcode = 0;


  for(int i = 0; i< 4096; i++) {
    cpu->memory[i] = 0;
  }
  
  for(int i = 0; i < 80; i++) {
    cpu->memory[i] = chip8_fontset[i];
  }

  for(int i = 0; i< 16; i++) {
    cpu->stack[i] = cpu->key[i] = cpu->V[i] = 0;
  }

  cpu->drawFlag = true;
  cpu->delay_timer = 0;
  cpu->sound_timer = 0;
}

void chip8_input(struct chip8 *cpu) {

}

int chip8_cycle(struct chip8 *cpu) {
  //Fetch opCode
  cpu->opcode = cpu->memory[cpu->pc] << 8 | cpu->memory[cpu->pc+1];
  //Decode opCode
  switch(cpu->opcode & 0xF000) {
  case 0x0000:
    switch(cpu->opcode & 0x000F) {
    case 0x0000:
      for(int i = 0; i < 2048; i++)
	sprites[i] = 0x0;
      cpu->drawFlag = true;
      cpu->pc += 2;
      break;
    case 0x000E: // 00EE: Return from subroutine
      cpu->sp--;
      cpu->pc = cpu->stack[cpu->sp];
      cpu->pc += 2;
      break;
    default:
      printf("Unknown opcode: 0x%X\n", cpu->opcode);
      return -1;
    }
    break;

  case 0x1000: //1NNN: Jump to address NNN
    cpu->pc = cpu->opcode & 0x0FFF;
    break;
    
  case 0x2000: // Call subroutine at address NNN
    cpu->stack[cpu->sp] = cpu->pc;
    cpu->sp++;
    cpu->pc = cpu->opcode & 0x0FFF;
    break;

  case 0x3000: //3XNN: Skip the following instruction if the value of register VX equals NN
    if(cpu->V[(cpu->opcode & 0x0F00) >> 8] == (cpu->opcode & 0x00FF))
      cpu->pc += 4;
    else
      cpu->pc += 2;
    break;

  case 0x4000: //4XNN: Skip the following instruction if the value of register VX is not equal to NN
    if(cpu->V[(cpu->opcode & 0x0F00) >> 8] != (cpu->opcode & 0x00FF))
      cpu->pc += 4;
    else
      cpu->pc += 2;
    break;

  case 0x5000:
    printf("WHAA: 0x5\n");
    break;
    
  case 0x6000: // 0x6XNN: Sets VX to NN.
    cpu->V[(cpu->opcode & 0x0F00) >> 8] = cpu->opcode & 0x00FF;
    cpu->pc += 2;
    break;

  case 0x7000: // 7XNN: Add NN to VX
    cpu->V[(cpu->opcode & 0x0F00) >> 8] += cpu->opcode & 0x00FF;
    cpu->pc += 2;
    break;

  case 0x8000:
    switch(cpu->opcode & 0x000F) {
    case 0x0000: // 8XY0: Set VX to the value in VY
      cpu->V[(cpu->opcode & 0x0F00) >> 8] =
	cpu->V[(cpu->opcode & 0x00F0) >> 4];
      cpu->pc += 2;
      break;

    case 0x0001:
      cpu->V[(cpu->opcode & 0x0F00) >> 8] |=
	cpu->V[(cpu->opcode & 0x00F0) >> 4];
      cpu->pc += 2;
      break;
      
    case 0x0002:
      cpu->V[(cpu->opcode & 0x0F00) >> 8] &=
	cpu->V[(cpu->opcode & 0x00F0) >> 4];
      cpu->pc += 2;
      break;

    case 0x0003:
      cpu->V[(cpu->opcode & 0x0F00) >> 8] ^=
	cpu->V[(cpu->opcode & 0x00F0) >> 4];
      cpu->pc += 2;      
      break;
      
    case 0x0004:
      if(cpu->V[(cpu->opcode & 0x00F0) >> 4] >
	 (0xFF - cpu->V[(cpu->opcode & 0x0F00) >> 8])) 
	cpu->V[0xF] = 1; 
      else 
	cpu->V[0xF] = 0;					      

      cpu->V[(cpu->opcode & 0x0F00) >> 8] +=
	cpu->V[(cpu->opcode & 0x00F0) >> 4];

      cpu->pc += 2;
      break;

    case 0x0005:
      if(cpu->V[(cpu->opcode) & 0x00F0 >> 4] >
	 cpu->V[(cpu->opcode & 0x0F00) >> 8])
	cpu->V[0xF] = 0;
      else
	cpu->V[0xF] = 1;

      cpu->V[(cpu->opcode & 0x0F00) >> 8] -=
	cpu->V[(cpu->opcode & 0x00F0) >> 4];

      cpu->pc += 2;      
      break;

    case 0x0006:
      cpu->V[0xF] = cpu->V[(cpu->opcode & 0x0F00) >> 8] & 0x1;
      cpu->V[(cpu->opcode & 0x0F00) >> 8] >>= 1;
      cpu->pc += 2;
      break;

    case 0x0007:
      printf("whaaaa: 0x8007\n");
      break;

    case 0x000E:
      printf("whaaaa: 0x800E\n");
      break;      
      
    default:
      printf("Unknown opcode: 0x%X\n", cpu->opcode);
      return -1;            
    }
    break;

  case 0x9000:
    printf("WHAAAA 0x9000\n");
    break;
    
  case 0xA000:
    cpu->I = (cpu->opcode & 0x0FFF);
    cpu->pc += 2;
    break;

  case 0xB000:
    printf("whaaa 0xB000\n");
    
  case 0xC000: // CXNN: Set VX to a random number with a mask of NN
    cpu->V[(cpu->opcode & 0x0F00) >> 8] = (rand()%0xFF) & (cpu->opcode & 0x00FF);
    cpu->pc += 2;
    break;
    
  case 0xD000:
    {
      unsigned short x = cpu->V[(cpu->opcode & 0x0F00) >> 8];
      unsigned short y = cpu->V[(cpu->opcode & 0x00F0) >> 4];
      unsigned short height = cpu->opcode & 0x000F;
      unsigned short pixel;

      cpu->V[0xF] = 0;
      for(int ypos = 0; ypos < height; ypos++) {
	pixel = cpu->memory[cpu->I + ypos];
	for(int xpos = 0; xpos < 8; xpos++) {
	  if((pixel & (0x80 >> xpos)) != 0) {
	    if(sprites[(x + xpos + ((y + ypos) * 64))] == 1)
	      cpu->V[0xF] = 1;
	    sprites[x + xpos + ((y + ypos) * 64)] ^= 1;
	  }
	}
      }
      
      cpu->drawFlag = true;
      cpu->pc += 2;
    }
    break;

  case 0xE000:
    switch(cpu->opcode & 0x00FF) {
    case 0x00A1: //EXA1: Skip the following instruction if the key corresponding to the hex value currently stored in register VX is not pressed
      if(cpu->key[cpu->V[(cpu->opcode & 0x0F00) >> 8]] == 0)
	cpu->pc += 4;
      else
	cpu->pc += 2;
      break;

    case 0x009E:
      if(cpu->key[cpu->V[(cpu->opcode & 0x0F00) >> 8]] != 0)
	cpu->pc += 4;
      else
	cpu->pc += 2;
      break;

    default:
      printf("Unknown opcode: 0x%X\n", cpu->opcode);
      return -1;      
    }
    break;
    
  case 0xF000:
    switch (cpu->opcode & 0x00FF) {
    case 0x0007: //FX07: Store the value of the delay timer in VX      
      cpu->V[(cpu->opcode & 0x0F00) >> 8] = cpu->delay_timer;
      cpu->pc += 2;
      break;

    case 0x000A:
      {
	bool keyPress = false;
	for(int i = 0; i < 16; i++) {
	  if(cpu->key[i] != 0) {
	    cpu->V[(cpu->opcode & 0x0F00) >> 8] = i;
	    keyPress = true;
	  }
	}
	if(!keyPress)
	  return 0;
	cpu->pc += 2;
      }
      break;
	
    case 0x0015: //FX15: Set the delay timer to the value of VX
      cpu->delay_timer = cpu->V[(cpu->opcode & 0x0F00) >> 8];
      cpu->pc += 2;
      break;

    case 0x0018:
      cpu->sound_timer = cpu->V[(cpu->opcode & 0x0F00) >> 8];
      cpu->pc += 2;
      break;

    case 0x001E:
      if(cpu->I + cpu->V[(cpu->opcode & 0x0F00) >> 8] > 0xFFF)
	cpu->V[0xF] = 1;
      else
	cpu->V[0xF] = 0;

      cpu->I += cpu->V[(cpu->opcode & 0x0F00) >> 8];
      cpu->pc += 2;
      break;
      
    case 0x0029:
      cpu->I = cpu->V[(cpu->opcode & 0x0F00) >> 8] * 0x5;
      cpu->pc += 2;
      break;

    case 0x0033:
      cpu->memory[cpu->I]=cpu->V[(cpu->opcode & 0x0F00) >> 8] / 100;
      cpu->memory[cpu->I+1]=(cpu->V[(cpu->opcode & 0x0F00) >> 8]
			   / 10) % 10;
      cpu->memory[cpu->I+2]=(cpu->V[(cpu->opcode & 0x0F00) >> 8]
			   % 100) % 10;      
      cpu->pc += 2;
      break;

    case 0x0055:
      printf("WHAT:0x055");
      cpu->pc += 2;
      break;

    case 0x0065:
      for(int i = 0; i <= (cpu->opcode & 0x0F00) >> 8; i++)
	cpu->V[i] = cpu->memory[cpu->I + i];

      cpu->I += ((cpu->opcode & 0x0F00) >> 8) + 1;
      cpu->pc += 2;
      break;      

    default:
      printf("Unknown opcode: 0x%X\n", cpu->opcode);
      return -1;
    }
    break;

  default:
    printf("Unknown opcode: 0x%X\n", cpu->opcode);
    return -1;    
  }

  //Update timers
  if(cpu->delay_timer > 0) {
    --cpu->delay_timer;
  }

  if(cpu->sound_timer > 0)
    --cpu->sound_timer;

  return 0;
}

void chip8_loadProgram(struct chip8 *cpu, unsigned char *buf, int bufsize) {
  for(int i = 0; i < bufsize; i++) {
    cpu->memory[i+512] = *buf++;
  }
}
