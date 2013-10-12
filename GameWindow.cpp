/* GameWindow.cpp
 * 코더: 고성훈
 */

#include <string>

#include "GameWindow.h"
#include "Renderer.h"
#include "OpenWorld.h"
#include "GameEngine.h"
#include "Keyboard.h"

#pragma comment(lib, "winmm.lib")

#define BLINKING_FREQ 30
#define CARET "|"
#define CARET_BLANK " "

bool isLMB_Clicked = false, isRMB_Clicked = false;
int MouseX;
int MouseY;
openImageRender *NpcBox;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline bool LMBClicked()
{
	if(isLMB_Clicked && !isRMB_Clicked) {
		return true;
	} else return false;
}

inline bool RMBClicked()
{
	if(isRMB_Clicked && !isLMB_Clicked) {
		return true;
	} else return false;
}

inline bool LRMBClicked()
{
	if(isLMB_Clicked && isRMB_Clicked) {
		return true;
	} else return false;
}

inline bool isCursorOnPos(int x, int y)
{
	POINT Cur;

	GetCursorPos(&Cur);

	if(Cur.x == x && Cur.y == y)
		return true;

	return false;
}

inline bool isCursorOnRange(int x, int y, int Width, int Height)
{
	//POINT Cur;

	//GetCursorPos(&Cur);
	
#ifdef DEBUG
	//Render_DrawEmptyBox_Text(x, y, Width, Height, "CurRange", 0x99AAAAAA);
#endif

	if(MouseX >= x && MouseX <= x + Width && MouseY >= y && MouseY <= y + Height)
		return true;
	else return false;
}

bool openMessageBox(int Width, int Height, LPSTR pText, LPSTR pTitle, int OMB_Type)
{
	openWindow *pWindow = new openWindow(ScreenWidth / 2 - Width / 2, ScreenHeight / 2 - Height / 2, Width, Height);
	

	//pWindow->Show();
	return false;
}

