#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>


void clean_ressources(SDL_Window *w, SDL_Renderer *r, SDL_Texture *t){

	if(t != NULL)
		SDL_DestroyTexture(t);
	if(r != NULL)
		SDL_DestroyRenderer(r);
	if(w != NULL)
		SDL_DestroyWindow(w);

	SDL_Quit();

}


void initWindow(){
	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;
	SDL_Surface *picture =  NULL;
	SDL_Texture *texture = NULL;
	SDL_Rect dest_rect = {0, 0, 640, 480};

	SDL_Texture *img = NULL;

	int w,h;

	if(SDL_Init(SDL_INIT_VIDEO) != 0){

		SDL_Log("error > %s\n", SDL_GetError());
		clean_ressources(NULL, NULL, NULL);
		exit(EXIT_FAILURE);
	}

	window = SDL_CreateWindow("Display picture test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1920, 1080, SDL_WINDOW_MAXIMIZED);


	if(window == NULL){

		SDL_Log("error > %s\n", SDL_GetError());
		clean_ressources(NULL, NULL, NULL);
		exit(EXIT_FAILURE);

	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
	if(renderer == NULL){

		SDL_Log("error > %s\n", SDL_GetError());
		clean_ressources(window, NULL, NULL);
		exit(EXIT_FAILURE);

	}




	/* picture = SDL_LoadBMP("test_picture_bitmap.bmp");
	if(picture == NULL){

		SDL_Log("error > %s\n", SDL_GetError());
		clean_ressources(window, renderer, NULL);
		exit(EXIT_FAILURE);

	}

	texture = SDL_CreateTextureFromSurface(renderer, picture);
	SDL_FreeSurface(picture);

	if(texture == NULL){

		SDL_Log("error > %s\n", SDL_GetError());
		clean_ressources(window, renderer, NULL);
		exit(EXIT_FAILURE);

	}

	if(SDL_QueryTexture(texture, NULL, NULL, &dest_rect.w, &dest_rect.h) != 0){

		SDL_Log("error > %s\n", SDL_GetError());
		clean_ressources(window, renderer, texture);
		exit(EXIT_FAILURE);
	} 

	if(SDL_RenderCopy(renderer, texture, NULL, &dest_rect) != 0){
		
		SDL_Log("error > %s\n", SDL_GetError());
		clean_ressources(window, renderer, texture);
		exit(EXIT_FAILURE);
	}*/

    SDL_SetRenderDrawColor(renderer, 255, 249, 242, 255);

    SDL_RenderClear(renderer);


	SDL_RenderPresent(renderer);

	SDL_Event e;

	while (1){
		if (SDL_PollEvent(&e))
		{
			if (SDL_QUIT == e.type)
			{
				break;
			}
		}
	}
	clean_ressources(window, renderer, texture);

	SDL_Quit();

}


