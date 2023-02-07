make:
	clear
	gcc \
		-I. \
		nvideo/nvideo.c \
		nvideo/output/terminal.c \
		nvideo/output/dump.c \
		-c \
		-g

	mv nvideo.o nvideo_part.o
	gcc \	
		-o nvideo.o
		nvideo_part.o \
		terminal.o \
		dump.o

test: main.c
	make
	gcc \
		-I. \
		-c \
		-g \
		main.c 

	gcc \
		-o main \
		main.o \
		nvideo.o

	./main

clean:
	rm -f main
	rm -f *.o
	clear
