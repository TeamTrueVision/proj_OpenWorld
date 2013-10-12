/* GameEngine.h
 * 설명: 게임 엔진 호출용 함수 또는 사용되는 변수 선언
 * 코더: 고성훈
 */

#pragma once

#define COMPILED __DATE__ //엔진 컴파일 시간 출력용 - __DATE__ 매크로는 컴파일러에서 자체 지원함.
#define VERSION "Non-Stable v0.01 alpha test"

extern int ScreenWidth;
extern int ScreenHeight;
extern int Fade, Faded;
extern int DirectX_LostDevice;
extern bool Game_BugDetected;

extern void InitEngine();
extern void UpdateEngineStatus(long _FrameTime);
extern void CloseEngine();

extern int LogoIntro();