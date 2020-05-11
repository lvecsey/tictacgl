/*
    UDP socket functions
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

#include <fcntl.h>
#include <sys/types.h>

#include <errno.h>

#ifdef WINNT
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <netdb.h>
#endif

#include "socketwork.h"

int prep_socketwork(struct sockaddr_in *bind_addr) {

  struct protoent *pent;

  socklen_t addrlen;

  int retval;
  
  int s;

  pent = getprotobyname("UDP");
  if (pent == NULL) {
    perror("getprotobyname");
    return -1;
  }

  s = socket(AF_INET, SOCK_DGRAM, pent->p_proto); 
  if (s == -1) {
    perror("socket");
    return -1;
  }

  addrlen = sizeof(struct sockaddr_in);
  
  retval = bind(s, (const struct sockaddr*) bind_addr, addrlen);
  if (retval == -1) {
    perror("bind");
    fprintf(stderr, "%s: errno %d\n", __FUNCTION__, errno);
    return -1;
  }

  return s;

}

int setNonblocking(int fd) {

#ifdef WINNT

  u_long iMode = 1;
  
  ioctlsocket(fd, FIONBIO, &iMode);

  return 0;
  
#else

  int flags;

  int retval;
  
  retval = fcntl(fd, F_GETFL, 0);

  if (retval == -1) {
    flags = 0;
  }
  
  retval = fcntl(fd, F_SETFL, flags | O_NONBLOCK);

  return retval;
  
#endif
  
}
  
