#include "Servidor.h"

int _tmain(int argc, LPTSTR argv[]) {

	HANDLE hThread;
	TCHAR buf[TAM];




#ifdef UNICODE 
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
	_setmode(_fileno(stderr), _O_WTEXT);
#endif





	return 0;
}





//=============== THREADS ========================//


DWORD WINAPI RecebeLeitores(LPVOID param) {
	HANDLE hPipe;


	while (!TERMINAR && total < N_MAX_LEITORES) {
		_tprintf(TEXT("[ESCRITOR] Vou passar a criacao de uma cópia do pipe '%s' ... (CreateNamedPipe)\n"), PIPE_NAME);

		//sentido de enviar OUTBOUND
		PipeLeitores[total] = CreateNamedPipe(PIPE_NAME, PIPE_ACCESS_OUTBOUND, PIPE_WAIT | PIPE_TYPE_MESSAGE
			| PIPE_READMODE_MESSAGE, N_MAX_LEITORES, TAM * sizeof(TCHAR), TAM * sizeof(TCHAR), 1000, NULL);

		if (PipeLeitores[total] == INVALID_HANDLE_VALUE) {
			_tperror(TEXT("Erro na ligação ao leitor!"));
			exit(-1);
		}

		//sentido de receber INBOUND
		hPipe = CreateNamedPipe(PIPE_NAME2, PIPE_ACCESS_INBOUND, PIPE_WAIT | PIPE_TYPE_MESSAGE
			| PIPE_READMODE_MESSAGE, N_MAX_LEITORES, TAM * sizeof(TCHAR), TAM * sizeof(TCHAR),
			1000, NULL);
		if (hPipe == INVALID_HANDLE_VALUE) {
			_tperror(TEXT("Erro na ligação ao leitor!"));
			exit(-1);
		}

		_tprintf(TEXT("[ESCRITOR] Esperar ligação de um leitor... (ConnectNamedPipe)\n"));
		if (!ConnectNamedPipe(PipeLeitores[total], NULL)) {
			_tperror(TEXT("Erro na ligação ao leitor!"));
			exit(-1);
		}

		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AtendeCliente, (LPVOID)hPipe, 0, NULL);
		total++;
	}


	for (int i = 0; i < total; i++) {
		DisconnectNamedPipe(PipeLeitores[i]);
		_tprintf(TEXT("[ESCRITOR] Vou desligar o pipe... (CloseHandle)\n"));
		CloseHandle(PipeLeitores[i]);
	}

	return 0;
}