/* Renderer.cpp
 * �ڴ�: ����
 */

#include <Windows.h>
#include <cstdio>

#include "Renderer.h"
#include "ApplicationLogger.h"
#include "GameEngine.h"
#include "OpenWorld.h"

#define DEBUG_TEXT_FONTFACE "Lucida Console"
#define DEBUG_TEXT_WIDTH 14

HWND hWndMain; //������ �ڵ�

LPDIRECT3D9 pDxD3d = NULL;
LPDIRECT3DDEVICE9 pD3dDevice = NULL;
D3DCAPS9 pD3dCaps;
D3DPRESENT_PARAMETERS d3dpp;
LPDIRECT3DVERTEXBUFFER9 d3dVertexBuf;

#ifdef DEBUG
openFontRender *pFont_Debug;
#endif

void DevceLost();

int RendererInit()
{
	if(isWindowMode && ScreenWidth < 1024 && ScreenHeight < 768) {
		ErrorMessageBox("OpenWorld �� �����ϱ⿡ �ػ󵵰� �ʹ� �۽��ϴ�. (�ּ� 1024x768)");
		Quit = 1;
	}

	HRESULT h; //���� ���� üũ�ϱ� ���� ����
	DEVMODE lpDevMode; //���÷��� ��� (ũ��)
	int ModeNum = 0; //���
	BOOL RetVal;
	bool FoundSuitable = false;

	int BestWidth = -1; //������ �ʺ�
	int BestHeight = -1; //������ ����

	do { //ȭ�� ũ�� ��� ����
		RetVal = EnumDisplaySettings(NULL, ModeNum, &lpDevMode);
		ModeNum++;

		//�ʺ� 1024 �̻�, 1920 ���ϸ� ������ �ʺ� ����(��������)
		if(lpDevMode.dmPelsWidth >= 1024 && lpDevMode.dmPelsWidth <= 1920 && lpDevMode.dmBitsPerPel == 32) {
			ApplicationLogger.write("Suitable display : [%d] %dx%d, %d bpp", ModeNum, lpDevMode.dmPelsWidth,lpDevMode.dmPelsHeight,
				lpDevMode.dmBitsPerPel);

			BestWidth = lpDevMode.dmPelsWidth;
			BestHeight = lpDevMode.dmPelsHeight;
			FoundSuitable = false;
		} else //���������� �ƴ� ���
			ApplicationLogger.write("[%d] - %dx%d, %d bpp", ModeNum, lpDevMode.dmPelsWidth, lpDevMode.dmPelsHeight, lpDevMode.dmBitsPerPel);

		if(lpDevMode.dmPelsWidth == 1920 && lpDevMode.dmPelsHeight == 1080) { //�ִ� ���� â ũ��
			BestWidth = 1920;
			BestHeight = 1080;

			RetVal = false;
		}

		//����Ʈ ���ַ�� ��ã�Ҵµ� Ȥ�� 1024x768 ��������� �̳� ����
		if(lpDevMode.dmPelsWidth == 1024 && lpDevMode.dmPelsHeight == 768 && FoundSuitable == false) {
			BestWidth = 1024;
			BestHeight = 768;
		}
	} while(RetVal);

	if(BestWidth != -1)
		ApplicationLogger.write("Found suitable display resolution - %dx%d", BestWidth, BestHeight);
	else
		ApplicationLogger.write("Coudn't find suitable display resolution");

	pDxD3d = Direct3DCreate9(D3D_SDK_VERSION);

	if(!pDxD3d) {
		ErrorMessageBox("DirectX ������ �����Ͽ����ϴ�.");

		return -1;
	}

	D3DDISPLAYMODE d3ddm;

	if(FAILED(pDxD3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm))) {
		ErrorMessageBox("DirectX ���� ���÷��� ��带 ���� �� �����ϴ�.");

		return -1;
	}

	if(!isWindowMode) {
		ScreenWidth = d3ddm.Width;
		ScreenHeight = d3ddm.Height;
	}

	if(!isWindowMode && ScreenWidth < 1024 && ScreenHeight < 768) {
		ErrorMessageBox("OpenWorld �� �����ϱ⿡ �ػ󵵰� �ʹ� �۽��ϴ�. (�ּ� 1024x768)");
		Quit = 1;
	}

	ZeroMemory(&d3dpp, sizeof(d3dpp));

	//���� �ϼ��� �ڵ� ���� �ƴ�!!
	if(!isWindowMode) { //������ ����̸� ������ ���� ����
		d3dpp.Windowed = false;
		d3dpp.BackBufferWidth = ScreenWidth;
		d3dpp.BackBufferHeight = ScreenHeight;
	} else { //���� �ҽ��ڵ�� �ɼǿ��� ���� �� �����ϵ��� ���߿� �ٲ� ����
		d3dpp.Windowed = true;
		d3dpp.BackBufferWidth = 1024;
		d3dpp.BackBufferHeight = 768;
	}

	D3DMULTISAMPLE_TYPE mst = D3DMULTISAMPLE_8_SAMPLES;
	DWORD Quality = 0;

	if(SUCCEEDED(pDxD3d->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, true, mst, &Quality))) {
			if(SUCCEEDED(pDxD3d->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_D24S8, true, mst, &Quality))) {
					d3dpp.MultiSampleType = mst;
					d3dpp.MultiSampleQuality = Quality - 1;
			}
	}

	d3dpp.BackBufferFormat = d3ddm.Format;
    d3dpp.BackBufferCount = 1;
	//d3dpp.MultiSampleType = D3DMULTISAMPLE_8_SAMPLES;
    //d3dpp.MultiSampleQuality = 0;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	//d3dpp.SwapEffect = D3DSWAPEFFECT_FLIP;
    d3dpp.hDeviceWindow = hWndMain;
    d3dpp.EnableAutoDepthStencil = false;
    d3dpp.AutoDepthStencilFormat = D3DFMT_UNKNOWN;
    d3dpp.Flags = 0;
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE; //D3DPRESENT_INTERVAL_DEFAULT -> �⺻ ȭ���ֻ���

	h = pDxD3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWndMain, D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, &pD3dDevice);

	if(FAILED(h)) {
		ErrorMessageBox("DirectX �ʱ�ȭ�� �����Ͽ����ϴ�.");

		return -1;
	}

	RendererSet(); //������ �߰� �ʱ�ȭ

	return 0;
}

