CC = g++

CFLAGS =

OBJS = lex.yy.o utility.o main.o circular_array.o

OBJSP = utility.o circular_array.o prob.o

OBJSM = utility.o circular_Array.o multi.o

all: preprocessor probabilities multiwords

preprocessor: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o preprocessor -ll

probabilities: $(OBJSP)
	$(CC) $(CFLAGS) $(OBJSP) -o probabilities -ll

multiwords: $(OBJSM)
	$(CC) $(CFLAGS) $(OBJSM) -o multiwords -ll

multi.o: helper.h utility.h
	$(CC) $(CFLAGS) -c multiwords.cpp -o multi.o

prob.o: helper.h utility.h
	$(CC) $(CFLAGS) -c probabilities.cpp -o prob.o

main.o: helper.h utility.h tokenizer.h
	$(CC) $(CFLAGS) -c main.cpp

utility.o: utility.h helper.h
	$(CC) $(CFLAGS) -c utility.cpp

circular_array.o: circular_array.h
	$(CC) $(CFLAGS) -c circular_array.cpp

lex.yy.c: expression.l
	flex expression.l

lex.yy.o: lex.yy.c helper.h tokenizer.h
	$(CC) $(CFLAGS) -c lex.yy.c

clean:
	rm -f preprocessor probabilities multiwords $(OBJS) $(OBJSP) $(OBJSM) lex.yy.c ./data/*
