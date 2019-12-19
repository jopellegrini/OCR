# Makefile

VPATH = ./:../Segmentation:../NeuralNetwork

CC = gcc
CPPFLAGS = `pkg-config --cflags sdl` `pkg-config --cflags gtk+-3.0` -MMD
CFLAGS = -Wall -O3
LDLIBS = `pkg-config --libs sdl` -lSDL_image `pkg-config --libs gtk+-3.0` -lm

SRC = main.c segmentation.c color_treatments.c median_filter.c pixel_operations.c file_utils.c letters_classifier.c makeText.c network.c neuron.c saving.c training.c training_example.c

OBJ = ${SRC:.c=.o}
DEP = ${SRC:.c=.d}

all: main

main: ${OBJ}

.PHONY: clean

clean:
	${RM} ${OBJ}   # remove object files
	${RM} ${DEP}   # remove dependency files
	${RM} main     # remove main program

-include ${DEP}
# END
