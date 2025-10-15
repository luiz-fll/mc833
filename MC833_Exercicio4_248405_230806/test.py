import sys
import subprocess
import os
# import pandas as pd



if __name__ == "__main__":
    port = 50000
    clients: list[subprocess.Popen] = []
    results = []
    for backlog in range(11):
        with (open(f"tests/server_{backlog}.out", 'w') as server_f,
              open(f"tests/server_{backlog}.err", 'w') as server_err):
            server = subprocess.Popen(["./servidor", str(port), str(backlog), "1"], stdout=server_f, stderr=server_f)
            #subprocess.run(["sleep", "1"])

            for i in range(10):
                with (open(f"tests/{backlog}_{i}.out", 'w') as f,
                    open(f"tests/{backlog}_{i}.err", 'w') as err):
                    clients.append(subprocess.Popen(["./cliente", "127.0.0.1", str(port)], stdout=f, stderr=err))
            
            os.system(f'ss -ant | grep ":{port}" | grep ESTAB | wc -l > tmp.txt')
            with open('tmp.txt', 'r') as f:
                s = f.readline()
                print(s)
                established = int(s)
                rejected = 10 - established
                results.append((established, rejected))
                # Conta conexões ESTABLISHED com o servidor (filtra pela porta usada)
                #ESTABLISHED=$(ss -ant | grep ":$PORTA " | grep ESTAB | wc -l)
                #REJEITADAS=$((MAX_CLIENTES - ESTABLISHED))
        server.kill()
        port += 1
    subprocess.run(["sleep", "1"])
    for c in clients:
        c.kill()
    print(results)
    
    

