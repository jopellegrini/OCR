#include "makeText.h"
#include <string.h>

int main()
{
	size_t h = 0;
	size_t w = 0;
	size_t charNumber = 0;

	int *matrice = loadImage("DocImages/Capture.bmp", &w, &h);

	struct DocumentBlocks_t *doc = MakeDocTreeFromImage(matrice, &charNumber, w, h);

	char string[charNumber];
	DocToString(doc, matrice, string);

	FILE *filept = fopen("outputFile.txt", "w+");
	fprintf(filept, "%s", string);
	fclose(filept);

	WriteDebugFile(doc, matrice, charNumber);

	return 0;
}
