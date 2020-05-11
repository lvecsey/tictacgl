/*
    Look through the list of nodes and match on dest_addr, returning process_id
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

#include <string.h>

#include "node.h"

#ifdef WINNT
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <netdb.h>
#endif

#include <stdint.h>

#include "nodefetch_processid.h"

int nodefetch_processid(node *nodes, long int num_nodes, struct sockaddr_in *match_addr, uint64_t *process_id) {

  long int nodeno;

  unsigned char *expecting_ipaddress;
  uint16_t expecting_port;

  expecting_ipaddress = (unsigned char*) &(match_addr->sin_addr.s_addr);
  expecting_port = ntohs(match_addr->sin_port);
    
  for (nodeno = 0; nodeno < num_nodes; nodeno++) {

    unsigned char *check_ipaddress;
    uint16_t check_port;

    check_ipaddress = (unsigned char*) &(nodes[nodeno].dest_addr.sin_addr.s_addr);
    check_port = ntohs(nodes[nodeno].dest_addr.sin_port);

    if (!memcmp(check_ipaddress, expecting_ipaddress, 4) && check_port == expecting_port) {
      break;
    }
      
  }

  if (nodeno < num_nodes) {
    process_id[0] = nodes[nodeno].process_id;
  }

  else {

    return -1;
      
  }

  return 0;
  
}


