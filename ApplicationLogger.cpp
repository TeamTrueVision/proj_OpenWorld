/* ApplicationLogger.cpp
 * 코더: 고성훈
 */

//#define _CRT_SECURE_NO_WARNINGS 1 //컴파일시 보안 관련 오류 안띄우기

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
#pragma comment(linker,"/entry:WinMainCRTStartup /subsystem:console") //콘솔창 띄우기용

FILE *pFileHandler = NULL; //파일 핸들러
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
	//보안 문제가 뜨지만, 이는 마소의 병신같은 개짓으로 인해 권장방법대로 fopen_s 함수를 사용시 치명적인 버그가 발생.
	//버그 내용: fclose 이후 더이상 파일의 접근이 허용되지 않음. (Access denied)

	if(!pFileHandler) {
		MessageBox(NULL, "Log 파일 열기에 실패하였습니다.", "OpenWorld", MB_OK);
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

	//가변인수(...) 처리용 루틴
	va_start(arg, fmt);
	vsprintf_s(buf, fmt, arg);
	va_end(arg);

	now_time = time(NULL);
	localtime_s(&pTime, &now_time); //현재 시간을 구함.

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