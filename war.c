#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>

#define NUM_TERRITORIOS 10
#define MAX_NOME_LEN 30
#define NUM_MISSOES 3
#define COR_JOGADOR 1
#define COR_COMPUTADOR 2

typedef struct {
    char nome[MAX_NOME_LEN];
    int corDono;
    int tropas;
} Territorio;

Territorio *alocarMapa();
void inicializarTerritorios(Territorio *mapa);
void liberarMemoria(Territorio *mapa);

void exibirMenuPrincipal();
void exibirMapa(const Territorio *mapa);
void exibirMissao(int missaoID, int corInimigo);

void faseDeAtaque(Territorio *mapa, int corAtacante);
void simularAtaque(Territorio *origem, Territorio *destino, int corAtacante);
int sortearMissao();
int verificarVitoria(const Territorio *mapa, int missaoID, int corInimigo);

void limparBufferEntrada();
int rolarDado();

int main() {
    setlocale(LC_ALL, "Portuguese");
    srand(time(NULL));

    Territorio *mapa = alocarMapa();
    if (mapa == NULL) {
        printf("Erro de memoria. Saindo.\n");
        return 1;
    }
    
    inicializarTerritorios(mapa);
    int missaoID = sortearMissao();
    int corInimigoMissao = (missaoID == 1) ? COR_COMPUTADOR : 0; 

    int escolha = -1;
    int vitoria = 0;

    printf("\n*** War Estruturado ***\n");

    do {
        printf("\n\n--- Rodada ---\n");
        exibirMapa(mapa);
        exibirMissao(missaoID, corInimigoMissao);
        exibirMenuPrincipal();

        printf("Escolha sua acao (0, 1, 2): ");
        if (scanf("%d", &escolha) != 1) {
            escolha = -1;
            limparBufferEntrada();
            printf("Entrada invalida.\n");
            continue;
        }
        limparBufferEntrada();

        switch (escolha) {
            case 1:
                faseDeAtaque(mapa, COR_JOGADOR);
                break;
            case 2:
                vitoria = verificarVitoria(mapa, missaoID, corInimigoMissao);
                if (vitoria) {
                    printf("\n🎉 MISSAO CUMPRIDA! VOCE VENCEU! 🎉\n");
                } else {
                    printf("\nMissao ainda nao completa.\n");
                }
                break;
            case 0:
                printf("\nEncerrando...\n");
                break;
            default:
                printf("\nOpcao invalida.\n");
                break;
        }

        if (escolha != 0 && vitoria == 0) {
            printf("\nPressione ENTER para continuar...");
            getchar(); 
        }
    } while (escolha != 0 && vitoria == 0);

    liberarMemoria(mapa);
    return 0;
}

Territorio *alocarMapa() {
    return (Territorio *)calloc(NUM_TERRITORIOS, sizeof(Territorio));
}

void inicializarTerritorios(Territorio *mapa) {
    // 5 países da América do Sul para o Jogador (COR_JOGADOR)
    const char *paises_sul[] = {"Brasil", "Argentina", "Chile", "Colombia", "Peru"};
    // 5 países da Europa para o PC (COR_COMPUTADOR)
    const char *paises_europa[] = {"Espanha", "Franca", "Alemanha", "Russia", "Italia"};

    for (int i = 0; i < NUM_TERRITORIOS; i++) {
        if (i < NUM_TERRITORIOS / 2) {
            // Territórios do Jogador (0 a 4)
            strncpy(mapa[i].nome, paises_sul[i], MAX_NOME_LEN - 1);
            mapa[i].corDono = COR_JOGADOR;
        } else {
            // Territórios do PC (5 a 9)
            strncpy(mapa[i].nome, paises_europa[i - NUM_TERRITORIOS / 2], MAX_NOME_LEN - 1);
            mapa[i].corDono = COR_COMPUTADOR;
        }

        mapa[i].nome[MAX_NOME_LEN - 1] = '\0';
        mapa[i].tropas = (rand() % 2) + 2;
    }
}

void liberarMemoria(Territorio *mapa) {
    if (mapa != NULL) {
        free(mapa);
    }
}

void exibirMenuPrincipal() {
    printf("\n--- Opcoes ---\n");
    printf("1. Atacar\n");
    printf("2. Verificar Vitoria\n");
    printf("0. Sair\n");
    printf("--------------\n");
}

