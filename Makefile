CC=gcc
CFLAGS=-ggdb -Wall --std=c11
LIBS=`pkg-config --libs ncurses`

run: digitap
	@./digitap

digitap: main.c 
	$(CC) $(CFLAGS) -o digitap main.c $(LIBS)

clean:
	rm -f digitap

.PHONY: clean
