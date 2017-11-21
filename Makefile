CC = gcc-6
OBJS = main.out
CFLAGS = -std=c11
LFLAGS = -lallegro -lallegro_primitives -lallegro_main -lallegro_font -lallegro_ttf

main.out: main.c grid.h button.h
	$(CC) $(CFLAGS) main.c $(LFLAGS) -o $(OBJS)

clean:
	\rm *.out
