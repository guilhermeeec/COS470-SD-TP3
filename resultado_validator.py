import sys

def verificar_arquivo(resultado_file, n, r, diferenca_file):
    with open(resultado_file, 'r') as file:
        linhas = file.readlines()

    # Verificar o número de linhas
    if len(linhas) != n * r:
        return False

    # Verificar a ordem das linhas e a frequência de cada processo
    processos = [0] * n
    primeiro_tempo = None
    ultimo_tempo = None

    for linha in linhas:
        processo, tempo = linha.strip().split(" - ")
        processo = int(processo[1:-1])  # Extrair o número do processo
        tempo = tempo.split(":")
        horas = int(tempo[0])
        minutos = int(tempo[1])
        segundos, microsegundos = map(int, tempo[2].split("."))

        # Verificar a ordem do relógio do sistema
        if ultimo_tempo is not None and (
                horas < ultimo_tempo[0] or
                (horas == ultimo_tempo[0] and minutos < ultimo_tempo[1]) or
                (horas == ultimo_tempo[0] and minutos == ultimo_tempo[1] and segundos < ultimo_tempo[2]) or
                (horas == ultimo_tempo[0] and minutos == ultimo_tempo[1] and segundos == ultimo_tempo[2] and microsegundos <= ultimo_tempo[3])
        ):
            return False

        # Verificar se cada processo escreveu r vezes
        processos[processo] += 1
        if processos[processo] > r:
            return False

        # Salvar o primeiro e último tempo
        if primeiro_tempo is None:
            primeiro_tempo = (horas, minutos, segundos, microsegundos)
        ultimo_tempo = (horas, minutos, segundos, microsegundos)

    # Calcular a diferença de tempo em segundos.microssegundos
    diferenca = calcular_diferenca_tempo(primeiro_tempo, ultimo_tempo)
    salvar_diferenca_tempo(diferenca_file, diferenca)

    return True

def calcular_diferenca_tempo(tempo_inicial, tempo_final):
    segundos_inicial = tempo_inicial[0] * 3600 + tempo_inicial[1] * 60 + tempo_inicial[2]
    microssegundos_inicial = tempo_inicial[3]
    segundos_final = tempo_final[0] * 3600 + tempo_final[1] * 60 + tempo_final[2]
    microssegundos_final = tempo_final[3]
    
    diferenca_segundos = segundos_final - segundos_inicial
    diferenca_microssegundos = microssegundos_final - microssegundos_inicial
    
    # Ajustar a diferença se for negativa
    if diferenca_microssegundos < 0:
        diferenca_microssegundos += 1000000
        diferenca_segundos -= 1

    return f"{diferenca_segundos}.{diferenca_microssegundos:06d}"

def salvar_diferenca_tempo(nome_arquivo, diferenca):
    with open(nome_arquivo, 'a') as file:
        file.write(diferenca+"\n")

# Obter os argumentos da linha de comando
resultado_file = sys.argv[1]
n = int(sys.argv[2])
r = int(sys.argv[3])
diferenca_file = sys.argv[4]

correto = verificar_arquivo(resultado_file, n, r, diferenca_file)
if correto:
    print("The result is valid.")
else:
    print("The result is invalid.")
