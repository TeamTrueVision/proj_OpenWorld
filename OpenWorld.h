/* OpenWorld.h
 * 설명: 소스 전체에서 사용되는 함수 및 변수 선언
 * 코더: 고성훈
 */

#pragma once

#include <Windows.h>

#define DEBUG 1
#define COMPILED __DATE__ //엔진 컴파일 시간 출력용 - __DATE__ 매크로는 컴파일러에서 자체 지원함.

extern int g_ScreenWidth;
extern int g_ScreenHeight;
extern int Quit;

extern HINSTANCE hInst;
extern int isWindowMode;

extern void MessageProcess();