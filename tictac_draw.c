
#define _GNU_SOURCE
#include <math.h>

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "mini_gxkit.h"

#include "tictac.h"

#include "tictac_draw.h"

int draw_line(image_t *img, drawpack *dp, point_t *pt_start, point_t *pt_end, pixel_t fill_color) {

  double aspect;

  long int n;

  double v;

  long int xpos, ypos;

  point_t pt;

  long int nend;
  
  aspect = ((double) (img->xres)) / img->yres;

  nend = dp->nend;
  
  for (n = 0; n < nend; n++) {
    v = n; v /= nend;

    pt = (point_t) { .x = pt_start->x * (1.0 - v) + pt_end->x * v, .y = pt_start->y * (1.0 - v) + pt_end->y * v };

    pt.x /= aspect;
    pt.y *= -1.0;
    
    xpos = (img->xres >> 1) * pt.x; xpos += img->xres >> 1;
    ypos = (img->yres >> 1) * pt.y; ypos += img->yres >> 1;	  
    img->rgb[ypos * img->xres + xpos] = fill_color;
	    
  }

  return 0;
  
}

int draw_winningline(image_t *img, drawpack *dp, long int winlineno, pixel_t fill_color) {

  point_t winlines[20];
  
  point_t *pt_start, *pt_end;

  long int xw, yh;

  double spangrid;

  spangrid = dp->spangrid;
  
  xw = 3;
  yh = 3;
  
  winlines[0] = (point_t) { .x = -spangrid + 2.0 * spangrid * 0 / (xw - 1), .y = -spangrid + 2.0 * spangrid * 0 / (yh - 1) };
  winlines[1] = (point_t) { .x = -spangrid + 2.0 * spangrid * 2 / (xw - 1), .y = -spangrid + 2.0 * spangrid * 0 / (yh - 1) };
  winlines[2] = (point_t) { .x = -spangrid + 2.0 * spangrid * 0 / (xw - 1), .y = -spangrid + 2.0 * spangrid * 1 / (yh - 1) };
  winlines[3] = (point_t) { .x = -spangrid + 2.0 * spangrid * 2 / (xw - 1), .y = -spangrid + 2.0 * spangrid * 1 / (yh - 1) };
  winlines[4] = (point_t) { .x = -spangrid + 2.0 * spangrid * 0 / (xw - 1), .y = -spangrid + 2.0 * spangrid * 2 / (yh - 1) };
  winlines[5] = (point_t) { .x = -spangrid + 2.0 * spangrid * 2 / (xw - 1), .y = -spangrid + 2.0 * spangrid * 2 / (yh - 1) };

  winlines[6] = (point_t) { .x = -spangrid + 2.0 * spangrid * 0 / (xw - 1), .y = -spangrid + 2.0 * spangrid * 0 / (yh - 1) };
  winlines[7] = (point_t) { .x = -spangrid + 2.0 * spangrid * 0 / (xw - 1), .y = -spangrid + 2.0 * spangrid * 2 / (yh - 1) };
  winlines[8] = (point_t) { .x = -spangrid + 2.0 * spangrid * 1 / (xw - 1), .y = -spangrid + 2.0 * spangrid * 0 / (yh - 1) };
  winlines[9] = (point_t) { .x = -spangrid + 2.0 * spangrid * 1 / (xw - 1), .y = -spangrid + 2.0 * spangrid * 2 / (yh - 1) };
  winlines[10] = (point_t) { .x = -spangrid + 2.0 * spangrid * 2 / (xw - 1), .y = -spangrid + 2.0 * spangrid * 0 / (yh - 1) };
  winlines[11] = (point_t) { .x = -spangrid + 2.0 * spangrid * 2 / (xw - 1), .y = -spangrid + 2.0 * spangrid * 2 / (yh - 1) };

  winlines[12] = (point_t) { .x = -spangrid + 2.0 * spangrid * 0 / (xw - 1), .y = -spangrid + 2.0 * spangrid * 0 / (yh - 1) };
  winlines[13] = (point_t) { .x = -spangrid + 2.0 * spangrid * 2 / (xw - 1), .y = -spangrid + 2.0 * spangrid * 2 / (yh - 1) };
  winlines[14] = (point_t) { .x = -spangrid + 2.0 * spangrid * 0 / (xw - 1), .y = -spangrid + 2.0 * spangrid * 2 / (yh - 1) };
  winlines[15] = (point_t) { .x = -spangrid + 2.0 * spangrid * 2 / (xw - 1), .y = -spangrid + 2.0 * spangrid * 0 / (yh - 1) };

  winlines[16] = (point_t) { .x = -spangrid + 2.0 * spangrid * 0 / (xw - 1), .y = -spangrid + 2.0 * spangrid * 1 / (yh - 1) };
  winlines[17] = (point_t) { .x = -spangrid + 2.0 * spangrid * 2 / (xw - 1), .y = -spangrid + 2.0 * spangrid * 1 / (yh - 1) };
  winlines[18] = (point_t) { .x = -spangrid + 2.0 * spangrid * 1 / (xw - 1), .y = -spangrid + 2.0 * spangrid * 0 / (yh - 1) };
  winlines[19] = (point_t) { .x = -spangrid + 2.0 * spangrid * 1 / (xw - 1), .y = -spangrid + 2.0 * spangrid * 2 / (yh - 1) };
  
  pt_start = winlines + winlineno * 2;
  pt_end = (pt_start + 1);

  draw_line(img, dp, pt_start, pt_end, fill_color);
  
  return 0;
  
}

