CFLAGS=-Wall

all: least

least: least.o terminal.o
	$(CC) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	$(RM) *.o least