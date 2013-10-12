/* Renderer.h
 * ����: ���ӿ������� ���Ǵ� ���� �Լ� �� Ŭ���� ����
 * �ڴ�: ����
 */

#pragma once

#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>

#include "OpenWorld.h"

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)

//���ڰ� x, y �� �޴� �Լ��� ��� �̹����� ȭ�� ����� ��µǵ��� ���ִ� ��ũ��
//ȭ�� �߾Ӱ� ���õǾ� �ְ�, ���ڰ����� x, y �� �޴� ��� �Լ��� ��� ������.
#define DISPLAY_TO_CENTER(width, height) ((ScreenWidth / 2) - (width / 2)), ((ScreenHeight / 2) - (height / 2))
#define DISPLAY_TO_CENTER_WH(width, height) ((ScreenWidth / 2) - (width / 2)), ((ScreenHeight / 2) - (height / 2)), width, height

extern HWND hWndMain; //������ �ڵ�

extern int  RendererInit();
extern void RendererSet(); //������ �߰� �ʱ�ȭ �� ����(����̽� �ν�Ʈ ������)
extern void RendererBeginFrame();
extern void RendererEndFrame();
extern void RendererClose();
extern bool DeviceLost();

extern void Render_DrawLine(int x1, int y1, int x2, int y2, D3DCOLOR Color);
extern void Render_DrawEmptyBox(int x, int y, int Width, int Height, D3DCOLOR Color); //���� �� ����
extern void Render_DrawBox(int x, int y, int Width, int Height, D3DCOLOR Color); //���� �� �� ����

#ifdef DEBUG
extern void Render_DrawEmptyBox_Text(int x, int y, int Width, int Height,
									const char *msg, D3DCOLOR Color); //������

extern void DebugPrint(char *msg);
#endif

//Ŀ���� ���ؽ�
struct openVertex {
	float x, y, z, rhw;
	DWORD dwColor;
	float tu, tv;
};

//��Ʈ ������ - �����ͷ� �����Ͽ� ����Ѵ�.
//����ÿ��� �� Renderer.cpp ������ ����ϰ�, �۷ι��� ������ RendererSet() �� RendererEndFrame() �� �߰��� �������ش�.
class openFontRender {
private:
	D3DXFONT_DESC font_desc;
	LPD3DXFONT pFont;
	RECT Rect;

public:
	openFontRender(LPSTR FontFace, int Height, bool isBold = false,
					bool isItalic = false, bool isUnderlined = false);
	~openFontRender();

	void write(LPCSTR pText, int x, int y, D3DCOLOR FontColor, int isAlignCenter = 0/* �߾����� */);
	RECT getRect() { return Rect; };
	void Destroy(); //��Ʈ �����Լ� - ����! ����ÿ��� ������ ����
};

//�̹��� ������ - �����ͷ� �����Ͽ� ����Ѵ�.
//�ַ� ���ȭ��, �ΰ� ���� ����ϱ� ���� Ŭ����
class openImageRender {
private:
	LPD3DXSPRITE pD3dxSprite;
	LPDIRECT3DTEXTURE9 pD3dTexture;
	D3DXVECTOR2 Sc; //���� �������� ���� - �׸��� Ư�� �κи� ���
	D3DXVECTOR3 Pos; //�׸��� ��ġ ����
	D3DXMATRIX Mat; //ũ�� ����
	float ResizedWidth, ResizedHeight; //FitToScreen ����� ���� ����

public:
	openImageRender(LPCSTR filename, int Width, int Height, bool FitToScreen = false); //Width = �׸��� �ʺ�, Height = �׸��� ����
	~openImageRender();

	void DisplayImage(int x, int y, int Alpha = 255);
	void DetachImage(); //�׸��� ȭ�鿡�� �����ϱ� ���� ����.
	void Release(); //��������Ʈ�� �ؽ��ĸ� ��.��.��.��.
};