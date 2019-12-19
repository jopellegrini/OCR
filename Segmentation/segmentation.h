#ifndef SEGMENTATION_H
#define SEGMENTATION_H value
#include "DocumentBlocks.h"
#include <stdlib.h>

// main function that will be called by the main program
struct DocumentBlocks_t *SegmentOnBinArray(int binArray[], size_t w, size_t h, size_t *charNumber);

void CutInY(struct DocumentBlocks_t *block, size_t *charNumber);

void CutInX(struct DocumentBlocks_t *block, size_t *charNumber);

void FindBiggestWhiteStripe(int *array, size_t length, size_t *iMax,
		size_t *maxStripeLenght);

int *MakeYhist(struct DocumentBlocks_t *block);

int *MakeXhist(struct DocumentBlocks_t *block);

void PrintHist(int *array, size_t length);

void InitBlock1Y(struct DocumentBlocks_t *block,
		struct DocumentBlocks_t *block1, size_t maxStripeLenght);

void InitBlock2Y(struct DocumentBlocks_t *block,
		struct DocumentBlocks_t *block2, size_t maxStripeLenght, size_t imax);

void InitBlock1X(struct DocumentBlocks_t *block,
		struct DocumentBlocks_t *block2, size_t imax);

void InitBlock2X(struct DocumentBlocks_t *block,
		struct DocumentBlocks_t *block2, size_t maxStripeLenght, size_t imax);
#endif /* ifndef SEGMENTATION_H */
