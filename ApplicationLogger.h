/* ApplicationLogger.h
 * 설명: 각종 디버깅 정보나 오류 내역 출력
 * 코더: 고성훈
 */

#pragma once

extern void ErrorMessageBox(const char *msg);

class openLogger
{
private:
public:
	openLogger();
	~openLogger();

	void write(const char *fmt, ...); //파일에 로깅
	void dbgc(const char *fmt, ...); //콘솔창에 직접로깅
};

static openLogger ApplicationLogger;