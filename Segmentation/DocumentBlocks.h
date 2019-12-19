#ifndef DOCUMENT_BLOCK_H
#define DOCUMENT_BLOCK_H value

#include <stdlib.h>

typedef enum TYPE {
	BLOCK,
	LIGNE,
	WORD,
	CHAR,
}TYPE;

struct DocumentBlocks_t {
	/* data */
	size_t x, y, w, h, nbChildren, DocH, DocW;

	int isUpper;

	TYPE type;

	int **binArray;

	void **sons;
};


#endif /* ifndef DOCUMENT_BLOCK_H */
