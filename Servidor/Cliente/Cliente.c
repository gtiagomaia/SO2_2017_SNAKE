/* ======================================================== */
/* Cliente.C                                                */
/* Programa base (esqueleto) para aplica��es Windows        */
/* =========================================================*/
// Cria uma janela de nome "Janela Principal" e pinta fundo
// de branco

// Modelo para programas Windows:
//  Composto por 2 fun��es: 
//	   WinMain() = Ponto de entrada dos programas windows
//			1) Define, cria e mostra a janela
//			2) Loop de recep��o de mensagens provenientes do Windows
//     WinProc() = Processamentos da janela (pode ter outro nome)
//			1) � chamada pelo Windows (callback) 
//			2) Executa c�digo em fun��o da mensagem recebida

//	   WinMain()
//	1. Definir caracter�sticas de uma classe de janela
//  2. Registar a classe no Windows NT
//  3. Criar uma janela dessa classe
//  4. Mostrar a janela
//  5. Iniciar a execu��o do loop de mensagens
//    
//     WinProc()
//  1. Switch em fun��o da mensagem recebida do Windows

// ============================================================================
// In�cio do programa
// ============================================================================
// Este header tem de se incluir sempre porque define os prot�tipos das fun��es 
// do Windows API e os tipos usados na programa��o Windows
#include <windows.h>
#include <tchar.h>
#include <conio.h>

// Include dos Headers
#include "Cliente.h"
#include "resource.h"


// Pr�-declara��o da fun��o WndProc (a que executa os procedimentos da janela por
// "callback") 
LRESULT CALLBACK TrataEventos(HWND, UINT, WPARAM, LPARAM);

BOOL GetTecla(HWND hWnd, WPARAM wParam);

