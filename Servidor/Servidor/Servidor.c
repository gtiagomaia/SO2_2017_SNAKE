#include "Servidor.h"



/// *** VARIAVEIS GLOBAIS *** ///

JOGADOR lista_jogadores_ligados[N_MAX_JOGADORES];		//informacao dos jogadores ligados
MENSG msg_do_jogador;
MENSG msg_server;
HANDLE hPipe_jogador_escrita[N_MAX_JOGADORES];					//array de pipes dos jogadores ligados
HANDLE hPipe_jogador_leitura[N_MAX_JOGADORES];
HANDLE hThread_jogadores[N_MAX_JOGADORES];
HANDLE hMutex;
int contador_jogadores;									//conta o numero de jogadores que se ligam ao servidor
int num_jogadores_registados = 0;
BOOL TERMINAR = FALSE;
BOOL INICIAR_JOGO = FALSE;
OVERLAPPED overlapped_sincronizacao;
HANDLE hEvento;

/// *** PROTOTIPOS       *** ///

DWORD WINAPI RecebeJOGADOR(LPVOID param);
DWORD WINAPI AtendeJOGADOR(LPVOID param);


/// **** **** **** **** **** ///


int _tmain(int argc, LPTSTR argv[]) {

	HANDLE hThread;
	TCHAR buf[BUFFER_TAM];

#ifdef UNICODE 
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
	_setmode(_fileno(stderr), _O_WTEXT);
#endif


	//
	hEvento = CreateEvent(NULL, FALSE, FALSE, NULL);
	LPCTSTR Nome_Mutex = TEXT("mutex_para_comunicaodupla");
	hMutex = CreateMutex(NULL, FALSE, Nome_Mutex);



	while (!TERMINAR && contador_jogadores < N_MAX_JOGADORES)	{

		//ZeroMemory(&overlapped_sincronizacao, size(overlapped_sincronizacao));
		//overlapped_sincronizacao.hEvent = hEvento;


		_tprintf(TEXT("[SERVIDOR] Vou passar a criacao de uma cópia do pipe '%s' ... (CreateNamedPipe)\n"), PIPE_OUTPUT);
		//CRIAR PIPE sentido de ESCREVER para cliente OUTBOUND ====================================== //
		hPipe_jogador_escrita[contador_jogadores] = CreateNamedPipe(PIPE_OUTPUT, PIPE_ACCESS_OUTBOUND, PIPE_WAIT | PIPE_TYPE_MESSAGE
			| PIPE_READMODE_MESSAGE, N_MAX_JOGADORES, sizeof(msg_do_jogador), sizeof(msg_do_jogador), 1000, NULL);

				if (hPipe_jogador_escrita[contador_jogadores] == INVALID_HANDLE_VALUE) {
					_tprintf(TEXT("[SERVIDOR] Erro na ligação ao jogador '%d'!"), contador_jogadores);
					exit(-1);
				}

		// CRIAR PIPE PARA de LER do cliente INBOUND ====================================== //
		hPipe_jogador_leitura[contador_jogadores] = CreateNamedPipe(PIPE_INPUT, PIPE_ACCESS_DUPLEX, PIPE_WAIT | PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE,
			N_MAX_JOGADORES, sizeof(msg_do_jogador), sizeof(msg_do_jogador), 1000, NULL);

				if (hPipe_jogador_leitura[contador_jogadores] == INVALID_HANDLE_VALUE) {
					_tperror(TEXT("[SERVIDOR] Erro na ligação ao jogador '%d'!"), contador_jogadores);
					exit(-1);
				}
	
		//Ligar-se ao cliente
		_tprintf(TEXT("[SERVIDOR] Ligar-se a cliente... (ConnectNamedPipe)\n"));

		if (!ConnectNamedPipe(hPipe_jogador_escrita[contador_jogadores], &overlapped_sincronizacao)) {
			_tperror(TEXT("[SERVIDOR] Erro na ligação ao jogador '%d'!"), contador_jogadores);
			exit(-1);
		}
	


		_tprintf(TEXT("[SERVIDOR]aguardar....\n"));
		//WaitForSingleObject(hEvento, INFINITE);


		if (INICIAR_JOGO == FALSE) {
			_tprintf(TEXT("[SERVIDOR] vou atender jogador... (creating thread)\n"));
			hThread_jogadores[contador_jogadores] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AtendeJOGADOR, (LPVOID)contador_jogadores, 0, NULL);
			_tprintf(TEXT("-> ligado a jogador [%d]\n"), contador_jogadores);
			contador_jogadores++;
		}
		else {
			break;	//sai do ciclo e dá inicio ao jogo / logica
		}
	}



	
	_tprintf(TEXT("[SERVIDOR] Vou desligar.....\n"));
	WaitForMultipleObjects(contador_jogadores, hThread_jogadores, TRUE, INFINITE);
	//desliga pipes dos jogadores
	
	for (int i = 0; i <= contador_jogadores; i++) {
		DisconnectNamedPipe(hPipe_jogador_escrita[i]);
		DisconnectNamedPipe(hPipe_jogador_leitura[i]);
		CloseHandle(hPipe_jogador_escrita[i]);
		CloseHandle(hPipe_jogador_leitura[i]);
		CloseHandle(hThread_jogadores[i]);
	}





	return 0;
}




