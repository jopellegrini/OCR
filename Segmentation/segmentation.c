#include <stdlib.h>
#include <stdio.h>
#include "DocumentBlocks.h"
#include "segmentation.h"
#include <assert.h>

#define treshold 0

struct DocumentBlocks_t *SegmentOnBinArray(int *binArray, size_t w, size_t h, size_t *charNumber)
{
	// - Do cuts in y axis so that there is nowhere left to cut (recursively)
	//
	// - Do cut on X axis so that there is nowhere left to cut
	//   at this point we will have individual characters
	//
	//   supposed to return a pointer on the document

	// here we make the first struct block that will represent the whole document
	struct DocumentBlocks_t *wholeDocument = malloc(sizeof(struct DocumentBlocks_t));
	assert(wholeDocument != NULL);

	wholeDocument->x = 0;
	wholeDocument->y = 0;
	wholeDocument->w = w;
	wholeDocument->h = h;
	wholeDocument->DocH = h;
	wholeDocument->DocW = w;
	wholeDocument->nbChildren = 0;
	wholeDocument->binArray = &binArray;

	CutInY(wholeDocument, charNumber);

	return wholeDocument;
}

void CutInX(struct DocumentBlocks_t *block, size_t *charNumber)
{
		int *Xhist = MakeXhist(block);
		size_t imax = 0;
		size_t maxStripeLenght = 0;
		FindBiggestWhiteStripe(Xhist, block->w, &imax, &maxStripeLenght);
		free(Xhist);

		if (maxStripeLenght == 0) {
			++(*charNumber);
			block->type = CHAR;
			CutInY(block, charNumber);
			return;
		}

		if (imax == 0) {
			block->x += imax + maxStripeLenght;
			block->w -= imax + maxStripeLenght;
			CutInX(block, charNumber);
		}

		else if (imax + maxStripeLenght == block->w) {
			block->w -= maxStripeLenght;
			CutInX(block, charNumber);
		}
		else if (maxStripeLenght >= block->h / 4) {
			// here it is a space between two words
			block->type = LIGNE;
			++(*charNumber);
			block->sons = calloc(2, sizeof(unsigned long));
			assert(block->sons != NULL);
			block->nbChildren = 2;
			struct DocumentBlocks_t *word1 = malloc(sizeof(struct DocumentBlocks_t));
			assert(word1 != NULL);
			InitBlock1X(block, word1, imax);
			block->sons[0] = word1;
			CutInX(word1, charNumber);
			struct DocumentBlocks_t *word2 = malloc(sizeof(struct DocumentBlocks_t));
			assert(word2 != NULL);
			InitBlock2X(block, word2, maxStripeLenght, imax);
			block->sons[1] = word2;
			CutInX(word2, charNumber);
		}
		else if (maxStripeLenght){
			// here a space between characters
			block->type = WORD;
			block->sons = calloc(2, sizeof(unsigned long));
			assert(block->sons != NULL);
			block->nbChildren = 2;
			struct DocumentBlocks_t *word1 = malloc(sizeof(struct DocumentBlocks_t));
			assert(word1 != NULL);
			InitBlock1X(block, word1, imax);
			block->sons[0] = word1;
			CutInX(word1, charNumber);
			struct DocumentBlocks_t *word2 = malloc(sizeof(struct DocumentBlocks_t));
			assert(word2 != NULL);
			InitBlock2X(block, word2, maxStripeLenght, imax);
			block->sons[1] = word2;
			CutInX(word2, charNumber);
		}

		return;
}

void CutInY(struct DocumentBlocks_t *block, size_t *charNumber)
{
	int *YHist = MakeYhist(block);
	size_t imax = 0;
	size_t maxStripeLenght = 0;
	FindBiggestWhiteStripe(YHist, block->h, &imax, &maxStripeLenght);
	free(YHist);


	if (imax == 0 && maxStripeLenght != 0) {
		block->y += imax + maxStripeLenght;
		block->h -= maxStripeLenght + imax;
		CutInY(block, charNumber);
	}

	else if (imax + maxStripeLenght  == block->h && maxStripeLenght != 0) {
		block->h -= maxStripeLenght;
		CutInY(block, charNumber);
	}

	else if (maxStripeLenght <= block->DocH / 100) {
		if (block->type == CHAR)
			return;

		block->type = LIGNE;
		CutInX(block, charNumber);
		return;
	}

	else if (block->type != CHAR) {
		++(*charNumber);
		block->type = BLOCK;

		block->sons = calloc(2, sizeof(unsigned long));
		assert(block->sons != NULL);
		block->nbChildren = 2;
		struct DocumentBlocks_t *block1 = malloc(sizeof(struct DocumentBlocks_t));
		assert(block1 != NULL);
		InitBlock1Y(block, block1, imax);
		block->sons[0] = block1;
		CutInY(block1, charNumber);
		struct DocumentBlocks_t *block2 = malloc(sizeof(struct DocumentBlocks_t));
		assert(block2 != NULL);
		InitBlock2Y(block, block2, maxStripeLenght, imax);
		block->sons[1] = block2;
		CutInY(block2, charNumber);
	}
}

