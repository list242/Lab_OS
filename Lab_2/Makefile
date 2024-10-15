.PHONY: all clean run

all: mycat

mycat: mycat.c
	gcc mycat.c -o mycat -Wall -Wextra

run: mycat
	./mycat

clean:
	rm -rf *.o mycat
