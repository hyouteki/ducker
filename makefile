CLAGS = -Wall -Wextra

default: build

build: ducker.c clean
	gcc ducker.c -o ducker $(CFLAGS)

clean:
	rm -f ducker
