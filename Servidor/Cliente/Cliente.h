#include <windows.h>
#include <tchar.h>

#define TAM 256
#define MAX_JOGADORES 20
#define MIN_LIN 10
#define MAX_LIN 80
#define MIN_COL 10
#define MAX_COL 80

typedef struct {
	TCHAR cima;
	TCHAR baixo;
	TCHAR esq;
	TCHAR dir;
} TECLAS;

typedef struct {
	TCHAR username[TAM];
	int status;				// 0: Logado, à espera do início do jogo  -|-  1: Em "modo de jogo"
	int score;
	TECLAS teclas;
} JOGADOR;

typedef struct {
	int max_jogadores;
	// Serpente
	int N, A, T;
	// Objetos
	int O, efeito, duracao, vulgar, invulgar, raro;
	// Dimensões
	int linhas, colunas;
} DADOS_JOGO;