#ifndef COLOR_TREATMENTS_H
#define COLOR_TREATMENTS_H value

//#include "SDL.h"
#include "SDL/SDL.h"
#include "DocumentBlocks.h"

int *GetMatrix(SDL_Surface *image_surface, size_t *w, size_t *h);
void Greyscale(SDL_Surface *image);
void Binarize(SDL_Surface *bitmapSurfaceImage);
int *matrix(char *path, size_t *h, size_t *w);
void _DrawSegmentation(struct DocumentBlocks_t *doc, SDL_Surface *image);
void DrawSegmentation(struct DocumentBlocks_t *doc, SDL_Surface *image);


#endif /* ifndef COLOR_TREATMENTS_H */
