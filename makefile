CFLAGS = -g
CC=gcc
SVR = svr
CLI = cli

OBJ = $(patsubst %.c, so_o/%.o, $(filter-out svr.c cli.c,$(wildcard *.c)))

all:so_o $(SVR) $(CLI)

$(SVR): $(OBJ) so_o/$(SVR).o
	$(CC) $(CFLAGS) -o $@ $(OBJ) so_o/$(SVR).o

$(CLI): $(OBJ) so_o/$(CLI).o
	$(CC) $(CFLAGS) -o $@ $(OBJ) so_o/$(CLI).o

so_o:
	mkdir -p so_o

so_o/%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -fr so_o
	rm -fr $(SVR)
	rm -fr $(CLI)
