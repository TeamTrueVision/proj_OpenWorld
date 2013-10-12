/* Project OpenWorld
 *   - Team TrueVision
 * 
 * ������Ʈ �ڴ�: ����, �۹ο�, ���¿�
 */

/* OpenWorld.cpp
 * �ڴ�: ����
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

LRESULT CALLBACK WndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam); //������ �ݹ� ���ν���
LRESULT CALLBACK EditWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam); //������Ʈ�� �ݹ� ���ν���
bool InitWindowsForm(HINSTANCE hInstance, int nCmdShow, int isWindowMode); //������ �� �̴ϼȶ�����
ATOM WndClassRegister(HINSTANCE hInstance); //������ Ŭ���� ��� �Լ�
void MessageProcess(); //������ �޽��� ó�� �Լ�

extern HWND hWndMain; //Renderer.cpp �� ����
HINSTANCE hInst;
TCHAR szTitle[] = "OpenWorld"; //������ ����(Ÿ��Ʋ)
TCHAR szWindowClass[] = "OpenWorld"; //������ Ŭ������(������󿡼� ���п�)
int Quit = 0; //OpenWorld.h - Quit ����� ������ �������� ���п�
int isWindowMode; //ini ���Ͽ��� ������ ���� �����ϵ��� ��������� üũ��
WNDPROC EditOrigWndProc; //������Ʈ�� ���� ���ν���
WNDCLASS EditWndClass; //������Ʈ�� Ŭ����

int APIENTRY WinMain(HINSTANCE hInstance,
					 HINSTANCE hPrevinstance,
					 LPSTR lpszCmdParam,
					 int nCmdShow)
{
	if(FindWindow(szWindowClass, szTitle) != NULL) { //�̹� �������̸�
		MessageBox(NULL, _TEXT("�̹� OpenWorld�� �������Դϴ�."), szTitle, MB_OK);

		return 0;
	}

	ApplicationLogger.write("OpenWorld started.");
	ApplicationLogger.write("Application version: %s\n", VERSION);
	
	//�ؽ�Ʈ ���� ��Ʈ�� proc ����
	GetClassInfo(NULL, "edit", &EditWndClass);

	EditOrigWndProc = (WNDPROC)EditWndClass.lpfnWndProc;
	EditWndClass.lpfnWndProc = EditWndProc;
	EditWndClass.hInstance = hInst;
	EditWndClass.lpszClassName = "Edit";

	//������ Ŭ���� ���
	WndClassRegister(hInstance);
	RegisterClass((LPWNDCLASS)&EditWndClass);

	ApplicationLogger.write("Configure display option...");

	Option.OptionFile(".\\configure.ini");

	if(Option.getOptionByInt("OpenWorld", "window_mode") == -1) { //ini ������ �����Ǿ��ų� �ʵ� �Ǵ� �ɼ� ������ ������ �ִ°��
		isWindowMode = 0;
		ApplicationLogger.write("Coudn't loaded window mode ini value - default set: 0");
	} else {
		isWindowMode = Option.getOptionByInt("OpenWorld", "window_mode");

		if(isWindowMode < 0 || isWindowMode > 1) { //0 �Ǵ� 1 �̿��� ������ �������ִ� ���
			isWindowMode = 0;
			ApplicationLogger.write("window mode ini value has wrong value. default set: 0");
		}
	}

	ScreenWidth = Option.getOptionByInt("OpenWorld", "width");
	ScreenHeight = Option.getOptionByInt("OpenWorld", "height");

	if(ScreenWidth < 1 || ScreenHeight < 1) {
		ApplicationLogger.write("Display setting has wrong value. - default set: width=1024, height=768");
		MessageBox(NULL, "configure.ini �� width �Ǵ� height ���� �߸� �����Ǿ� �ֽ��ϴ�.\n�⺻���� width=1024, height=768 �� �缳�� �Ǿ� ����˴ϴ�.", "OpenWorld", MB_OK);

		Option.setOptionByInt("OpenWorld", "width", 1024);
		Option.setOptionByInt("OpenWorld", "height", 768);

		ScreenWidth = 1024;
		ScreenHeight = 768;
	}

	if(!InitWindowsForm(hInstance, nCmdShow, isWindowMode)) { //false �� ���Ϲ��� ���
		MessageBox(NULL, "OpenWorld â�� �ʱ�ȭ�� �����Ͽ����ϴ�.", szTitle, MB_OK);

		return 0;
	}

	int Start_Time = 0, End_Time = 0, Frame_Time = 0;

	InitEngine();
	
	//��Ʈ��
	while(Quit != 1) {
		MessageProcess();

		if(LogoIntro()) //�ΰ� ����� ������
			break;
	}

	//���ΰ���
	while(Quit != 1) {
		MessageProcess();

		//�����ڵ��϶��� ������ IME�� ����� ����(�ȱ׷��� �ѱ�Ű �����־ z ���� ��ųŰ �ȳ���)
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
		if(!GetMessage(&Message, NULL, 0, 0)) void(); //�ƹ��͵� ��������.
		else {
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}
	}
}

bool InitWindowsForm(HINSTANCE hInstance, int nCmdShow, int isWindowMode)
{
	HWND hWnd; //â �ڵ鰪

	hInst = hInstance;

	if(!isWindowMode) { //������ ��� üũ
		hWnd = CreateWindowEx(NULL, szWindowClass, szTitle, WS_POPUP,
			0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
			NULL, (HMENU)NULL, hInst, NULL);
	} else {
		MessageBox(NULL, "OpenWorld�� ������ ���� ����˴ϴ�.", szTitle, MB_OK);

		hWnd = CreateWindowEx(NULL, szWindowClass, szTitle,
			WS_TILED | WS_MINIMIZEBOX | WS_SYSMENU, 0, 0,
			ScreenWidth, ScreenHeight, NULL, (HMENU)NULL, hInst, NULL);
	}

	if(!hWnd) //������ ���� hWnd �ڵ��� ����� �������� ���� ��� false ����
		return false;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	hWndMain = hWnd;

	//ShowCursor(false); //Ŀ�� ������ - ���� ���� �̴ϼȶ������� �߰��ʿ� ���� �ּ�ó��.

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
	case WM_MOUSEMOVE: //���콺 ����
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
	case WM_KEYDOWN: //Ű���� ����
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
	case WM_CHAR: //Ű �Է½�
		KeyPressed = wParam;
		break;
	case WM_ACTIVATEAPP: //�ּ�ȭ�Ǿ����� Ȯ�� - �ּ�ȭ �� ����̽� �ν�Ʈ ������ ����� ����. (��üȭ���϶��� �ش�)
		if(!wParam && !isWindowMode) { //��üȭ���̰� �ּ�ȭ �Ǿ�����
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
	case WM_CHAR: //Ű �Է½�
		//������ ����
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