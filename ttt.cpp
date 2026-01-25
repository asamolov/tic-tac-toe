/*
 * Small dumb tic-tac-toe
 */

#include "ttt.h"

#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <stdio.h>

void init_game(Game* game);
void clear_game(Game* game);
void render_bg(SDL_Renderer* renderer);
void render_game(SDL_Renderer* renderer, Game* game);
void draw_x(SDL_Renderer* renderer, size_t x, size_t y);
void draw_o(SDL_Renderer* renderer, size_t x, size_t y);
void stroke(SDL_Renderer* renderer, size_t x, size_t y, Stroke stroke);
void highlight(SDL_Renderer* renderer, size_t x, size_t y, Cell cell);
inline size_t s2x(int screen_x);
inline size_t s2y(int screen_y);
void on_click(Game* game, size_t x, size_t y);
bool update_result(Game* game);

int main(int argc, char* argv[]) {
  SDL_Init(SDL_INIT_VIDEO);
  SDL_Window* window = NULL;
  SDL_Renderer* renderer = NULL;
  window = SDL_CreateWindow("Tic Tac Toe", SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                            SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  if (!window) {
    printf("Failed to create window: %s\n", SDL_GetError());
    goto exit;
  }

  renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (!renderer) {
    printf("Failed to create renderer: %s\n", SDL_GetError());
    goto exit;
  }

  SDL_Event e;
  bool quit;
  quit = false;
  Game game;
  init_game(&game);
  while (!quit) {
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        quit = true;
      } else if (e.type == SDL_MOUSEBUTTONUP) {
        on_click(&game, s2x(e.button.x), s2y(e.button.y));
      }
    }
    render_bg(renderer);
    render_game(renderer, &game);
    SDL_RenderPresent(renderer);
  }
exit:
  if (renderer) SDL_DestroyRenderer(renderer);
  if (window) SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}

void render_bg(SDL_Renderer* renderer) {
  SDL_SetColor(renderer, COLOR_BG);
  SDL_RenderClear(renderer);
  SDL_SetColor(renderer, COLOR_GRID);
  for (int x = 1; x < FIELD_WIDTH; x++) {
    SDL_RenderDrawLine(renderer, x * CELL_WIDTH, 0, x * CELL_WIDTH,
                       SCREEN_HEIGHT);
  }
  for (int y = 1; y < FIELD_HEIGHT; y++) {
    SDL_RenderDrawLine(renderer, 0, y * CELL_HEIGHT, SCREEN_WIDTH,
                       y * CELL_HEIGHT);
  }
}

void render_game(SDL_Renderer* renderer, Game* game) {
  highlight(renderer, game->last_move.x, game->last_move.y,
            game->field[game->last_move.x][game->last_move.y]);
  for (size_t x = 0; x < FIELD_WIDTH; x++) {
    for (size_t y = 0; y < FIELD_HEIGHT; y++) {
      if (game->stroke[x][y] != NONE)
        highlight(renderer, x, y, game->field[x][y]);
      switch (game->field[x][y]) {
        case CELL_X:
          draw_x(renderer, x, y);
          break;
        case CELL_O:
          draw_o(renderer, x, y);
          break;
      }
      //stroke(renderer, x, y, game->stroke[x][y]);
    }
  }
}

void clear_game(Game* game) {
  for (size_t x = 0; x < FIELD_WIDTH; x++) {
    for (size_t y = 0; y < FIELD_HEIGHT; y++) {
      game->field[x][y] = EMPTY_CELL;
      game->stroke[x][y] = NONE;
    }
  }
  game->current = PLAYER_X;
  game->result = RESULT_DRAW;
  game->state = GAME_START;
  game->last_move.x = -1;
  game->last_move.y = -1;
}

void init_game(Game* game) {
  clear_game(game);
  game->field[0][0] = CELL_X;
  game->field[1][1] = CELL_O;
  game->field[1][2] = CELL_O;
  game->field[2][2] = CELL_X;
  game->stroke[0][0] = VERT;
  game->stroke[1][1] = HOR;
  game->stroke[1][2] = DIAG_MAIN;
  game->stroke[2][2] = DIAG_OPP;
  game->last_move.x = 1;
  game->last_move.y = 1;
}