void RendererSet()
{
	if(!pD3dDevice) {
		ErrorMessageBox("ġ������ DirectX ������ ���� ���α׷��� �����մϴ�.");
		Quit = 1;

		return;
	}

	HRESULT hr;

	//�ʿ���� ������ ��� ����
	hr = pD3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	if(FAILED(hr))
		ErrorMessageBox("DirectX ������ ������ �����Ͽ����ϴ�.");
	hr = pD3dDevice->SetRenderState(D3DRS_LIGHTING, false);
	if(FAILED(hr))
		ErrorMessageBox("DirectX ������ ������ �����Ͽ����ϴ�.");
	hr = pD3dDevice->SetRenderState(D3DRS_ZENABLE, false);
	if(FAILED(hr))
		ErrorMessageBox("DirectX ������ ������ �����Ͽ����ϴ�.");
	pD3dDevice->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, true);

	hr = pD3dDevice->GetDeviceCaps(&pD3dCaps);
	if(FAILED(hr))
		ErrorMessageBox("DirectX ������ ������ �����Ͽ����ϴ�.");

	ApplicationLogger.write("Max texture size = %dx%d", pD3dCaps.MaxTextureWidth, pD3dCaps.MaxTextureHeight);

	//��Ʈ ������ Ŭ������ �ʱ�ȭ�� ���⼭.
#ifdef DEBUG
	pFont_Debug = new openFontRender(DEBUG_TEXT_FONTFACE, DEBUG_TEXT_WIDTH, 0, 0, 0);
#endif

	HRESULT h;

	h = pD3dDevice->CreateVertexBuffer(4 * sizeof(openVertex), D3DUSAGE_WRITEONLY,
		D3DFVF_CUSTOMVERTEX, D3DPOOL_MANAGED, &d3dVertexBuf, NULL);

	if(FAILED(h)) {
		ErrorMessageBox("DirectX ���ؽ� ���۸� ������ �� �����ϴ�.");
		Quit = 1;
	}

	h = pD3dDevice->SetStreamSource(0, d3dVertexBuf, sizeof(openVertex), 0);

	if(FAILED(h)) {
		ErrorMessageBox("DirectX ��Ʈ�� �ҽ� ������ �Ұ����մϴ�.");
		Quit = 1;
	}

	pD3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
}