int tictac_drawpiece(image_t *img, drawpack *dp, tictac *position, pixel_t fill_color) {

  double radius;

  double radians;

  point_t pt;

  long int nend;

  long int n;

  double spangrid;

  long int xi, yi;

  long int xw, yh;

  double aspect;

  long int xpos, ypos;
  
  nend = dp->nend;

  spangrid = dp->spangrid;

  aspect = ((double) img->xres) / img->yres;

  yi = position->piece_pos / 3;
  xi = position->piece_pos % 3;
  
  xw = 3;
  yh = 3;
  
  switch(position->piece_type) {
  case PIECE_TICTACO:

    radius = 0.25;

    for (n = 0; n < nend; n++) {
      radians = (2.0 * n * M_PI) / nend;
      sincos(radians, &(pt.y), &(pt.x));

      pt.x *= radius;
      pt.y *= radius;

      pt.x += -spangrid + 2.0 * spangrid * xi / (xw - 1);
      pt.y += -spangrid + 2.0 * spangrid * yi / (yh - 1);	  

      pt.x /= aspect;
      pt.y *= -1.0;
	  
      xpos = (img->xres >> 1) * pt.x; xpos += img->xres >> 1;
      ypos = (img->yres >> 1) * pt.y; ypos += img->yres >> 1;	  
      img->rgb[ypos * img->xres + xpos] = fill_color;
    }
	
    break;
  case PIECE_TICTACX:

    {

      point_t pt_start;
      point_t pt_end;

      pt_start = (point_t) { .x = -0.25, .y = -0.25 };
      pt_end = (point_t) { .x = 0.25, .y = 0.25 };	  

      pt_start.x += -spangrid + 2.0 * spangrid * xi / (xw - 1);
      pt_start.y += -spangrid + 2.0 * spangrid * yi / (yh - 1);	  
      pt_end.x += -spangrid + 2.0 * spangrid * xi / (xw - 1);
      pt_end.y += -spangrid + 2.0 * spangrid * yi / (yh - 1);	  
	  
      draw_line(img, dp, &pt_start, &pt_end, fill_color);

      pt_start = (point_t) { .x = 0.25, .y = -0.25 };
      pt_end = (point_t) { .x = -0.25, .y = 0.25 };	  

      pt_start.x += -spangrid + 2.0 * spangrid * xi / (xw - 1);
      pt_start.y += -spangrid + 2.0 * spangrid * yi / (yh - 1);	  
      pt_end.x += -spangrid + 2.0 * spangrid * xi / (xw - 1);
      pt_end.y += -spangrid + 2.0 * spangrid * yi / (yh - 1);	  
	  
      draw_line(img, dp, &pt_start, &pt_end, fill_color);
	  
    }
	  
    break;
  }

  return 0;
  
}

int tictac_draw(image_t *img, drawpack *dp, tictac *positions, pixel_t fill_color) {

  long int xi, yi;

  long int xw, yh;

  tictac *pos_ptr;

  long int xpos, ypos;

  xw = 3;
  yh = 3;

  for (yi = 0; yi < yh; yi++) {
    for (xi = 0; xi < xw; xi++) {

      long int hsno;

      hsno = yi * xw + xi;
      
      pos_ptr = positions + hsno;

      tictac_drawpiece(img, dp, pos_ptr, fill_color);
      
    }
  }

  {

    long int xpos_start;
    long int xpos_end;

    xpos_start = (1 * img->xres) / 5;
    xpos_end = (4 * img->xres) / 5;    
    
    ypos = (1 * img->yres) / 3;
    for (xpos = xpos_start; xpos < xpos_end; xpos++) {
      img->rgb[ypos * img->xres + xpos] = fill_color;
    }

    ypos = (2 * img->yres) / 3;
    for (xpos = xpos_start; xpos < xpos_end; xpos++) {
      img->rgb[ypos * img->xres + xpos] = fill_color;
    }

  } 
  
  xpos = (2 * img->xres) / 5;
  for (ypos = 0; ypos < img->yres; ypos++) {
    img->rgb[ypos * img->xres + xpos] = fill_color;
  }

  xpos = (3 * img->xres) / 5;
  for (ypos = 0; ypos < img->yres; ypos++) {
    img->rgb[ypos * img->xres + xpos] = fill_color;
  }
  
  return 0;

}
