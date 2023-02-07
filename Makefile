make:
	clear
	gcc \
		-I. \
		nvideo/nvideo.c \
		nvideo/output/terminal.c \
		nvideo/output/dump.c \
		-c \
		-g

	ar rs nvideo.a nvideo.o terminal.o dump.o

clean:
	rm -f main
	rm -rf *.o
	rm -rf *.a
	clear

test: main.c
	make clean
	make
	gcc \
		-I. \
		-c \
		-g \
		main.c 

	gcc \
		-o main \
		main.o \
		nvideo.a

	./main
