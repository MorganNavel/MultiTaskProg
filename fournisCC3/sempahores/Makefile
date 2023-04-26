
BIN=bin/avion bin/passager

SRCS1=avion.c
SRCS2=passager.c


default: $(BIN)


obj/%.o: %.c
	gcc -Wall -Iinclude -c $< -o $@

bin/avion: $(SRCS1:%.c=obj/%.o) obj/simulation.o
	gcc -o $@ $+

bin/passager: $(SRCS2:%.c=obj/%.o)  obj/simulation.o
	gcc -o $@ $+

clean:
	rm -f $(BIN) *~
