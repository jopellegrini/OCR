#include <stdlib.h>
#include <stdio.h>
#include "SDL.h"
#include "SDL_image.h"
#include "color_treatments.h"

int main(int argc, char **argv)
{		
	SDL_Surface *image;
	
	if(argc < 2)
	{
		printf("Too few parameters, form: ./exe path_to_the_picture\n");
		return 1;
	}
	
	image = IMG_Load(argv[1]);
	if(!image) 
		printf("IMG_Load: %s\n", IMG_GetError());

	display_image(image);
	
	Greyscale(image);
	
	display_image(image);

	Binarize(image);

	display_image(image);
	
	return 0;
}
