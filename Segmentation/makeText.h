#ifndef TO_TEXT_H
#define TO_TEXT_H value

#include <stdlib.h>
#include <stdio.h>
#include "DocumentBlocks.h"


void WriteDebugFile(struct DocumentBlocks_t *doc, int *binArrayOnImage, size_t charNumber);

struct DocumentBlocks_t *MakeDocTreeFromImage(int *image, size_t *charNumber, size_t w, size_t h);

char *DocToString(struct DocumentBlocks_t *doc, int *array, char str[]);

void __docTostring(char *string, struct DocumentBlocks_t *doc, size_t *i, int *array);

int *makeMatriceFromChar(struct DocumentBlocks_t *doc, int *array);

void WriteBdD(FILE *filept, struct DocumentBlocks_t *doc, int * binArray);

int *loadImage(char *filename, size_t *w, size_t *h);

#endif /* ifndef TO_TEXT_H */
