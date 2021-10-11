#include "chip8.h"
#include <SDL2/SDL.h>
#include <stdbool.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 320

unsigned char keysym[4*4] = {
  '1','2','3','4',
  'q','w','e','r',
  'a','s','d','f',
  'z','x','c','v'
};

typedef struct {
  SDL_Window *window;
  SDL_Renderer *renderer;
} GFX;

GFX gfx;

void gfx_init();
void gfx_quit();
void gfx_draw();

int main(int argc, char *argv[]) {
  FILE* rom;
  argv++;
  if(argc > 1) {
    puts(*argv);
    rom = fopen(*argv, "rb");
  }  else
    rom = fopen("pong2.c8", "rb");

  fseek(rom, 0L, SEEK_END);
  int size = ftell(rom);
  rewind(rom);

  unsigned char buf[size];
  for(int i = 0; i < size; i++) {
    buf[i] = fgetc(rom);

  }

  // Init chip8
  struct chip8 *cpu = (struct chip8 *) malloc(sizeof(struct chip8));
  chip8_init(cpu);
  chip8_loadProgram(cpu, buf, size);

  gfx_init();
  
  bool running = true;
  SDL_Event e;
  while(chip8_cycle(cpu) >= 0 && running) {
    while(SDL_PollEvent(&e) != 0) {
      if(e.type == SDL_QUIT) running = false;
      for(int i = 0; i < 4*4; i++)
	if(e.key.keysym.sym == keysym[i])
	  cpu->key[i] = (cpu->key[i]) == 0 ? 1 : 0;
    }
    if(cpu->drawFlag) {
      SDL_Delay(25);      
      gfx_draw();
      cpu->drawFlag = false;
    }
  }

  // Clean up
  free((struct chip8 *) cpu);

  gfx_quit();
  return EXIT_SUCCESS;
}

void gfx_draw() {
  SDL_RenderClear(gfx.renderer);

  for(int y = 0; y < 32; y++) {
    for(int x = 0; x < 64; x++) {
      if(sprites[(y*64)+x] != 0) {
	SDL_Rect pxl = {x*10, y*10, 10, 10};
	SDL_SetRenderDrawColor(gfx.renderer,
			       255, 255, 255, 0);
	SDL_RenderFillRect(gfx.renderer, &pxl);
      }
    }
  }

  SDL_SetRenderDrawColor(gfx.renderer, 0, 0, 0, 0);
  SDL_RenderPresent(gfx.renderer);      
}

void gfx_init() {
  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
    printf("Failed to initialize SDL: %s\n", SDL_GetError());
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

  // Create window
  gfx.window = SDL_CreateWindow("Chip-8",
			  SDL_WINDOWPOS_UNDEFINED,
			  SDL_WINDOWPOS_UNDEFINED,
			  SCREEN_WIDTH, SCREEN_HEIGHT,
			  SDL_WINDOW_SHOWN);
  if(gfx.window == NULL)
    printf("Failed to create window: %s\n", SDL_GetError());

  // Create renderer
  gfx.renderer = SDL_CreateRenderer(gfx.window, -1,
				   SDL_RENDERER_ACCELERATED);
  if(gfx.renderer == NULL)
    printf("Failed to create renderer: %s\n", SDL_GetError());
}

void gfx_quit() {
  SDL_DestroyWindow(gfx.window);
  SDL_DestroyRenderer(gfx.renderer);
  SDL_Quit();
}
