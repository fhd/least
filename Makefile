all: least

least: least.c
	$(CC) -o $@ $^
