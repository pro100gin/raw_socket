CFLAGS = -g -O2 -Wall -pedantic -ansi
LFLAGS =

vpath %.h ../include/
vpath %.c ../lib/

client.o: client.c
		gcc -c $< $(CFLAGS) $(LFLAGS)

clean:
		rm -rf *.o *.out
