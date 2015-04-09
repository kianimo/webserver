CFLAGS=-std=c11 -Wall

all: src/
	mkdir -p bin
	$(CC) $(CFLAGS) -c src/*.c
	$(CC) $(CFLAGS) *.o -o bin/SERVER
	rm *.o

clean:
	rm -rf bin
