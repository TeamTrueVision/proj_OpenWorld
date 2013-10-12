/* ApplicationLogger.cpp
 * �ڴ�: ����
 */

//#define _CRT_SECURE_NO_WARNINGS 1 //�����Ͻ� ���� ���� ���� �ȶ���

#include <cstdarg>
#include <cstdio>
#include <ctime>
#include <Windows.h>
#include <tchar.h>
#include <iostream>

#include "ApplicationLogger.h"
#include "OpenWorld.h"

#define LOGGING_FILENAME ".\\Application.log"

#ifdef DEBUG
#pragma comment(linker,"/entry:WinMainCRTStartup /subsystem:console") //�ܼ�â �����

FILE *pFileHandler = NULL; //���� �ڵ鷯
time_t now_time;
struct tm pTime;
char strTime[32];

#endif

extern HWND hWndMain;

void ErrorMessageBox(const char *msg)
{
	ShowWindow(hWndMain, SW_HIDE);
	MessageBox(NULL, msg, "OpenWorld", MB_OK);
	ApplicationLogger.write("Error message: %s", msg);
	ShowWindow(hWndMain, SW_SHOW);
}

openLogger::openLogger()
{
#ifdef DEBUG
	pFileHandler = fopen(LOGGING_FILENAME, "wt");
	//���� ������ ������, �̴� ������ ���Ű��� �������� ���� ��������� fopen_s �Լ��� ���� ġ������ ���װ� �߻�.
	//���� ����: fclose ���� ���̻� ������ ������ ������ ����. (Access denied)

	if(!pFileHandler) {
		MessageBox(NULL, "Log ���� ���⿡ �����Ͽ����ϴ�.", "OpenWorld", MB_OK);
		exit(0);
	}

	system("title OpenWorld Debug Console");
	std::ios::sync_with_stdio();

	write("Logging started at %s", LOGGING_FILENAME);
#endif
}

openLogger::~openLogger()
{
#ifdef DEBUG
	write("Logging finish.");

	FreeConsole();
	fclose(pFileHandler);
#endif
}

void openLogger::write(const char *fmt, ...)
{
#ifdef DEBUG
	char buf[1024];
	va_list arg;

	//�����μ�(...) ó���� ��ƾ
	va_start(arg, fmt);
	vsprintf_s(buf, fmt, arg);
	va_end(arg);

	now_time = time(NULL);
	localtime_s(&pTime, &now_time); //���� �ð��� ����.

	sprintf_s(strTime, sizeof(strTime), "%d/%d/%d %d:%d:%d", pTime.tm_mon + 1, pTime.tm_mday,
		pTime.tm_year + 1900, pTime.tm_hour, pTime.tm_min, pTime.tm_sec);

	printf("[%s] %s\n", strTime, buf);
	fprintf(pFileHandler, "[%s] %s\n", strTime, buf);

	fflush(pFileHandler);
#endif
}

void openLogger::dbgc(const char *fmt, ...)
{
#ifdef DEBUG
	char buf[1024];
	va_list arg;

	va_start(arg, fmt);
	vsprintf_s(buf, fmt, arg);
	va_end(arg);

	now_time = time(NULL);
	localtime_s(&pTime, &now_time);

	sprintf_s(strTime, sizeof(strTime), "%d/%d/%d %d:%d:%d", pTime.tm_mon + 1, pTime.tm_mday,
		pTime.tm_year + 1900, pTime.tm_hour, pTime.tm_min, pTime.tm_sec);

	printf("DbgPrint [%s] %s\n", strTime, buf);
#endif
}