SRC = main.c
DEST = main.elf
CC_FLAGS = -Wall -Wextra
CC = gcc

clean:
	rm -f *.elf

all:
	${CC} ${SRC} ${CC_FLAGS} -o ${DEST}

run:
	./main.elf

go: clean all run
