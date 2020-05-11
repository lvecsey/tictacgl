#ifndef TICTAC_H
#define TICTAC_H

#include <stdint.h>

enum { PIECE_TICTACE, PIECE_TICTACX, PIECE_TICTACO };

typedef struct {

  uint64_t piece_type;

  uint64_t piece_pos;
  
} tictac;

enum { TCMD_NONE, TCMD_DISPLAY, TCMD_RESETGAME };

typedef struct {

  unsigned char id[4];

  uint64_t cmd;

  uint64_t process_id;

  uint64_t numseq;

  uint64_t fwd_counter;
  
} header;

typedef struct {

  tictac positions[9];

  tictac latest_move;
  
} positionpack;

#endif
