SRC = performance_comparison.c
DEST = performance_comparison.elf
CC_FLAGS = -Weverything -Wall -Wextra
CC = clang

go: clean all run

clean:
	rm -f *.elf

all:
	${CC} ${SRC} ${CC_FLAGS} -o ${DEST}

run:
	./performance_comparison.elf