void InitBlock2X(struct DocumentBlocks_t *block,
		struct DocumentBlocks_t *block2, size_t maxStripeLenght, size_t imax)
{
		block2->x = block->x + maxStripeLenght + imax;
		block2->w = block->w - maxStripeLenght - imax;
		block2->binArray = block->binArray;
		block2->y = block->y;
		block2->h = block->h;
		block2->DocH = block->DocH;
		block2->DocW = block->DocW;
		block2->type = WORD;
		block2->nbChildren = 0;
}

void InitBlock2Y(struct DocumentBlocks_t *block,
		struct DocumentBlocks_t *block2, size_t maxStripeLenght, size_t imax)
{
		block2->y = block->y + maxStripeLenght + imax;
		block2->h = block->h - maxStripeLenght - imax;
		block2->binArray = block->binArray;
		block2->x = block->x;
		block2->w = block->w;
		block2->DocH = block->DocH;
		block2->DocW = block->DocW;
		block2->type = BLOCK;
		block2->nbChildren = 0;
}

void InitBlock1X(struct DocumentBlocks_t *block,
		struct DocumentBlocks_t *block1, size_t imax)
{
		block1->x = block->x;
		block1->w = imax;
		block1->binArray = block->binArray;
		block1->y = block->y;
		block1->h = block->h;
		block1->DocH = block->DocH;
		block1->DocW = block->DocW;
		block1->type = WORD;
		block1->nbChildren = 0;
}

void InitBlock1Y(struct DocumentBlocks_t *block,
		struct DocumentBlocks_t *block1, size_t imax)
{
		block1->y = block->y;
		block1->h = imax;
		block1->binArray = block->binArray;
		block1->x = block->x;
		block1->w = block->w;
		block1->DocH = block->DocH;
		block1->DocW = block->DocW;
		block1->type = BLOCK;
		block1->nbChildren = 0;
}

void lengthofsubsequnce(size_t i, size_t *sequL, int *array, size_t length)
{
	*sequL = 0;
	while (array[i] <= treshold && i < length)
	{
		(*sequL)++;
		i++;
	}
}

void FindBiggestWhiteStripe(int *array, size_t length, size_t *iMax, size_t *maxStripeLenght)
{
	size_t stripeLenght = 0;
	*maxStripeLenght = 0;
	*iMax = 0;

	for (size_t i = 0; i < length; ++i) {
		if (array[i] <= treshold){
			lengthofsubsequnce(i, &stripeLenght, array, length);
		}
		if(stripeLenght > *maxStripeLenght) {
			*maxStripeLenght = stripeLenght;
			*iMax = i;
			stripeLenght = 0;
		}
	}
}

int *MakeXhist(struct DocumentBlocks_t *block)
{
	int *binArray = *block->binArray;
	int *XHist = calloc(block->w, sizeof(int));
	assert(XHist != NULL);

	for (size_t i = block->x; i < block->w + block->x; ++i) {
		for (size_t j = block->y; j < block->h + block->y; ++j) {
			if (binArray[j * (block->DocW) + i] == 0)
				*(XHist + i - block->x) += 1;
		}
	}

	return XHist;
}

int *MakeYhist(struct DocumentBlocks_t *block)
{
	int *binArray = *block->binArray;
	int *YHist = calloc(block->h, sizeof(int));
	assert(YHist != NULL);

	for (size_t i = block->y; i < block->h + block->y; ++i) {
		for (size_t j = block->x; j < block->w + block->x; ++j) {
			if (binArray[i * block->DocW + j] == 0)
				*(YHist + i - block->y) += 1;
		}
	}
	return YHist;
}

