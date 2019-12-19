// image_operations.h

# ifndef IMAGE_OPERATIONS_H_
# define IMAGE_OPERATIONS_H_

# include <stdlib.h>
# include <SDL/SDL.h>
//# include <SDL.h>
# include <err.h>
# include "pixel_operations.h"

void wait_for_keypressed(void);
void noiseReduction(SDL_Surface *img);
SDL_Surface* display_image(SDL_Surface *img);

# endif
