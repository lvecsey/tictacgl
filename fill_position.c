/*
    Fill a Tic-tac-toe board position from the list of moves.
    Copyright (C) 2020  Lester Vecsey

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

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "tictac.h"

#include "tictaclist.h"

int fill_position(struct tictaclist *tl_root, tictac *positions) {

  struct tictaclist *nlp;

  long int moveno;
  
  nlp = tl_root->next;

  while (nlp != NULL) {

    positions[nlp->move.piece_pos] = nlp->move;
    
    nlp = nlp->next;

    moveno++;
    
  }

  return 0;
  
}


