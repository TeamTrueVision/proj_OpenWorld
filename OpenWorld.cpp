/* Project OpenWorld
 *   - Team TrueVision
 * 
 * 프로젝트 코더: 고성훈, 송민욱, 조태연
 */

/* OpenWorld.cpp
 * 코더: 고성훈
 */

#include <Windows.h>
#include <tchar.h>
#include <ctime>
#include <imm.h>

#include "OpenWorld.h"
#include "GameEngine.h"
#include "ApplicationLogger.h"
#include "LoadOption.h"
#include "GameWindow.h"
#include "Keyboard.h"

#pragma comment(lib, "imm32.lib")

LRESULT CALLBACK WndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam); //윈도우 콜백 프로시져
LRESULT CALLBACK EditWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam); //에딧컨트롤 콜백 프로시져
bool InitWindowsForm(HINSTANCE hInstance, int nCmdShow, int isWindowMode); //윈도우 폼 이니셜라이저
ATOM WndClassRegister(HINSTANCE hInstance); //윈도우 클래스 등록 함수
void MessageProcess(); //윈도우 메시지 처리 함수

extern HWND hWndMain; //Renderer.cpp 에 존재
HINSTANCE hInst;
TCHAR szTitle[] = "OpenWorld"; //윈도우 제목(타이틀)
TCHAR szWindowClass[] = "OpenWorld"; //윈도우 클래스명(윈도우상에서 구분용)
int Quit = 0; //OpenWorld.h - Quit 명령이 떨어진 상태인지 구분용
int isWindowMode; //ini 파일에서 윈도우 모드로 실행하도록 설정됬는지 체크용
WNDPROC EditOrigWndProc; //에딧컨트롤 원래 프로시져
WNDCLASS EditWndClass; //에딧컨트롤 클래스

int APIENTRY WinMain(HINSTANCE hInstance,
					 HINSTANCE hPrevinstance,
					 LPSTR lpszCmdParam,
					 int nCmdShow)
{
	if(FindWindow(szWindowClass, szTitle) != NULL) { //이미 실행중이면
		MessageBox(NULL, _TEXT("이미 OpenWorld가 실행중입니다."), szTitle, MB_OK);

		return 0;
	}

	ApplicationLogger.write("OpenWorld started.");
	ApplicationLogger.write("Application version: %s\n", VERSION);
	
	//텍스트 에딧 컨트롤 proc 변경
	GetClassInfo(NULL, "edit", &EditWndClass);

	EditOrigWndProc = (WNDPROC)EditWndClass.lpfnWndProc;
	EditWndClass.lpfnWndProc = EditWndProc;
	EditWndClass.hInstance = hInst;
	EditWndClass.lpszClassName = "Edit";

	//윈도우 클래스 등록
	WndClassRegister(hInstance);
	RegisterClass((LPWNDCLASS)&EditWndClass);

	ApplicationLogger.write("Configure display option...");

	Option.OptionFile(".\\configure.ini");

	if(Option.getOptionByInt("OpenWorld", "window_mode") == -1) { //ini 파일이 누락되었거나 필드 또는 옵션 설정에 오류가 있는경우
		isWindowMode = 0;
		ApplicationLogger.write("Coudn't loaded window mode ini value - default set: 0");
	} else {
		isWindowMode = Option.getOptionByInt("OpenWorld", "window_mode");

		if(isWindowMode < 0 || isWindowMode > 1) { //0 또는 1 이외의 값으로 설정되있는 경우
			isWindowMode = 0;
			ApplicationLogger.write("window mode ini value has wrong value. default set: 0");
		}
	}

	ScreenWidth = Option.getOptionByInt("OpenWorld", "width");
	ScreenHeight = Option.getOptionByInt("OpenWorld", "height");

	if(ScreenWidth < 1 || ScreenHeight < 1) {
		ApplicationLogger.write("Display setting has wrong value. - default set: width=1024, height=768");
		MessageBox(NULL, "configure.ini 내 width 또는 height 값이 잘못 설정되어 있습니다.\n기본값인 width=1024, height=768 로 재설정 되어 실행됩니다.", "OpenWorld", MB_OK);

		Option.setOptionByInt("OpenWorld", "width", 1024);
		Option.setOptionByInt("OpenWorld", "height", 768);

		ScreenWidth = 1024;
		ScreenHeight = 768;
	}

	if(!InitWindowsForm(hInstance, nCmdShow, isWindowMode)) { //false 를 리턴받은 경우
		MessageBox(NULL, "OpenWorld 창의 초기화에 실패하였습니다.", szTitle, MB_OK);

		return 0;
	}

	int Start_Time = 0, End_Time = 0, Frame_Time = 0;

	InitEngine();
	
	//인트로
	while(Quit != 1) {
		MessageProcess();

		if(LogoIntro()) //로고 출력이 끝나면
			break;
	}

	//메인게임
	while(Quit != 1) {
		MessageProcess();

		//메인핸들일때는 무조건 IME를 영어로 설정(안그러면 한글키 눌려있어서 z 같은 스킬키 안나감)
		if(GetFocus() == hWndMain)
			ImmSetConversionStatus(ImmGetContext(hWndMain), IME_CMODE_ALPHANUMERIC, (DWORD)0);

		if(Frame_Time == 0)
			Frame_Time = 1;

		UpdateEngineStatus(Frame_Time);

		End_Time = clock();
		Frame_Time = End_Time - Start_Time;
		Start_Time = End_Time;
	}

	CloseEngine();

#ifdef DEBUG
	ShowWindow(hWndMain, SW_HIDE);
	system("pause");
#endif
	
	return 0;
}

