#!/bin/bash

# Configurações iniciais
PORT=$1
SLEEP=2
HOST="127.0.0.1"
MAX_CLIENTES=10

# Verifica o tcp_max_syn_backlog atual do sistema
echo "$HOST $PORT"
echo "tcp_max_syn_backlog do sistema: $(cat /proc/sys/net/ipv4/tcp_max_syn_backlog)"
echo ""

printf "%-8s | %-20s | %-20s\n" "Backlog" "Conexões ESTABLISHED" "Conexões Rejeitadas"
echo "---------------------------------------------------------------"

for BACKLOG in {0..10}; do
    ./servidor $PORT "$BACKLOG" $SLEEP > /dev/null &
    SERVER_PID=$!

    sleep 1

    for i in $(seq 1 $MAX_CLIENTES); do
        ./cliente $HOST $PORT > /dev/null 2> /dev/null &
    done

    #wait
    #kill $SERVER_PID
    # Conta conexões ESTABLISHED com o servidor (filtra pela porta usada)
    ESTABLISHED=$(ss -ant | grep ":$PORT " | grep -c ESTAB)
    REJEITADAS=$((MAX_CLIENTES - ESTABLISHED))

    # Mostra os resultados
    printf "%-8s | %-20s | %-20s\n" "$BACKLOG" "$ESTABLISHED" "$REJEITADAS"
    PORT=$((PORT + 1))
    # Pequeno delay entre testes
    sleep 1
    kill -s SIGINT $SERVER_PID
done
