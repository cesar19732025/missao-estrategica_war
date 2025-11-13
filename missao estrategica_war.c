#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*
    Programa: Simulação de Ataques entre Territórios com Missões
    Autor: [Seu Nome]
    Descrição:
        - Adiciona missões estratégicas individuais para cada jogador.
        - Missões são sorteadas automaticamente a partir de um vetor de strings.
        - Cada missão é armazenada dinamicamente (malloc) por jogador.
        - Verificação das missões ocorre ao final de cada turno.
        - Mantém modularização, uso de ponteiros e gerenciamento de memória.
*/

/* Definição da estrutura Territorio */
typedef struct {
    char nome[30];   // Nome do território
    char cor[10];    // Cor do exército (dono)
    int tropas;      // Quantidade de tropas
} Territorio;

/* Protótipos */
Territorio* alocarTerritorios(int n);
void cadastrarTerritorios(Territorio* mapa, int n);
void exibirTerritorios(Territorio* mapa, int n);
void atacar(Territorio* atacante, Territorio* defensor);
void liberarMemoria(Territorio* mapa, int n, char** missoesJogadores, int numJogadores);
int lerInteiro(const char* prompt, int min, int max);

/* Novas funções relacionadas às missões */
void atribuirMissao(char* destino, char* missoes[], int totalMissoes);
int verificarMissao(char* missao, Territorio* mapa, int tamanho, const char* corJogador);
void exibirMissao(const char* missao);

