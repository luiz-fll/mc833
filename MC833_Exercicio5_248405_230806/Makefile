CFLAGS = -Wall -Werror -O2
# LDFLAGS = -lm

.PHONY: all

all: servidor_fork servidor_select cliente

servidor_fork: utils.c servidor_fork.c
	gcc $(CFLAGS) servidor_fork.c utils.c -o servidor_fork

servidor_select: servidor_select.c utils.c
	gcc $(CFLAGS) servidor_select.c utils.c -o servidor_select

cliente: cliente.c utils.c
	gcc $(CFLAGS) cliente.c utils.c -o cliente

clean:
	rm -f servidor_select
	rm -f servidor_fork
	rm -f cliente
