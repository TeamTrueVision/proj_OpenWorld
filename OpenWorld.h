/* OpenWorld.h
 * ����: �ҽ� ��ü���� ���Ǵ� �Լ� �� ���� ����
 * �ڴ�: ����
 */

#pragma once

#include <Windows.h>

#define DEBUG 1
#define COMPILED __DATE__ //���� ������ �ð� ��¿� - __DATE__ ��ũ�δ� �����Ϸ����� ��ü ������.

extern int g_ScreenWidth;
extern int g_ScreenHeight;
extern int Quit;

extern HINSTANCE hInst;
extern int isWindowMode;

extern void MessageProcess();