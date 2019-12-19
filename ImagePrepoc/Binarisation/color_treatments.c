#include <err.h>
//#include <SDL.h>
//#include <SDL_image.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <stdio.h>
#include "pixel_operations.h"
#include "color_treatments.h"

#define BLANC 0
#define NOIR 1

void wait_for_keypressed(void)
{
    SDL_Event event;
    for (;;) {
      SDL_PollEvent(&event);
      switch (event.type)
      {
        case SDL_KEYDOWN: return;
        default: break;
      }
    }
}

SDL_Surface* display_image(SDL_Surface *img)
{
    SDL_Surface *screen;
    screen = SDL_SetVideoMode(img->w, img->h, 0, SDL_SWSURFACE|SDL_ANYFORMAT);
    if (screen == NULL) {
      errx(1, "Couldn't set %dx%d video mode: %s\n",
           img -> w, img -> h, SDL_GetError());
    }
    if(SDL_BlitSurface(img, NULL, screen, NULL) < 0)
      warnx("BlitSurface error: %s\n", SDL_GetError());
    SDL_UpdateRect(screen, 0, 0, img -> w, img -> h);
    wait_for_keypressed();
    return screen;
}

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
            Uint8 a = ((r+g+b)/3 >= 160) ? 255 : 0;
            Uint32 new_pixel = SDL_MapRGB(bitmapSurfaceImage->format, a, a, a);

			// Put the new pixel at the place of the current one
            put_pixel(bitmapSurfaceImage, j, i, new_pixel);
        }
    }
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
            // Uint8 a = 0.3*r + 0.59*g + 0.11*b;
			Uint8 a = (r + g + b) / 3;
            Uint32 new_pixel = SDL_MapRGB(image->format, a, a, a);

			// Put the new pixel at the place of the current one
            put_pixel(image, j, i, new_pixel);
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
