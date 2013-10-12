/* GameEngine.cpp
 * �ڴ�: ����
 */

#include <Windows.h>
#include <cstdlib>
#include <cstdio>

#include "GameEngine.h"
#include "OpenWorld.h"
#include "ApplicationLogger.h"
#include "LoadOption.h"
#include "Renderer.h"
#include "GameWindow.h"
#include "PlayerStatusInfo.h"
#include "Keyboard.h"

#define DEBUG_TEXT				\
	"OpenWorld Debug Mode - Compiled date: %s\n" \
	"OW Version: %s\nDirectX SDK Version: %d\n\n"	\
	"Current FPS: %3.1f fps\n"	\
	"Average FPS: %3.1f fps\n"	\
	"FrameCount: %d\n"			\
	"Mouse Pos: (%d,%d)\n"		\
	"Key pressed: %d\n"		\
	, COMPILED, VERSION, D3D_SDK_VERSION, CurrentFps, AverageFps, FrameCount, MouseX, MouseY, KeyPressed

#define CHATBOX_X 30
#define CHATBOX_Y ScreenHeight - 50
#define CHATBOX_W ScreenWidth - 800
#define CHATBOX_H 15

/////////////////////////////////////////////////////////////

int ScreenWidth = 1024;
int ScreenHeight = 768;

int DirectX_LostDevice = 0;
double FrameTime = 0; //������
float AverageFps = 0; //��� ������
float CurrentFps = 0; //���� ������
bool Game_BugDetected = false;
#ifdef DEBUG
long FrameCount = 0;  //������ī��Ʈ
char frame_c[1024];
char tempch[512];
#endif

openImageRender *Background;
openTextBox *ChattingBox;
openFontRender *Font_Chatting;
openFontRender *Font_ChattingBox;

inline void CheckDirectXLost()
{
	while(DirectX_LostDevice) {
		if(DeviceLost()) { //����̽� ���� ������ ����������
			Background->Release();
			Font_Chatting->Destroy();
			ChattingBox->Destroy();
			Font_ChattingBox->Destroy();

			Background = new openImageRender("splash.png", 1280, 768, true);
			ChattingBox = new openTextBox(10, CHATBOX_H, "ChatEdit");
			Font_Chatting = new openFontRender("���� ���", 17);
			Font_ChattingBox = new openFontRender("���� ���", 17);
		}
	}
}

void InitEngine()
{
	ApplicationLogger.write("Engine initializing...");
	ApplicationLogger.write("Engine compiled at %s\n", COMPILED);

	if(RendererInit() == -1) {
		Quit = 1;

		return ;
	}

	//ShowCursor(false);

	//���� ������ �̴ϼȶ���¡
	Background = new openImageRender("splash.png", 1280, 768, true);
	ChattingBox = new openTextBox(10, CHATBOX_H, "ChatEdit");
	Font_Chatting = new openFontRender("���� ���", 17);
	Font_ChattingBox = new openFontRender("���� ���", 17);

	ApplicationLogger.write("Engine initialized.");
	return ;
}

int LogoIntro()
{
#ifdef DEBUG
	return 1;
#else
	static int alpha = 0;
	static bool IntroFaded = false, IntroEnded = false;

	CheckDirectXLost();

	RendererBeginFrame();

	if(KeyPressed == VK_RETURN || KeyPressed == VK_SPACE) { //���� �Ǵ� �����̽��� ������ ��Ʈ�� ����
		return 1;
	}

	if(!IntroFaded && alpha < 255) { //���İ� ����
		Sleep(10);

		alpha += 3;
	}
	if(alpha == 255) IntroFaded = true;
	if(IntroFaded && alpha > 0) { //���İ� ����
		Sleep(10);

		alpha -= 3;
	}
	if(alpha <= 0) IntroEnded = true;

	Background->DisplayImage(0, 0, alpha);

	RendererEndFrame();

	if(IntroEnded) {
		Background->Release();

		return 1;
	}

	return 0;
#endif
}

void UpdateEngineStatus(long _FrameTime)
{
	//////////////////////////////////////////////////////////////////////////////////////////////

	CheckDirectXLost();

	FrameTime   = _FrameTime / 1000.0F;
	AverageFps  = AverageFps * 0.9F + (1000.0F / (float)_FrameTime) * 0.1F;
	CurrentFps  = 1000.0F / (float)_FrameTime;
	FrameCount++;
	
	if(Player.getHP_Percent() > 100 || Player.getMP_Percent() > 100) {
		ErrorMessageBox("���ӳ����� �� �������� ��ġ�� ����Ǿ����ϴ�.");

		Game_BugDetected = true;
		Quit = true;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////

	RendererBeginFrame();

#ifdef DEBUG
	sprintf_s(frame_c, DEBUG_TEXT);

	if(LMBClicked())
		strcat_s(frame_c, "\nLMB clicked.");
	if(RMBClicked())
		strcat_s(frame_c, "\nRMB clicked.");
	if(LRMBClicked())
		strcat_s(frame_c, "\nLRMB clicked.");

	DebugPrint(frame_c);
#endif

	Render_DrawEmptyBox(CHATBOX_X, CHATBOX_Y - 5, CHATBOX_W, CHATBOX_H + 10, 0xFF000000);
	//Font_Chatting->write(ChattingBox->getDisplayText(), CHATBOX_X+5, CHATBOX_Y+5, 0xFF000000);
	Font_ChattingBox->write(tempch, CHATBOX_X + 5, CHATBOX_Y - 20, 0xFF000000);

	//////////////////////////////////////////////////////////////////////////////////////////////

	/*
	if(isCursorOnRange((ScreenWidth - 100) / 2, (ScreenHeight - 100) / 2, 100, 100)) {
		strcat_s(frame_c, "\n\nCursor in TestBox");
	}
	*/

	if(ChattingBox->ShowText(CHATBOX_X, CHATBOX_Y, CHATBOX_W)) 
		strcpy_s(tempch, ChattingBox->getTypedText());

	/*
	if(isCursorOnRange(CHATBOX_X, CHATBOX_Y, CHATBOX_W, CHATBOX_H)) {
		ChattingBox->setFocus();

		if(KeyPressed == VK_RETURN) {
			strcpy_s(tempch, ChattingBox->getTypedText());
		}
	}
	else ChattingBox->outFocus();
	*/
	//////////////////////////////////////////////////////////////////////////////////////////////

	RendererEndFrame();
}

void CloseEngine()
{
	RendererClose();

	delete Background;
	delete Font_Chatting;
	delete ChattingBox;
	delete Font_ChattingBox;

	ApplicationLogger.write("Engine closed.");
}