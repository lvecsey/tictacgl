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

#ifndef SOCKETWORK_H
#define SOCKETWORK_H

#ifdef WINNT
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <netdb.h>
#endif

int prep_socketwork(struct sockaddr_in *bind_addr);

int setNonblocking(int fd);

#endif
