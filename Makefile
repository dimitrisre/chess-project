OUT_NAME = chess 

${OUT_NAME}: board.o data.o game.o search.o jEvalFunction.o
	#Making the chess
	gcc -Wall board.o data.o game.o search.o jEvalFunction.o  -o ${OUT_NAME}

board.o: board.c
		gcc -c board.c

data.o: data.c
		gcc -c data.c

game.o: game.c
		gcc -c game.c

search.o: search.c
		gcc -c search.c

jEvalFunction.o: jEvalFunction.c
		gcc -c jEvalFunction.c

board.c: defines.h

data.c: defines.h

game.c: defines.h

search.c: defines.h

jEvalFunction.c: defines.h

all:
	gcc -c board.c
	gcc -c data.c
	gcc -c game.c
	gcc -c search.c
	gcc -c jEvalFunction.c
	gcc -Wall board.o data.o game.o search.o jEvalFunction.o  -o ${OUT_NAME}



	
clean:
	rm *.o
	rm ${OUT_NAME}
