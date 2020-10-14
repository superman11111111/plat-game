#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdint.h>

#include <time.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 400
#define REFRESH_RATE 20 
#define bool int

struct Dimensions {
	int width;
	int height;
};

typedef struct Dimensions Dimensions;

struct LTexture {
	SDL_Texture* mTexture;
	Dimensions* dim;
};

typedef struct LTexture LTexture;

SDL_Window* window;
SDL_Surface* screen;
SDL_Renderer* renderer;
LTexture texture = { NULL, NULL };
SDL_Event e;
SDL_Color textColorBlack = { 0, 0, 0 };

TTF_Font* font;
short framerate;
unsigned long int framecount;
int8_t quit;
char fps_string[10];
char fps_fstring[16];

void error(char* msg) {
	perror(msg);
	exit(1);
}

void warning(char* msg) {
	perror(msg);
}


void freeTexture() {
	if (texture.mTexture != NULL) {
		SDL_DestroyTexture(texture.mTexture);
		texture.mTexture = NULL;
		texture.dim = NULL;
	}
}


int renderText(char* text, SDL_Color textColor) {
	int success = 1;
	freeTexture();
	SDL_Surface *textSurface = TTF_RenderText_Solid(font, text, textColor);
	if (textSurface != NULL) {
		texture.mTexture = SDL_CreateTextureFromSurface(renderer, textSurface);	
		if(texture.mTexture != NULL) {
			Dimensions* dim = malloc(sizeof(Dimensions));
			dim->width = textSurface->w;
			dim->height = textSurface->h;
			texture.dim = dim;
			SDL_FreeSurface(textSurface);
		} else {
			warning("Unable to create texture from rendered text");
			success = 0;
		}
	} else { 
		warning("Unable to render text surface");
		success = 0;
	}
	return success;
}


void render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip) {
	SDL_Rect renderQ = { x, y, texture.dim->width, texture.dim->height};
	if (clip != NULL) {
		renderQ.w = clip->w;
		renderQ.h = clip->h;
	}
	SDL_RenderCopyEx(renderer, texture.mTexture, clip, &renderQ, angle, center, flip);
}


void updateFramerate() {
	
	if (framecount % REFRESH_RATE == 0 && framecount > 0 && SDL_GetTicks() > 0) {
		framerate = (framecount * 1000) / SDL_GetTicks();
		sprintf(fps_fstring, "%i%s", framerate, fps_string);
		renderText(fps_fstring, textColorBlack);
	}
}


void handleKey(SDL_Keycode key) {
	switch (key) {
		case SDLK_w:
			printf("move up\n");
			break;
		case SDLK_a:
			printf("move left\n");
			break;
		case SDLK_s:
			printf("move down\n");
			break;
		case SDLK_d:
			printf("move right\n");
			break;
		case SDLK_SPACE:
			printf("jump!\n");
			break;
		case SDLK_ESCAPE:
			quit = 1;
			break;
		default:
			printf("different key\n");
			break;
	}
}


void draw() {
	framecount++;
	/* Handle events on queue */
	while (SDL_PollEvent(&e)!=0) {
		/* SDL QUIT Event */
		if (e.type == SDL_QUIT) quit = 1;
		else if (e.type == SDL_KEYDOWN) handleKey(e.key.keysym.sym);
	}
	/* Clear screen */
	SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
	SDL_RenderClear(renderer);

	/* Update FPS */
	updateFramerate();

	/* Render current frame */
	SDL_Point point = { 100, 100 };
	SDL_Rect rect = { 0, SCREEN_HEIGHT-100, SCREEN_WIDTH, SCREEN_HEIGHT};
	SDL_SetRenderDrawColor(renderer, 102, 43, 0, 0xff);
	SDL_RenderFillRect(renderer, &rect);
	SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
/*	render((SCREEN_WIDTH-texture.dim->width)/2, (SCREEN_HEIGHT-texture.dim->height)/2, &rect, 0.0, NULL, SDL_FLIP_NONE);
*/	render((SCREEN_WIDTH-texture.dim->width), 0, NULL, 0.0, NULL, SDL_FLIP_NONE);

	/*Update screen */
	SDL_RenderPresent(renderer);

}


void setup() {
	printf("setup\n");
	/* Initialize SDL */
	if (SDL_Init(SDL_INIT_VIDEO)<0) error("Could not initialize SDL");
	/* Set texture filtering to linear */
	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) warning("Linear texture filtering is not enabled!");
	
	/* Create window */
	window = SDL_CreateWindow("PlatGame", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (window==NULL) error("Window could not be created");

	/* Create renderer for window */
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer==NULL) error("Renderer could not be created");

	/* Initialize renderer color */
	SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);

	/* Initialize SDL_ttf */
	if (TTF_Init() == -1) error("Could not initialize TTF");
	
	/* Initialize variables */
	quit = 0;
	framecount = 0;
	strcpy(fps_string, " fps");

	font = TTF_OpenFont("font/OpenSans-Regular.ttf", 24);
	if (!font) { 
		printf("Failed to load font! %s\n", TTF_GetError());
		exit(1);
	}
	if (!renderText(" ", textColorBlack)) { 
		printf("Failed to render text texture!\n");
	}
}


void destroy() {
	freeTexture();
	TTF_CloseFont(font);
	font = NULL;
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	window = NULL;
	renderer = NULL;
	TTF_Quit();
	SDL_Quit();
}


int main(int argc, char* argv[]) {
	setup();
	while (quit!=1) draw();
	destroy();
	return 0;	
}