//============================================================================//
//=============== THREADS ====================================================//
//============================================================================//

/// ========= RECEBE JOGADOR ====================================
DWORD WINAPI RecebeJOGADOR(LPVOID param) {
	

	//for(int i = 0; i < contador_jogadores; i++) {
	//	DisconnectNamedPipe(Pipe_jogadores[i]);
	//	_tprintf(TEXT("[SERVIDOR] Vou desligar o pipe  do jogador '%d'... (CloseHandle)\n"));
	//	CloseHandle(Pipe_jogadores[i]);
	//}

	return 0;
}

/// ========= ATENDE JOGADOR ====================================
DWORD WINAPI AtendeJOGADOR(LPVOID param)
{
	TCHAR buf[BUFFER_TAM]; //256
	DWORD ntamBytes, n1;
	//HANDLE pipe = (HANDLE)param;
	int iJogador = (LPVOID)param;

	BOOL jogador_termina, resultado;
	jogador_termina = resultado = FALSE;



	_tprintf(TEXT("debug  '%d : bytes %d' (WriteFile)\n"), iJogador, sizeof(msg_server));
	//BOAS VINDAS AO JOGADOR E PEDIR USERNAME
	_tcscpy_s(msg_server.mensagem, BUFFER_TAM, TEXT("== :: Bem-vindo ao servidor SNAKE SO2 :: ==\nUsername> "));
     msg_server.idcomando = PEDIDO_USERNAME;


	resultado = WriteFile(hPipe_jogador_escrita[iJogador], &msg_server, sizeof(msg_server), &n1, NULL);
	_tprintf(TEXT("[SERVIDOR] Escrever no pipe do jogador  '%d : bytes %d' -> mengs boas vindas (WriteFile)\n"), iJogador, n1);


	if (resultado == FALSE) {
		_tprintf(TEXT("[SERVIDOR] falha no envio   %d\n"), GetLastError);
		CloseHandle(hPipe_jogador_leitura[iJogador]);
	}
	else {
		TERMINAR = TRUE;
	}



	while (TERMINAR)
	{	
		//ler
		_tprintf(TEXT("[SERVIDOR] vou ler do pipe jogador '%d' \n"), iJogador);
		resultado = ReadFile(hPipe_jogador_leitura[iJogador], &msg_do_jogador, sizeof(msg_do_jogador), &ntamBytes, NULL);
		_tprintf(TEXT("[SERVIDOR] li do jogador '%d' : %d bytes  - '%s' command: %d \n"), iJogador, ntamBytes, msg_do_jogador.mensagem, msg_do_jogador.idcomando);

		if (!resultado) {
			_tprintf(TEXT("[SERVIDOR] falha na rececao  : bytes:%d   error: %d\n"), ntamBytes, GetLastError());
			if (GetLastError() == ERROR_BROKEN_PIPE) {
				if (DisconnectNamedPipe(hPipe_jogador_leitura[iJogador])) _tprintf(TEXT("[%d] hPipe_leitura desconectado\n", iJogador));
				if (DisconnectNamedPipe(hPipe_jogador_escrita[iJogador])) _tprintf(TEXT("[%d] hPipe_escrita desconectado\n", iJogador));
				if (CloseHandle(hPipe_jogador_leitura[iJogador])) _tprintf(TEXT("[%d] hPipe_leitura encerrado\n", iJogador));
				if (CloseHandle(hPipe_jogador_escrita[iJogador])) _tprintf(TEXT("[%d] hPipe_escrita encerrado\n", iJogador));
			}
			//TERMINAR = TRUE;
			break;
		}
		//_tprintf(TEXT("[SERVIDOR] ok, jogador '%d' '%s'"), iJogador, msg_do_jogador.jogador.username);

		if (_tcslen(msg_do_jogador.jogador.username) > 0) {
			_tprintf(TEXT("[SERVIDOR] Este é o ' %s ' username do jogador com lenght[%d]  \n"), msg_do_jogador.jogador.username, _tcslen(msg_do_jogador.jogador.username));
			_stprintf_s(msg_server.mensagem, BUFFER_TAM, TEXT("[SERVIDOR] '%s' confirmado!"), msg_do_jogador.jogador.username);
			_tcscpy_s(lista_jogadores_ligados[num_jogadores_registados++].username, NOME_TAM, msg_do_jogador.jogador.username);

		}
		else {
			_stprintf_s(msg_server.mensagem, BUFFER_TAM, TEXT("[SERVIDOR] username nao definido"));
		}

		if (msg_do_jogador.idcomando == PEDIDO_TERMINAR) // terminar
		{
			_stprintf_s(msg_server.mensagem, BUFFER_TAM, TEXT("[SERVIDOR] obrigado '%s' , vai-se desligar"), msg_do_jogador.jogador.username);
			TERMINAR = FALSE;
		}

		//ReleaseMutex(hMutex);
		WriteFile(hPipe_jogador_escrita[iJogador], &msg_server, sizeof(msg_server), &ntamBytes, NULL);
		//_tprintf(TEXT("[SERVIDOR] Escrever no pipe do jogador bytes '%d' : '%s' (WriteFile)\n"), ntamBytes, lista_jogadores_ligados[iJogador].username);

	}
	return 0;
}

