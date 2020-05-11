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

#ifndef NODEFETCH_PROCESSID_H
#define NODEFETCH_PROCESSID_H

#include <string.h>

#include "node.h"

#ifdef WINNT
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <netdb.h>
#endif

#include <stdint.h>

int nodefetch_processid(node *nodes, long int num_nodes, struct sockaddr_in *match_addr, uint64_t *process_id);

#endif
