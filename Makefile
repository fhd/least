CFLAGS=-Wall
LDFLAGS=-lm

all: least

least: least.o terminal.o
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	$(RM) *.o least