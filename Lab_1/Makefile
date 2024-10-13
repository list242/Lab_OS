.PHONY: all clean run

all: myls

myls: main.c
	gcc main.c -o myls -Wall -Wextra

run: myls
	./myls

clean:
	rm -rf *.o myls
