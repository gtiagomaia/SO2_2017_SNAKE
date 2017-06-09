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
#define N_MAX_JOGADORES 20
#define BUFFER_TAM 256
#define NOME_TAM 100 



typedef struct {
	TCHAR username[NOME_TAM];
	int pontuacao;
}JOGADOR;

typedef struct {
	TCHAR mensagem[BUFFER_TAM];
	int idcomando;
	JOGADOR jogador;
}MENSG;


// estrutura serpente
// . head pos_x pos_y
// . tail pos_x pos_y
// . T	segmentos
// . O	objetos
// . S  segundos /sobre objeto
// . A	numero de serpentes automaticas
// . 