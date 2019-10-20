#include <err.h>
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "pixel_operations.h"


void init_sdl()
{
    // Init only the video part.
    // If it fails, die with an error message.
    if(SDL_Init(SDL_INIT_VIDEO) == -1)
        errx(1,"Could not initialize SDL: %s.\n", SDL_GetError());
}

SDL_Surface* load_image(char *path)
{
    SDL_Surface *img;

    // Load an image using SDL_image with format detection.
    // If it fails, die with an error message.
    img = IMG_Load(path);
    if (!img)
        errx(3, "can't load %s: %s", path, IMG_GetError());

    return img;
}

SDL_Surface* display_image(SDL_Surface *img)
{
    SDL_Surface *screen;

    // Set the window to the same size as the image
    screen = SDL_SetVideoMode(img->w, img->h, 0, SDL_SWSURFACE|SDL_ANYFORMAT);
    if (screen == NULL)
    {
        // error management
        errx(1, "Couldn't set %dx%d video mode: %s\n",
                img->w, img->h, SDL_GetError());
    }

    // Blit onto the screen surface
    if(SDL_BlitSurface(img, NULL, screen, NULL) < 0)
        warnx("BlitSurface error: %s\n", SDL_GetError());

    // Update the screen
    SDL_UpdateRect(screen, 0, 0, img->w, img->h);

    // return the screen for further uses
    return screen;
}

void wait_for_keypressed()
{
    SDL_Event event;

    // Wait for a key to be down.
    do
    {
        SDL_PollEvent(&event);
    } while(event.type != SDL_KEYDOWN);

    // Wait for a key to be up.
    do
    {
        SDL_PollEvent(&event);
    } while(event.type != SDL_KEYUP);
}

void InitMatrix(SDL_Surface* image_surface, Uint8* matrix)
{
	for(int i=0; i < image_surface->h; ++i)
	{
		// width
		for(int j=0; j < image_surface->w; ++j)
		{
			Uint32 pixel = get_pixel(image_surface, j, i);
			matrix[i * image_surface->w + j] = (pixel >= 110) ? 255 : 0;
		}

	}
	
}

void SDL_FreeSurface(SDL_Surface *surface);

int main()
{
	SDL_Surface* image_surface;
    SDL_Surface* screen_surface;
	

	init_sdl();
	image_surface = load_image("/home/hugo-buissez/serm.jpg");
	// Displaying the image
	screen_surface = display_image(image_surface);
	
	Uint8 mat1[image_surface->h * image_surface->w];
	wait_for_keypressed();


	// Greyscale the picture
	for(int i=0; i < image_surface->h; ++i){
		for(int j=0; j < image_surface->w; ++j){
			Uint32 pixel = get_pixel(image_surface, j, i);
			Uint8 r, g, b;
			// Modify r, g, b in memory
			SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
			Uint8 a = 0.3*r + 0.59*g + 0.11*b;
			Uint32 new_pixel = SDL_MapRGB(image_surface->format, a, a, a);
			// Put the new pixel at the place of the current one
			put_pixel(image_surface, j, i, new_pixel);

		}


	}



	// Supposed to binarise the picture
	/* for(int i=0; i < image_surface->h; ++i){
		// width
		for(int j=0; j < image_surface->w; ++j){
			Uint32 pixel = get_pixel(image_surface, j, i);
			Uint8 r, g, b;
			// Modify r, g, b in memory
			SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
			// If the pixel value >= 122 then white, else black
			Uint8 a = (pixel>=122) ? 255 : 0;
			Uint32 new_pixel = SDL_MapRGB(image_surface->format, a, a, a);
			// Put the new pixel at the place of the current one
			put_pixel(image_surface, j, i, new_pixel);

		}


	} */

	update_surface(screen_surface, image_surface);
	

	InitMatrix(image_surface, mat1);

	wait_for_keypressed();
	SDL_FreeSurface(image_surface);
	SDL_FreeSurface(screen_surface);


    return 0;
}









