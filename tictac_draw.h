#ifndef TICTAC_DRAW_H
#define TICTAC_DRAW_H

#include "mini_gxkit.h"

#include "tictac.h"

typedef struct {

  long int nend;

  double spangrid;
  
} drawpack;

int tictac_drawpiece(image_t *img, drawpack *dp, tictac *position, pixel_t fill_color);

int tictac_draw(image_t *img, drawpack *dp, tictac *positions, pixel_t fill_color);

enum { WL_ACROSS1, WL_ACROSS2, WL_ACROSS3, WL_VERT1, WL_VERT2, WL_VERT3, WL_DIAG1, WL_DIAG2 };

int draw_winningline(image_t *img, drawpack *dp, long int winlineno, pixel_t fill_color);

#endif
