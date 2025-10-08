import sys
import subprocess
import pandas as pd



if __name__ == "__main__":
    port = 60000
    clients: list[subprocess.Popen] = []
    for backlog in range(11):
        with (open(f"tests/server_{backlog}.out", 'w') as server_f,
              open(f"tests/server_{backlog}.err", 'w') as server_err):
            server = subprocess.Popen(["./servidor", str(port), str(backlog), "1"], stdout=server_f, stderr=server_f)
            subprocess.run(["sleep", "1"])

            for i in range(10):
                with (open(f"tests/{backlog}_{i}.out", 'w') as f,
                    open(f"tests/{backlog}_{i}.err", 'w') as err):
                    clients.append(subprocess.Popen(["./cliente", "127.0.0.1", str(port)], stdout=f, stderr=err))
            
            a = clients[0]
            # subprocess.run(["sleep", "1"])
        server.kill()
        port += 1
    subprocess.run(["sleep", "1"])
    for c in clients:
        c.kill()
    
    

