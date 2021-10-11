chip8: main.o chip8.o
	gcc main.o chip8.o -lSDL2 -o chip8

main.o: src/main.c 
	gcc -c src/main.c

chip8.o: src/chip8.c 
	gcc -c src/chip8.c

clean:
	rm *.o chip8

