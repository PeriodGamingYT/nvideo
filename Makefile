make:
	gcc \
		-I. \
		nvideo/nvideo.c \
		nvideo/output/terminal.c \
		nvideo/output/dump.c \
		-c \
		-g

test: main.c nvideo.o
	gcc \
		-I. \
		-c \
		-g \
		main.c 

	gcc \
		-o main \
		main.o \
		nvideo.o \
		dump.o \
		terminal.o

	./main

clean:
	rm -f main
	rm -f *.o

run:
	make clean
	make
	clear
	./main