int main() {
    srand((unsigned int)time(NULL)); // Garantir aleatoriedade para rand()

    printf("=== SISTEMA DE TERRITORIOS (SIMULACAO DE ATAQUES) COM MISSOES ===\n");

    int n = lerInteiro("Informe o numero total de territorios: ", 1, 200);

    /* Alocação dinâmica do vetor de territórios */
    Territorio* mapa = alocarTerritorios(n);
    if (!mapa) {
        fprintf(stderr, "Erro: memoria insuficiente.\n");
        return 1;
    }

    /* Cadastro dos territorios */
    cadastrarTerritorios(mapa, n);

    /* Definicao de jogadores - aqui usamos 2 jogadores (pode expandir) */
    const int numJogadores = 2;
    char coresJogadores[numJogadores][10]; // cores informadas pelos jogadores
    for (int i = 0; i < numJogadores; i++) {
        printf("Informe a cor/nome do jogador %d (max 9 chars) que representara seu exército: ", i + 1);
        if (scanf("%9s", coresJogadores[i]) != 1) {
            strncpy(coresJogadores[i], "Jogador", sizeof(coresJogadores[i]) - 1);
            coresJogadores[i][sizeof(coresJogadores[i]) - 1] = '\0';
        }
    }

    /* Vetor de missoes pre-definidas (pelo menos 5) */
    char* missoesDisponiveis[] = {
        "Possuir pelo menos 3 territorios",
        "Possuir pelo menos 10 tropas no total",
        "Conquistar um territorio chamado Capital",
        "Possuir 3 territorios consecutivos no mapa",
        "Ter mais tropas totais que qualquer outro jogador"
    };
    const int totalMissoes = sizeof(missoesDisponiveis) / sizeof(missoesDisponiveis[0]);

    /* Aloca e atribui missao dinamicamente para cada jogador */
    char** missoesJogadores = (char*) malloc(sizeof(char) * numJogadores);
    if (!missoesJogadores) {
        fprintf(stderr, "Erro: memoria insuficiente para missoes.\n");
        free(mapa);
        return 1;
    }

    for (int i = 0; i < numJogadores; i++) {
        /* sorteia missao e aloca memoria exata para a string */
        int idx = rand() % totalMissoes;
        size_t len = strlen(missoesDisponiveis[idx]) + 1;
        missoesJogadores[i] = (char*) malloc(len);
        if (!missoesJogadores[i]) {
            fprintf(stderr, "Erro: memoria insuficiente para missao do jogador %d.\n", i+1);
            /* limpeza parcial antes de sair */
            for (int j = 0; j < i; j++) free(missoesJogadores[j]);
            free(missoesJogadores);
            free(mapa);
            return 1;
        }
        /* atribui a missao (passagem por referência: destino já alocado) */
        atribuirMissao(missoesJogadores[i], missoesDisponiveis, totalMissoes);
    }

    /* Exibe as missoes uma vez no inicio (passagem por valor para exibir) */
    printf("\n=== MISSÕES SORTEADAS ===\n");
    for (int i = 0; i < numJogadores; i++) {
        printf("Jogador %d (cor %s) - Missao: ", i + 1, coresJogadores[i]);
        exibirMissao(missoesJogadores[i]); /* exibe por valor (const char*) */
    }
    printf("=================================\n");

    /* Menu interativo: exibir, atacar, sair */
    int opcao;
    int vencedor = -1; /* -1 = nenhum vencedor ainda, senao indice do jogador vencedor */
    do {
        printf("\nMenu:\n");
        printf(" 1 - Exibir territorios\n");
        printf(" 2 - Realizar ataque\n");
        printf(" 3 - Sair\n");
        opcao = lerInteiro("Escolha uma opcao: ", 1, 3);

        if (opcao == 1) {
            exibirTerritorios(mapa, n);
        } else if (opcao == 2) {
            exibirTerritorios(mapa, n);
            int atk_idx = lerInteiro("Indice do territorio atacante (1..n): ", 1, n) - 1;
            int def_idx = lerInteiro("Indice do territorio defensor (1..n): ", 1, n) - 1;

            if (atk_idx < 0 || atk_idx >= n || def_idx < 0 || def_idx >= n) {
                printf("Erro: indices invalidos.\n");
                continue;
            }

            Territorio* atacante = mapa + atk_idx;
            Territorio* defensor = mapa + def_idx;

            if (atk_idx == def_idx) {
                printf("Erro: atacante e defensor sao o mesmo territorio.\n");
                continue;
            }
            if (strcmp(atacante->cor, defensor->cor) == 0) {
                printf("Erro: nao e permitido atacar um territorio da propria cor.\n");
                continue;
            }
            if (atacante->tropas <= 0) {
                printf("Erro: atacante nao possui tropas suficientes para atacar.\n");
                continue;
            }

            /* Realiza o ataque (passando ponteiros) */
            atacar(atacante, defensor);

            /* Exibe os dois territorios envolvidos após o ataque */
            printf("\n>>> Estado apos o ataque <<<\n");
            printf("Atacante '%s' - Cor: %s - Tropas: %d\n", atacante->nome, atacante->cor, atacante->tropas);
            printf("Defensor  '%s' - Cor: %s - Tropas: %d\n", defensor->nome, defensor->cor, defensor->tropas);

            /* Ao final do turno, verificar se algum jogador cumpriu sua missao */
            for (int j = 0; j < numJogadores; j++) {
                if (verificarMissao(missoesJogadores[j], mapa, n, coresJogadores[j])) {
                    vencedor = j;
                    break;
                }
            }
            if (vencedor != -1) {
                printf("\n** MISSÃO CUMPRIDA! VENCEDOR: Jogador %d (cor %s) **\n",
                       vencedor + 1, coresJogadores[vencedor]);
                break; /* encerra o loop principal */
            }
        }
    } while (opcao != 3);

    /* Liberar memoria (mapa e missoes) */
    liberarMemoria(mapa, n, missoesJogadores, numJogadores);

    printf("Programa finalizado. Memoria liberada.\n");
    return 0;
}

/* ============================================================
   Funções existentes (ajustadas) e novas funções criadas
   ============================================================ */

/* Aloca dinamicamente um vetor de Territorio com calloc e retorna o ponteiro */
Territorio* alocarTerritorios(int n) {
    Territorio* mapa = (Territorio*) calloc((size_t)n, sizeof(Territorio));
    return mapa; /* NULL se falhar */
}

/* Função para cadastro dos territorios.
   Usa ponteiro (mapa) e preenche cada posicao */
