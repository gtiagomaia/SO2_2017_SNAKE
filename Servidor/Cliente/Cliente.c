/* ======================================================== */
/* Cliente.C                                                */
/* Programa base (esqueleto) para aplicações Windows        */
/* =========================================================*/
// Cria uma janela de nome "Janela Principal" e pinta fundo
// de branco

// Modelo para programas Windows:
//  Composto por 2 funções: 
//	   WinMain() = Ponto de entrada dos programas windows
//			1) Define, cria e mostra a janela
//			2) Loop de recepção de mensagens provenientes do Windows
//     WinProc() = Processamentos da janela (pode ter outro nome)
//			1) É chamada pelo Windows (callback) 
//			2) Executa código em função da mensagem recebida

//	   WinMain()
//	1. Definir características de uma classe de janela
//  2. Registar a classe no Windows NT
//  3. Criar uma janela dessa classe
//  4. Mostrar a janela
//  5. Iniciar a execução do loop de mensagens
//    
//     WinProc()
//  1. Switch em função da mensagem recebida do Windows

// ============================================================================
// Início do programa
// ============================================================================
// Este header tem de se incluir sempre porque define os protótipos das funções 
// do Windows API e os tipos usados na programação Windows
#include <windows.h>
#include <tchar.h>
#include <conio.h>

// Include dos Headers
#include "Cliente.h"
#include "resource.h"


// Pré-declaração da função WndProc (a que executa os procedimentos da janela por
// "callback") 
LRESULT CALLBACK TrataEventos(HWND, UINT, WPARAM, LPARAM);

BOOL GetTecla(HWND hWnd, WPARAM wParam);

