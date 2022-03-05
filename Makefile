SRC = main.c
DEST = main.elf
CC_FLAGS = -Wall -Wextra
CC = gcc

go: clean all run

clean:
	rm -f *.elf

all:
	${CC} ${SRC} ${CC_FLAGS} -o ${DEST}

run:
	./main.elf