BOOL CALLBACK TrataEventoLogin(HWND hWnd, UINT m, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK TrataEventoServidorLocal(HWND hWnd, UINT m, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK TrataEventoServidorRemoto(HWND hWnd, UINT m, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK TrataEventoPontua��o(HWND hWnd, UINT m, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK TrataEventoSair(HWND hWnd, UINT m, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK TrataEventoAjuda(HWND hWnd, UINT m, WPARAM wParam, LPARAM lParam);

// Nome da classe da janela usado mais abaixo na defini��o das  
// propriedades da classe da janela
TCHAR *szProgName = TEXT("Snake");

JOGADOR jogador;
DADOS_JOGO jogo;

BOOL dados_verificados = FALSE;

HINSTANCE hInstGlobal;


// ============================================================================
// FUN��O DE IN�CIO DO PROGRAMA: WinMain()
// ============================================================================
// Em Windows, o programa come�a sempre a sua execu��o na fun��o WinMain()
// que desempenha o papel da fun��o main() do C em modo consola
// WINAPI indica o "tipo da fun��o" (WINAPI para todas as declaradas nos headers
// do Windows e CALLBACK para as fun��es de processamento da janela)
// Par�metros:
//   hInst: Gerado pelo Windows, � o handle (n�mero) da inst�ncia deste programa 
//   hPrevInst: Gerado pelo Windows, � sempre NULL para o NT (era usado no Windows 3.1)
//   lpCmdLine: Gerado pelo Windows, � um ponteiro para uma string terminada por 0
//              destinada a conter par�metros para o programa 
//   nCmdShow:  Par�metro que especifica o modo de exibi��o da janela (usado em  
//				ShowWindow()
//
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow) {

	HWND hWindow;		// hWnd � o handler da janela, gerado mais abaixo 
						// por CreateWindow()
	MSG lpMsg;			// MSG � uma estrutura definida no Windows para as mensagens
	WNDCLASSEX wcApp;	// WNDCLASSEX � uma estrutura cujos membros servem para 
						// definir as caracter�sticas da classe da janela
	HACCEL hAccel;		// Handler da resource accelerators (teclas de atalho)
	
	int resp = 0;
	/*resp = AutenticarServidor();
	if (resp == 1)
	FlagServidor = 0;
	else
	FlagServidor = 1;*/


	// Defini��o das caracter�sticas da janela "wcApp" 
	// (Valores dos elementos da estrutura "wcApp" do tipo WNDCLASSEX)

	wcApp.cbSize = sizeof(WNDCLASSEX);		// Tamanho da estrutura WNDCLASSEX
	wcApp.hInstance = hInst;				// Inst�ncia da janela actualmente exibida 

	hInstGlobal = hInst;					// "hInst" � par�metro de WinMain e vem inicializada da�

	wcApp.lpszClassName = szProgName;		// Nome da janela (neste caso = nome do programa)
	wcApp.lpfnWndProc = TrataEventos;		// Endere�o da fun��o de processamento da janela 
											// ("WndProc" foi declarada no in�cio e encontra-se
											// mais abaixo)

											// Estilo da janela: 
	wcApp.style = CS_NOCLOSE;				// - Remove o 'X' para fechar a aplica��o permitindo
											// controlar a sa�da do jogo/aplica��o pela Barra de Menus
		

	wcApp.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1));		// "hIcon" = handler do �con normal
	wcApp.hIconSm = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1));	// "hIconSm" = handler do �con pequeno

	wcApp.hCursor = LoadCursor(NULL, IDC_ARROW);	// "hCursor" = handler do cursor (rato)
													// "IDC_ARROW" Aspecto "seta" 

	wcApp.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);				// Classe do menu que a janela pode ter

	wcApp.cbClsExtra = 0;						// Livre, para uso particular
	wcApp.cbWndExtra = 0;						// Livre, para uso particular

	wcApp.hbrBackground = GetStockObject(GRAY_BRUSH);			// "hbrBackground" = handler para "brush" de pintura do fundo da
												// janela. Devolvido por "GetStockObject".

	// Registar a classe "wcApp" no Windows
	if (!RegisterClassEx(&wcApp))
		return(0);
	
	// Cria��o da janela
	hWindow = CreateWindow(
		szProgName,				// Nome da janela (programa) definido acima
		TEXT("S(O2)nake"),		// Texto que figura na barra da janela
		WS_OVERLAPPEDWINDOW,	// Estilo da janela (WS_OVERLAPPED= normal)
		CW_USEDEFAULT,			// Posi��o x pixels (default=� direita da �ltima)
		CW_USEDEFAULT,			// Posi��o y pixels (default=abaixo da �ltima)
		800,					// Largura da janela (em pixels)
		600,					// Altura da janela (em pixels)
		(HWND)HWND_DESKTOP,		// handle da janela pai (se se criar uma a partir 
								// de outra) ou HWND_DESKTOP se a janela for
								// a primeira, criada a partir do "desktop"
		(HMENU)NULL,			// handle do menu da janela (se tiver menu)
		(HINSTANCE)hInst,		// handle da inst�ncia do programa atual
								// ("hInst" � declarado num dos par�metros
								// de WinMain(), valor atribu�do pelo Windows)
		0);				// N�o h� par�metros adicionais para a janela

	// Carregar as defini��es dos atalhos de op��es dos Menus
	// LoadAccelerators(inst�ncia_programa, ID_da_resource_atalhos)
	hAccel = LoadAccelerators(hInst, (LPCWSTR)IDR_ACCELERATOR1);


	// Mostrar a janela
	ShowWindow(hWindow, nCmdShow);	// "hWnd"= handler da janela, devolvido 
								// por "CreateWindow"; "nCmdShow"= modo de
								// exibi��o (p.e. normal, modal); � passado
								// como par�metro de WinMain()

	UpdateWindow(hWindow);			// Refrescar a janela (Windows envia � janela
								// uma mensagem para pintar, mostrar dados,
								// (refrescar), etc)

								// ============================================================================
								// 5. Loop de Mensagens
								// ============================================================================
								// O Windows envia mensagens �s janelas (programas). Estas mensagens ficam numa
								// fila de espera at� que GetMessage(...) possa ler "a mensagem seguinte"	

								// Par�metros de "getMessage":
								//  1)	"&lpMsg"=Endere�o de uma estrutura do tipo MSG ("MSG lpMsg" ja foi 
								//		declarada no in�cio de WinMain()):
								/*			HWND hwnd		handler da janela a que se destina a mensagem
								UINT message	Identificador da mensagem
								WPARAM wParam	Par�metro, p.e. c�digo da tecla premida
								LPARAM lParam	Par�metro, p.e. se ALT tamb�m estava premida
								DWORD time		Hora a que a mensagem foi enviada pelo Windows
								POINT pt		Localiza��o do mouse (x, y)
								2)   handle da window para a qual se pretendem receber mensagens
								(=NULL se se pretendem receber as mensagens para todas as janelas
								pertencentes ao thread actual)
								3)	 C�digo limite inferior das mensganes que se pretendem receber
								4)   C�digo limite superior das mensagens que se pretendem receber
								*/

								// NOTA: GetMessage() devolve 0 quando for recebida a mensagem de fecho da janela,
								// 	     terminando ent�o o loop de recep��o de mensagens, e o programa 


	while (GetMessage(&lpMsg, NULL, 0, 0)) {

		if (!TranslateAccelerator(hWindow, hAccel, &lpMsg)) {
			TranslateMessage(&lpMsg);		// Pr�-processamento da mensagem
			DispatchMessage(&lpMsg);		// Enviar a mensagem traduzida de volta
		}
	}
	/*
	//� mais SEGURO o seguinte ciclo de recep��o de mensagens, para saber se houve um erro
	BOOL bRet;
	while( (bRet = GetMessage( &lpMsg, NULL, 0, 0 )) != 0)
	{
	if (bRet == -1)
	{
	// handle the error and possibly exit
	}
	else
	{
	TranslateMessage(&lpMsg);
	DispatchMessage(&lpMsg);
	}
	}*/
	// ============================================================================
	// 6. Fim do programa
	// ============================================================================
	return((int)lpMsg.wParam);		// Retorna-se sempre o par�metro "wParam" da
									// estrutura "lpMsg"
}

