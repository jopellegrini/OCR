#include "makeText.h"
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "segmentation.h"
#include "color_treatments.h"
#include "../NeuralNetwork/letters_classifier.h"

void printArrayOnFile(FILE *filept, struct DocumentBlocks_t *doc, int *binArray)
{
	for (size_t i = doc->y; i < doc->h + doc->y; ++i) {
		for (size_t j = doc->x; j < doc->w + doc->x; ++j) {

			if (binArray[i * doc->DocW + j] == 0)
				fprintf(filept, " 1");
			else
				fprintf(filept, " 0");
		}
	}
}

void printArray(size_t w, size_t h, int *array)
{
	for (size_t i = 0; i < h; ++i) {
		for (size_t j = 0; j < w; ++j) {
			if (array[i * w + j] == 1)
				printf("0 ");
			else
				printf(". ");
		}
		printf("\n");
	}
	printf("\n");
}

void printLetterOnFile(FILE *filept, struct DocumentBlocks_t *doc, int *binArray)
{
	for (size_t i = doc->y; i < doc->h + doc->y; ++i) {
		for (size_t j = doc->x; j < doc->w + doc->x; ++j) {

			if (binArray[i * doc->DocW + j] == 0)
				fprintf(filept, "0 ");
			else
				fprintf(filept, ". ");

		}
		fprintf(filept, "\n");
	}

}

void __WriteDebugFile(FILE *filept, struct DocumentBlocks_t *doc, int *binArray)
{
	// function used to write all the characters found in the doc into a txt file
	//
	if (doc->type == 3)
	{
		printLetterOnFile(filept, doc, binArray);
		fprintf(filept, "\n");
	}
	size_t i = 0;
	while (i < doc->nbChildren) {
		__WriteDebugFile(filept, doc->sons[i], binArray);
		i++;
	}
}

void WriteBdD(FILE *filept, struct DocumentBlocks_t *doc, int * binArray)
{
	// function used to write la base de données
	//
	if (doc->type == 3)
	{
		fprintf(filept, "%lu %lu", doc->w, doc->h);
		printArrayOnFile(filept, doc, binArray);
		fprintf(filept, "\n");
	}

	size_t i = 0;
	while (i < doc->nbChildren) {
		WriteBdD(filept, doc->sons[i], binArray);
		i++;
	}
}

int *makeMatriceFromChar(struct DocumentBlocks_t *doc, int *array)
{
	int *matrice = calloc(doc->w * doc->h, sizeof(int));

	for (size_t i = 0; i < doc->h; ++i) {
		for (size_t j = 0; j < doc->w; ++j) {
			if (array[(i + doc->y) * doc->DocW + (j + doc->x)] == 0)
				matrice[i * doc->w + j] = 1;;
		}
	}
	return matrice;
}

void __docTostring(char str[], struct DocumentBlocks_t *doc, size_t *i, int *array)
{
	if (doc->type == 3) {
		int *matrice = makeMatriceFromChar(doc, array);
		char c = classifyImage(matrice, doc->w, doc->h);
		printf("%c", c);
		str[*i] = c;
		++(*i);
		free(matrice);
	}
	// si on est sur une ligne il y a un espace qui separe les enfants
	else if (doc->type == 1) {
			if (doc->sons[0] != NULL)
				__docTostring(str, doc->sons[0], i, array);

			str[*i] = ' ';
			++(*i);
			printf(" ");

			if (doc->sons[1] != NULL)
				__docTostring(str, doc->sons[1], i, array);
	}
	else if (doc->type == 0) {
			if (doc->sons[0] != NULL)
				__docTostring(str, doc->sons[0], i, array);

			str[*i] = '\n';
			++(*i);
			printf("\n");

			if (doc->sons[1] != NULL)
				__docTostring(str, doc->sons[1], i, array);
	}
	else {
		for (size_t j = 0; j < doc->nbChildren; ++j) {
			__docTostring(str, doc->sons[j], i, array);
		}
	}
}

char *DocToString(struct DocumentBlocks_t *doc, int *array, char str[])
{
	/* Use the document struct to make the string that will go to txt file*/

	size_t i = 0;

	initClassifier("../NeuralNetwork/Tools/Data/OCR_network_v2");

	__docTostring(str, doc, &i, array);

	return str;
}

void WriteDebugFile(struct DocumentBlocks_t *doc, int *binArrayOnImage, size_t charNumber)
{
	FILE *filept = fopen("DocTxt/BaseDeDonne.txt", "w+");
	printf("%lu\n", charNumber);
	fprintf(filept, "%lu\n", charNumber);
	WriteBdD(filept, doc, binArrayOnImage);
	fclose(filept);

	FILE *filept2 = fopen("DocTxt/debug.txt", "w+");
	__WriteDebugFile(filept2, doc, binArrayOnImage);
	fclose(filept2);

}

int *loadImage(char *filename, size_t *w, size_t *h)
{
	SDL_Surface *image;
	image = IMG_Load(filename);

	if(!image)
		printf("IMG_Load: %s\n", IMG_GetError());

	int *binArray = matrix(filename, w, h);
	return binArray;
}

struct DocumentBlocks_t *MakeDocTreeFromImage(int *image, size_t *charNumber, size_t w, size_t h)
{
	struct DocumentBlocks_t *doc = SegmentOnBinArray(image, w, h, charNumber);

	return doc;
}
