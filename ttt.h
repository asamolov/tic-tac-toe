#pragma once

#include <SDL.h>

const SDL_Color COLOR_X = {184, 63, 63, SDL_ALPHA_OPAQUE};
const SDL_Color COLOR_O = {96, 209, 212, SDL_ALPHA_OPAQUE};
const SDL_Color COLOR_BG = {0x11, 0x11, 0x11, SDL_ALPHA_OPAQUE};
const SDL_Color COLOR_GRID = {0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE};
const SDL_Color COLOR_STROKE = {0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE / 2};

#define SDL_SetColor(renderer, color)                                 \
  SDL_SetRenderDrawColor((renderer), (color).r, (color).g, (color).b, \
                         (color).a);

const int THICKNESS = 20;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;

const int FIELD_WIDTH = 3;
const int FIELD_HEIGHT = 3;

const int CELL_WIDTH = SCREEN_WIDTH / FIELD_WIDTH;
const int CELL_HEIGHT = SCREEN_HEIGHT / FIELD_HEIGHT;

typedef enum { CELL_X = 'x', CELL_O = 'o', EMPTY_CELL = ' ' } Cell;

typedef enum { PLAYER_X = 'x', PLAYER_O = 'o'} Player;

typedef enum { RESULT_X_WON, RESULT_O_WON, RESULT_DRAW } Result;

typedef enum { GAME_START, GAME_IN_PROGRESS, GAME_END } State;

typedef enum { VERT = '|', HOR = '-', DIAG_MAIN = '\\', DIAG_OPP = '/', NONE = ' ' } Stroke;


typedef struct {	
  Cell field[FIELD_WIDTH][FIELD_HEIGHT];
  Stroke stroke[FIELD_WIDTH][FIELD_HEIGHT];
  Player current;
  Result result;
  State state;
  struct LastMove {
	int x; int y;
  } last_move;
} Game;