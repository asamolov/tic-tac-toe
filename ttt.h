#pragma once

#include <raylib.h>
#include <stdlib.h>

#ifndef __min
#define __min(a, b)         \
  ({                        \
    __typeof__(a) _a = (a); \
    __typeof__(b) _b = (b); \
    _a < _b ? _a : _b;      \
  })
#endif

const Color COLOR_X = {184, 63, 63, 0xFF};
const Color COLOR_O = {96, 209, 212, 0xFF};
const Color COLOR_BG = {0x11, 0x11, 0x11, 0xFF};
const Color COLOR_GRID = {0xFF, 0xFF, 0xFF, 0xFF};
const Color COLOR_STROKE = {0xFF, 0xFF, 0xFF, 0xEF};

const int THICKNESS = 20;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;

#define FIELD_WIDTH 3
#define FIELD_HEIGHT 3

#define CELL_WIDTH (SCREEN_WIDTH / FIELD_WIDTH)
#define CELL_HEIGHT (SCREEN_HEIGHT / FIELD_HEIGHT)

typedef enum { CELL_X = 'x', CELL_O = 'o', EMPTY_CELL = ' ' } Cell;

typedef enum { PLAYER_X = 'x', PLAYER_O = 'o' } Player;

typedef enum { RESULT_X_WON, RESULT_O_WON, RESULT_DRAW } Result;

typedef enum { GAME_START, GAME_IN_PROGRESS, GAME_END } State;

typedef enum {
  VERT = '|',
  HOR = '-',
  DIAG_MAIN = '\\',
  DIAG_OPP = '/',
  NONE = ' '
} Stroke;

typedef struct {
  Cell field[FIELD_WIDTH][FIELD_HEIGHT];
  Stroke stroke[FIELD_WIDTH][FIELD_HEIGHT];
  Player current;
  Result result;
  State state;
  struct LastMove {
    size_t x;
    size_t y;
  } last_move;
} Game;