/* Renderer.h
 * 설명: 게임엔진에서 사용되는 각종 함수 및 클래스 선언
 * 코더: 고성훈
 */

#pragma once

#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>

#include "OpenWorld.h"

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)

//인자값 x, y 를 받는 함수인 경우 이미지가 화면 가운데로 출력되도록 해주는 매크로
//화면 중앙과 관련되어 있고, 인자값으로 x, y 를 받는 모든 함수에 사용 가능함.
#define DISPLAY_TO_CENTER(width, height) ((ScreenWidth / 2) - (width / 2)), ((ScreenHeight / 2) - (height / 2))
#define DISPLAY_TO_CENTER_WH(width, height) ((ScreenWidth / 2) - (width / 2)), ((ScreenHeight / 2) - (height / 2)), width, height

extern HWND hWndMain; //윈도우 핸들

extern int  RendererInit();
extern void RendererSet(); //렌더러 추가 초기화 및 리셋(디바이스 로스트 오류시)
extern void RendererBeginFrame();
extern void RendererEndFrame();
extern void RendererClose();
extern bool DeviceLost();

extern void Render_DrawLine(int x1, int y1, int x2, int y2, D3DCOLOR Color);
extern void Render_DrawEmptyBox(int x, int y, int Width, int Height, D3DCOLOR Color); //속이 빈 상자
extern void Render_DrawBox(int x, int y, int Width, int Height, D3DCOLOR Color); //속이 꽉 찬 상자

#ifdef DEBUG
extern void Render_DrawEmptyBox_Text(int x, int y, int Width, int Height,
									const char *msg, D3DCOLOR Color); //디버깅용

extern void DebugPrint(char *msg);
#endif

//커스텀 버텍스
struct openVertex {
	float x, y, z, rhw;
	DWORD dwColor;
	float tu, tv;
};

//폰트 렌더러 - 포인터로 선언하여 사용한다.
//선언시에는 꼭 Renderer.cpp 에서만 사용하고, 글로벌로 선언후 RendererSet() 와 RendererEndFrame() 에 추가로 설정해준다.
class openFontRender {
private:
	D3DXFONT_DESC font_desc;
	LPD3DXFONT pFont;
	RECT Rect;

public:
	openFontRender(LPSTR FontFace, int Height, bool isBold = false,
					bool isItalic = false, bool isUnderlined = false);
	~openFontRender();

	void write(LPCSTR pText, int x, int y, D3DCOLOR FontColor, int isAlignCenter = 0/* 중앙정렬 */);
	RECT getRect() { return Rect; };
	void Destroy(); //폰트 제거함수 - 주의! 보통시에는 사용되지 않음
};

//이미지 렌더러 - 포인터로 선언하여 사용한다.
//주로 배경화면, 로고 등을 출력하기 위한 클래스
class openImageRender {
private:
	LPD3DXSPRITE pD3dxSprite;
	LPDIRECT3DTEXTURE9 pD3dTexture;
	D3DXVECTOR2 Sc; //아직 구현되지 않음 - 그림의 특정 부분만 출력
	D3DXVECTOR3 Pos; //그림의 위치 설정
	D3DXMATRIX Mat; //크기 조정
	float ResizedWidth, ResizedHeight; //FitToScreen 적용시 비율 설정

public:
	openImageRender(LPCSTR filename, int Width, int Height, bool FitToScreen = false); //Width = 그림의 너비, Height = 그림의 높이
	~openImageRender();

	void DisplayImage(int x, int y, int Alpha = 255);
	void DetachImage(); //그림을 화면에서 제거하기 위해 사용됨.
	void Release(); //스프라이트와 텍스쳐를 파.개.한.다.
};