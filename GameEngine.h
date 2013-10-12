/* GameEngine.h
 * ����: ���� ���� ȣ��� �Լ� �Ǵ� ���Ǵ� ���� ����
 * �ڴ�: ����
 */

#pragma once

#define COMPILED __DATE__ //���� ������ �ð� ��¿� - __DATE__ ��ũ�δ� �����Ϸ����� ��ü ������.
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