all: least

least: least.o terminal.o
	$(CC) -o $@ $^

%.o: %.c
	$(CC) -c $<

clean:
	$(RM) *.o least