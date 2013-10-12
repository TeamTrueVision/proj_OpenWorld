/* GameWindow.h
 * 설명: 게임내에서 사용되는 각종 윈도우 관련 소스
 * 코더: 고성훈
 */

#pragma once

#include <Windows.h>

#include "Renderer.h"

#define OMB_OK 1
#define OMB_OKCANCEL 2
#define OMB_YESNO 3

//마우스 버튼의 클릭 체크용 변수
extern bool isLMB_Clicked;
extern bool isRMB_Clicked;

//마우스 좌표계
extern int MouseX;
extern int MouseY;

//마우스 클릭시 상태를 반환하는 함수
extern inline bool LMBClicked();
extern inline bool RMBClicked();
extern inline bool LRMBClicked();

extern inline bool isCursorOnPos(int x, int y); //마우스가 특정 위치에 존재하면 true 반환
extern inline bool isCursorOnRange(int x, int y, int Width, int Height); //마우스가 특정 범위에 존재하면 true 반환

extern bool openMessageBox(int Width, int Height, LPSTR pText, LPSTR pTitle, int OMB_Type);
extern void DrawStatusBar(); //Panel 그리기 함수
extern void DrawNpcDialog(int NpcNum);

//게임상의 창 베이스를 그리는 클래스
class openWindow
{
private:
	int x, y, Width, Height;

public:
	openWindow(int x, int y, int Width, int Height);
	~openWindow();

	//void Show();
	//void Destroy();
};

//커스텀 텍스트에딧 컨트롤
class openTextBox
{
private:
	HWND TextBox_Edit;
	openFontRender *Font_ChattingOutput;
	int  x, y;
	int  MaxLen;
	int  Width;
	int  Height;
	int  NowPos;
	int  TotalNowTyped;
	int  PosArray[256];
	char text[1024];
	char display_text[1024];
	bool isVisible;
	bool isFocused;

	void setFocus();
	void outFocus();
	char *getDisplayText();

public:
	openTextBox(int _MaxLen, int _Height, char *ctrlname);
	~openTextBox();
	
	bool ShowText(int _x, int _y, int _Width); //엔터키 쳤을시 true 반환, 다른경우는 false 반환
	void setVisible(bool _isVisible);
	char *getTypedText();
	void Destroy();
};

//커스텀 버튼 컨트롤
class openButton
{
private:
	bool isTrue; //버튼이 생성되었는지 확인
	char button_text[50];
	int  x, y, Width, Height;
	bool isEnabled;
	bool isVisible;

public:
	openButton(char *_button_text, int x, int y, int Width, int Height, bool _isEnabled = true);
	~openButton();

	bool isClicked();

	void DisplayButton(bool _isVisible);
	void EnableButton(bool _isEnable);
	void Reset();
	void Destroy();
};