LRESULT CALLBACK TrataEventos(HWND hWnd, UINT m, WPARAM wParam, LPARAM lParam) {
	switch (m) {
			case WM_CREATE:

				case WM_COMMAND:
				
					switch (LOWORD(wParam)){

						case ID_LOGIN:
							DialogBox(hInstGlobal, IDD_DIALOG_LOGIN, hWnd, TrataEventoLogin);
							break;

						case ID_SAIR:
							if (MessageBox(hWnd, TEXT("Deseja mesmo sair?"), TEXT("Aviso"), MB_YESNO | MB_ICONQUESTION) == IDYES){
								//DesligarDoServidor();
								PostQuitMessage(0);
							}
							break;
						
						case ID_AJUDA:
							DialogBox(hInstGlobal, IDD_DIALOG_AJUDA, hWnd, TrataEventoAjuda);
							break;
					} // switch WM_COMMAND

				break; // case WM_COMMAND

			break; // case WM_CREATE


	case WM_DESTROY:	// Destruir a janela e terminar o programa 
						// "PostQuitMessage(Exit Status)"		
		PostQuitMessage(0);
		break;
	default:
		// Neste exemplo, para qualquer outra mensagem (p.e. "minimizar","maximizar","restaurar") // n�o � efectuado nenhum processamento, apenas se segue o "default" do Windows			
		return(DefWindowProc(hWnd, m, wParam, lParam));
	}
	return(0);
}

