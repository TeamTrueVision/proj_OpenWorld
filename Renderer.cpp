/* Renderer.cpp
 * 코더: 고성훈
 */

#include <Windows.h>
#include <cstdio>

#include "Renderer.h"
#include "ApplicationLogger.h"
#include "GameEngine.h"
#include "OpenWorld.h"

#define DEBUG_TEXT_FONTFACE "Lucida Console"
#define DEBUG_TEXT_WIDTH 14

HWND hWndMain; //윈도우 핸들

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
		ErrorMessageBox("OpenWorld 를 실행하기에 해상도가 너무 작습니다. (최소 1024x768)");
		Quit = 1;
	}

	HRESULT h; //오류 등을 체크하기 위한 변수
	DEVMODE lpDevMode; //디스플레이 모드 (크기)
	int ModeNum = 0; //모드
	BOOL RetVal;
	bool FoundSuitable = false;

	int BestWidth = -1; //최적의 너비
	int BestHeight = -1; //최적의 높이

	do { //화면 크기 계산 루프
		RetVal = EnumDisplaySettings(NULL, ModeNum, &lpDevMode);
		ModeNum++;

		//너비 1024 이상, 1920 이하면 최적의 너비 설정(최적범위)
		if(lpDevMode.dmPelsWidth >= 1024 && lpDevMode.dmPelsWidth <= 1920 && lpDevMode.dmBitsPerPel == 32) {
			ApplicationLogger.write("Suitable display : [%d] %dx%d, %d bpp", ModeNum, lpDevMode.dmPelsWidth,lpDevMode.dmPelsHeight,
				lpDevMode.dmBitsPerPel);

			BestWidth = lpDevMode.dmPelsWidth;
			BestHeight = lpDevMode.dmPelsHeight;
			FoundSuitable = false;
		} else //최적범위가 아닌 경우
			ApplicationLogger.write("[%d] - %dx%d, %d bpp", ModeNum, lpDevMode.dmPelsWidth, lpDevMode.dmPelsHeight, lpDevMode.dmBitsPerPel);

		if(lpDevMode.dmPelsWidth == 1920 && lpDevMode.dmPelsHeight == 1080) { //최대 게임 창 크기
			BestWidth = 1920;
			BestHeight = 1080;

			RetVal = false;
		}

		//베스트 리솔루션 못찾았는데 혹시 1024x768 들어있으면 이놈 선택
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
		ErrorMessageBox("DirectX 생성에 실패하였습니다.");

		return -1;
	}

	D3DDISPLAYMODE d3ddm;

	if(FAILED(pDxD3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm))) {
		ErrorMessageBox("DirectX 에서 디스플레이 모드를 구할 수 없습니다.");

		return -1;
	}

	if(!isWindowMode) {
		ScreenWidth = d3ddm.Width;
		ScreenHeight = d3ddm.Height;
	}

	if(!isWindowMode && ScreenWidth < 1024 && ScreenHeight < 768) {
		ErrorMessageBox("OpenWorld 를 실행하기에 해상도가 너무 작습니다. (최소 1024x768)");
		Quit = 1;
	}

	ZeroMemory(&d3dpp, sizeof(d3dpp));

	//아직 완성된 코드 블럭이 아님!!
	if(!isWindowMode) { //윈도우 모드이면 윈도우 모드로 실행
		d3dpp.Windowed = false;
		d3dpp.BackBufferWidth = ScreenWidth;
		d3dpp.BackBufferHeight = ScreenHeight;
	} else { //이쪽 소스코드는 옵션에서 받은 뒤 대입하도록 나중에 바꿀 예정
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
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE; //D3DPRESENT_INTERVAL_DEFAULT -> 기본 화면주사율

	h = pDxD3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWndMain, D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, &pD3dDevice);

	if(FAILED(h)) {
		ErrorMessageBox("DirectX 초기화에 실패하였습니다.");

		return -1;
	}

	RendererSet(); //렌더러 추가 초기화

	return 0;
}

