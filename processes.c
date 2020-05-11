/*
    Functions for handling a collection of nodes or processes
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

#include <string.h>
#include <errno.h>

#ifdef WINNT
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <netdb.h>
#endif

#include "node.h"

#include "processes.h"

#ifdef WINNT
#include "getline.h"
#endif

int cmp_node(const void *a, const void *b) {

  node *first;
  node *second;

  first = (node*) a;
  second = (node*) b;

  if (first->process_id < second->process_id) {
    return -1;
  }

  if (first->process_id > second->process_id) {
    return 1;
  }
  
  return 0;

}

int load_processes(char *process_fn, node **nodes, long int *num_nodes) {

  FILE *fp;
  char *line;
  size_t len;

  long int process_id;
  
  long int input_ipaddress[4];
  long int input_port;

  int retval;

  struct nodelist nl_root;

  struct nodelist *nlp;
  
  node *np;

  long int nodecount;
  
  fp = fopen(process_fn, "r");
  if (fp == NULL) {
    perror("fopen");
    return -1;
  }

  line = NULL;
  len = 0;

  nl_root.next = NULL;

  nodecount = 0;
  
  while ((retval = getline(&line,&len,fp)) != -1) {

    if (len <= 0) {
      continue;
    }

    retval = sscanf(line, "%ld->%ld.%ld.%ld.%ld:%ld", &process_id, input_ipaddress+0, input_ipaddress+1, input_ipaddress+2, input_ipaddress+3, &input_port);

    if (retval != 6) {
      continue;
    }

    nlp = malloc(sizeof(struct nodelist));
    if (nlp == NULL) {
      perror("malloc");
      return -1;
    }

    np = &(nlp->n);

    {
      unsigned char ip_address[4];
      uint16_t port;

      np[0] = (node) { .state = 0, .process_id = process_id };

      ip_address[0] = input_ipaddress[0];
      ip_address[1] = input_ipaddress[1];
      ip_address[2] = input_ipaddress[2];
      ip_address[3] = input_ipaddress[3];
      port = input_port;

      np->dest_addr.sin_family = AF_INET;
      memcpy(&(np->dest_addr.sin_addr.s_addr), ip_address, 4);
      np->dest_addr.sin_port = htons(port);

    }

    nlp->next = nl_root.next;

    nl_root.next = nlp;

    nodecount++;
    
  }

  free(line);

  {

    long int nodeno;

    node *nodescol;

    nodescol = malloc(sizeof(node) * nodecount);
    if (nodescol == NULL) {
      perror("malloc");
      return -1;
    }
    
    nlp = nl_root.next;
    for (nodeno = 0; nodeno < nodecount; nodeno++) {
      struct nodelist *nlp_next;
      nlp_next = nlp->next;
      nodescol[nodeno] = nlp->n;
      free(nlp);
      nlp = nlp_next;
    }

    qsort(nodescol, nodecount, sizeof(node), cmp_node);
    
    nodes[0] = nodescol;
    
  }
  
  retval = fclose(fp);
  if (retval == -1) {
    perror("fclose");
    return -1;
  }

  num_nodes[0] = nodecount;
  
  return 0;

}
