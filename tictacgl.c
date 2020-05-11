/*
    Play tic tac toe over a connection with a number of peers.
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

#include <time.h>

#include <inttypes.h>

#ifdef _WIN32
#  ifdef _WIN64
#    define PRI_SIZET PRIu64
#  else
#    define PRI_SIZET PRIu32
#  endif
#else
#  define PRI_SIZET "zu"
#endif

#include "mini_gxkit.h"

#include "flip_img.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <string.h>
#include <errno.h>

#include <GLFW/glfw3.h>

#include <GL/gl.h>

#ifdef WINNT
#define htobe64(x) __builtin_bswap64(x)
#define be64toh(x) __builtin_bswap64(x)
#else
#include <endian.h>
#endif

#include "tictac.h"

#include "tictaclist.h"

#include "tictacgame.h"

#include "tictac_draw.h"

#include "tictacgl.h"

#include "processes.h"
#include "fill_address.h"
#include "nodefetch_processid.h"

#include "fill_position.h"

#include "socketwork.h"

#define DEFAULT_PROCESSFN "processes.txt"

#define DEFAULT_XRES 1920
#define DEFAULT_YRES 1080

#define MAX_PKTSIZE 1536

image_t img_bg;
image_t img_workspace;
image_t img_final;

drawpack dp;

tictacgame game;

tictac positions[9];

int bound_sock;

node *nodes;

long int num_nodes;

long int my_nodeno;

int render(GLFWwindow *window) {
  
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f );
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glDrawPixels(img_final.xres, img_final.yres, GL_RGB, GL_UNSIGNED_SHORT, img_final.rgb);  
  
  return 0;
  
}

int initial_positions(tictac *positions) {

  long int piece_pos;

  for (piece_pos = 0; piece_pos < 9; piece_pos++) {
    positions[piece_pos] = (tictac) { .piece_pos = piece_pos, .piece_type = PIECE_TICTACE };
  }

  return 0;

}

int inrange_gridpos(long int xres, long int yres, point_t *cursor_pos, long int *tposx, long int *tposy) {

  long int tpos[2];

  double cx, cy;
  
  if (cursor_pos->x >= 0 && cursor_pos->x < xres) {
    if (cursor_pos->y >= 0 && cursor_pos->y < yres) {

      cursor_pos->y = (yres - 1 - cursor_pos->y);

      cx = cursor_pos->x - (xres >> 1);
      cx /= xres >> 1;
	
      cy = cursor_pos->y - (yres >> 1);
      cy /= yres >> 1;

      if (cx < -1.0/5.0) {
	tpos[0] = 0;
      }
      else if (cx < 1.0/5.0 && cx >= -1.0/5.0) {
	tpos[0] = 1;
      }
      else {
	tpos[0] = 2;
      }

      if (cy < -1.0/3.0) {
	tpos[1] = 0;
      }
      else if (cy < 1.0/3.0 && cy >= -1.0/3.0) {
	tpos[1] = 1;
      }
      else {
	tpos[1] = 2;
      }

      tpos[1] = (2 - tpos[1]);
      
      *tposx = tpos[0];
      *tposy = tpos[1];  

      return 1;
      
    }

  }
  
  return 0;

}

int checkmove(tictacgame *game, tictac *position) {

  int retval;

  memset(positions, 0, sizeof(tictac) * 9);
  
  retval = fill_position(&(game->tl_root), positions);
  if (retval == -1) {
    printf("Trouble with fill_position.\n");
    return -1;
  }

  if (positions[position->piece_pos].piece_type == PIECE_TICTACE) {

    return 1;
    
  }
  
  return -1;
  
}

int wongame(tictacgame *game, long int *winlineno) {

  long int xi, yi;

  long int xw, yh;

  uint64_t piecetype;
  
  xw = 3;
  yh = 3;

  yi = 0; xi = 0;
  piecetype = positions[yi*xw+xi].piece_type;
  if (piecetype == PIECE_TICTACX || piecetype == PIECE_TICTACO) {
    for (xi = 1; xi < xw; xi++) {
      if (positions[yi*xw+xi].piece_type != piecetype) {
	break;
      }
    }
    if (xi == xw) {
      winlineno[0] = WL_ACROSS1;
      return 1;
    }
  }
  
  yi = 1; xi = 0;
  piecetype = positions[yi*xw+xi].piece_type;
  if (piecetype == PIECE_TICTACX || piecetype == PIECE_TICTACO) {
    for (xi = 1; xi < xw; xi++) {
      if (positions[yi*xw+xi].piece_type != piecetype) {
	break;
      }
    }
    if (xi == xw) {
      winlineno[0] = WL_ACROSS2;
      return 1;
    }
  }
  
  yi = 2; xi = 0;
  piecetype = positions[yi*xw+xi].piece_type;
  if (piecetype == PIECE_TICTACX || piecetype == PIECE_TICTACO) {
    for (xi = 1; xi < xw; xi++) {
      if (positions[yi*xw+xi].piece_type != piecetype) {
	break;
      }
    }
    if (xi == xw) {
      winlineno[0] = WL_ACROSS3;
      return 1;
    }
  }

  xi = 0; yi = 0;
  piecetype = positions[yi*xw+xi].piece_type;
  if (piecetype == PIECE_TICTACX || piecetype == PIECE_TICTACO) {
    for (yi = 1; yi < yh; yi++) {
      if (positions[yi*xw+xi].piece_type != piecetype) {
	break;
      }
    }
    if (yi == yh) {
      winlineno[0] = WL_VERT1;
      return 1;
    }
  }  

  xi = 1; yi = 0;
  piecetype = positions[yi*xw+xi].piece_type;
  if (piecetype == PIECE_TICTACX || piecetype == PIECE_TICTACO) {
    for (yi = 1; yi < yh; yi++) {
      if (positions[yi*xw+xi].piece_type != piecetype) {
	break;
      }
    }
    if (yi == yh) {
      winlineno[0] = WL_VERT2;
      return 1;
    }
  }  

  xi = 2; yi = 0;
  piecetype = positions[yi*xw+xi].piece_type;
  if (piecetype == PIECE_TICTACX || piecetype == PIECE_TICTACO) {
    for (yi = 1; yi < yh; yi++) {
      if (positions[yi*xw+xi].piece_type != piecetype) {
	break;
      }
    }
    if (yi == yh) {
      winlineno[0] = WL_VERT3;
      return 1;
    }
  }    

  xi = 0; yi = 0;
  piecetype = positions[yi*xw+xi].piece_type;
  if (piecetype == PIECE_TICTACX || piecetype == PIECE_TICTACO) {
    for ( xi = 1, yi = 1; xi < 3 && yi < 3; xi++, yi++) {
      if (positions[yi*xw+xi].piece_type != piecetype) {
	break;
      }
    }
    if (xi == xw && yi == yh) {
      winlineno[0] = WL_DIAG1;
      return 1;
    }
  }

  xi = 0; yi = 2;
  piecetype = positions[yi*xw+xi].piece_type;
  if (piecetype == PIECE_TICTACX || piecetype == PIECE_TICTACO) {
    for ( xi = 1, yi = 1; xi < 3 && yi >= 0; xi++, yi--) {
      if (positions[yi*xw+xi].piece_type != piecetype) {
	break;
      }
    }
    if (xi == xw && yi == -1) {
      winlineno[0] = WL_DIAG2;
      return 1;
    }
  }

  return 0;
  
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {

  pixel_t white = { 65535, 65535, 65535 };  
  pixel_t red = { 65535, 0, 0 };
  
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {

    point_t cursor_pos;

    int retval;

    long int tposx, tposy;

    long int hsno;

    tictac latest_move;
    
    if (game.game_state == TICTAC_GAMEOVER) {
      return;
    }
    
    glfwGetCursorPos(window, &(cursor_pos.x), &(cursor_pos.y));
    
    retval = inrange_gridpos(img_workspace.xres, img_workspace.yres, &cursor_pos, &tposx, &tposy);
    if (retval == 1) {

      printf("Event at (%ld,%ld)\n", tposx, tposy);
      
      hsno = tposy * 3 + tposx;

      latest_move = (tictac) { .piece_type = game.piecetype_nextplay, .piece_pos = hsno };

      retval = checkmove(&game, &latest_move);
      if (retval != -1) {

	retval = tictaclist_insertend(&(game.tl_root), &latest_move);
	if (retval == -1) {
	  printf("Trouble inserting move into game.\n");
	  return;
	}

	tictac_drawpiece(&img_workspace, &dp, &latest_move, white);

	memset(positions, 0, sizeof(tictac) * 9);
  
	retval = fill_position(&(game.tl_root), positions);
	if (retval == -1) {
	  printf("Trouble with fill_position.\n");
	  return;
	}
	
      }
	      
      {
	long int winlineno;
      
	retval = wongame(&game, &winlineno);
	if (retval == 1) {

	  game.game_state = TICTAC_GAMEOVER;
	  
	  retval = draw_winningline(&img_workspace, &dp, winlineno, red);

	  printf("Game over.\n");
	  
	}
      }
      
    }

    {

      char out_buf[MAX_PKTSIZE];
      
      header hout;

      positionpack pout;
      
      uint64_t val;
      
      char *wptr;

      size_t len;

      socklen_t addrlen;
      
      struct sockaddr_in *target_addrp;

      int flags;

      long int nodeno;

      long int piece_pos;
      
      memcpy(hout.id, "TTT0", 4);
      hout.cmd = TCMD_DISPLAY;
      hout.process_id = nodes[my_nodeno].process_id;
      hout.numseq = 0;
      hout.fwd_counter = 0;

      pout = (positionpack) { .latest_move = latest_move };
      
      wptr = out_buf;
      
      memcpy(wptr, hout.id, sizeof(hout.id));
      wptr += sizeof(hout.id);

      val = htobe64(hout.cmd);
      memcpy(wptr, &val, sizeof(uint64_t));
      wptr += sizeof(uint64_t);

      val = htobe64(hout.process_id);
      memcpy(wptr, &val, sizeof(uint64_t));
      wptr += sizeof(uint64_t);

      val = htobe64(hout.numseq);
      memcpy(wptr, &val, sizeof(uint64_t));
      wptr += sizeof(uint64_t);

      val = htobe64(hout.fwd_counter);
      memcpy(wptr, &val, sizeof(uint64_t));
      wptr += sizeof(uint64_t);
      
      for (piece_pos = 0; piece_pos < 9; piece_pos++) {

	tictac *pos_ptr;

	pos_ptr = positions + piece_pos;

	val = htobe64(pos_ptr->piece_type);
	memcpy(wptr, &val, sizeof(uint64_t));
	wptr += sizeof(uint64_t);
	      
	val = htobe64(pos_ptr->piece_pos);
	memcpy(wptr, &val, sizeof(uint64_t));
	wptr += sizeof(uint64_t);

      }

      val = htobe64(pout.latest_move.piece_type);
      memcpy(wptr, &val, sizeof(uint64_t));
      wptr += sizeof(uint64_t);

      val = htobe64(pout.latest_move.piece_pos);
      memcpy(wptr, &val, sizeof(uint64_t));
      wptr += sizeof(uint64_t);

      if (latest_move.piece_type == PIECE_TICTACX) {
	game.piecetype_nextplay = PIECE_TICTACO;
      }
      else if (latest_move.piece_type == PIECE_TICTACO) {
	game.piecetype_nextplay = PIECE_TICTACX;
      }
      
      len = (wptr - out_buf);

      addrlen = sizeof(struct sockaddr_in);

      flags = 0;

      do {

	nodeno = (rand() % num_nodes);

      } while (nodeno == my_nodeno);

      show_node(nodes + nodeno);
      
      target_addrp = &(nodes[nodeno].dest_addr);

      retval = sendto(bound_sock, out_buf, len, flags, (struct sockaddr*) target_addrp, addrlen);

      if (retval != len) {
	printf("%s: Trouble sending packet.\n", __FUNCTION__);
      }
      
    }
    
  }
    
}

int refreshpositions_tictac() {

  long int num_pixels;

  size_t img_sz;

  pixel_t white = { 65535, 65535, 65535 };

  int retval;
  
  num_pixels = img_workspace.xres * img_workspace.yres;
  img_sz = num_pixels * sizeof(pixel_t);

  memcpy(img_workspace.rgb, img_bg.rgb, img_sz);

  retval = tictac_draw(&img_workspace, &dp, positions, white);
  if (retval == -1) {
    printf("Trouble calling tictac_draw.\n");
    return -1;
  }
  
  return 0;

}

int cleargame_tictac() {

  printf("Clearing game.\n");
    
  if (game.tl_root.next != NULL) {
    tictaclist_clear(&(game.tl_root));
  }
    
  game = (tictacgame) { .game_state = TICTAC_GAMEACTIVE, .tl_root = { .next = NULL }, .piecetype_nextplay = PIECE_TICTACX };    

  initial_positions(positions);
  
  return 0;
  
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

  int retval;

  long int num_pixels;

  size_t img_sz;

  if (key == GLFW_KEY_BACKSPACE && action == GLFW_PRESS) {

    int out_fd;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    ssize_t bytes_written;

    char *outrgb_fn = "tictacgl_screenshot.rgb";

    image_t flipped;
    
    num_pixels = img_workspace.xres * img_workspace.yres;
    img_sz = num_pixels * sizeof(pixel_t);

    flipped = (image_t) { .rgb = malloc(img_sz), .xres = img_workspace.xres, .yres = img_workspace.yres };
    if (flipped.rgb == NULL) {
      perror("malloc");
      return;
    }
    
    out_fd = open(outrgb_fn, O_CREAT | O_TRUNC | O_WRONLY, mode);
    if (out_fd == -1) {
      perror("open");
      return;
    }

    retval = flip_img(&img_workspace, &flipped);
    
    bytes_written = write(out_fd, flipped.rgb, img_sz);
    if (bytes_written != img_sz) {
      perror("write");
    }

    free(flipped.rgb);
    
    retval = close(out_fd);
    if (retval == -1) {
      perror("close");
      return;
    }
    
  }
  
  else if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {

    glfwSetWindowShouldClose(window, 1);
    
  }

  else if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {

    cleargame_tictac();    

    refreshpositions_tictac();
    
    {

      char out_buf[MAX_PKTSIZE];
      
      header hout;

      uint64_t val;
      
      char *wptr;

      size_t len;

      socklen_t addrlen;
      
      struct sockaddr_in *target_addrp;

      int flags;

      long int nodeno;

      memcpy(hout.id, "TTT0", 4);
      hout.cmd = TCMD_RESETGAME;
      hout.process_id = nodes[my_nodeno].process_id;
      hout.numseq = 0;
      hout.fwd_counter = 0;
      
      wptr = out_buf;
      
      memcpy(wptr, hout.id, sizeof(hout.id));
      wptr += sizeof(hout.id);

      val = htobe64(hout.cmd);
      memcpy(wptr, &val, sizeof(uint64_t));
      wptr += sizeof(uint64_t);

      val = htobe64(hout.process_id);
      memcpy(wptr, &val, sizeof(uint64_t));
      wptr += sizeof(uint64_t);

      val = htobe64(hout.numseq);
      memcpy(wptr, &val, sizeof(uint64_t));
      wptr += sizeof(uint64_t);

      val = htobe64(hout.fwd_counter);
      memcpy(wptr, &val, sizeof(uint64_t));
      wptr += sizeof(uint64_t);
      
      len = (wptr - out_buf);

      addrlen = sizeof(struct sockaddr_in);

      flags = 0;

      do {

	nodeno = (rand() % num_nodes);

      } while (nodeno == my_nodeno);

      printf("Sending to nodeno %ld\n", nodeno);
      
      target_addrp = &(nodes[nodeno].dest_addr);
      
      retval = sendto(bound_sock, out_buf, len, flags, (struct sockaddr*) target_addrp, addrlen);

      if (retval != len) {
	printf("%s: Trouble sending packet\n", __FUNCTION__);
      }
      
    }
    
  }
  
}

int fill_highlightsquares(point_t *hs, long int xres, long int yres) {

  long int wspan;
  long int hspan;

  hspan = yres / 3;
  wspan = xres / 5;
  
  hs[0] = (point_t) { .x = -(wspan>>1) - wspan, .y = -(hspan>>1) + hspan };
  hs[1] = (point_t) { .x = -(wspan>>1) - 0, .y = -(hspan>>1) + hspan };
  hs[2] = (point_t) { .x = -(wspan>>1) + wspan, .y = -(hspan>>1) + hspan };  

  hs[3] = (point_t) { .x = -(wspan>>1) - wspan, .y = -(hspan>>1) - 0 };
  hs[4] = (point_t) { .x = -(wspan>>1) - 0, .y = -(hspan>>1) - 0 };
  hs[5] = (point_t) { .x = -(wspan>>1) + wspan, .y = -(hspan>>1) - 0 };  

  hs[6] = (point_t) { .x = -(wspan>>1) - wspan, .y = -(hspan>>1) - hspan };
  hs[7] = (point_t) { .x = -(wspan>>1) - 0, .y = -(hspan>>1) - hspan };
  hs[8] = (point_t) { .x = -(wspan>>1) + wspan, .y = -(hspan>>1) - hspan };  
  
  return 0;
  
}

int main(int argc, char *argv[]) {

  long int xres, yres;
  long int num_pixels;
  size_t img_sz;

  int retval;

  GLFWwindow *window;

  tictac positions_blank[9] = { 0 };
  
  point_t highlight_squares[9];
  
  pixel_t white = { 65535, 65535, 65535 };

  pixel_t gray = { 32768, 32768, 32768 };
  
  header hd;
  
  char *process_fn;

  char *bindip_addrportstr;

  char *publicip_addrportstr;
  
  struct sockaddr_in bind_addr, public_addr;

  char *in_buf, *out_buf;
  
  uint64_t process_id;
  
#ifdef WINNT
  WSADATA wsaData;
#endif
    
  process_fn = argc > 1 ? argv[1] : DEFAULT_PROCESSFN;

  bindip_addrportstr = argc>2 ? argv[2] : NULL;

  publicip_addrportstr = argc>3 ? argv[3] : NULL;

  if (process_fn == NULL || bindip_addrportstr == NULL || publicip_addrportstr == NULL) {
    printf("Usage: processes.txt bind_ip:port public_ip:port\n");
    return 0;
  }

  retval = fill_address(bindip_addrportstr, &bind_addr);
  if (retval == -1) {
    fprintf(stderr, "Trouble filling address for bind_ip:port\n");
    return -1;
  }

  retval = fill_address(publicip_addrportstr, &public_addr);
  if (retval == -1) {
    fprintf(stderr, "Trouble filling address for public_ip:port\n");
    return -1;
  }

  xres = DEFAULT_XRES;
  yres = DEFAULT_YRES;
  num_pixels = xres * yres;
  img_sz = num_pixels * sizeof(pixel_t);

  img_bg = (image_t) { .rgb = malloc(img_sz), .xres = xres, .yres = yres };
  if (img_bg.rgb == NULL) {
    perror("malloc");
    return -1;
  }

  img_workspace = (image_t) { .rgb = malloc(img_sz), .xres = xres, .yres = yres };
  if (img_workspace.rgb == NULL) {
    perror("malloc");
    return -1;
  }

  img_final = (image_t) { .rgb = malloc(img_sz), .xres = xres, .yres = yres };
  if (img_final.rgb == NULL) {
    perror("malloc");
    return -1;
  }
  
  memset(img_bg.rgb, 0, img_sz);

  retval = initial_positions(positions);
  
  retval = tictac_draw(&img_bg, &dp, positions_blank, white);
  if (retval == -1) {
    printf("Trouble calling tictac_draw.\n");
    return -1;
  }

  memcpy(img_workspace.rgb, img_bg.rgb, img_sz);
  
  printf("Compiled against GLFW %i.%i.%i\n", GLFW_VERSION_MAJOR, GLFW_VERSION_MINOR, GLFW_VERSION_REVISION);
  
  retval = load_processes(process_fn, &nodes, &num_nodes);
  if (retval == -1) {
    fprintf(stderr, "%s: Trouble loading process_fn %s\n", __FUNCTION__, process_fn);
    return -1;
  }
  
  printf("Loaded %ld nodes\n", num_nodes);

  retval = show_nodes(nodes, num_nodes);
  
  retval = nodefetch_processid(nodes, num_nodes, &public_addr, &process_id);
  if (retval == -1) {
    fprintf(stderr, "public_ip:port did not match a process id in process_fn %s\n", process_fn);
    return -1;
  }

  {

    long int nodeno;

    for (nodeno = 0; nodeno < num_nodes; nodeno++) {
      if (nodes[nodeno].process_id == process_id) {
	my_nodeno = nodeno;
	break;
      }
    }

    if (nodeno == num_nodes) {
      fprintf(stderr, "%s: Trouble assigning my_nodeno.\n", __FUNCTION__);
      return -1;
    }
    
  }
  
#ifdef WINNT
  printf("Matched process_id %" PRI_SIZET "\n", process_id);
#else
  printf("Matched process_id %lu\n", process_id);
#endif  

  printf("my_nodeno %ld\n", my_nodeno);
  
#ifdef WINNT
  if (WSAStartup(MAKEWORD(1,1), &wsaData) == SOCKET_ERROR) {
    printf ("Error initialising WSA.\n");
    return -1;
  }
#endif
  
  bound_sock = prep_socketwork(&bind_addr);
  if (bound_sock == -1) {
    fprintf(stderr, "%s: Trouble binding to socket.\n", __FUNCTION__);
    return -1;
  }

  retval = setNonblocking(bound_sock);
  if (retval == -1) {
    fprintf(stderr, "%s: Trouble setting socket to non-blocking.\n", __FUNCTION__);
    return -1;
  }
  
  if (!glfwInit()) {
    fprintf(stderr, "%s: Trouble initializing glfw.\n", __FUNCTION__);
    return -1;
  }
  
  window = glfwCreateWindow(img_workspace.xres, img_workspace.yres, "tictacgl", glfwGetPrimaryMonitor(), NULL);

  glfwMakeContextCurrent(window);

  glfwSetKeyCallback(window, key_callback);

  glfwSetMouseButtonCallback(window, mouse_button_callback);
  
  retval = fill_highlightsquares(highlight_squares, img_workspace.xres, img_workspace.yres);
  if (retval == -1) {
    printf("Trouble filling highlight square coordinates.\n");
    return -1;
  }
  
  memcpy(&(hd.id), "TTT0", 4);

  game = (tictacgame) { .game_state = TICTAC_GAMEACTIVE, .tl_root = { .next = NULL }, .piecetype_nextplay = PIECE_TICTACX };

  dp = (drawpack) { .spangrid = 0.70, .nend = 4500 };

  in_buf = malloc(MAX_PKTSIZE);
  if (in_buf == NULL) {
    perror("malloc");
    return -1;
  }

  out_buf = malloc(MAX_PKTSIZE);
  if (out_buf == NULL) {
    perror("malloc");
    return -1;
  }

  srand(time(NULL));
  
  while (!glfwWindowShouldClose(window)) {

    point_t cursor_pos;

    long int base_x, base_y;

    long int hsno;

    long int tposx, tposy;

    memcpy(img_final.rgb, img_workspace.rgb, img_sz);
    
    glfwGetCursorPos(window, &(cursor_pos.x), &(cursor_pos.y));

    retval = inrange_gridpos(img_workspace.xres, img_workspace.yres, &cursor_pos, &tposx, &tposy);
    if (retval == 1) {

	long int offset;

	long int xi, yi;
	
	long int xw, yh;
	
	xw = img_workspace.xres / 5;
	yh = img_workspace.yres / 3;
	
	hsno = tposy * 3 + tposx;

	base_x = highlight_squares[hsno].x;
	base_y = highlight_squares[hsno].y;

	base_x += (img_final.xres >> 1);
	base_y += (img_final.yres >> 1);

	for (xi = 0; xi < xw; xi++) {
	  for (yi = 0; yi < yh; yi++) {

	    offset = (base_y + yi) * img_final.xres + base_x + xi; 

	    if (img_final.rgb[offset].r == 0 && img_final.rgb[offset].g == 0 && img_final.rgb[offset].b == 0) {
	      img_final.rgb[offset].r += gray.r;
	      img_final.rgb[offset].g += gray.g;
	      img_final.rgb[offset].b += gray.b;	  
	    }
	      
	  }

	}

    }

    {

      socklen_t addrlen;

      ssize_t bytes_read;

      struct sockaddr_in dest_addr;

      int flags;

      size_t len;
      
      addrlen = sizeof(struct sockaddr_in);
      flags = 0;

      bytes_read = recvfrom(bound_sock, in_buf, MAX_PKTSIZE, flags, (struct sockaddr*) &dest_addr, &addrlen); 

      if (bytes_read == -1) {
	if (errno == EAGAIN) {

	}
	else {
	  perror("recvfrom");
	}
      }
      
      else if (bytes_read >= sizeof(uint64_t)) {

	uint64_t val;

	header hin, hout;

	positionpack pin, pout;
	
	char *rptr, *wptr;

	long int piece_pos;

	long int nodeno;

	pixel_t red = { .r = 65535, .g = 0, .b = 0 };
	
	struct sockaddr_in *target_addrp;
	
	rptr = in_buf;

	memcpy(hin.id, rptr, sizeof(hin.id));
	rptr += sizeof(hin.id);
	
	memcpy(&val, rptr, sizeof(uint64_t));
	rptr += sizeof(uint64_t);
	hin.cmd = be64toh(val);

	memcpy(&val, rptr, sizeof(uint64_t));
	rptr += sizeof(uint64_t);
	hin.process_id = be64toh(val);

	memcpy(&val, rptr, sizeof(uint64_t));
	rptr += sizeof(uint64_t);
	hin.numseq = be64toh(val);

	memcpy(&val, rptr, sizeof(uint64_t));
	rptr += sizeof(uint64_t);
	hin.fwd_counter = be64toh(val);
		
	switch(hin.cmd) {
	case TCMD_DISPLAY:
	  
	  if (hin.fwd_counter < num_nodes) {

	    for (piece_pos = 0; piece_pos < 9; piece_pos++) {

	      tictac *pos_ptr;

	      pos_ptr = positions + piece_pos;

	      memcpy(&val, rptr, sizeof(uint64_t));	      
	      pos_ptr->piece_type = be64toh(val);
	      rptr += sizeof(uint64_t);
	      
	      memcpy(&val, rptr, sizeof(uint64_t));
	      pos_ptr->piece_pos = be64toh(val);
	      rptr += sizeof(uint64_t);

	    }

	    memcpy(&val, rptr, sizeof(uint64_t));
	    pin.latest_move.piece_type = be64toh(val);
	    rptr += sizeof(uint64_t);

	    memcpy(&val, rptr, sizeof(uint64_t));
	    pin.latest_move.piece_pos = be64toh(val);
	    rptr += sizeof(uint64_t);

	    retval = checkmove(&game, &(pin.latest_move));

	    if (retval != -1) {
	
	      retval = tictaclist_insertend(&(game.tl_root), &(pin.latest_move));
	      if (retval == -1) {
		printf("Trouble inserting move into game.\n");
		return -1;
	      }
	    
	      tictac_drawpiece(&img_workspace, &dp, &(pin.latest_move), white);

	      memset(positions, 0, sizeof(tictac) * 9);
  
	      retval = fill_position(&(game.tl_root), positions);
	      if (retval == -1) {
		printf("Trouble with fill_position.\n");
		return -1;
	      }

	      if (pin.latest_move.piece_type == PIECE_TICTACX) {
		game.piecetype_nextplay = PIECE_TICTACO;
	      }
	      else if (pin.latest_move.piece_type == PIECE_TICTACO) {
		game.piecetype_nextplay = PIECE_TICTACX;
	      }
	      
	    }

	    {
	      long int winlineno;
      
	      retval = wongame(&game, &winlineno);
	      if (retval == 1) {

		game.game_state = TICTAC_GAMEOVER;
	  
		retval = draw_winningline(&img_workspace, &dp, winlineno, red);

		printf("Game over.\n");
	  
	      }
	    }
	    
	    wptr = out_buf;
	    
	    memcpy(hout.id, hin.id, sizeof(hin.id));
	    hout.cmd = hin.cmd;
	    hout.process_id = process_id;
	    hout.numseq = ((hin.numseq + 1) % num_nodes);
	    hout.fwd_counter = (hin.fwd_counter + 1);

	    pout.latest_move = pin.latest_move;
	    
	    memcpy(wptr, hout.id, sizeof(hout.id));
	    wptr += sizeof(hout.id);

	    memcpy(wptr, &(hout.cmd), sizeof(uint64_t));
	    wptr += sizeof(uint64_t);

	    val = htobe64(hout.process_id);
	    memcpy(wptr, &val, sizeof(uint64_t));
	    wptr += sizeof(uint64_t);

	    val = htobe64(hout.numseq);
	    memcpy(wptr, &val, sizeof(uint64_t));
	    wptr += sizeof(uint64_t);

	    val = htobe64(hout.fwd_counter);
	    memcpy(wptr, &val, sizeof(uint64_t));
	    wptr += sizeof(uint64_t);
	    
	    for (piece_pos = 0; piece_pos < 9; piece_pos++) {

	      tictac *pos_ptr;

	      pos_ptr = positions + piece_pos;

	      val = htobe64(pos_ptr->piece_type);
	      memcpy(wptr, &val, sizeof(uint64_t));
	      wptr += sizeof(uint64_t);
	      
	      val = htobe64(pos_ptr->piece_pos);
	      memcpy(wptr, &val, sizeof(uint64_t));
	      wptr += sizeof(uint64_t);

	    }
	    
	    val = htobe64(pout.latest_move.piece_type);
	    memcpy(wptr, &val, sizeof(uint64_t));
	    wptr += sizeof(uint64_t);

	    val = htobe64(pout.latest_move.piece_pos);
	    memcpy(wptr, &val, sizeof(uint64_t));
	    wptr += sizeof(uint64_t);
	    
	    len = (wptr - out_buf);
	    
	    addrlen = sizeof(struct sockaddr_in);

	    flags = 0;

	    nodeno = hout.numseq;

	    target_addrp = &(nodes[nodeno].dest_addr);
      
	    retval = sendto(bound_sock, out_buf, len, flags, (struct sockaddr*) target_addrp, addrlen);

	    if (retval != len) {
	      printf("%s: Trouble sending packet.\n", __FUNCTION__);
	    }
	    
	  }
	  
	  break;

	case TCMD_RESETGAME:

	  cleargame_tictac();

	  refreshpositions_tictac();
	  
	  break;
	  
	}
	
      }

    }
    
    render(window);
 
    glfwSwapBuffers(window);
    glfwPollEvents();

  }

#ifndef WINNT  
  retval = close(bound_sock);
  if (retval == -1) {
    perror("close");
    return -1;
  }
#endif
  
  free(img_final.rgb);
  free(img_workspace.rgb);
  free(img_bg.rgb);

  free(in_buf);
  free(out_buf);
  
  glfwTerminate();
  
  return 0;

}
