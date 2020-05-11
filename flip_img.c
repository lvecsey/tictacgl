/*
  Graphics Kit library - flip an image vertically
  Copyright (C) 2015  Lester Vecsey

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>. 
*/

#include <string.h>

#include "mini_gxkit.h"

int flip_img(image_t *i, image_t *tmp_cache) {

  long int xpos, ypos;

  for (ypos = 0; ypos < i->yres; ypos++) {
    for (xpos = 0; xpos < i->xres; xpos++) {
      tmp_cache->rgb[ypos * i->xres + xpos] = i->rgb[(i->yres-ypos-1) * i->xres + xpos];

    }
  }

  memcpy(i->rgb, tmp_cache->rgb, i->xres * i->yres * sizeof(pixel_t));
  
  return 0;

}
