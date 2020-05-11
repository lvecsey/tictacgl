#ifndef TICTACGAME_H
#define TICTACGAME_H

#include <stdint.h>

#include "tictaclist.h"

enum { TICTAC_NONE, TICTAC_GAMEACTIVE, TICTAC_GAMEOVER };

typedef struct {

  uint64_t game_state;
  
  struct tictaclist tl_root;

  uint64_t piecetype_nextplay;
  
} tictacgame;

#endif
