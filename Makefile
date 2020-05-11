
CC=gcc

CFLAGS=-O3 -Wall -g -pg

LDFLAGS=-L./

LIBS=-lnodeprocess -lm -lglfw -lGL

all : tictacgl

libnodeprocess.a : nodefetch_processid.o fill_address.o processes.o node.o
	ar r $@ $^

tictacgl : libnodeprocess.a tictaclist.o fill_position.o tictac_draw.o flip_img.o socketwork.o tictacgl.o
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) $(LIBS)
