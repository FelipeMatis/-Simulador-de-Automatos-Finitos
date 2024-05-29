#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int estado_atual;
    char simbolo_entrada;
    int proximo_estado;
} Transicao;

typedef struct {
    int num_estados;
    char *alfabeto;
    int num_transicoes;
    Transicao *transicoes;
    int estado_inicial;
    int *estados_finais;
    int num_estados_finais;
} AutomatoFinito;

void ler_definicao_automato(const char *nome_arquivo, AutomatoFinito *af) {
    FILE *arquivo = fopen(nome_arquivo, "r");
    if (!arquivo) {
        perror("Falha ao abrir o arquivo de definição do autômato");
        exit(EXIT_FAILURE);
    }

    fscanf(arquivo, "%d", &af->num_estados);

    char buffer[100];
    fscanf(arquivo, "%s", buffer);
    af->alfabeto = strdup(buffer);

    fscanf(arquivo, "%d", &af->num_transicoes);

    af->transicoes = (Transicao *)malloc(af->num_transicoes * sizeof(Transicao));
    for (int i = 0; i < af->num_transicoes; i++) {
        fscanf(arquivo, "%d %c %d", &af->transicoes[i].estado_atual, &af->transicoes[i].simbolo_entrada, &af->transicoes[i].proximo_estado);
    }

    fscanf(arquivo, "%d", &af->estado_inicial);

    fscanf(arquivo, "%d", &af->num_estados_finais);

    af->estados_finais = (int *)malloc(af->num_estados_finais * sizeof(int));
    for (int i = 0; i < af->num_estados_finais; i++) {
        fscanf(arquivo, "%d", &af->estados_finais[i]);
    }

    fclose(arquivo);
}

char **ler_casos_teste(const char *nome_arquivo, int *num_casos) {
    FILE *arquivo = fopen(nome_arquivo, "r");
    if (!arquivo) {
        perror("Falha ao abrir o arquivo de casos de teste");
        exit(EXIT_FAILURE);
    }

    fscanf(arquivo, "%d", num_casos);

    char **casos = (char **)malloc((*num_casos) * sizeof(char *));
    for (int i = 0; i < *num_casos; i++) {
        char buffer[100];
        fscanf(arquivo, "%s", buffer);
        casos[i] = strdup(buffer);
    }

    fclose(arquivo);
    return casos;
}

int simular_automato(AutomatoFinito *af, const char *entrada) {
    int estado_atual = af->estado_inicial;
    for (int i = 0; entrada[i] != '\0'; i++) {
        int transicao_encontrada = 0;
        for (int j = 0; j < af->num_transicoes; j++) {
            if (af->transicoes[j].estado_atual == estado_atual && af->transicoes[j].simbolo_entrada == entrada[i]) {
                estado_atual = af->transicoes[j].proximo_estado;
                transicao_encontrada = 1;
                break;
            }
        }
        if (!transicao_encontrada) {
            return 0; 
        }
    }

    for (int i = 0; i < af->num_estados_finais; i++) {
        if (af->estados_finais[i] == estado_atual) {
            return 1; 
        }
    }
    return 0; 
}

int main (int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Uso: %s <arquivo_definicao_automato> <arquivo_casos_teste> <arquivo_saida>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *arquivo_automato = argv[1];
    const char *arquivo_casos_teste = argv[2];
    const char *arquivo_saida = argv[3];

    AutomatoFinito af;
    ler_definicao_automato(arquivo_automato, &af);

    int num_casos;
    char **casos_teste = ler_casos_teste(arquivo_casos_teste, &num_casos);

    FILE *saida = fopen(arquivo_saida, "w");
    if (!saida) {
        perror("Falha ao abrir o arquivo de saída");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < num_casos; i++) {
        int resultado = simular_automato(&af, casos_teste[i]);
        fprintf(saida, "%s: %s\n", casos_teste[i], resultado ? "Aceito" : "Rejeitado");
        free(casos_teste[i]);
    }

    free(casos_teste);
    fclose(saida);

    free(af.alfabeto);
    free(af.transicoes);
    free(af.estados_finais);

    return EXIT_SUCCESS;
}