void cadastrarTerritorios(Territorio* mapa, int n) {
    for (int i = 0; i < n; i++) {
        Territorio* t = mapa + i; // acesso via ponteiro
        printf("\nCadastro do %do territorio:\n", i + 1);

        printf("Nome (sem espacos) (max 29 chars): ");
        if (scanf("%29s", t->nome) != 1) {
            /* limpar buffer e definir nome padrao se falhar */
            strcpy(t->nome, "Territorio");
        }

        printf("Cor do exercito (max 9 chars): ");
        if (scanf("%9s", t->cor) != 1) {
            strcpy(t->cor, "Neutro");
        }

        t->tropas = lerInteiro("Quantidade de tropas (>=0): ", 0, 1000000);
    }
}

/* Exibe todos os territorios do mapa. Acessa por ponteiros. */
void exibirTerritorios(Territorio* mapa, int n) {
    printf("\n=== TERRITORIOS CADASTRADOS ===\n");
    for (int i = 0; i < n; i++) {
        Territorio* t = mapa + i;
        printf("Indice %d:\n", i + 1);
        printf(" Nome: %s\n", t->nome);
        printf(" Cor do exercito: %s\n", t->cor);
        printf(" Tropas: %d\n", t->tropas);
        printf("-------------------------------------\n");
    }
}

/*
    Função de ataque:
    - Recebe ponteiros para atacante e defensor.
    - Simula uma rolagem de dado (1..6) para cada um usando rand().
    - Se atacante ganhar (valor maior): defensor muda de dono (cor) para a do atacante,
      e recebe metade (inteiro) das tropas do atacante.
    - Se atacante perder (ou empatar), atacante perde 1 tropa (min 0).
*/
void atacar(Territorio* atacante, Territorio* defensor) {
    int dado_atk = (rand() % 6) + 1; // 1..6
    int dado_def = (rand() % 6) + 1; // 1..6

    printf("\nRolagem: Atacante '%s' (cor %s) -> Dado: %d\n", atacante->nome, atacante->cor, dado_atk);
    printf("         Defensor  '%s' (cor %s) -> Dado: %d\n", defensor->nome, defensor->cor, dado_def);

    if (dado_atk > dado_def) {
        /* atacante vence */
        int transfer = atacante->tropas / 2; // metade inteira das tropas do atacante
        if (transfer <= 0) {
            /* não dá pra transferir tropas; atacante perde 1 tropa */
            printf("Atacante venceu a rolagem, mas nao havia tropas suficientes para transferir.\n");
            if (atacante->tropas > 0) atacante->tropas -= 1;
            return;
        }

        /* Transferir cor e tropas para o defensor */
        strncpy(defensor->cor, atacante->cor, sizeof(defensor->cor) - 1);
        defensor->cor[sizeof(defensor->cor) - 1] = '\0';

        /* Atualiza tropas do defensor e do atacante */
        defensor->tropas = transfer;
        atacante->tropas -= transfer;

        /* Garantia: atacante nao pode ficar negativo */
        if (atacante->tropas < 0) atacante->tropas = 0;

        printf("Resultado: Atacante venceu! Territorio '%s' agora pertence a '%s' e recebeu %d tropas.\n",
               defensor->nome, defensor->cor, defensor->tropas);
    } else {
        /* Empate ou defensor vence */
        printf("Resultado: Atacante perdeu (ou empatou). Atacante perde 1 tropa.\n");
        if (atacante->tropas > 0) atacante->tropas -= 1;
    }
}

/* Atribui (sorteia) uma missao para 'destino'.
   'destino' deve ser um buffer previamente alocado (malloc) pelo chamador.
   missoes[] eh um vetor de strings disponiveis.
*/
void atribuirMissao(char* destino, char* missoes[], int totalMissoes) {
    int idx = rand() % totalMissoes;
    strcpy(destino, missoes[idx]); /* copia a string sorteada para destino */
}

/* Exibe a missao (passagem por valor/por const char*). */
void exibirMissao(const char* missao) {
    printf("%s\n", missao);
}

