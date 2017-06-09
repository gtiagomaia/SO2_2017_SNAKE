#pragma once

#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>

/// CONSTANTS
#define PEDIDO_USERNAME 100
#define PEDIDO_TERMINAR 50


#define PIPE_INPUT TEXT("\\\\.\\pipe\\PIPE_IN")
#define PIPE_OUTPUT TEXT("\\\\.\\pipe\\PIPE_OUT")
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
	int status;				// 0: Logado, � espera do in�cio do jogo  -|-  1: Em "modo de jogo"
	int score;
	TECLAS teclas;
} JOGADOR;

typedef struct {
	int max_jogadores;
	// Serpente
	int N, A, T;
	// Objetos
	int O, efeito, duracao, vulgar, invulgar, raro;
	// Dimens�es
	int linhas, colunas;
} DADOS_JOGO;

typedef struct {
	TCHAR mensagem[TAM];
	int idcomando;
	JOGADOR jogador;
	DADOS_JOGO jogo;

}MENSG;


// estrutura serpente
// . head pos_x pos_y
// . tail pos_x pos_y
// . T	segmentos
// . O	objetos
// . S  segundos /sobre objeto
// . A	numero de serpentes automaticas
// . 