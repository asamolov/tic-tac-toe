/*
 * Small dumb tic-tac-toe
 */

#include "ttt.h"

#include <raylib.h>
#include <stdbool.h>
#include <stdlib.h>

void init_game(Game* game);
void clear_game(Game* game);
void render_bg();
void render_game(Game* game);
RenderTexture2D prepare_x();
RenderTexture2D prepare_o();
void draw_x(size_t x, size_t y);
void draw_o(size_t x, size_t y);
void stroke(size_t x, size_t y, Stroke stroke);
void highlight(size_t x, size_t y, Cell cell);
inline size_t s2x(int screen_x);
inline size_t s2y(int screen_y);
void on_click(Game* game, size_t x, size_t y);
bool update_result(Game* game);

Shader shader = {0};
RenderTexture2D tex_x = {0};
RenderTexture2D tex_o = {0};

int main(int argc, char* argv[]) {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tic Tac Toe");
  if (!IsWindowReady()) {
    TraceLog(LOG_ERROR, "Failed to create window");
    goto exit;
  }

  shader = LoadShader(0, "shaders/bloom.fs");
  if (!IsShaderReady(shader)) {
    TraceLog(LOG_ERROR, "Failed to load shader");
    goto exit;
  }
  tex_x = prepare_x();
  tex_o = prepare_o();
  Game game;
  init_game(&game);
  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
      on_click(&game, s2x(GetMouseX()), s2y(GetMouseY()));
    }
    BeginDrawing();
    render_bg();
    render_game(&game);
    DrawFPS(0, 0);
    EndDrawing();
  }
exit:
  UnloadShader(shader);
  UnloadRenderTexture(tex_x);
  UnloadRenderTexture(tex_o);
  CloseWindow();
  return 0;
}

void render_bg() {
  ClearBackground(COLOR_BG);
  for (int x = 1; x < FIELD_WIDTH; x++) {
    DrawLine(x * CELL_WIDTH, 0, x * CELL_WIDTH, SCREEN_HEIGHT, COLOR_GRID);
  }
  for (int y = 1; y < FIELD_HEIGHT; y++) {
    DrawLine(0, y * CELL_HEIGHT, SCREEN_WIDTH, y * CELL_HEIGHT, COLOR_GRID);
  }
}

void render_game(Game* game) {
  highlight(game->last_move.x, game->last_move.y,
            game->field[game->last_move.x][game->last_move.y]);
  for (size_t x = 0; x < FIELD_WIDTH; x++) {
    for (size_t y = 0; y < FIELD_HEIGHT; y++) {
      if (game->stroke[x][y] != NONE) highlight(x, y, game->field[x][y]);
      // BeginShaderMode(shader);
      switch (game->field[x][y]) {
        case CELL_X:
          draw_x(x, y);
          break;
        case CELL_O:
          draw_o(x, y);
          break;
      }
      // EndShaderMode();
      //  stroke(renderer, x, y, game->stroke[x][y]);
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

RenderTexture2D prepare_x() {
  RenderTexture2D texture = LoadRenderTexture(CELL_WIDTH, CELL_HEIGHT);
  Color c = COLOR_X;
  int center_x = CELL_WIDTH / 2;
  int center_y = CELL_HEIGHT / 2;
  int padding = __min(CELL_HEIGHT, CELL_WIDTH) / 4;
  BeginTextureMode(texture);
  DrawLineEx(CLITERAL(Vector2){center_x - padding, center_y - padding},
             CLITERAL(Vector2){center_x + padding, center_y + padding},
             THICKNESS, c);
  DrawLineEx(CLITERAL(Vector2){center_x - padding, center_y + padding},
             CLITERAL(Vector2){center_x + padding, center_y - padding},
             THICKNESS, c);
  EndTextureMode();
  return texture;
}

void draw_x(size_t x, size_t y) {
  DrawTexture(tex_x.texture, CELL_WIDTH * x, CELL_HEIGHT * y, WHITE);
}

void highlight(size_t x, size_t y, Cell cell) {
  Color c = COLOR_BG;
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
  DrawRectangleLinesEx(CLITERAL(Rectangle){x * CELL_WIDTH, y * CELL_HEIGHT,
                                           CELL_WIDTH, CELL_HEIGHT},
                       THICKNESS, c);
}

void stroke(size_t x, size_t y, Stroke stroke) {
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
  Color c = COLOR_STROKE;
  DrawLineEx(CLITERAL(Vector2){start_x, start_y},
             CLITERAL(Vector2){end_x, end_y}, THICKNESS / 2, c);
}

RenderTexture2D prepare_o() {
  RenderTexture2D texture = LoadRenderTexture(CELL_WIDTH, CELL_HEIGHT);
  int center_x = CELL_WIDTH / 2;
  int center_y = CELL_HEIGHT / 2;
  int padding = __min(CELL_HEIGHT, CELL_WIDTH) / 4;
  BeginTextureMode(texture);
  DrawCircle(center_x, center_y, padding + THICKNESS, COLOR_O);
  DrawCircle(center_x, center_y, padding, COLOR_BG);
  EndTextureMode();
  return texture;
}

void draw_o(size_t x, size_t y) {
  DrawTexture(tex_o.texture, CELL_WIDTH * x, CELL_HEIGHT * y, WHITE);
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

size_t s2x(int screen_x) { return screen_x / CELL_WIDTH; }

size_t s2y(int screen_y) { return screen_y / CELL_HEIGHT; }