void draw_x(SDL_Renderer* renderer, size_t x, size_t y) {
  SDL_Color c = COLOR_X;
  int center_x = CELL_WIDTH * x + CELL_WIDTH / 2;
  int center_y = CELL_HEIGHT * y + CELL_HEIGHT / 2;
  int padding = __min(CELL_HEIGHT, CELL_WIDTH) / 4;
  thickLineRGBA(renderer, center_x - padding, center_y - padding,
                center_x + padding, center_y + padding, THICKNESS, c.r, c.g,
                c.b, c.a);
  thickLineRGBA(renderer, center_x - padding, center_y + padding,
                center_x + padding, center_y - padding, THICKNESS, c.r, c.g,
                c.b, c.a);
}

void highlight(SDL_Renderer* renderer, size_t x, size_t y, Cell cell) {
  SDL_Color c = COLOR_BG;
  SDL_Color bg = COLOR_BG;
  switch (cell) {
    case EMPTY_CELL:
      return;
    case CELL_X:
      c = COLOR_X;
      break;
    case CELL_O:
      c = COLOR_O;
      break;
  }
  c.a /= 2;
  boxRGBA(renderer, x * CELL_WIDTH, y * CELL_HEIGHT, (x + 1) * CELL_WIDTH,
          (y + 1) * CELL_HEIGHT, c.r, c.g, c.b, c.a);
  boxRGBA(renderer, x * CELL_WIDTH + THICKNESS, y * CELL_HEIGHT + THICKNESS,
          (x + 1) * CELL_WIDTH - THICKNESS, (y + 1) * CELL_HEIGHT - THICKNESS,
          bg.r, bg.g, bg.b, bg.a);
}

void stroke(SDL_Renderer* renderer, size_t x, size_t y, Stroke stroke) {
  int start_x, start_y, end_x, end_y;
  switch (stroke) {
    case NONE:
      return;
    case VERT:
      start_x = end_x = CELL_WIDTH * x + CELL_WIDTH / 2;
      start_y = CELL_HEIGHT * y;
      end_y = start_y + CELL_HEIGHT;
      break;
    case HOR:
      start_x = CELL_WIDTH * x;
      end_x = start_x + CELL_WIDTH;
      start_y = end_y = CELL_HEIGHT * y + CELL_HEIGHT / 2;
      break;
    case DIAG_MAIN:
      start_x = CELL_WIDTH * x;
      start_y = CELL_HEIGHT * y;
      end_x = start_x + CELL_WIDTH;
      end_y = start_y + CELL_HEIGHT;
      /* code */
      break;
    case DIAG_OPP:
      start_x = CELL_WIDTH * x;
      end_y = CELL_HEIGHT * y;
      end_x = start_x + CELL_WIDTH;
      start_y = end_y + CELL_HEIGHT;
      /* code */
      break;
  }
  SDL_Color c = COLOR_STROKE;
  thickLineRGBA(renderer, start_x, start_y, end_x, end_y, THICKNESS / 2, c.r,
                c.g, c.b, c.a);
}

void draw_o(SDL_Renderer* renderer, size_t x, size_t y) {
  int center_x = CELL_WIDTH * x + CELL_WIDTH / 2;
  int center_y = CELL_HEIGHT * y + CELL_HEIGHT / 2;
  int padding = __min(CELL_HEIGHT, CELL_WIDTH) / 4;
  filledCircleRGBA(renderer, center_x, center_y, padding + THICKNESS, COLOR_O.r,
                   COLOR_O.g, COLOR_O.b, COLOR_O.a);
  filledCircleRGBA(renderer, center_x, center_y, padding, COLOR_BG.r,
                   COLOR_BG.g, COLOR_BG.b, COLOR_BG.a);
}

