#include <err.h>
//#include <SDL.h>
//#include <SDL_image.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <stdio.h>
#include "pixel_operations.h"
#include "color_treatments.h"
#include "DocumentBlocks.h"
#include "median_filter.h"

#define BLANC 0
#define NOIR 1

int *GetMatrix(SDL_Surface *image_surface, size_t *w, size_t *h)
{
	*w = image_surface->w;
	*h = image_surface->h;
	int *matrix = calloc((*w * *h), sizeof(int));

    for(size_t i=0; i < *h; ++i)
    {
        for(size_t j=0; j < *w; ++j)
        {
            Uint32 pixel = get_pixel(image_surface, j, i);
			Uint8 r, g, b;
			SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
            matrix[i * image_surface->w + j] = (r == 0) ? BLANC: NOIR;
        }
    }

	return matrix;
}

void _DrawSegmentation(struct DocumentBlocks_t *doc, SDL_Surface *image)
{
	if (doc->type == 3) {
		for (size_t j = doc->y; j < doc->h + doc->y; j++) {
			for (size_t i = doc->x; i < doc->w + doc->x; ++i) {
				Uint32 pixel = SDL_MapRGB(image->format, 255, 0, 0);
				put_pixel(image, i, j, pixel);
			}
		}
	}

	for (size_t i = 0; i < doc->nbChildren; ++i) {
		_DrawSegmentation(doc->sons[i], image);
	}
}

void DrawSegmentation(struct DocumentBlocks_t *doc, SDL_Surface *image)
{
	_DrawSegmentation(doc, image);
	display_image(image);
}

void Greyscale(SDL_Surface *image)
{
    for(int i=0; i < image->h; ++i)
	{
        for(int j=0; j < image->w; ++j)
		{
            Uint32 pixel = get_pixel(image, j, i);
            Uint8 r, g, b;

			// Modify r, g, b in memory
            SDL_GetRGB(pixel, image->format, &r, &g, &b);
//            Uint8 a = 0.3*r + 0.59*g + 0.11*b;
			Uint8 a = (r + g + b) / 3;
            Uint32 new_pixel = SDL_MapRGB(image->format, a, a, a);

			// Put the new pixel at the place of the current one
            put_pixel(image, j, i, new_pixel);
        }
	}
}

void Binarize(SDL_Surface *bitmapSurfaceImage)
{
    for(int i=0; i < bitmapSurfaceImage->h; ++i)
	{
        for(int j=0; j < bitmapSurfaceImage->w; ++j)
		{
            Uint32 pixel = get_pixel(bitmapSurfaceImage, j, i);
            Uint8 r, g, b;

			// Modify r, g, b in memory
            SDL_GetRGB(pixel, bitmapSurfaceImage->format, &r, &g, &b);

			// If the pixel value >= 122 then white, else black
            Uint8 a = ((r+g+b)/3 >= 200) ? 255 : 0;
            Uint32 new_pixel = SDL_MapRGB(bitmapSurfaceImage->format, a, a, a);

			// Put the new pixel at the place of the current one
            put_pixel(bitmapSurfaceImage, j, i, new_pixel);
        }
    }
}

int *matrix(char *path, size_t *h, size_t *w)
{
	SDL_Surface *image_surface = SDL_LoadBMP(path);

	Greyscale(image_surface);
	Binarize(image_surface);

	return GetMatrix(image_surface, h, w);
}
