#!/bin/bash

# Valores de n a serem testados
n_values=(2 4 8 16 32 64)

# Fixar r e k
r=5
k=1

# Loop através dos valores de n
for n in "${n_values[@]}"
do
    # Executa o coordinator em segundo plano
    ./coordinator 4 $n $r &

    # Armazena o PID do coordinator
    coordinator_pid=$!

    sleep 5

    # Executa os processos em segundo plano
    for ((i=0; i<n; i++))
    do
        ./process $i $r $k &
    done

    # Aguarda o coordinator terminar de executar
    wait $coordinator_pid

    # Executa o validador de resultado e armazena a saída em uma variável
    resultado_output=$(python3 resultado_validator.py "resultado.txt" $n $r "test2_time_execution.txt")

    # Verifica se a saída do validador de resultado contém a mensagem esperada
    if [[ $resultado_output == "The result is valid." ]]; then
        # Executa o validador de log e armazena a saída em uma variável
        log_output=$(python3 log_validator.py)
        
        # Verifica se a saída do validador de log contém a mensagem esperada
        if [[ $log_output == "Log file was successfully validated" ]]; then
            echo "Test $n: All validations done successfully."
        else
            echo "Test $n: Error: Log validation failed."
        fi
    else
        echo "Test $n: Error: Result validation failed."
    fi

    # Limpa os arquivos de resultado e log para o próximo teste
    rm resultado.txt
    rm log.txt

    sleep 5
done
