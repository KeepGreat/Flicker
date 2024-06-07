#ifndef ED_CONST_H
#define ED_CONST_H

#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include "SDL2/SDL_mixer.h"
#include "SDL2/SDL_ttf.h"
#include <ctime>

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800
#define FPS 60
#define mSPF 1000 / FPS

#define ENTITY_WIDTH 100
#define ENTITY_HEIGHT 100
#define BULLET_WIDTH 20
#define BULLET_HEIGHT 20

#define PLAYER 5
#define PHASE 5
#define COIN 200

#define TURRET_COST 100
#define TURRET_REALM 400
#define TURRET_DAMAGE 1
#define BULLET_SPEED 10
#define BULLET_DAMAGE 1
#define FROST_COST 100
#define FROST_REALM 200
#define FROST_DAMAGE 0

#define ENERMY_DE_HEALTH 4
#define ENERMY_DE_SPEED 5

#define TEXT_NUMBER 4

SDL_Window *win = NULL;
SDL_Renderer *renderer = NULL;
SDL_Color de_color = {200, 200, 200, 255};
TTF_Font *font = NULL;
Mix_Music *bgm = NULL;
int GameState = 1;
int IsPause = 0;
int Player = PLAYER;
int Level = 0;
int MaxLevel = 0;
int Phase = PHASE;
int Coin = COIN;
int PageIndex = 1;
int ConsoleMode = 0;
clock_t StartTick = clock();
int ElimiCount = 0;
char filename[20]; //你居然会影响下面的int变量初始化!?

#endif