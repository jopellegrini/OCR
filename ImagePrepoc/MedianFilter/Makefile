
# Setting the compiler and the default linker program
CC = gcc
# options for pre-processor (-I, -include, -D ... )
CPPFLAGS = `pkg-config --cflags sdl` -MMD
# main compilation options
CFLAGS = -std=c99 -O3

LDLIBS= `pkg-config --libs sdl` -lSDL_image


SRC = main.c median_filter.c pixel_operations.c
OBJ = ${SRC:.c=.o}
DEP = ${SRC:.c=.d}

all: main

main: ${OBJ}

.PHONY: clean

clean:
	${RM} ${OBJ}
	${RM} ${DEP}
	${RM} main

-include ${DEP}