/// ========= ENVIA MENSAGEM PARA TODOS =========================
//void DifundeMensagem(TCHAR *buf) {
//
//	DWORD ntam;
//
//	//Escrever para todos os jogadores inscritos
//	for (int i = 0; i < contador_jogadores; i++) {
//		if (!WriteFile(hPipe_jogadores_saida[i], buf, _tcslen(buf)*sizeof(TCHAR), &ntam, NULL)) {
//			_tperror(TEXT("[SERVIDOR] Escrever no pipe do jogador  '%d : %s' (WriteFile)\n"), i, lista_jogadores_ligados[i].username);
//			exit(-1);
//		}
//	}
//	_tprintf(TEXT("[SERVIDOR] Enviei %d bytes aos %d leitores... (WriteFile)\n"), ntam, contador_jogadores);
//
//}



//============================================================================//
//=============== REGISTRY ===================================================//
//============================================================================//

int GuardaDadosRegistry() {

	HKEY chave;
	DWORD queAconteceu;
	int i = 0;

	//Criar/abrir uma chave em HKEY_LOCAL_MACHINE\Software
	if (RegCreateKeyEx(HKEY_CURRENT_USER, TEXT("[SERVIDOR] Software\\SNAKE_SO2"), 0, NULL, REG_OPTION_VOLATILE, KEY_ALL_ACCESS, NULL, &chave, &queAconteceu) != ERROR_SUCCESS) {
		_tprintf(TEXT("Erro ao criar/abrir chave no Registry\n\n"));
	}
	else {
		if (queAconteceu == REG_CREATED_NEW_KEY)	//Se a chave foi criada, inicializar os valores
		{
			_tprintf(TEXT("[SERVIDOR] Chave: HKEY_CURRENT_USER\Software\SNAKE_SO2 criada no Registry\n\n"));
			RegSetValueEx(chave, TEXT("JogadoresRegistados"), 0, REG_BINARY, (LPBYTE) lista_jogadores_ligados, sizeof(JOGADOR));
			_tprintf(TEXT("[SERVIDOR] Jogadores Registados guardados no Registry\n"));
		}
		else if (queAconteceu == REG_OPENED_EXISTING_KEY)		//Se a chave foi aberta, substituir valores lá guardados
		{
			_tprintf(TEXT("[SERVIDOR] Chave: HKEY_CURRENT_USER\Software\SNAKE_SO2 aberta no Registry\n\n"));
			RegSetValueEx(chave, TEXT("JogadoresRegistados"), 0, REG_BINARY, (LPBYTE) lista_jogadores_ligados, sizeof(JOGADOR));
			//_tprintf(TEXT("[SERVIDOR] Jogadores Registados guardados no Registry\n\n"));
		}
	}
	RegCloseKey(chave);

	return TRUE;
}

int Carrega_Lista_Jogadores_Registry() {

	HKEY chave;
	DWORD queAconteceu;
	DWORD tamanho;
	int i = 0;
	//Criar/abrir uma chave em HKEY_CURRENT_USER\Software\Bomberman
	if (RegCreateKeyEx(HKEY_CURRENT_USER, TEXT("Software\\SNAKE_SO2"), 0, NULL, REG_OPTION_VOLATILE,
		KEY_ALL_ACCESS, NULL, &chave, &queAconteceu) != ERROR_SUCCESS) {
		_tprintf(TEXT("Erro ao criar/abrir chave no Registry\n\n"));
		return 1;
	}
	else {
		if (queAconteceu == REG_CREATED_NEW_KEY)	//Se a chave foi criada, inicializar os valores
		{
			_tprintf(TEXT("Não existe chave no Registry\n\n"));
			return 1;
		}
		else if (queAconteceu == REG_OPENED_EXISTING_KEY)	//Se a chave foi aberta, ler os valores lá guardados
		{
			_tprintf(TEXT("Chave: HKEY_CURRENT_USER\Software\Snake SNAKE_SO2 no Registry\n\n"));
			tamanho = sizeof(lista_jogadores_ligados);
			RegQueryValueEx(chave, TEXT("JogadoresRegistados"), NULL, NULL, (LPBYTE)lista_jogadores_ligados, &tamanho);
			_tprintf(TEXT("Jogadores Registados carregados do Registry\n\n"));
		}
	}
	RegCloseKey(chave);
	return TRUE;
}


void DEBUG_MOSTRA_LISTA() {
	for (int i = 0; i < num_jogadores_registados; i++) {
		_tprintf(TEXT("%d . jogador[' %s '] pontuacao [' %d ']\n"), i, lista_jogadores_ligados[i].username, lista_jogadores_ligados[i].pontuacao);
	}
}