void RendererSet()
{
	if(!pD3dDevice) {
		ErrorMessageBox("치명적인 DirectX 오류로 인해 프로그램을 종료합니다.");
		Quit = 1;

		return;
	}

	HRESULT hr;

	//필요없는 렌더링 기술 제거
	hr = pD3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	if(FAILED(hr))
		ErrorMessageBox("DirectX 렌더러 설정에 실패하였습니다.");
	hr = pD3dDevice->SetRenderState(D3DRS_LIGHTING, false);
	if(FAILED(hr))
		ErrorMessageBox("DirectX 렌더러 설정에 실패하였습니다.");
	hr = pD3dDevice->SetRenderState(D3DRS_ZENABLE, false);
	if(FAILED(hr))
		ErrorMessageBox("DirectX 렌더러 설정에 실패하였습니다.");
	pD3dDevice->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, true);

	hr = pD3dDevice->GetDeviceCaps(&pD3dCaps);
	if(FAILED(hr))
		ErrorMessageBox("DirectX 렌더러 설정에 실패하였습니다.");

	ApplicationLogger.write("Max texture size = %dx%d", pD3dCaps.MaxTextureWidth, pD3dCaps.MaxTextureHeight);

	//폰트 렌더러 클래스의 초기화는 여기서.
#ifdef DEBUG
	pFont_Debug = new openFontRender(DEBUG_TEXT_FONTFACE, DEBUG_TEXT_WIDTH, 0, 0, 0);
#endif

	HRESULT h;

	h = pD3dDevice->CreateVertexBuffer(4 * sizeof(openVertex), D3DUSAGE_WRITEONLY,
		D3DFVF_CUSTOMVERTEX, D3DPOOL_MANAGED, &d3dVertexBuf, NULL);

	if(FAILED(h)) {
		ErrorMessageBox("DirectX 버텍스 버퍼를 생성할 수 없습니다.");
		Quit = 1;
	}

	h = pD3dDevice->SetStreamSource(0, d3dVertexBuf, sizeof(openVertex), 0);

	if(FAILED(h)) {
		ErrorMessageBox("DirectX 스트림 소스 설정이 불가능합니다.");
		Quit = 1;
	}

	pD3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
}

void RendererClose() //프로그램을 종료하기 전 꼭 이 함수를 호출하여 리소스를 해제해야한다.
{
	if(d3dVertexBuf) d3dVertexBuf->Release();
	if(pD3dDevice) pD3dDevice->Release();
	if(pDxD3d) pDxD3d->Release();

	ChangeDisplaySettings(NULL, 0);

	ApplicationLogger.write("Renderer closed.");
}

void RendererBeginFrame() //프레임 시작시 호출되는 함수
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

void RendererEndFrame() //프레임 끝날 때 호출되는 함수
{
	HRESULT h;

	pD3dDevice->EndScene();
	
	h = pD3dDevice->Present(NULL, NULL, NULL, NULL);

	if(h == D3DERR_DEVICELOST) { //만약 디바이스로스트가 발생한 경우
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

			//각종 렌더러 리셋
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
		ErrorMessageBox("예상치 못한 DirectX 디바이스 손실 복구 오류입니다.\n프로그램 손상으로 종료합니다.");
		Quit = 1;

		return false;
	}

	RendererSet();

	DirectX_LostDevice = false;
	ApplicationLogger.write("Device restored.");
	
	return isPerfectLost;
}

void DebugPrint(char *msg) //디버그 메시지 출력
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

	//DT_CALCRECT 를 먼저 수행하고 난 후에 글자 출력이 가능하다. SeaPerl.
	//덕분에 하루를 통째로 날려먹었습니다 ^^
	pFont->DrawText(0, pText, -1, &Rect, DT_CALCRECT | DT_LEFT, FontColor); //| DT_SINGLELINE
	pFont->DrawText(0, pText, -1, &Rect, DT_LEFT, FontColor); //| DT_SINGLELINE
}

void openFontRender::Destroy()
{
	if(pFont) pFont = NULL; //pFont->Release() 시 액세스 위반 문제 발생, 나중에 해결해야함. (Release 와 NULL 둘 다 같은역할)
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
		ErrorMessageBox("DirectX 스프라이트 생성에 오류가 발생하였습니다.\n프로그램을 종료합니다.");
		Quit = 1;

		return ;
	}

	h = D3DXCreateTextureFromFileEx(pD3dDevice, filename, Width, Height, 1, D3DPOOL_DEFAULT,
		D3DFMT_A8B8G8R8, D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_DEFAULT, 0, NULL, NULL, &pD3dTexture);

	if(FAILED(h)) {
		ErrorMessageBox("DirectX 텍스쳐 생성에 오류가 발생하였습니다.\n프로그램을 종료합니다.");
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