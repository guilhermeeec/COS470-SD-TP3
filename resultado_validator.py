def verificar_arquivo(resultado_file, n, r):
    with open(resultado_file, 'r') as file:
        linhas = file.readlines()
    
    # Verificar o número de linhas
    if len(linhas) != n * r:
        return False
    
    # Verificar a ordem das linhas e a frequência de cada processo
    processos = [0] * n
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
        
        ultimo_tempo = (horas, minutos, segundos, microsegundos)
    
    return True

# Exemplo de uso:
n = 2  # Número de processos
r = 10  # Número de repetições
resultado_file = "resultado.txt"

correto = verificar_arquivo(resultado_file, n, r)
if correto:
    print("O arquivo está correto.")
else:
    print("O arquivo está incorreto.")