BOOL CALLBACK TrataEventoLogin(HWND hWnd, UINT m, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK TrataEventoServidorLocal(HWND hWnd, UINT m, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK TrataEventoServidorRemoto(HWND hWnd, UINT m, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK TrataEventoPontuação(HWND hWnd, UINT m, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK TrataEventoSair(HWND hWnd, UINT m, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK TrataEventoAjuda(HWND hWnd, UINT m, WPARAM wParam, LPARAM lParam);

// Nome da classe da janela usado mais abaixo na definição das  
// propriedades da classe da janela
TCHAR *szProgName = TEXT("Snake");

JOGADOR jogador;
DADOS_JOGO jogo;

BOOL dados_verificados = FALSE;

HINSTANCE hInstGlobal;


// ============================================================================
// FUNÇÂO DE INÍCIO DO PROGRAMA: WinMain()
// ============================================================================
// Em Windows, o programa começa sempre a sua execução na função WinMain()
// que desempenha o papel da função main() do C em modo consola
// WINAPI indica o "tipo da função" (WINAPI para todas as declaradas nos headers
// do Windows e CALLBACK para as funções de processamento da janela)
// Parâmetros:
//   hInst: Gerado pelo Windows, é o handle (número) da instância deste programa 
//   hPrevInst: Gerado pelo Windows, é sempre NULL para o NT (era usado no Windows 3.1)
//   lpCmdLine: Gerado pelo Windows, é um ponteiro para uma string terminada por 0
//              destinada a conter parâmetros para o programa 
//   nCmdShow:  Parâmetro que especifica o modo de exibição da janela (usado em  
//				ShowWindow()
//
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow) {

	HWND hWindow;		// hWnd é o handler da janela, gerado mais abaixo 
						// por CreateWindow()
	MSG lpMsg;			// MSG é uma estrutura definida no Windows para as mensagens
	WNDCLASSEX wcApp;	// WNDCLASSEX é uma estrutura cujos membros servem para 
						// definir as características da classe da janela
	HACCEL hAccel;		// Handler da resource accelerators (teclas de atalho)
	
	int resp = 0;
	/*resp = AutenticarServidor();
	if (resp == 1)
	FlagServidor = 0;
	else
	FlagServidor = 1;*/


	// Definição das características da janela "wcApp" 
	// (Valores dos elementos da estrutura "wcApp" do tipo WNDCLASSEX)

	wcApp.cbSize = sizeof(WNDCLASSEX);		// Tamanho da estrutura WNDCLASSEX
	wcApp.hInstance = hInst;				// Instância da janela actualmente exibida 

	hInstGlobal = hInst;					// "hInst" é parâmetro de WinMain e vem inicializada daí

	wcApp.lpszClassName = szProgName;		// Nome da janela (neste caso = nome do programa)
	wcApp.lpfnWndProc = TrataEventos;		// Endereço da função de processamento da janela 
											// ("WndProc" foi declarada no início e encontra-se
											// mais abaixo)

											// Estilo da janela: 
	wcApp.style = CS_NOCLOSE;				// - Remove o 'X' para fechar a aplicação permitindo
											// controlar a saída do jogo/aplicação pela Barra de Menus
		

	wcApp.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1));		// "hIcon" = handler do ícon normal
	wcApp.hIconSm = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1));	// "hIconSm" = handler do ícon pequeno

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
	
	// Criação da janela
	hWindow = CreateWindow(
		szProgName,				// Nome da janela (programa) definido acima
		TEXT("S(O2)nake"),		// Texto que figura na barra da janela
		WS_OVERLAPPEDWINDOW,	// Estilo da janela (WS_OVERLAPPED= normal)
		CW_USEDEFAULT,			// Posição x pixels (default=à direita da última)
		CW_USEDEFAULT,			// Posição y pixels (default=abaixo da última)
		800,					// Largura da janela (em pixels)
		600,					// Altura da janela (em pixels)
		(HWND)HWND_DESKTOP,		// handle da janela pai (se se criar uma a partir 
								// de outra) ou HWND_DESKTOP se a janela for
								// a primeira, criada a partir do "desktop"
		(HMENU)NULL,			// handle do menu da janela (se tiver menu)
		(HINSTANCE)hInst,		// handle da instância do programa atual
								// ("hInst" é declarado num dos parâmetros
								// de WinMain(), valor atribuído pelo Windows)
		0);				// Não há parâmetros adicionais para a janela

	// Carregar as definições dos atalhos de opções dos Menus
	// LoadAccelerators(instância_programa, ID_da_resource_atalhos)
	hAccel = LoadAccelerators(hInst, (LPCWSTR)IDR_ACCELERATOR1);


	// Mostrar a janela
	ShowWindow(hWindow, nCmdShow);	// "hWnd"= handler da janela, devolvido 
								// por "CreateWindow"; "nCmdShow"= modo de
								// exibição (p.e. normal, modal); é passado
								// como parâmetro de WinMain()

	UpdateWindow(hWindow);			// Refrescar a janela (Windows envia à janela
								// uma mensagem para pintar, mostrar dados,
								// (refrescar), etc)

								// ============================================================================
								// 5. Loop de Mensagens
								// ============================================================================
								// O Windows envia mensagens às janelas (programas). Estas mensagens ficam numa
								// fila de espera até que GetMessage(...) possa ler "a mensagem seguinte"	

								// Parâmetros de "getMessage":
								//  1)	"&lpMsg"=Endereço de uma estrutura do tipo MSG ("MSG lpMsg" ja foi 
								//		declarada no início de WinMain()):
								/*			HWND hwnd		handler da janela a que se destina a mensagem
								UINT message	Identificador da mensagem
								WPARAM wParam	Parâmetro, p.e. código da tecla premida
								LPARAM lParam	Parâmetro, p.e. se ALT também estava premida
								DWORD time		Hora a que a mensagem foi enviada pelo Windows
								POINT pt		Localização do mouse (x, y)
								2)   handle da window para a qual se pretendem receber mensagens
								(=NULL se se pretendem receber as mensagens para todas as janelas
								pertencentes ao thread actual)
								3)	 Código limite inferior das mensganes que se pretendem receber
								4)   Código limite superior das mensagens que se pretendem receber
								*/

								// NOTA: GetMessage() devolve 0 quando for recebida a mensagem de fecho da janela,
								// 	     terminando então o loop de recepção de mensagens, e o programa 


	while (GetMessage(&lpMsg, NULL, 0, 0)) {

		if (!TranslateAccelerator(hWindow, hAccel, &lpMsg)) {
			TranslateMessage(&lpMsg);		// Pré-processamento da mensagem
			DispatchMessage(&lpMsg);		// Enviar a mensagem traduzida de volta
		}
	}
	/*
	//É mais SEGURO o seguinte ciclo de recepção de mensagens, para saber se houve um erro
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
	return((int)lpMsg.wParam);		// Retorna-se sempre o parâmetro "wParam" da
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
		// Neste exemplo, para qualquer outra mensagem (p.e. "minimizar","maximizar","restaurar") // não é efectuado nenhum processamento, apenas se segue o "default" do Windows			
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
				MessageBox(hWnd, TEXT("Verificação necessária!"), TEXT("Aviso"), MB_OK);
			return TRUE;

		case ID_CHECK_DADOS: // Verifica se introduziu todos os dados necessários
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
				wcscat_s(erro, TAM, TEXT(" \"Automáticas\"\0"));
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
				wcscat_s(erro, TAM, TEXT(" \"Duração Efeito\"\0"));
				error = TRUE;
			}

			jogo.duracao = GetDlgItemInt(hWnd, IDC_EDIT_DURACAO_MAPA, &sucess, FALSE);
			if (!sucess) {
				wcscat_s(erro, TAM, TEXT(" \"Duração no Mapa\"\0"));
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

BOOL TrataEventoPontuação(HWND hWnd, UINT m, WPARAM wParam, LPARAM lParam)
{
	return 0;
}

BOOL TrataEventoSair(HWND hWnd, UINT m, WPARAM wParam, LPARAM lParam)
{
	return 0;
}

BOOL TrataEventoAjuda(HWND hWnd, UINT m, WPARAM wParam, LPARAM lParam){
	
	SetWindowText(IDC_TEXTO_AJUDA, TEXT("Em construção!!!"));
	
	return 0;
}



