/* GameWindow.h
 * ����: ���ӳ����� ���Ǵ� ���� ������ ���� �ҽ�
 * �ڴ�: ����
 */

#pragma once

#include <Windows.h>

#include "Renderer.h"

#define OMB_OK 1
#define OMB_OKCANCEL 2
#define OMB_YESNO 3

//���콺 ��ư�� Ŭ�� üũ�� ����
extern bool isLMB_Clicked;
extern bool isRMB_Clicked;

//���콺 ��ǥ��
extern int MouseX;
extern int MouseY;

//���콺 Ŭ���� ���¸� ��ȯ�ϴ� �Լ�
extern inline bool LMBClicked();
extern inline bool RMBClicked();
extern inline bool LRMBClicked();

extern inline bool isCursorOnPos(int x, int y); //���콺�� Ư�� ��ġ�� �����ϸ� true ��ȯ
extern inline bool isCursorOnRange(int x, int y, int Width, int Height); //���콺�� Ư�� ������ �����ϸ� true ��ȯ

extern bool openMessageBox(int Width, int Height, LPSTR pText, LPSTR pTitle, int OMB_Type);
extern void DrawStatusBar(); //Panel �׸��� �Լ�
extern void DrawNpcDialog(int NpcNum);

//���ӻ��� â ���̽��� �׸��� Ŭ����
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

//Ŀ���� �ؽ�Ʈ���� ��Ʈ��
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
	
	bool ShowText(int _x, int _y, int _Width); //����Ű ������ true ��ȯ, �ٸ����� false ��ȯ
	void setVisible(bool _isVisible);
	char *getTypedText();
	void Destroy();
};

//Ŀ���� ��ư ��Ʈ��
class openButton
{
private:
	bool isTrue; //��ư�� �����Ǿ����� Ȯ��
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