#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdint.h>

#include <time.h>
#include "game.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 400
#define REFRESH_RATE 20 
#define SPEED 10

SDL_Window* window;
SDL_Surface* screen;
SDL_Renderer* renderer;
LTexture* fpsTexture; 
LTexture* playerTexture;
SDL_Event e;
SDL_Color textColorBlack = { 0, 0, 0 };
Player player = {10, 10, 0, 0, 0, 0, NULL};

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


LTexture* freeTexture(LTexture* texture) {
	if (!texture) texture = malloc(sizeof *texture);
	if (texture->mTexture != NULL) {
		SDL_DestroyTexture(texture->mTexture);
		texture->mTexture = NULL;
		texture->width = 0;
		texture->height = 0;
	}
	return texture;
}


LTexture* loadImgToTexture(LTexture* texture, char* path) {
	texture = freeTexture(texture);
	SDL_Texture* newTexture;
	SDL_Surface* imgSurface = IMG_Load(path);
	if (imgSurface) {
		SDL_SetColorKey(imgSurface, SDL_TRUE, SDL_MapRGB(imgSurface->format,0,0xff,0xff));
		newTexture = SDL_CreateTextureFromSurface(renderer, imgSurface);
		if (!newTexture) {
			printf("Unable to create texture %s\n", SDL_GetError());
		} else {
			texture->width = imgSurface->w;
			texture->height = imgSurface->h;
		}
		SDL_FreeSurface(imgSurface);
	} else printf("Unable to load image %s\n", IMG_GetError());
	
	texture->mTexture = newTexture;
	return texture;
}


LTexture* renderTextToTexture(LTexture* texture, char* text, SDL_Color textColor) {
	printf("renderTextToTexture\n");
	int success = 1;
	texture = freeTexture(texture);
	SDL_Surface *textSurface = TTF_RenderText_Solid(font, text, textColor);
	if (textSurface) {
		texture->mTexture = SDL_CreateTextureFromSurface(renderer, textSurface);	
		if(texture->mTexture != NULL) {
			texture->width = textSurface->w;
			texture->height = textSurface->h;
			SDL_FreeSurface(textSurface);
		} else {
			warning("Unable to create texture from rendered text");
			success = 0;
		}
	} else { 
		warning("Unable to render text surface");
		success = 0;
	}
	return texture;
}


void renderTexture(LTexture* texture, int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip) {
	SDL_Rect renderQ = { x, y, texture->width, texture->height};
	if (clip != NULL) {
		renderQ.w = clip->w;
		renderQ.h = clip->h;
	}
	SDL_RenderCopyEx(renderer, texture->mTexture, clip, &renderQ, angle, center, flip);
}
/*
void render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip) {
	SDL_Rect renderQ = { x, y, texture.dim->width, texture.dim->height};
	if (clip != NULL) {
		renderQ.w = clip->w;
		renderQ.h = clip->h;
	}
	SDL_RenderCopyEx(renderer, texture.mTexture, clip, &renderQ, angle, center, flip);
}
*/

void updateFramerateAndTexture() {
	if (framecount % REFRESH_RATE == 0 && framecount > 0 && SDL_GetTicks() > 0) {
		printf("updateFPS\n");	
		framerate = (framecount * 1000) / SDL_GetTicks();
		sprintf(fps_fstring, "%i%s", framerate, fps_string);
		fpsTexture = renderTextToTexture(fpsTexture, fps_fstring, textColorBlack);
	}
}


void handleKey(SDL_Keycode key) {
	printf("HandleKey\n");
	switch (key) {
		case SDLK_w:
			printf("move up\n");
			player.vy = -SPEED;
			break;
		case SDLK_a:
			printf("move left\n");
			player.vx = -SPEED;
			break;
		case SDLK_s:
			printf("move down\n");
			player.vy = SPEED;
			break;
		case SDLK_d:
			printf("move right\n");
			player.vx = SPEED;
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


void movePlayer() {
	player.x += player.vx;
	if ((player.x < 0) || (player.x + 2 * player.width > SCREEN_WIDTH)) {
		player.x -= player.vx;
	}
	player.y += player.vy;
	if ((player.y<0) || (player.y+ 2 * player.height > SCREEN_HEIGHT)) {
		player.y -= player.vy;
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
	updateFramerateAndTexture();
	if (fpsTexture)	renderTexture(fpsTexture, (SCREEN_WIDTH-fpsTexture->width), 0, NULL, 0.0, NULL, SDL_FLIP_NONE);

	/* Render ground */
	SDL_Point point = { 100, 100 };
	SDL_Rect rect = { 0, SCREEN_HEIGHT-100, SCREEN_WIDTH, SCREEN_HEIGHT};
	SDL_SetRenderDrawColor(renderer, 102, 43, 0, 0xff);
	SDL_RenderFillRect(renderer, &rect);
	SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);

	/* Render player */
/*	printf("%i%i%i%i\n", player.x, player.y, player.vx, player.vy);
*/	movePlayer(player);
	if (playerTexture) {
		renderTexture(playerTexture, player.x, player.y, NULL, 0.0, NULL, SDL_FLIP_NONE);
	
	}
/*	render((SCREEN_WIDTH-texture.dim->width)/2, (SCREEN_HEIGHT-texture.dim->height)/2, &rect, 0.0, NULL, SDL_FLIP_NONE);
*/
	
	/*Update screen */
	SDL_RenderPresent(renderer);

}


void setup() {
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

	/* Initialize PNG loading */
	if (!(IMG_Init(IMG_INIT_PNG)&IMG_INIT_PNG)) error("Could not initialize SDL_image");

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

	/* Initialize Player */
	playerTexture = loadImgToTexture(playerTexture, "img/player.bmp");
	printf("setup\n");
	if (playerTexture) printf("things\n");
/*
	fpsTexture = renderTextToTexture(fpsTexture, "teasdasd", textColorBlack);
	if (fpsTexture = renderTextToTexture(fpsTexture, " test 123", textColorBlack)) { 
		printf("Failed to render text texture!\n");
	}
*/}


void destroy() {
	freeTexture(playerTexture);
	freeTexture(fpsTexture);
	free(playerTexture);
	free(fpsTexture);
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