void on_click(Game* game, size_t x, size_t y) {
  // if game is stopped -
  switch (game->state) {
    case GAME_END:
    case GAME_START:
      clear_game(game);
      game->state = GAME_IN_PROGRESS;
      break;
    case GAME_IN_PROGRESS:
      // clicked on field
      if (game->field[x][y] != EMPTY_CELL) break;
      // fill the cell
      game->field[x][y] = game->current == PLAYER_X ? CELL_X : CELL_O;
      game->last_move.x = x;
      game->last_move.y = y;
      // next player
      game->current = game->current == PLAYER_X ? PLAYER_O : PLAYER_X;
      if (update_result(game)) {
        game->state = GAME_END;
      }
      break;
  }
}

bool has_more_moves(Game* game) {
  for (size_t x = 0; x < FIELD_WIDTH; x++) {
    for (size_t y = 0; y < FIELD_HEIGHT; y++) {
      if (game->field[x][y] == EMPTY_CELL) return true;
    }
  }
  return false;
}

// true = game is over, false = otherwise
bool update_result(Game* game) {
  // check by row
  bool can_win;
  for (size_t x = 0; x < FIELD_WIDTH; x++) {
    can_win = true;
    if (game->field[x][0] == EMPTY_CELL) continue;
    for (size_t y = 1; y < FIELD_HEIGHT; y++) {
      if (game->field[x][y - 1] != game->field[x][y]) {
        can_win = false;
        break;
      }
    }
    if (can_win) {
      game->result = game->field[x][0] == CELL_X ? RESULT_X_WON : RESULT_O_WON;
      for (size_t y = 0; y < FIELD_HEIGHT; y++) {
        game->stroke[x][y] = VERT;
      }
      return true;
    }
  }
  // check by col
  for (size_t y = 0; y < FIELD_HEIGHT; y++) {
    can_win = true;
    if (game->field[0][y] == EMPTY_CELL) continue;
    for (size_t x = 1; x < FIELD_WIDTH; x++) {
      if (game->field[x - 1][y] != game->field[x][y]) {
        can_win = false;
        break;
      }
    }
    if (can_win) {
      game->result = game->field[0][y] == CELL_X ? RESULT_X_WON : RESULT_O_WON;
      for (size_t x = 0; x < FIELD_WIDTH; x++) {
        game->stroke[x][y] = HOR;
      }
      return true;
    }
  }

  // check diag
  if (FIELD_HEIGHT == FIELD_WIDTH) {
    // main diag
    can_win = game->field[0][0] != EMPTY_CELL;
    for (size_t i = 1; i < FIELD_HEIGHT && can_win; i++) {
      if (game->field[i - 1][i - 1] != game->field[i][i]) {
        can_win = false;
      }
    }
    if (can_win) {
      game->result = game->field[0][0] == CELL_X ? RESULT_X_WON : RESULT_O_WON;
      for (size_t i = 0; i < FIELD_HEIGHT; i++) {
        game->stroke[i][i] = DIAG_MAIN;
      }
      return true;
    }
    // opposite diag
    can_win = game->field[0][FIELD_HEIGHT - 1] != EMPTY_CELL;
    for (size_t i = 1; i < FIELD_HEIGHT; i++) {
      if (game->field[i - 1][FIELD_HEIGHT - i] !=
          game->field[i][FIELD_HEIGHT - i - 1]) {
        can_win = false;
      }
    }
    if (can_win) {
      game->result = game->field[0][FIELD_HEIGHT - 1] == CELL_X ? RESULT_X_WON
                                                                : RESULT_O_WON;
      for (size_t i = 0; i < FIELD_HEIGHT; i++) {
        game->stroke[i][FIELD_HEIGHT - 1 - i] = DIAG_OPP;
      }
      return true;
    }
  }

  if (!has_more_moves(game)) {
    game->result = RESULT_DRAW;
    return true;
  }

  return false;
}

inline size_t s2x(int screen_x) { return screen_x / CELL_WIDTH; }

inline size_t s2y(int screen_y) { return screen_y / CELL_HEIGHT; }
