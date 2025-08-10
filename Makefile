CC = gcc
FLAGS = -l pam

SRC = main.c auth.c
OBJ = ${SRC:.c=.o}

bsdm: ${OBJ}
	${CC} ${FLAGS} $^ -o $@

main.o: main.c
	${CC} ${FLAGS} -c $^ -o $@

auth.o: auth.c
	${CC} ${FLAGS} -c $^ -o $@


clean: 
	rm ${OBJ} bsdm
