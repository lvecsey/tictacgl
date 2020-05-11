
#include <stdio.h>

#include <inttypes.h>

#ifdef WINNT
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <netdb.h>
#endif

#ifdef _WIN32
#  ifdef _WIN64
#    define PRI_SIZET PRIu64
#  else
#    define PRI_SIZET PRIu32
#  endif
#else
#  define PRI_SIZET "zu"
#endif

#include "node.h"

int show_node(node *np) {

  unsigned char *ip_address;
  uint16_t port;

  ip_address = (unsigned char*) &(np->dest_addr.sin_addr.s_addr);
  port = ntohs(np->dest_addr.sin_port);

#ifdef WINNT
  printf("Process[%" PRI_SIZET "]: %u.%u.%u.%u:%u\n", np->process_id, ip_address[0], ip_address[1], ip_address[2], ip_address[3], port);
#else
  printf("Process[%lu]: %u.%u.%u.%u:%u\n", np->process_id, ip_address[0], ip_address[1], ip_address[2], ip_address[3], port);
#endif
  
  return 0;
  
}

int show_nodes(node *nodes, long int num_nodes) {

  long int nodeno;

  for (nodeno = 0; nodeno < num_nodes; nodeno++) {
    show_node(nodes + nodeno);
  }

  return 0;

}