void RendererClose() //���α׷��� �����ϱ� �� �� �� �Լ��� ȣ���Ͽ� ���ҽ��� �����ؾ��Ѵ�.
{
	if(d3dVertexBuf) d3dVertexBuf->Release();
	if(pD3dDevice) pD3dDevice->Release();
	if(pDxD3d) pDxD3d->Release();

	ChangeDisplaySettings(NULL, 0);

	ApplicationLogger.write("Renderer closed.");
}

void RendererBeginFrame() //������ ���۽� ȣ��Ǵ� �Լ�
{
	if(!pD3dDevice) {
		ApplicationLogger.write("No device.");
		
		return ;
	}

	HRESULT h;

	h = pD3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(255, 255, 255), 1.0f, 0);
	
	if(h != D3D_OK)
		ApplicationLogger.write("Renderer clear failed.");

	h = pD3dDevice->BeginScene();

	if(h != D3D_OK)
		ApplicationLogger.write("Renderer BeginFrame failed.");
}

void RendererEndFrame() //������ ���� �� ȣ��Ǵ� �Լ�
{
	HRESULT h;

	pD3dDevice->EndScene();
	
	h = pD3dDevice->Present(NULL, NULL, NULL, NULL);

	if(h == D3DERR_DEVICELOST) { //���� ����̽��ν�Ʈ�� �߻��� ���
		ApplicationLogger.write("Device lost!");
		DirectX_LostDevice = true;

		if(d3dVertexBuf)
			d3dVertexBuf->Release();
	}
}

bool DeviceLost()
{
	HRESULT h;
	bool isPerfectLost = false;

	do {
		h = pD3dDevice->TestCooperativeLevel();

		if(h == D3DERR_DEVICENOTRESET) {
			isPerfectLost = true;

			//���� ������ ����
#ifdef DEBUG
			pFont_Debug->Destroy();
#endif

			pD3dDevice->Reset(&d3dpp);
		}

		MessageProcess();
	} while(h != D3D_OK && Quit == 0);

	ApplicationLogger.write("Restoring device and state...");

	h = pD3dDevice->TestCooperativeLevel();

	if(h != D3D_OK) {
		ErrorMessageBox("����ġ ���� DirectX ����̽� �ս� ���� �����Դϴ�.\n���α׷� �ջ����� �����մϴ�.");
		Quit = 1;

		return false;
	}

	RendererSet();

	DirectX_LostDevice = false;
	ApplicationLogger.write("Device restored.");
	
	return isPerfectLost;
}

void DebugPrint(char *msg) //����� �޽��� ���
{
#ifdef DEBUG
	pFont_Debug->write(msg, 2, 2, 0xFF00AA00);
#endif
}

void Render_DrawLine(int x1, int y1, int x2, int y2, D3DCOLOR Color)
{
	openVertex vertices[4];

	vertices[0].x   = (float)x1;
	vertices[0].y   = (float)y1;
	vertices[0].z   = 0.5f;
	vertices[0].rhw = 1.0f;
	vertices[0].dwColor = Color;

	vertices[1].x   = (float)x2;
	vertices[1].y   = (float)y2;
	vertices[1].z   = 0.5f;
	vertices[1].rhw = 1.0f;
	vertices[1].dwColor = vertices[0].dwColor;

	void *pVertices;

	if(FAILED(d3dVertexBuf->Lock(0, sizeof(vertices), (void **)&pVertices, 0))) {
		ApplicationLogger.write("VertexBuffer lock failed.");

        return ;
	}

	memcpy(pVertices, vertices, 2 * sizeof(openVertex));

    d3dVertexBuf->Unlock();

	pD3dDevice->SetTexture(0, NULL);
    pD3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1, &vertices, sizeof(openVertex));
}

void Render_DrawEmptyBox(int x, int y, int Width, int Height, D3DCOLOR Color)
{
	Render_DrawLine(x, y, x + Width, y, Color);
	Render_DrawLine(x + Width, y, x + Width, y + Height, Color);
	Render_DrawLine(x + Width, y + Height, x, y + Height, Color);
	Render_DrawLine(x, y + Height, x, y, Color);
}

#ifdef DEBUG
void Render_DrawEmptyBox_Text(int x, int y, int Width, int Height, const char *msg, D3DCOLOR Color)
{
	pFont_Debug->write(msg, x + 2, y + 2, Color);
	Render_DrawLine(x, y, x + Width, y, Color);
	Render_DrawLine(x + Width, y, x + Width, y + Height, Color);
	Render_DrawLine(x + Width, y + Height, x, y + Height, Color);
	Render_DrawLine(x, y + Height, x, y, Color);
}
#endif

