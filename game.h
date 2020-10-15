typedef struct Dimensions {
	int width;
	int height;
} Dimensions;

typedef struct LTexture {
	SDL_Texture* mTexture;
	int width;
	int height;
} LTexture;

typedef struct Player {
	int width;
	int height;
	int x;
	int y;
	int vx;
	int vy;
	SDL_Color* color;
} Player; 

typedef struct Player Player;