void exibirMapa(const Territorio *mapa) {
    printf("\n--- Estado do Mapa ---\n");
    printf("| ID | Nome        | Dono    | Tropas |\n");
    printf("----------------------------------------\n");
    for (int i = 0; i < NUM_TERRITORIOS; i++) {
        const char *dono = (mapa[i].corDono == COR_JOGADOR) ? "JOGADOR" : "PC";
        printf("| %-2d | %-11s | %-7s | %-6d |\n", i + 1, mapa[i].nome, dono, mapa[i].tropas);
    }
    printf("----------------------------------------\n");
}

void exibirMissao(int missaoID, int corInimigo) {
    printf("\n--- Missao Secreta ---\n");
    switch (missaoID) {
        case 1:
            printf("1. Destruir COMPLETAMENTE o exercito **PC (%d)**.\n", corInimigo);
            break;
        case 2:
            printf("2. Conquistar **7 territorios**.\n");
            break;
        case 3:
            printf("3. Conquistar **todos os territorios** (%d).\n", NUM_TERRITORIOS);
            break;
        default:
            printf("Missao desconhecida.\n");
            break;
    }
}

int rolarDado() {
    return (rand() % 6) + 1;
}

void simularAtaque(Territorio *origem, Territorio *destino, int corAtacante) {
    
    if (origem->tropas <= 1) {
        printf("ATAQUE CANCELADO: Tropas insuficientes.\n");
        return;
    }

    printf("\n--- Resultado do Ataque ---\n");

    int dadoAtaque = rolarDado();
    int dadoDefesa = rolarDado();

    printf("Atacante (%s): %d | Defensor (%s): %d\n", origem->nome, dadoAtaque, destino->nome, dadoDefesa);

    if (dadoAtaque > dadoDefesa) {
        printf("VITORIA! Defensor perde 1 tropa.\n");
        destino->tropas--;
    } else {
        printf("DERROTA! Atacante perde 1 tropa.\n");
        origem->tropas--;
    }

    if (destino->tropas <= 0) {
        printf("\nTERRITORIO CONQUISTADO: %s!\n", destino->nome);
        destino->corDono = corAtacante;
        destino->tropas = 1;
        origem->tropas--;
    }
}

void faseDeAtaque(Territorio *mapa, int corAtacante) {
    int idOrigem, idDestino;

    printf("\n--- Iniciando Ataque ---\n");
    printf("ID (1 a %d) do territorio de origem: ", NUM_TERRITORIOS);
    if (scanf("%d", &idOrigem) != 1) { limparBufferEntrada(); return; }

    printf("ID (1 a %d) do territorio de destino: ", NUM_TERRITORIOS);
    if (scanf("%d", &idDestino) != 1) { limparBufferEntrada(); return; }
    limparBufferEntrada();

    if (idOrigem < 1 || idOrigem > NUM_TERRITORIOS || idDestino < 1 || idDestino > NUM_TERRITORIOS) {
        printf("ID(s) fora do limite.\n");
        return;
    }

    Territorio *origem = &mapa[idOrigem - 1];
    Territorio *destino = &mapa[idDestino - 1];

    if (origem->corDono != corAtacante) {
        printf("Origem nao e sua.\n");
        return;
    }
    if (destino->corDono == corAtacante) {
        printf("Destino ja e seu.\n");
        return;
    }
    
    simularAtaque(origem, destino, corAtacante);
}

int sortearMissao() {
    return (rand() % NUM_MISSOES) + 1;
}

int verificarVitoria(const Territorio *mapa, int missaoID, int corInimigo) {
    int territoriosJogador = 0;
    int inimigoEliminado = 1;

    for (int i = 0; i < NUM_TERRITORIOS; i++) {
        if (mapa[i].corDono == COR_JOGADOR) {
            territoriosJogador++;
        }
        if (mapa[i].corDono == corInimigo && corInimigo != 0) {
            inimigoEliminado = 0;
        }
    }

    switch (missaoID) {
        case 1:
            return inimigoEliminado;
        case 2:
            return (territoriosJogador >= 7);
        case 3:
            return (territoriosJogador == NUM_TERRITORIOS);
        default:
            return 0;
    }
}

void limparBufferEntrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}
