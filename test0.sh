#!/bin/bash

# Executa o coordinator em segundo plano
./coordinator 4 2 10 &

# Armazena o PID do coordinator
coordinator_pid=$!

sleep 5

# Executa os processos em segundo plano
./process 0 10 2 &
./process 1 10 2 &

# Aguarda o coordinator terminar de executar
wait $coordinator_pid

# Executa o validador de resultado e armazena a saída em uma variável
resultado_output=$(python3 resultado_validator.py "resultado.txt" 2 10 "test0_time_execution.txt")

# Verifica se a saída do validador de resultado contém a mensagem esperada
if [[ $resultado_output == "The result is valid." ]]; then
    # Executa o validador de log e armazena a saída em uma variável
    log_output=$(python3 log_validator.py)
    
    # Verifica se a saída do validador de log contém a mensagem esperada
    if [[ $log_output == "Log file was successfully validated" ]]; then
        echo "All validations done successfully."
    else
        echo "Error: Log validation failed."
    fi
else
    echo "Erro: Result validation failed."
fi