/*
    verificarMissao:
    - Avalia se a missao dada foi cumprida pelo jogador de cor 'corJogador'
    - Implementação simples baseada em deteccao de palavras-chave nas strings de missoes
    - Retorna 1 se cumprida, 0 se nao
*/
int verificarMissao(char* missao, Territorio* mapa, int tamanho, const char* corJogador) {
    /* Caso 1: Possuir pelo menos 3 territorios */
    if (strstr(missao, "3 territorios") != NULL) {
        int count = 0;
        for (int i = 0; i < tamanho; i++) {
            if (strcmp(mapa[i].cor, corJogador) == 0) count++;
        }
        return (count >= 3) ? 1 : 0;
    }

    /* Caso 2: Possuir pelo menos 10 tropas no total */
    if (strstr(missao, "10 tropas") != NULL) {
        int total = 0;
        for (int i = 0; i < tamanho; i++) {
            if (strcmp(mapa[i].cor, corJogador) == 0) total += mapa[i].tropas;
        }
        return (total >= 10) ? 1 : 0;
    }

    /* Caso 3: Conquistar um territorio chamado Capital */
    if (strstr(missao, "Capital") != NULL) {
        for (int i = 0; i < tamanho; i++) {
            if (strcmp(mapa[i].nome, "Capital") == 0 && strcmp(mapa[i].cor, corJogador) == 0) {
                return 1;
            }
        }
        return 0;
    }

    /* Caso 4: Possuir 3 territorios consecutivos no mapa */
    if (strstr(missao, "3 territorios consecutivos") != NULL) {
        if (tamanho < 3) return 0;
        for (int i = 0; i <= tamanho - 3; i++) {
            if (strcmp(mapa[i].cor, corJogador) == 0 &&
                strcmp(mapa[i+1].cor, corJogador) == 0 &&
                strcmp(mapa[i+2].cor, corJogador) == 0) {
                return 1;
            }
        }
        return 0;
    }

    /* Caso 5: Ter mais tropas totais que qualquer outro jogador (compara soma por cor)
       Aqui, com dois jogadores, verificamos se corJogador tem total estritamente maior.
    */
    if (strstr(missao, "mais tropas totais") != NULL) {
        int totalJogador = 0;
        /* calcular total tropas de corJogador */
        for (int i = 0; i < tamanho; i++) {
            if (strcmp(mapa[i].cor, corJogador) == 0) totalJogador += mapa[i].tropas;
        }
        /* procurar maxima entre outras cores */
        int maxOutros = 0;
        /* comparar com todas as cores diferentes encontradas */
        for (int i = 0; i < tamanho; i++) {
            if (strcmp(mapa[i].cor, corJogador) != 0) {
                /* soma tropas dessa cor */
                int soma = 0;
                const char* corAtual = mapa[i].cor;
                /* somar apenas se for a primeira vez que encontramos essa cor (evitar recalculo repetido) */
                int jaCalculada = 0;
                for (int k = 0; k < i; k++) {
                    if (strcmp(mapa[k].cor, corAtual) == 0) { jaCalculada = 1; break; }
                }
                if (jaCalculada) continue;
                for (int j = 0; j < tamanho; j++) {
                    if (strcmp(mapa[j].cor, corAtual) == 0) soma += mapa[j].tropas;
                }
                if (soma > maxOutros) maxOutros = soma;
            }
        }
        return (totalJogador > maxOutros) ? 1 : 0;
    }

    /* Se a missao nao for reconhecida, retornar 0 (nao cumprida) */
    return 0;
}

/* Libera a memoria alocada para o mapa e para as missoes dos jogadores */
void liberarMemoria(Territorio* mapa, int n, char** missoesJogadores, int numJogadores) {
    if (mapa) free(mapa);
    if (missoesJogadores) {
        for (int i = 0; i < numJogadores; i++) {
            if (missoesJogadores[i]) free(missoesJogadores[i]);
        }
        free(missoesJogadores);
    }
}

/* Le um inteiro com prompt e valida entre min e max */
int lerInteiro(const char* prompt, int min, int max) {
    int valor;
    int ok;
    do {
        printf("%s", prompt);
        ok = (scanf("%d", &valor) == 1);
        if (!ok) {
            /* limpar buffer */
            int c;
            while ((c = getchar()) != EOF && c != '\n');
            printf("Entrada invalida. Tente novamente.\n");
            continue;
        }
        if (valor < min || valor > max) {
            printf("Valor fora do intervalo (%d..%d). Tente novamente.\n", min, max);
            ok = 0;
        }
    } while (!ok);
    return valor;
}