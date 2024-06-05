CLAGS = -Wall -Wextra

default: run

run: build
	sudo ./ducker run duckerfile

build: ducker.c clean
	gcc ducker.c -o ducker $(CFLAGS)

buildscript: duckerscript.c duckerscript.h
	gcc duckerscript.c -o duckerscript $(CFLAGS)

clean:
	rm -f ducker duckerscript