void MessageProcess()
{
	MSG Message;

	if(PeekMessage(&Message, NULL, 0, 0, PM_NOREMOVE)) {
		if(!GetMessage(&Message, NULL, 0, 0)) void(); //아무것도 하지않음.
		else {
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}
	}
}

bool InitWindowsForm(HINSTANCE hInstance, int nCmdShow, int isWindowMode)
{
	HWND hWnd; //창 핸들값

	hInst = hInstance;

	if(!isWindowMode) { //윈도우 모드 체크
		hWnd = CreateWindowEx(NULL, szWindowClass, szTitle, WS_POPUP,
			0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
			NULL, (HMENU)NULL, hInst, NULL);
	} else {
		MessageBox(NULL, "OpenWorld가 윈도우 모드로 실행됩니다.", szTitle, MB_OK);

		hWnd = CreateWindowEx(NULL, szWindowClass, szTitle,
			WS_TILED | WS_MINIMIZEBOX | WS_SYSMENU, 0, 0,
			ScreenWidth, ScreenHeight, NULL, (HMENU)NULL, hInst, NULL);
	}

	if(!hWnd) //오류로 인해 hWnd 핸들이 제대로 설정되지 않은 경우 false 리턴
		return false;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	hWndMain = hWnd;

	//ShowCursor(false); //커서 가리기 - 게임 엔진 이니셜라이저에 추가됨에 따라 주석처리.

	return true;
}

ATOM WndClassRegister(HINSTANCE hInstance)
{
	WNDCLASSEX WndClass;

	WndClass.cbSize = sizeof(WNDCLASSEX);
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hIconSm = LoadIcon(NULL, (LPCTSTR)IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = (WNDPROC)WndProc;
	WndClass.lpszClassName = szWindowClass;
	WndClass.lpszMenuName = NULL;
	WndClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	
	return RegisterClassEx(&WndClass);
}

LRESULT CALLBACK WndProc(HWND hWnd,
						 UINT Message,
						 WPARAM wParam,
						 LPARAM lParam)
{
	switch(Message) {
	case WM_CLOSE:
	case WM_QUIT:
	case WM_DESTROY:
		Quit = 1;
		break;
	case WM_LBUTTONUP:
	case WM_LBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_MOUSEMOVE: //마우스 관련
		MouseX = LOWORD(lParam);
		MouseY = HIWORD(lParam);

		if(Message == WM_LBUTTONDOWN)
			isLMB_Clicked = true;
		if(Message == WM_LBUTTONUP)
			isLMB_Clicked = false;

		if(Message == WM_RBUTTONDOWN)
			isRMB_Clicked = true;
		if(Message == WM_RBUTTONUP)
			isRMB_Clicked = false;

		break;
	case WM_MBUTTONUP:
		Quit = 1;
		break;
	case WM_MBUTTONDOWN:
		break;
	case WM_KEYDOWN: //키보드 관련
		if(wParam == VK_LSHIFT)
			isLShiftKeyPressed = true;
		else if(wParam == VK_RSHIFT)
			isRShiftKeyPressed = true;

		break;
	case WM_KEYUP:
		KeyPressed = NULL;

		if(wParam == VK_LSHIFT)
			isLShiftKeyPressed = false;
		else if(wParam == VK_RSHIFT)
			isRShiftKeyPressed = false;

		break;
	case WM_CHAR: //키 입력시
		KeyPressed = wParam;
		break;
	case WM_ACTIVATEAPP: //최소화되었는지 확인 - 최소화 시 디바이스 로스트 문제가 생기기 때문. (전체화면일때만 해당)
		if(!wParam && !isWindowMode) { //전체화면이고 최소화 되었으면
			DirectX_LostDevice = true;
			ApplicationLogger.write("App minimized - device lost!");
		}

		break;
	default:
		return DefWindowProc(hWnd, Message, wParam, lParam);
	}

	return 0;
}

LRESULT CALLBACK EditWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	int CurW, CurL;

	SendMessage(hWnd, EM_GETSEL, (WPARAM)&CurW, (LPARAM)&CurL);

	switch(Message) {
	case WM_KEYDOWN:
		//if(GetKeyState(VK_CONTROL) & 0x8000 && wParam == 'a')
		/*
		if(wParam == (VK_CONTROL | 'a'))
			SendMessage(hWnd, EM_SETSEL, 0, -1);

		if(GetKeyState(VK_SHIFT) & 0x8000 && (GetKeyState(VK_LEFT) & 0x8000 || GetKeyState(VK_UP) & 0x8000))
			SendMessage(hWnd, EM_SETSEL, CurL, CurW);

		if(GetKeyState(VK_SHIFT) & 0x8000 && GetKeyState(VK_RIGHT) & 0x8000)
			SendMessage(hWnd, EM_SETSEL, CurW, CurL-1);
		//*/

		if(wParam == VK_LSHIFT)
			isLShiftKeyPressed = true;
		if(wParam == VK_RSHIFT)
			isRShiftKeyPressed = true;

		break;
	case WM_KEYUP:
		KeyPressed = NULL;

		if(wParam == VK_LSHIFT)
			isLShiftKeyPressed = false;
		if(wParam == VK_RSHIFT)
			isRShiftKeyPressed = false;

		break;
	case WM_CHAR: //키 입력시
		//비프음 방지
		if(wParam == VK_RETURN) {
			KeyPressed = wParam;
			return 0;
		}
		if(wParam == VK_ESCAPE) {
			KeyPressed = wParam;
			return 0;
		}

		KeyPressed = wParam;
		break;
	}

	return CallWindowProc(EditOrigWndProc, hWnd, Message, wParam, lParam);
}