void DrawNpcDialog(int NpcNum)
{
	NpcBox = new openImageRender("test.png", 485, 215);

	NpcBox->DisplayImage(DISPLAY_TO_CENTER(485, 215));

	if(isCursorOnRange(DISPLAY_TO_CENTER_WH(485, 215)) && LMBClicked()) {
		NpcBox->DetachImage();

		return ;
	}

	NpcBox->Release();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
openTextBox::openTextBox(int _MaxLen, int _Height, char *ctrlname)
{
	TextBox_Edit = CreateWindow("Edit", NULL, WS_CHILD | WS_BORDER | WS_CLIPCHILDREN | ES_AUTOHSCROLL,
		0, 0, 10, 10, hWndMain, (HMENU)1, hInst, NULL);

	isFocused = false;
	Height = _Height;
	Font_ChattingOutput = new openFontRender("바탕체", Height);
	NowPos = 0;

	PostMessage(TextBox_Edit, EM_LIMITTEXT, (WPARAM)256, 0); //글자수 제한
}

openTextBox::~openTextBox()
{
	DestroyWindow(TextBox_Edit);

	if(Font_ChattingOutput)
		delete Font_ChattingOutput;
}

bool openTextBox::ShowText(int _x, int _y, int Width)
{
	unsigned int StartSel, EndSel;

	x = _x;
	y = _y;

	Font_ChattingOutput->write(getTypedText(), x, y, 0xFF000000);

	if(isCursorOnRange(x, y, Width, Height)) {
		setFocus();

		SendMessageA(TextBox_Edit, EM_GETSEL, (WPARAM)&StartSel, (LPARAM)&EndSel); //캐럿위치 얻기

		TotalNowTyped = strlen(getTypedText());
		NowPos = StartSel;
		PosArray[strlen(getTypedText())] = Font_ChattingOutput->getRect().right;

		if(timeGetTime() % 1000 >= 500) {
			if(StartSel == EndSel) { //x + 6 * StartSel      //Font_ChattingOutput->getRect().right + 2
				Render_DrawLine(PosArray[NowPos], y - 3, PosArray[NowPos], y + Height + 3, 0xFF00FFFF);
			} else {

			}
		} else void();

		if(KeyPressed == VK_RETURN)
			return true;
	} else outFocus();

	return false;
}

char *openTextBox::getTypedText()
{
	GetWindowText(TextBox_Edit, text, 1024);

	return text;
}

char *openTextBox::getDisplayText()
{
	if(!isVisible)
		return NULL;

	if(!isFocused) {
		strcpy_s(display_text, getTypedText());
		
		return display_text;
	}

	unsigned int StartSel, EndSel;
	char Front[256], Back[256], Middle[256];

	SendMessageA(TextBox_Edit, EM_GETSEL, (WPARAM)&StartSel, (LPARAM)&EndSel); //캐럿위치 얻기

	/*
	if(StartSel == EndSel) {
		memcpy(Front, &getTypedText()[0], StartSel);
		Front[StartSel] = '\0';

		memcpy(Back, &getTypedText()[StartSel], strlen(getTypedText()) - strlen(Front));
		Back[strlen(getTypedText()) - strlen(Front)] = '\0';
	} else {
		sprintf(display_text, "%s[%s]%s", Front, Middle, Back);

		return display_text;
	}

	if(timeGetTime() % 1000 >= 500) {
		sprintf_s(display_text, "%s%s%s", Front, CARET, Back);
	} else {
		sprintf_s(display_text, "%s%s%s", Front, CARET_BLANK, Back);
	}*/

	if(timeGetTime() % 1000 >= 500) {

	} else void();

	/*
	if(Blinking_freq <= BLINKING_FREQ) {
		sprintf_s(display_text, "%s%s%s", Front, CARET, Back);
		Blinking_freq++;
	} else if(Blinking_freq > BLINKING_FREQ && Blinking_freq <= BLINKING_FREQ * 2) {
		sprintf_s(display_text, "%s%s%s", Front, CARET_BLANK, Back);
		Blinking_freq++;
	}  else Blinking_freq = 0;
	//*/

	return display_text;
}

void openTextBox::setVisible(bool _isVisible)
{
	isVisible = _isVisible;
}

void openTextBox::setFocus()
{
	if(!isVisible)
		return ;

	SetFocus(TextBox_Edit);
	//hImc = ImmGetContext(TextBox_Edit);
	//ImmReleaseContext(TextBox_Edit, hImc);
	isFocused = true;
}

void openTextBox::outFocus()
{
	SetFocus(hWndMain);
	isFocused = false;
}

void openTextBox::Destroy()
{
	DestroyWindow(TextBox_Edit);
	isVisible = false;

	if(Font_ChattingOutput)
		delete Font_ChattingOutput;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
openButton::openButton(char *_button_text, int _x, int _y, int _Width, int _Height, bool _isEnabled)
{
	strcpy_s(button_text, _button_text);
	
	x = _x;
	y = _y;
	Width = _Width;
	Height = _Height;
	isEnabled = _isEnabled;
	isTrue = false;
}

void openButton::DisplayButton(bool _isVisible)
{
	if(isTrue == false)
		return ;

	char btn_txt[256];

	isVisible = _isVisible;
	sprintf_s(btn_txt, "Btn: %s", button_text);

	if(isVisible) { //버튼 보임
		if(isEnabled) { //활성화 된 버튼
			#ifdef DEBUG
				Render_DrawEmptyBox_Text(x, y, Width, Height, btn_txt, D3DCOLOR_XRGB(0, 255, 0));
			#else
			#endif
		} else { //비활성화
			#ifdef DEBUG
				Render_DrawEmptyBox_Text(x, y, Width, Height, btn_txt, D3DCOLOR_XRGB(255, 0, 0));
			#else
			#endif
		}
	} else { //버튼 안보임
		#ifdef DEBUG
			//Render_DrawEmptyBox_Text(x, y, Width, Height, btn_txt, D3DCOLOR_XRGB(0, 0, 255));
		#else
		#endif
	}
}

bool openButton::isClicked()
{
	if(isTrue == false || isEnabled == false)
		return false;

	if(isCursorOnRange(x, y, Width, Height) && LMBClicked())
		return true;
	else return false;
}

void openButton::EnableButton(bool _isEnable)
{
	if(isTrue == false)
		return ;

	isEnabled = _isEnable;
}

void openButton::Reset()
{
	isTrue = true;
}

void openButton::Destroy()
{
	isTrue = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
openWindow::openWindow(int x, int y, int Width, int Height)
{
}