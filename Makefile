TARGETS = proj2 main event manager resource system

all: ${TARGETS}

proj2: main.o event.o manager.o resource.o system.o
	gcc -g -Wall -Wextra -Werror -pthread -o proj2 main.o event.o manager.o resource.o system.o

main: main.c manager.c resource.c defs.h
	gcc -g -Wall -Wextra -Werror -pthread -c main.c

event: event.c defs.h
	gcc -g -Wall -Wextra -Werror -pthread -c event.c
  
manager: manager.c defs.h
	gcc -g -Wall -Wextra -Werror -pthread -c manager.c

resource: resource.c defs.h main.c
	gcc -g -Wall -Wextra -Werror -pthread -c resource.c

system: system.c defs.h
	gcc -g -Wall -Wextra -Werror -pthread -c system.c

clean: 
	rm -f proj2 *.o

valgrind:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./proj2