#pragma once

#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>

#define PIPE_NAME TEXT("\\\\.\\pipe\\PIPE_")
#define PIPE_NAME2 TEXT("\\\\.\\pipe\\teste2")
#define N_MAX_LEITORES 10
#define TAM 256

HANDLE PipeLeitores[N_MAX_LEITORES];
int total;
BOOL TERMINAR = FALSE;

DWORD WINAPI RecebeLeitores(LPVOID param);
DWORD WINAPI AtendeCliente(LPVOID param);
DWORD nn;