BOOL CALLBACK TrataEventoLogin(HWND hWnd, UINT m, WPARAM wParam, LPARAM lParam) {
	BOOL sucess = TRUE, error = FALSE;
	TCHAR erro[TAM] = TEXT("\0");

	switch (m) {
	case WM_INITDIALOG: 
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {

		case IDOK: // Passa os dados verificados ao servidor
			if (dados_verificados) {								// Pipe aqui!!!
				MessageBox(hWnd, TEXT("Dados submetidos!"), TEXT("Sucesso"), MB_OK);
				EndDialog(hWnd, 0);
				break;
			}
			else
				MessageBox(hWnd, TEXT("Verifica��o necess�ria!"), TEXT("Aviso"), MB_OK);
			return TRUE;

		case ID_CHECK_DADOS: // Verifica se introduziu todos os dados necess�rios
			wcscat_s(erro, TAM, TEXT("Erro no(s) campo(s): \0"));

			if (GetDlgItemText(hWnd, IDC_EDIT_NOME, jogador.username, TAM) == 0) {
			wcscat_s(erro, TAM, TEXT(" \"Nome\"\0"));
			error = TRUE;}

			jogo.max_jogadores = GetDlgItemInt(hWnd, IDC_EDIT_MAXJOG, &sucess , FALSE);
			if (!sucess) {
				wcscat_s(erro, TAM, TEXT(" \"Max Jogador\"\0"));
				error = TRUE;}

			jogo.linhas = GetDlgItemInt(hWnd, IDC_EDIT_LINHAS, &sucess, FALSE);
			if (!sucess) {
				wcscat_s(erro, TAM, TEXT(" \"Linhas\"\0"));
				error = TRUE;
			} else if (jogo.linhas < MIN_LIN || jogo.linhas > MAX_LIN) {
				wcscat_s(erro, TAM, TEXT(" \"Linhas\"\0"));
				error = TRUE;}

			jogo.colunas = GetDlgItemInt(hWnd, IDC_EDIT_COLUNAS, &sucess, FALSE);
			if (!sucess){
				wcscat_s(erro, TAM, TEXT(" \"Colunas\"\0"));
				error = TRUE;
			} else if (jogo.colunas < MIN_COL || jogo.colunas > MAX_COL) {
					wcscat_s(erro, TAM, TEXT(" \"Colunas\"\0"));
					error = TRUE;}

			jogo.N = GetDlgItemInt(hWnd, IDC_EDIT_CADENCIA, &sucess, FALSE);
			if (!sucess) {
				wcscat_s(erro, TAM, TEXT(" \"Cadencia(N)\"\0"));
				error = TRUE;
			}

			jogo.A = GetDlgItemInt(hWnd, IDC_EDIT_SERPENTES_AUTO, &sucess, FALSE);
			if (!sucess) {
				wcscat_s(erro, TAM, TEXT(" \"Autom�ticas\"\0"));
				error = TRUE;
			}

			jogo.T = GetDlgItemInt(hWnd, IDC_EDIT_TAM_SERPENTE, &sucess, FALSE);
			if (!sucess) {
				wcscat_s(erro, TAM, TEXT(" \"Tam Inicial(T)\"\0"));
				error = TRUE;
			}

			jogo.O = GetDlgItemInt(hWnd, IDC_EDIT_QTD_OBJ, &sucess, FALSE);
			if (!sucess) {
				wcscat_s(erro, TAM, TEXT(" \"Quantidade(O)\"\0"));
				error = TRUE;
			}

			jogo.efeito = GetDlgItemInt(hWnd, IDC_EDIT_DURACAO_EFEITO, &sucess, FALSE);
			if (!sucess) {
				wcscat_s(erro, TAM, TEXT(" \"Dura��o Efeito\"\0"));
				error = TRUE;
			}

			jogo.duracao = GetDlgItemInt(hWnd, IDC_EDIT_DURACAO_MAPA, &sucess, FALSE);
			if (!sucess) {
				wcscat_s(erro, TAM, TEXT(" \"Dura��o no Mapa\"\0"));
				error = TRUE;
			}

			jogo.vulgar = GetDlgItemInt(hWnd, IDC_EDIT_FREQ_VULGAR, &sucess, FALSE);
			if (!sucess) {
				wcscat_s(erro, TAM, TEXT(" \"Vulgar\"\0"));
				error = TRUE;
			} else if (jogo.vulgar < 1 || jogo.vulgar > 100) {
				wcscat_s(erro, TAM, TEXT(" \"Vulgar\"\0"));
				error = TRUE;}

			jogo.invulgar = GetDlgItemInt(hWnd, IDC_EDIT_FREQ_INVULGAR, &sucess, FALSE);
			if (!sucess) {
				wcscat_s(erro, TAM, TEXT(" \"Invulgar\"\0"));
				error = TRUE;
			} else if (jogo.invulgar < 1 || jogo.invulgar > 100) {
				wcscat_s(erro, TAM, TEXT(" \"Invulgar\"\0"));
				error = TRUE;}

			jogo.raro = GetDlgItemInt(hWnd, IDC_EDIT_FREQ_RARO, &sucess, FALSE);
			if (!sucess) {
				wcscat_s(erro, TAM, TEXT(" \"Raro\"\0"));
				error = TRUE;
			} else if (jogo.raro < 1 || jogo.raro > 100) {
				wcscat_s(erro, TAM, TEXT(" \"Raro\"\0"));
				error = TRUE;}

			wcscat_s(erro, TAM, TEXT("!\0"));
			if (error)
				MessageBox(hWnd, erro, TEXT("Erro"), MB_OK);
			else {
				dados_verificados = TRUE;
				MessageBox(hWnd, TEXT("Dados corretos!"), TEXT("Sucesso"), MB_OK);
			}
			return TRUE; // ID_CHECK_DADOS

			case IDC_BTN_BAIXO:
				DialogBox(hInstGlobal, IDD_DIALOG_TECLAS, hWnd, GetTecla(hWnd, wParam));

				break;

			case IDCANCEL:
				EndDialog(hWnd, 0);
				break; // CANCEL
			default:
				return(DefWindowProc(hWnd, m, wParam, lParam));
		}
		case WM_CLOSE:
			EndDialog(hWnd, 0);
			break;
	} // switch(m)
	return 0;
}

BOOL GetTecla(HWND hWnd, WPARAM wParam) {
	switch (wParam) {
	
	case WM_KEYDOWN:
		MessageBox(hWnd, wParam, TEXT("Tecla"), MB_OK);
	
	default:
		break;
	}
}

BOOL TrataEventoServidorLocal(HWND hWnd, UINT m, WPARAM wParam, LPARAM lParam)
{
	return 0;
}

BOOL TrataEventoServidorRemoto(HWND hWnd, UINT m, WPARAM wParam, LPARAM lParam)
{
	return 0;
}

BOOL TrataEventoPontua��o(HWND hWnd, UINT m, WPARAM wParam, LPARAM lParam)
{
	return 0;
}

BOOL TrataEventoSair(HWND hWnd, UINT m, WPARAM wParam, LPARAM lParam)
{
	return 0;
}

BOOL TrataEventoAjuda(HWND hWnd, UINT m, WPARAM wParam, LPARAM lParam){
	
	SetWindowText(IDC_TEXTO_AJUDA, TEXT("Em constru��o!!!"));
	
	return 0;
}



