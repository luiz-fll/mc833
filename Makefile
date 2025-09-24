CFLAGS = -Wall -Werror
# LDFLAGS = -lm

.PHONY: all

all: servidor cliente

servidor: servidor.c utils.c
	gcc $(CFLAGS) servidor.c utils.c -o servidor

cliente: cliente.c utils.c
	gcc $(CFLAGS) cliente.c utils.c -o cliente

clean:
	rm -f servidor
	rm -f cliente
