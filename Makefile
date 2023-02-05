make: main.c
	gcc \
		nvideo/nvideo.c \
		nvideo/output/terminal.c \
		nvideo/output/dump.c \
		main.c \
		-o main

clean:
	rm -f main

run:
	make clean
	make
	clear
	./main
