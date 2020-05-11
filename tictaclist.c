/*
    Manage a linked list of tictac position nodes.
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

#include "tictaclist.h"

int tictaclist_clear(struct tictaclist *tlp) {

  tlp = tlp->next;
  
  while (tlp != NULL) {

    struct tictaclist *tlp_next;

    tlp_next = tlp->next;

    free(tlp);

    tlp = tlp_next;
    
  }

  return 0;

}

int tictaclist_fillpositions(struct tictaclist *tlp, tictac *positions) {

  long int moveno;
  
  tlp = tlp->next;

  moveno = 0;
  
  while (tlp != NULL) {

    positions[moveno] = tlp->move;
    
    tlp = tlp->next;

    moveno++;
				   
  }

  return 0;

}

int tictaclist_insertend(struct tictaclist *tlp, tictac *position) {

  struct tictaclist *nl;
  
  while (tlp->next != NULL) {

    tlp = tlp->next;

  }

  nl = malloc(sizeof(struct tictaclist));
  if (nl == NULL) {
    perror("malloc");
    return -1;
  }

  nl->move = position[0];
  nl->next = NULL;

  tlp->next = nl;
  
  return 0;

}
