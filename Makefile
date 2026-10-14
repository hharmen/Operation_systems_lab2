all: parent child

parent: parent.c
	gcc -Wall -Wextra -g -o parent parent.c

child: child.c
	gcc -Wall -Wextra -g -o child child.c

clean:
	rm -f parent child

.PHONY: all clean