openFontRender::openFontRender(LPSTR FontFace, int Height, bool isBold, bool isItalic, bool isUnderlined)
{
	UINT Weight = FW_NORMAL;

	if(isBold) Weight = FW_BOLD;

	D3DXCreateFont(pD3dDevice, Height, 0, Weight, 1, isItalic, DEFAULT_CHARSET,
		OUT_TT_ONLY_PRECIS, PROOF_QUALITY, FIXED_PITCH | FF_MODERN, FontFace, &pFont);
}

openFontRender::~openFontRender()
{
	if(pFont) pFont = NULL;
}

void openFontRender::write(LPCSTR pText, int x, int y, D3DCOLOR FontColor, int isAlignCenter)
{
	Rect.left = x;
	Rect.top = y;
	Rect.right = 0;
	Rect.bottom = 0;

	//DT_CALCRECT �� ���� �����ϰ� �� �Ŀ� ���� ����� �����ϴ�. SeaPerl.
	//���п� �Ϸ縦 ��°�� �����Ծ����ϴ� ^^
	pFont->DrawText(0, pText, -1, &Rect, DT_CALCRECT | DT_LEFT, FontColor); //| DT_SINGLELINE
	pFont->DrawText(0, pText, -1, &Rect, DT_LEFT, FontColor); //| DT_SINGLELINE
}

void openFontRender::Destroy()
{
	if(pFont) pFont = NULL; //pFont->Release() �� �׼��� ���� ���� �߻�, ���߿� �ذ��ؾ���. (Release �� NULL �� �� ��������)
}

openImageRender::openImageRender(LPCSTR filename, int Width, int Height, bool FitToScreen)
{
	HRESULT h;

	pD3dxSprite = NULL;
	pD3dTexture = NULL;

	if(FitToScreen) {
		ResizedWidth = (float)ScreenWidth / (float)Width;
		ResizedHeight = (float)ScreenHeight / (float)Height;
	} else {
		ResizedWidth = (float)Width / (float)Width;
		ResizedHeight = (float)Height / (float)Height;
	}

	h = D3DXCreateSprite(pD3dDevice, &pD3dxSprite);

	if(FAILED(h)) {
		ErrorMessageBox("DirectX ��������Ʈ ������ ������ �߻��Ͽ����ϴ�.\n���α׷��� �����մϴ�.");
		Quit = 1;

		return ;
	}

	h = D3DXCreateTextureFromFileEx(pD3dDevice, filename, Width, Height, 1, D3DPOOL_DEFAULT,
		D3DFMT_A8B8G8R8, D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_DEFAULT, 0, NULL, NULL, &pD3dTexture);

	if(FAILED(h)) {
		ErrorMessageBox("DirectX �ؽ��� ������ ������ �߻��Ͽ����ϴ�.\n���α׷��� �����մϴ�.");
		Quit = 1;

		return ;
	}
}

openImageRender::~openImageRender()
{
	if(pD3dxSprite) pD3dxSprite = NULL;
	if(pD3dTexture) pD3dTexture = NULL;
}

void openImageRender::DetachImage()
{
	if(pD3dxSprite) pD3dxSprite->Release();
	if(pD3dTexture) pD3dTexture->Release();
}

void openImageRender::DisplayImage(int x, int y, int Alpha)
{
	HRESULT h;

	Pos.x = (float)x;
	Pos.y = (float)y;
	Pos.z = 0;

	D3DXMatrixTransformation2D(&Mat, 0, NULL, &D3DXVECTOR2(ResizedWidth, ResizedHeight), NULL, NULL, NULL); //&Sc
	pD3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(255, 255, 255), 1.0F, 0);

	pD3dxSprite->SetTransform(&Mat);
	h = pD3dxSprite->Begin(D3DXSPRITE_ALPHABLEND);
	pD3dxSprite->Draw(pD3dTexture, NULL, &D3DXVECTOR3(0.f, 0.f, 0.f), &Pos, D3DCOLOR_ARGB(Alpha, 255, 255, 255));

	pD3dxSprite->End();

	if(FAILED(h)) {
		Quit = 1;
	}
}

void openImageRender::Release()
{
	if(pD3dxSprite) pD3dxSprite->Release();
	if(pD3dTexture) pD3dTexture->Release();
}