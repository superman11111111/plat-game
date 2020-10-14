CC=gcc -std=c89

all : clean compile run
clean : 
	rm -f game.bin
compile : game.c
	$(CC) game.c -lSDL2 -lSDL2_ttf -o game.bin
run : game.bin
	./game.bin
